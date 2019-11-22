#!/usr/bin/perl

use strict;
use warnings;
use Test::More;
use FindBin qw($Bin);
use lib "$Bin/lib";
use MemcachedTest;

my $server = new_memcached();
my $sock = $server->sock;

# command syntax:
# mg [key] [flags]\r\n
# response:
# VA [size] [flags]\r\n
# data\r\n
# or:
# OK [flags]\r\n
# or:
# EN\r\n
# flags are single 'f' or 'f1234' or 'fTEXT'
#
# flags:
# - s: return item size
# - v: return item value
# - c: return item cas
# - t: return item TTL remaining (-1 for unlimited)
# - f: return client flags
# - l: return last access time
# - h: return whether item has been hit before
# - O(token): opaque to copy back.
# - k: return key
# - q: noreply semantics.
# - u: don't bump the item in LRU
# updaters:
# - N(token): vivify on miss, takes TTL as a argument
# - R(token): if token is less than item TTL win for recache
# - T(token): update remaining TTL
# FIXME: do I need a "if stale and no token sent, flip" explicit flag?
# extra response flags:
# - W: client has "won" the token
# - X: object is stale
# - Z: object has sent a winning token
#
# ms [key] [flags]\r\n
# value\r\n
# response:
# OK [flags]\r\n
# OK STORED, NS NOT_STORED, EX EXISTS, NF NOT_FOUND
#
# flags:
# - q: noreply
# - F(token): set client flags
# - C(token): compare CAS value
# - S(token): item size
# - T(token): TTL
# - O(token): opaque to copy back.
# - k: return key
# - I: invalid. set-to-invalid if CAS is older than it should be.
# Not implemented:
# - E: add if not exists (influences other options)
# - A: append (exclusive)
# - P: prepend (exclusive)
# - L: replace (exclusive)
# - incr/decr? pushing it, I guess.
#
# md [key] [flags]\r\n
# response:
# OK [flags]
# flags:
# - q: noreply
# - T(token): updates TTL
# - C(token): compare CAS value
# - I: invalidate. mark as stale, bumps CAS.
# - O(token): opaque to copy back.
# - k: return key
#
# mn\r\n
# response:
# MN\r\n

# metaget tests

# basic test
# - raw mget
# - raw mget miss
# - raw mget bad key

{
    print $sock "set foo 0 0 2\r\nhi\r\n";
    is(scalar <$sock>, "STORED\r\n", "stored test value");

    print $sock "me none\r\n";
    is(scalar <$sock>, "EN\r\n", "raw mget miss");

    print $sock "me foo\r\n";
    like(scalar <$sock>, qr/^ME foo /, "raw mget result");
}

# mget with arguments
# - set some specific TTL and get it back (within reason)
# - get cas
# - autovivify and bit-win

{
    print $sock "set foo2 0 90 2\r\nho\r\n";
    is(scalar <$sock>, "STORED\r\n", "stored test value");

    mget_is({ sock => $sock,
              flags => 's v',
              eflags => 's2' },
            'foo2', 'ho', "retrieved test value");

    # FIXME: figure out what I meant to do here.
    #my $res = mget($sock, 'foo2', 's t v');
}

# lease-test, use two sockets? one socket should be fine, actually.
# - get a win on autovivify
# - get a loss on the same command
# - have a set/cas fail
# - have a cas succeed
# - repeat for "triggered on TTL"
# - test just modifying the TTL (touch)
# - test fetching without value
{
    my $res = mget($sock, 'needwin', 's c v N30 t');
    like($res->{flags}, qr/[scvNt]+/, "got main flags back");
    like($res->{flags}, qr/W/, "got a win result");
    unlike($res->{flags}, qr/Z/, "no token already sent warning");

    # asked for size and TTL. size should be 0, TTL should be > 0 and < 30
    is($res->{size}, 0, "got zero size: autovivified response");
    my $ttl = get_flag($res, 't');
    ok($ttl > 0 && $ttl <= 30, "auto TTL is within requested window: $ttl");

    # try to fail this time.
    {
        my $res = mget($sock, 'needwin', 's t c v N30');
        ok(keys %$res, "got a non-empty response");
        unlike($res->{flags}, qr/W/, "not a win result");
        like($res->{flags}, qr/Z/, "object already sent win result");
    }

    # set back with the wrong CAS
    print $sock "ms needwin C5000 S2 T120\r\nnu\r\n";
    like(scalar <$sock>, qr/^EX /, "failed to SET: CAS didn't match");

    # again, but succeed.
    # TODO: the actual CAS command should work here too?
    my $cas = get_flag($res, 'c');
    print $sock "ms needwin C$cas S2 T120\r\nmu\r\n";
    like(scalar <$sock>, qr/^OK /, "SET: CAS matched");

    # now we repeat the original mget, but the data should be different.
    $res = mget($sock, 'needwin', 's k t c v N30');
    ok(keys %$res, "not a miss");
    ok(find_flags($res, 'sktc'), "got main flags back");
    unlike($res->{flags}, qr/[WZ]/, "not a win or token result");
    is(get_flag($res, 'k'), 'needwin', "key matches");
    $ttl = get_flag($res, 't');
    ok($ttl > 100 && $ttl <= 120, "TTL is within requested window: $ttl");
    is($res->{val}, "mu", "value matches");

    # now we do the whole routine again, but for "triggered on TTL being low"
    # TTL was set to 120 just now, so anything lower than this should trigger.
    $res = mget($sock, 'needwin', 's t c v N30 R130');
    ok(find_flags($res, 'stc'), "got main flags back");
    like($res->{flags}, qr/W/, "got a win result");
    unlike($res->{flags}, qr/Z/, "no token already sent warning");
    is($res->{val}, "mu", "value matches");

    # try to fail this time.
    {
        my $res = mget($sock, 'needwin', 's t c v N30 R130');
        ok(keys %$res, "got a non-empty response");
        unlike($res->{flags}, qr/W/, "not a win result");
        like($res->{flags}, qr/Z/, "object already sent win result");
        is($res->{val}, "mu", "value matches");
    }

    # again, but succeed.
    $cas = get_flag($res, 'c');
    print $sock "ms needwin C$cas S4 T300\r\nzuuu\r\n";
    like(scalar <$sock>, qr/^OK /, "SET: CAS matched");

    # now we repeat the original mget, but the data should be different.
    $res = mget($sock, 'needwin', 's t c v N30');
    ok(keys %$res, "not a miss");
    ok(find_flags($res, 'stc'), "got main flags back");
    unlike($res->{flags}, qr/[WZ]/, "not a win or token result");
    $ttl = get_flag($res, 't');
    ok($ttl > 250 && $ttl <= 300, "TTL is within requested window");
    ok($res->{size} == 4, "Size returned correctly");
    is($res->{val}, "zuuu", "value matches: " . $res->{val});

}

# test get-and-touch mode
{
    # Set key with lower initial TTL.
    print $sock "ms gatkey S4 T100\r\nooom\r\n";
    like(scalar <$sock>, qr/^OK /, "set gatkey");

    # Coolish side feature and/or bringer of bugs: 't' before 'T' gives TTL
    # before adjustment. 'T' before 't' gives TTL after adjustment.
    # Here we want 'T' before 't' to ensure we did adjust the value.
    my $res = mget($sock, 'gatkey', 's v T300 t');
    ok(keys %$res, "not a miss");
    unlike($res->{flags}, qr/[WZ]/, "not a win or token result");
    my $ttl = get_flag($res, 't');
    ok($ttl > 280 && $ttl <= 300, "TTL is within requested window: $ttl");
}

# test no-value mode
{
    # Set key with lower initial TTL.
    print $sock "ms hidevalue S4 T100\r\nhide\r\n";
    like(scalar <$sock>, qr/^OK /, "set hidevalue");

    my $res = mget($sock, 'hidevalue', 's t');
    ok(keys %$res, "not a miss");
    is($res->{val}, undef, "no value returned");

    $res = mget($sock, 'hidevalue', 's t v');
    ok(keys %$res, "not a miss");
    is($res->{val}, 'hide', "real value returned");
}

# test hit-before? flag
{
    print $sock "ms hitflag S3 T100\r\nhit\r\n";
    like(scalar <$sock>, qr/^OK /, "set hitflag");

    my $res = mget($sock, 'hitflag', 's t h');
    ok(keys %$res, "not a miss");
    is(get_flag($res, 'h'), 0, "not been hit before");

    $res = mget($sock, 'hitflag', 's t h');
    ok(keys %$res, "not a miss");
    is(get_flag($res, 'h'), 1, "been hit before");
}

# test no-update flag
{
    print $sock "ms noupdate S3 T100\r\nhit\r\n";
    like(scalar <$sock>, qr/^OK /, "set noupdate");

    my $res = mget($sock, 'noupdate', 's t u h');
    ok(keys %$res, "not a miss");
    is(get_flag($res, 'h'), 0, "not been hit before");

    # _next_ request should show a hit.
    # gets modified here but returns previous state.
    $res = mget($sock, 'noupdate', 's t h');
    is(get_flag($res, 'h'), 0, "still not a hit");

    $res = mget($sock, 'noupdate', 's t u h');
    is(get_flag($res, 'h'), 1, "finally a hit");
}

# test last-access time
{
    print $sock "ms la_test S2 T100\r\nla\r\n";
    like(scalar <$sock>, qr/^OK /, "set la_test");
    sleep 2;

    my $res = mget($sock, 'la_test', 's t l');
    ok(keys %$res, "not a miss");
    isnt(get_flag($res, 'l'), 0, "been over a second since most recently accessed");
}

# high level tests:
# - mget + mset with serve-stale
# - set a value
# - mget it back. should be no XZW tokens
# - invalidate via mdelete and mget/revalidate with mset
#   - remember failure scenarios!
#     - TTL timed out?
#     - CAS too high?
#   - also test re-setting as stale (CAS is below requested)
#     - this should probably be conditional.

{
    diag "starting serve stale with mdelete";
    my ($ttl, $cas, $res);
    print $sock "set toinv 0 0 3\r\nmoo\r\n";
    is(scalar <$sock>, "STORED\r\n", "stored key 'toinv'");

    $res = mget($sock, 'toinv', 's v');
    unlike($res->{flags}, qr/[XWZ]/, "no extra flags");

    # Lets mark the sucker as invalid, and drop its TTL to 30s
    diag "running mdelete";
    print $sock "md toinv I T30\r\n";
    like(scalar <$sock>, qr/^OK /, "mdelete'd key");

    # TODO: decide on if we need an explicit flag for "if I fetched a stale
    # value, does winning matter?
    # I think it's probably fine. clients can always ignore the win, or we can
    # add an option later to "don't try to revalidate if stale", perhaps.
    $res = mget($sock, 'toinv', 's t c v');
    ok(keys %$res, "not a miss");
    ok(find_flags($res, 'stc'), "got main flags back");
    like($res->{flags}, qr/W/, "won the recache");
    like($res->{flags}, qr/X/, "item is marked stale");
    $ttl = get_flag($res, 't');
    ok($ttl > 0 && $ttl <= 30, "TTL is within requested window");
    ok($res->{size} == 3, "Size returned correctly");
    is($res->{val}, "moo", "value matches");

    diag "trying to fail then stale set via mset";
    print $sock "ms toinv S1 T90 C0\r\nf\r\n";
    like(scalar <$sock>, qr/^EX /, "failed to SET: low CAS didn't match");

    print $sock "ms toinv S1 I T90 C0\r\nf\r\n";
    like(scalar <$sock>, qr/^OK /, "SET an invalid/stale item");

    diag "confirm item still stale, and TTL wasn't raised.";
    $res = mget($sock, 'toinv', 's t c v');
    like($res->{flags}, qr/X/, "item is marked stale");
    like($res->{flags}, qr/Z/, "win token already sent");
    unlike($res->{flags}, qr/W/, "didn't win: token already sent");
    $ttl = get_flag($res, 't');
    ok($ttl > 0 && $ttl <= 30, "TTL wasn't modified");

    # TODO: CAS too high?

    diag "do valid mset";
    $cas = get_flag($res, 'c');
    print $sock "ms toinv S1 T90 C$cas\r\ng\r\n";
    like(scalar <$sock>, qr/^OK /, "SET over the stale item");

    $res = mget($sock, 'toinv', 's t c v');
    ok(keys %$res, "not a miss");
    unlike($res->{flags}, qr/[WXZ]/, "no stale, win, or tokens");

    $ttl = get_flag($res, 't');
    ok($ttl > 30 && $ttl <= 90, "TTL was modified");
    ok($cas != get_flag($res, 'c'), "CAS was updated");
    is($res->{size}, 1, "size updated");
    is($res->{val}, "g", "value was updated");
}

# Quiet flag suppresses most output. Badly invalid commands will still
# generate something. Not weird to parse like 'noreply' token was...
# mget's with hits should return real data.
{
    diag "testing quiet flag";
    print $sock "ms quiet q S2\r\nmo\r\n";
    print $sock "md quiet q\r\n";
    print $sock "mg quiet s v q\r\n";
    diag "now purposefully cause an error\r\n";
    print $sock "ms quiet\r\n";
    like(scalar <$sock>, qr/^CLIENT_ERROR/, "resp not OK, or EN");

    # Now try a pipelined get. Throw an mnop at the end
    print $sock "ms quiet q S2\r\nbo\r\n";
    print $sock "mg quiet v q\r\nmg quiet v q\r\nmg quietmiss v q\r\nmn\r\n";
    # Should get back VA/data/VA/data/EN
    like(scalar <$sock>, qr/^VA 2/, "get response");
    like(scalar <$sock>, qr/^bo/, "get value");
    like(scalar <$sock>, qr/^VA 2/, "get response");
    like(scalar <$sock>, qr/^bo/, "get value");
    like(scalar <$sock>, qr/^MN/, "end token");
}

{
    my $k = 'otest';
    diag "testing mget opaque";
    print $sock "ms $k S2 T100\r\nra\r\n";
    like(scalar <$sock>, qr/^OK /, "set $k");

    my $res = mget($sock, $k, 't v Oopaque');
    is(get_flag($res, 'O'), 'opaque', "O flag returned opaque");
}

{
    diag "flag and token count errors";
    print $sock "mg foo m o o o o o o o o o\r\n";
    like(scalar <$sock>, qr/^CLIENT_ERROR invalid or duplicate flag/, "gone silly with flags");
}

{
    diag "pipeline test";
    print $sock "ms foo S2 T100\r\nna\r\n";
    like(scalar <$sock>, qr/^OK /, "set foo");
    print $sock "mg foo s\r\nmg foo s\r\nquit\r\nmg foo s\r\n";
    like(scalar <$sock>, qr/^OK /, "got resp");
    like(scalar <$sock>, qr/^OK /, "got resp");
    is(scalar <$sock>, undef, "final get didn't run");
}

# TODO: move wait_for_ext into Memcached.pm
sub wait_for_ext {
    my $sock = shift;
    my $target = shift || 0;
    my $sum = $target + 1;
    while ($sum > $target) {
        my $s = mem_stats($sock, "items");
        $sum = 0;
        for my $key (keys %$s) {
            if ($key =~ m/items:(\d+):number/) {
                # Ignore classes which can contain extstore items
                next if $1 < 3;
                $sum += $s->{$key};
            }
        }
        sleep 1 if $sum > $target;
    }
}

my $ext_path;
# Do a basic extstore test if enabled.
if (supports_extstore()) {
    diag "mget + extstore tests";
    $ext_path = "/tmp/extstore.$$";
    my $server = new_memcached("-m 64 -U 0 -o ext_page_size=8,ext_wbuf_size=2,ext_threads=1,ext_io_depth=2,ext_item_size=512,ext_item_age=2,ext_recache_rate=10000,ext_max_frag=0.9,ext_path=$ext_path:64m,slab_automove=0,ext_compact_under=1,no_lru_crawler");
    my $sock = $server->sock;

    my $value;
    {
        my @chars = ("C".."Z");
        for (1 .. 20000) {
            $value .= $chars[rand @chars];
        }
    }

    my $keycount = 10;
    for (1 .. $keycount) {
        print $sock "set nfoo$_ 0 0 20000 noreply\r\n$value\r\n";
    }

    wait_for_ext($sock);
    mget_is({ sock => $sock,
              flags => 's v',
              eflags => 's20000' },
            'nfoo1', $value, "retrieved test value");
    my $stats = mem_stats($sock);
    cmp_ok($stats->{get_extstore}, '>', 0, 'one object was fetched');

    my $ovalue = $value;
    for (1 .. 4) {
        $value .= $ovalue;
    }
    # Fill to eviction.
    $keycount = 1000;
    for (1 .. $keycount) {
        print $sock "set mfoo$_ 0 0 100000 noreply\r\n$value\r\n";
        # wait to avoid memory evictions
        wait_for_ext($sock, 1) if ($_ % 250 == 0);
    }

    print $sock "mg mfoo1 s v\r\n";
    is(scalar <$sock>, "EN\r\n");
    print $sock "mg mfoo1 s v q\r\nmn\r\n";
    is(scalar <$sock>, "MN\r\n");
    $stats = mem_stats($sock);
    cmp_ok($stats->{miss_from_extstore}, '>', 0, 'at least one miss');
}

###

# takes hash:
# - sock
# - args (metaget flags)
# - array of tokens
# - array of expected response tokens

# returns hash:
# - win (if won a condition)
# - array of tokens
# - value, etc?
# useful to chain together for further requests.
# works only with single line values. no newlines in value.
# FIXME: some workaround for super long values :|
# TODO: move this to lib/MemcachedTest.pm
sub mget_is {
    # single line values only
    my ($o, $key, $val, $msg) = @_;

    my $dval = defined $val ? "'$val'" : "<undef>";
    $msg ||= "$key == $dval";

    my $s = $o->{sock};
    my $flags = $o->{flags};
    my $eflags = $o->{eflags} || $flags;

    print $s "mg $key $flags\r\n";
    if (! defined $val) {
        my $line = scalar <$s>;
        if ($line =~ /^VA/) {
            $line .= scalar(<$s>);
        }
        Test::More::is($line, "EN\r\n", $msg);
    } else {
        my $len = length($val);
        my $body = scalar(<$s>);
        my $expected = "VA $len $eflags\r\n$val\r\n";
        if (!$body || $body =~ /^EN/) {
            Test::More::is($body, $expected, $msg);
            return;
        }
        $body .= scalar(<$s>);
        Test::More::is($body, $expected, $msg);
        return mget_res($body);
    }
    return {};
}

# only fetches values without newlines in it.
sub mget {
    my $s = shift;
    my $key = shift;
    my $flags = shift;

    print $s "mg $key $flags\r\n";
    my $header = scalar(<$s>);
    my $val = "\r\n";
    if ($header =~ m/^VA/) {
        $val = scalar(<$s>);
    }

    return mget_res($header . $val);
}

# parse out a response
sub mget_res {
    my $resp = shift;
    my %r = ();

    if ($resp =~ m/^VA (\d+) ([^\r]+)\r\n(.*)\r\n/gm) {
        $r{size} = $1;
        $r{flags} = $2;
        $r{val} = $3;
    } elsif ($resp =~ m/^OK ([^\r]+)\r\n/gm) {
        $r{flags} = $1;
        $r{hd} = 1;
    }

    return \%r;
}

sub get_flag {
    my $res = shift;
    my $flag = shift;
    #print STDERR "FLAGS: $res->{flags}\n";
    my @flags = split(/ /, $res->{flags});
    for my $f (@flags) {
        if ($f =~ m/^$flag/) {
            return substr $f, 1;
        }
    }
}

sub find_flags {
    my $res = shift;
    my $flags = shift;
    my @flags = split(//, $flags);
    for my $f (@flags) {
        return 0 unless get_flag($res, $f);
    }
    return 1;
}

done_testing();

END {
    unlink $ext_path if $ext_path;
}