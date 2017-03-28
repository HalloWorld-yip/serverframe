#pragma once



//package signal
#define		SIGNAL_HEARDBEATACK	(0)
#define		SIGNAL_VIDEOASK		(1)
#define		SIGNAL_TEXT			(1<<1)
#define		SIGNAL_VIDEOACK		(1<<2)
#define		SIGNAL_HEARDBEAT	(1<<3)
#define		SIGNAL_RE			(1<<4)
#define		SIGNAL_VIDEO		(1<<5)
#define		SIGNAL_AUDIO		(1<<6)
#define		SIGNAL_END			(1<<7)

#define		BROADCAST_PORT	3754
#define		LOCAL_PORT		3888
#define		VIDEO_PORT		3266
#define		AUDIO_PORT		3400
#define		VIDEO_W			640
#define		VIDEO_H			480

#define		STATUS_DEAD			(0)
#define		STATUS_ALIVE		(1)
#define		STATUS_BUSY			(1<<1)
#define		STATUS_WAITING		(1<<2)
#define		STATUS_UNREAD		(1<<3)
#define		STATUS_READY		(1<<4)

#define		UDPBUF			4096
#define		NAMELEN			128


#define		 MAX_BUFSIZE    (4096 << 4)

#define		MAX_SEARCHIP		100000