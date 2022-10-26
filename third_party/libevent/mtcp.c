#include "event2/event-config.h"
#include "evconfig-private.h"

#include <stdint.h>
#include <sys/types.h>
#include <sys/resource.h>
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/queue.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#ifdef EVENT__HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef EVENT__HAVE_SYS_TIMERFD_H
#include <sys/timerfd.h>
#endif

#include "event-internal.h"
#include "evsignal-internal.h"
#include "event2/thread.h"
#include "evthread-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"
#include "changelist-internal.h"
#include "time-internal.h"

// mtcp header
#include "mtcp_api.h"
#include "mtcp_epoll.h"

#include "mtcp_epolltable-internal.h"

struct mtcp_epollop {
    struct mtcp_epoll_event *events;
    int nevents;
    int mtcp_epfd;
};

static void*    mtcp_epoll_init(struct event_base *);
static int      mtcp_epoll_dispatch(struct event_base *, struct timeval *);
static void     mtcp_epoll_dealloc(struct event_base *);

static const struct eventop mtcp_epollops_changelist = {
	"epoll (with changelist)",
	mtcp_epoll_init,
	event_changelist_add_,
	event_changelist_del_,
	mtcp_epoll_dispatch,
	mtcp_epoll_dealloc,
	1, /* need reinit */
	EV_FEATURE_ET|EV_FEATURE_O1,
	EVENT_CHANGELIST_FDINFO_SIZE
};

static int mtcp_epoll_nochangelist_add(struct event_base *base, evutil_socket_t fd,
    short old, short events, void *p);
static int mtcp_epoll_nochangelist_del(struct event_base *base, evutil_socket_t fd,
    short old, short events, void *p);

const struct eventop mtcp_epollops = {
    "mtcp_epoll",
    mtcp_epoll_init,
	mtcp_epoll_nochangelist_add,
	mtcp_epoll_nochangelist_del,
	mtcp_epoll_dispatch,
	mtcp_epoll_dealloc,
	1, /* need reinit */
	EV_FEATURE_ET|EV_FEATURE_O1|EV_FEATURE_EARLY_CLOSE,
	0
};

#define INITIAL_NEVENT 32
#define MAX_NEVENT 4096
#define MAX_EPOLL_TIMEOUT_MSEC (35*60*1000)

/*!
 * \brief   init mtcp epoll handler
 * \param   base    event_base
 * \return  the created mtcp_epollop
 */
static void *
mtcp_epoll_init(struct event_base *base){
    int mtcp_epfd = -1;
    struct mtcp_epollop *mtcp_epollop;

    /* Create mtcp epoll handler */
    mtcp_epfd = mtcp_epoll_create(base->mctx, MAX_NEVENT);
    if(mtcp_epfd < 0){
        fprintf(stderr, "failed to create mtcp epoll\n");
        goto mtcp_epoll_init_exit;
    }

    /* Create event list */
    if (!(mtcp_epollop = mm_calloc(1, sizeof(struct mtcp_epollop)))) {
        fprintf(stderr, "failed to allocate memory for mtcp_epollop\n");
		goto close_mtcp_epoll;
	}

    mtcp_epollop->mtcp_epfd = mtcp_epfd;

    /* Initialize fields */
    mtcp_epollop->events = mm_calloc(INITIAL_NEVENT, sizeof(struct mtcp_epoll_event));
	if (mtcp_epollop->events == NULL) {
        fprintf(stderr, "failed to allocate memory for mtcp epoll event list\n");
		goto free_mtcp_epollop;
    }
    mtcp_epollop->nevents = INITIAL_NEVENT;

    if ((base->flags & EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST) != 0 ||
	    ((base->flags & EVENT_BASE_FLAG_IGNORE_ENV) == 0 &&
		evutil_getenv_("MTCP_EVENT_EPOLL_USE_CHANGELIST") != NULL)) {

		base->evsel = &mtcp_epollops_changelist;
	}

    evsig_init_(base);

	return (mtcp_epollop);

free_mtcp_epollop:
    free(mtcp_epollop);

close_mtcp_epoll:
    mtcp_close(base->mctx, mtcp_epfd);

mtcp_epoll_init_exit:
    return NULL;
}

/*!
 * \brief   convert type of change event to string
 * \param   change  index of changing type
 * \return  corresponding string
 */
static const char *
mtcp_change_to_string(int change)
{
	change &= (EV_CHANGE_ADD|EV_CHANGE_DEL);
	if (change == EV_CHANGE_ADD) {
		return "add";
	} else if (change == EV_CHANGE_DEL) {
		return "del";
	} else if (change == 0) {
		return "none";
	} else {
		return "???";
	}
}

/*!
 * \brief   convert type of mtcp epoll operation to string
 * \param   op  index of mtcp epoll operation
 * \return  corresponding string
 */
static const char *
mtcp_epoll_op_to_string(int op)
{
	return op == MTCP_EPOLL_CTL_ADD?"ADD":
	    op == MTCP_EPOLL_CTL_DEL?"DEL":
	    op == MTCP_EPOLL_CTL_MOD?"MOD":
	    "???";
}

#define MTCP_PRINT_CHANGES(op, events, ch, status)  \
	"MTCP Epoll %s(%d) on fd %d " status ". "  \
	"Old events were %d; "                     \
	"read change was %d (%s); "                \
	"write change was %d (%s); "               \
	"close change was %d (%s)",                \
	mtcp_epoll_op_to_string(op),               \
	events,                                    \
	ch->fd,                                    \
	ch->old_events,                            \
	ch->read_change,                           \
	mtcp_change_to_string(ch->read_change),    \
	ch->write_change,                          \
	mtcp_change_to_string(ch->write_change),   \
	ch->close_change,                          \
	mtcp_change_to_string(ch->close_change)

/*!
 * \brief   apply a single change to mtcp context based on given event_change
 * \param   base            the event base
 * \param   mtcp_epollop    specified mtcp_epollop
 * \param   ch              given event change
 * \return  apply state
 */
static int
mtcp_epoll_apply_one_change(struct event_base *base,
    struct mtcp_epollop *mtcp_epollop,
    const struct event_change *ch)
{
    struct mtcp_epoll_event epev;
    int op, events = 0;
	int idx;

    idx = MTCP_EPOLL_OP_TABLE_INDEX(ch);
    op = mtcp_epoll_op_table[idx].op;
	events = mtcp_epoll_op_table[idx].events;

    if (!events) {
		EVUTIL_ASSERT(op == 0);
		return 0;
	}

    if ((ch->read_change|ch->write_change) & EV_CHANGE_ET)
		events |= MTCP_EPOLLET;
    
    memset(&epev, 0, sizeof(epev));
	epev.data.sockid = ch->fd;
	epev.events = events;
	if (mtcp_epoll_ctl(base->mctx, mtcp_epollop->mtcp_epfd, op, ch->fd, &epev) == 0) {
		fprintf(stdout, "apply change to mtcp epoll: op - %d, events - %d, epoll id - %d, fd - %d\n", 
			op, events, mtcp_epollop->mtcp_epfd, ch->fd);
		event_debug((MTCP_PRINT_CHANGES(op, epev.events, ch, "okay")));
		return 0;
	}

    switch (op) {
    case MTCP_EPOLL_CTL_MOD:
		if (errno == ENOENT) {
			/* If a MOD operation fails with ENOENT, the
			 * fd was probably closed and re-opened.  We
			 * should retry the operation as an ADD.
			 */
			if (mtcp_epoll_ctl(base->mctx, mtcp_epollop->mtcp_epfd, MTCP_EPOLL_CTL_ADD, ch->fd, &epev) == -1) {
				event_warn("MTCP Epoll MOD(%d) on %d retried as ADD; that failed too",
				    (int)epev.events, ch->fd);
				return -1;
			} else {
				event_debug(("MTCP Epoll MOD(%d) on %d retried as ADD; succeeded.",
					(int)epev.events,
					ch->fd));
				return 0;
			}
		}
		break;
    case MTCP_EPOLL_CTL_ADD:
		if (errno == EEXIST) {
			/* If an ADD operation fails with EEXIST,
			 * either the operation was redundant (as with a
			 * precautionary add), or we ran into a fun
			 * kernel bug where using dup*() to duplicate the
			 * same file into the same fd gives you the same epitem
			 * rather than a fresh one.  For the second case,
			 * we must retry with MOD. */
			if (mtcp_epoll_ctl(base->mctx, mtcp_epollop->mtcp_epfd, MTCP_EPOLL_CTL_MOD, ch->fd, &epev) == -1) {
				event_warn("MTCP Epoll ADD(%d) on %d retried as MOD; that failed too",
				    (int)epev.events, ch->fd);
				return -1;
			} else {
				event_debug(("MTCP Epoll ADD(%d) on %d retried as MOD; succeeded.",
					(int)epev.events,
					ch->fd));
				return 0;
			}
		}
		break;
	case MTCP_EPOLL_CTL_DEL:
		if (errno == ENOENT || errno == EBADF || errno == EPERM) {
			/* If a delete fails with one of these errors,
			 * that's fine too: we closed the fd before we
			 * got around to calling epoll_dispatch. */
			event_debug(("MTCP Epoll DEL(%d) on fd %d gave %s: DEL was unnecessary.",
				(int)epev.events,
				ch->fd,
				strerror(errno)));
			return 0;
		}
		break;
	default:
		break;
    }

    event_warn(MTCP_PRINT_CHANGES(op, epev.events, ch, "failed"));
	return -1;
}

/*!
 * \brief   apply changes to mtcp context based on given event_change
 * \param   base            the event base
 * \return  apply state
 */
static int
mtcp_epoll_apply_changes(struct event_base *base)
{
	struct event_changelist *changelist = &base->changelist;
	struct mtcp_epollop *mtcp_epollop = base->evbase;
	struct event_change *ch;

	int r = 0;
	int i;

	for (i = 0; i < changelist->n_changes; ++i) {
		ch = &changelist->changes[i];
		if (mtcp_epoll_apply_one_change(base, mtcp_epollop, ch) < 0)
			r = -1;
	}

	return (r);
}

static int
mtcp_epoll_nochangelist_add(struct event_base *base, evutil_socket_t fd,
    short old, short events, void *p)
{
	struct event_change ch;
	ch.fd = fd;
	ch.old_events = old;
	ch.read_change = ch.write_change = ch.close_change = 0;
	if (events & EV_WRITE)
		ch.write_change = EV_CHANGE_ADD |
		    (events & EV_ET);
	if (events & EV_READ)
		ch.read_change = EV_CHANGE_ADD |
		    (events & EV_ET);
	if (events & EV_CLOSED)
		ch.close_change = EV_CHANGE_ADD |
		    (events & EV_ET);

	return mtcp_epoll_apply_one_change(base, base->evbase, &ch);
}

static int
mtcp_epoll_nochangelist_del(struct event_base *base, evutil_socket_t fd,
    short old, short events, void *p)
{
	struct event_change ch;
	ch.fd = fd;
	ch.old_events = old;
	ch.read_change = ch.write_change = ch.close_change = 0;
	if (events & EV_WRITE)
		ch.write_change = EV_CHANGE_DEL |
		    (events & EV_ET);
	if (events & EV_READ)
		ch.read_change = EV_CHANGE_DEL |
		    (events & EV_ET);
	if (events & EV_CLOSED)
		ch.close_change = EV_CHANGE_DEL |
		    (events & EV_ET);

	return mtcp_epoll_apply_one_change(base, base->evbase, &ch);
}

/*!
 * \brief   convert type of mtcp epoll operation to string
 * \param   op  index of mtcp epoll operation
 * \return  corresponding string
 */
static int
mtcp_epoll_dispatch(struct event_base *base, struct timeval *tv){
    struct mtcp_epollop *mtcp_epollop = base->evbase;
	struct mtcp_epoll_event *events = mtcp_epollop->events;
	int i, res;
	long timeout = -1;

    if (tv != NULL) {
		timeout = evutil_tv_to_msec_(tv);
		if (timeout < 0 || timeout > MAX_EPOLL_TIMEOUT_MSEC) {
			/* Linux kernels can wait forever if the timeout is
			 * too big; see comment on MAX_EPOLL_TIMEOUT_MSEC. */
			timeout = MAX_EPOLL_TIMEOUT_MSEC;
		}
	}

    mtcp_epoll_apply_changes(base);
    event_changelist_remove_all_(&base->changelist, base);

    EVBASE_RELEASE_LOCK(base, th_base_lock);

	fprintf(stdout, "start waiting, timeout: %d\n", timeout);
    res = mtcp_epoll_wait(base->mctx, mtcp_epollop->mtcp_epfd, events, mtcp_epollop->nevents, timeout);
	fprintf(stdout, "finish waiting\n");

    EVBASE_ACQUIRE_LOCK(base, th_base_lock);

    if (res < 0) {
		if (errno != EINTR) {
			event_warn("epoll_wait");
			return (-1);
		}

		return (0);
	}

    event_debug(("%s: mtcp_epoll_wait reports %d", __func__, res));
    EVUTIL_ASSERT(res <= mtcp_epollop->nevents);

    for (i = 0; i < res; i++) {
		int what = events[i].events;
		short ev = 0;

        if (what & (MTCP_EPOLLHUP|MTCP_EPOLLERR)) {
			ev = EV_READ | EV_WRITE;
		} else {
			if (what & MTCP_EPOLLIN)
				ev |= EV_READ;
			if (what & MTCP_EPOLLOUT)
				ev |= EV_WRITE;
			if (what & MTCP_EPOLLRDHUP)
				ev |= EV_CLOSED;
		}

		if (!ev)
			continue;

		evmap_io_active_(base, events[i].data.sockid, ev | EV_ET);
    }

    if (res == mtcp_epollop->nevents && mtcp_epollop->nevents < MAX_NEVENT) {
		/* We used all of the event space this time.  We should
		   be ready for more events next time. */
		int new_nevents = mtcp_epollop->nevents * 2;
		struct epoll_event *new_events;

		new_events = mm_realloc(mtcp_epollop->events,
		    new_nevents * sizeof(struct mtcp_epoll_event));
		if (new_events) {
			mtcp_epollop->events = new_events;
			mtcp_epollop->nevents = new_nevents;
		}
	}

	return (0);
}

static void
mtcp_epoll_dealloc(struct event_base *base)
{
	struct mtcp_epollop *mtcp_epollop = base->evbase;

	evsig_dealloc_(base);
	if (mtcp_epollop->events)
		mm_free(mtcp_epollop->events);
	if (mtcp_epollop->mtcp_epfd >= 0)
		close(mtcp_epollop->mtcp_epfd);

	memset(mtcp_epollop, 0, sizeof(struct mtcp_epollop));
	mm_free(mtcp_epollop);
}