#ifndef MTCP_EPOLLTABLE_INTERNAL_H_INCLUDED_
#define MTCP_EPOLLTABLE_INTERNAL_H_INCLUDED_

#define MTCP_EPOLL_OP_TABLE_INDEX(c) \
	(   (((c)->close_change&(EV_CHANGE_ADD|EV_CHANGE_DEL))) |		\
	    (((c)->read_change&(EV_CHANGE_ADD|EV_CHANGE_DEL)) << 2) |	\
	    (((c)->write_change&(EV_CHANGE_ADD|EV_CHANGE_DEL)) << 4) |	\
	    (((c)->old_events&(EV_READ|EV_WRITE)) << 5) |		\
	    (((c)->old_events&(EV_CLOSED)) << 1)				\
	    )

#if EV_READ != 2 || EV_WRITE != 4 || EV_CLOSED != 0x80 || EV_CHANGE_ADD != 1 || EV_CHANGE_DEL != 2
#error "Libevent's internals changed!  Regenerate the op_table in epolltable-internal.h"
#endif

#include "mtcp_api.h"
#include "mtcp_epoll.h"

static const struct operation {
	int events;
	int op;
} mtcp_epoll_op_table[] = {
	/* old=  0, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old=  0, write:  0, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:  0, read:  0, close:del */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  0, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old=  0, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:  0, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:  0, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old=  0, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  0, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old=  0, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old=  0, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  0, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  0, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old=  0, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old=  0, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  0, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old=  0, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old=  0, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  0, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:del, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:del, read:  0, close:del */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  0, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old=  0, write:del, read:del, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_ADD },
	/* old=  0, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  0, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old=  0, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  0, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old=  0, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old=  0, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  0, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old=  0, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old=  0, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old=  0, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  0, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old=  0, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old=  0, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old=  0, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old=  0, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old=  0, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old=  0, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old=  0, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old=  0, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  0, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old=  0, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old=  0, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  r, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old=  r, write:  0, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  r, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old=  r, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_DEL },
	/* old=  r, write:  0, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:  0, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  r, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old=  r, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  r, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old=  r, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old=  r, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  r, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  r, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old=  r, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old=  r, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  r, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old=  r, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old=  r, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  r, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  r, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old=  r, write:del, read:del, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old=  r, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  r, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  r, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old=  r, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  r, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old=  r, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old=  r, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  r, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old=  r, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old=  r, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old=  r, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  r, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old=  r, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old=  r, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old=  r, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old=  r, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old=  r, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old=  r, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old=  r, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old=  r, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  r, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old=  r, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old=  r, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  w, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old=  w, write:  0, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  w, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old=  w, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old=  w, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  w, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old=  w, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old=  w, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  w, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  w, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old=  w, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old=  w, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  w, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old=  w, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old=  w, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  w, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old=  w, write:del, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:del, read:  0, close:del */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  w, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  w, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old=  w, write:del, read:del, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old=  w, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  w, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  w, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old=  w, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  w, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old=  w, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old=  w, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  w, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old=  w, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old=  w, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old=  w, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  w, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old=  w, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old=  w, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old=  w, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old=  w, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old=  w, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old=  w, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old=  w, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old=  w, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  w, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old=  w, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old=  w, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= rw, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old= rw, write:  0, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old= rw, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old= rw, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old= rw, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= rw, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old= rw, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old= rw, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= rw, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old= rw, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old= rw, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old= rw, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= rw, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old= rw, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old= rw, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= rw, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old= rw, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old= rw, write:del, read:del, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_DEL },
	/* old= rw, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= rw, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old= rw, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old= rw, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= rw, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old= rw, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old= rw, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= rw, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old= rw, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old= rw, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old= rw, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old= rw, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old= rw, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old= rw, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old= rw, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old= rw, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old= rw, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old= rw, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old= rw, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old= rw, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= rw, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old= rw, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old= rw, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  c, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old=  c, write:  0, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:  0, close:del */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  c, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  c, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old=  c, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:  0, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  c, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old=  c, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  c, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old=  c, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old=  c, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  c, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  c, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old=  c, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old=  c, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  c, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old=  c, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old=  c, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  c, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:  0, close:del */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  c, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  c, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old=  c, write:del, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=  c, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=  c, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old=  c, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  c, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old=  c, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old=  c, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=  c, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old=  c, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old=  c, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old=  c, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old=  c, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old=  c, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old=  c, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old=  c, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old=  c, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old=  c, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old=  c, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old=  c, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old=  c, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=  c, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old=  c, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old=  c, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cr, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old= cr, write:  0, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cr, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old= cr, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:  0, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old= cr, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old= cr, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cr, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old= cr, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old= cr, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cr, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cr, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old= cr, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old= cr, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cr, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old= cr, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old= cr, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cr, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cr, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old= cr, write:del, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cr, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old= cr, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old= cr, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cr, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old= cr, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old= cr, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cr, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old= cr, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old= cr, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old= cr, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cr, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old= cr, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old= cr, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old= cr, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old= cr, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old= cr, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old= cr, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old= cr, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old= cr, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cr, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old= cr, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old= cr, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cw, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old= cw, write:  0, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cw, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old= cw, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old= cw, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cw, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old= cw, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old= cw, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cw, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:  0, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:  0, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cw, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old= cw, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old= cw, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cw, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old= cw, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old= cw, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cw, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:  0, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:  0, close:del */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old= cw, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cw, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old= cw, write:del, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old= cw, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old= cw, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old= cw, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cw, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old= cw, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old= cw, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old= cw, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old= cw, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old= cw, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old= cw, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old= cw, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old= cw, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old= cw, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old= cw, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old= cw, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old= cw, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old= cw, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old= cw, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old= cw, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old= cw, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old= cw, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old= cw, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=crw, write:  0, read:  0, close:  0 */
	{ 0, 0 },
	/* old=crw, write:  0, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:  0, close:xxx */
	{ 0, 255 },
	/* old=crw, write:  0, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:add, close:xxx */
	{ 0, 255 },
	/* old=crw, write:  0, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:  0, read:del, close:xxx */
	{ 0, 255 },
	/* old=crw, write:  0, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=crw, write:  0, read:xxx, close:add */
	{ 0, 255 },
	/* old=crw, write:  0, read:xxx, close:del */
	{ 0, 255 },
	/* old=crw, write:  0, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=crw, write:add, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:  0, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:  0, close:xxx */
	{ 0, 255 },
	/* old=crw, write:add, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:add, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:add, close:xxx */
	{ 0, 255 },
	/* old=crw, write:add, read:del, close:  0 */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:del, close:add */
	{ MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:del, close:del */
	{ MTCP_EPOLLOUT, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:add, read:del, close:xxx */
	{ 0, 255 },
	/* old=crw, write:add, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=crw, write:add, read:xxx, close:add */
	{ 0, 255 },
	/* old=crw, write:add, read:xxx, close:del */
	{ 0, 255 },
	/* old=crw, write:add, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=crw, write:del, read:  0, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:  0, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:  0, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:  0, close:xxx */
	{ 0, 255 },
	/* old=crw, write:del, read:add, close:  0 */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:add, close:add */
	{ MTCP_EPOLLIN|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:add, close:del */
	{ MTCP_EPOLLIN, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:add, close:xxx */
	{ 0, 255 },
	/* old=crw, write:del, read:del, close:  0 */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:del, close:add */
	{ MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_MOD },
	/* old=crw, write:del, read:del, close:del */
	{ MTCP_EPOLLIN|MTCP_EPOLLOUT|MTCP_EPOLLRDHUP, MTCP_EPOLL_CTL_DEL },
	/* old=crw, write:del, read:del, close:xxx */
	{ 0, 255 },
	/* old=crw, write:del, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=crw, write:del, read:xxx, close:add */
	{ 0, 255 },
	/* old=crw, write:del, read:xxx, close:del */
	{ 0, 255 },
	/* old=crw, write:del, read:xxx, close:xxx */
	{ 0, 255 },
	/* old=crw, write:xxx, read:  0, close:  0 */
	{ 0, 255 },
	/* old=crw, write:xxx, read:  0, close:add */
	{ 0, 255 },
	/* old=crw, write:xxx, read:  0, close:del */
	{ 0, 255 },
	/* old=crw, write:xxx, read:  0, close:xxx */
	{ 0, 255 },
	/* old=crw, write:xxx, read:add, close:  0 */
	{ 0, 255 },
	/* old=crw, write:xxx, read:add, close:add */
	{ 0, 255 },
	/* old=crw, write:xxx, read:add, close:del */
	{ 0, 255 },
	/* old=crw, write:xxx, read:add, close:xxx */
	{ 0, 255 },
	/* old=crw, write:xxx, read:del, close:  0 */
	{ 0, 255 },
	/* old=crw, write:xxx, read:del, close:add */
	{ 0, 255 },
	/* old=crw, write:xxx, read:del, close:del */
	{ 0, 255 },
	/* old=crw, write:xxx, read:del, close:xxx */
	{ 0, 255 },
	/* old=crw, write:xxx, read:xxx, close:  0 */
	{ 0, 255 },
	/* old=crw, write:xxx, read:xxx, close:add */
	{ 0, 255 },
	/* old=crw, write:xxx, read:xxx, close:del */
	{ 0, 255 },
	/* old=crw, write:xxx, read:xxx, close:xxx */
	{ 0, 255 },
};

#endif
