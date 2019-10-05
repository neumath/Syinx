#pragma once 
#include<event2/event.h>
#include <event2/listener.h>  
#include <event2/bufferevent.h>  
#include "Log.h"

#define USE_JSON_CONFIG
#define CMD_PRINTF_ERR
#define SOCKETS	                                         int
//libevent
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING           (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE)
#define SETOPT_THREADSAFE_OR_SOCKETS_BLOCKING_OR_FREE   (LEV_OPT_LEAVE_SOCKETS_BLOCKING | LEV_OPT_THREADSAFE |  LEV_OPT_CLOSE_ON_FREE)
#define SET_SOCKETS_EVENT_RDWR                          (EV_READ | EV_WRITE | EV_PERSIST)
#define SET_SOCKETS_EVENT_RD                            (EV_READ | EV_PERSIST)
//pthread
//主要线程
#define PRINCIPAL_PTHREAD                                public
//次要线程
#define MINOR_PTHREAD                                    public
//log
#define WRLOG(Type, Int, Char)                          (mLog.Log(__FILE__, __LINE__, Type, Int, Char))