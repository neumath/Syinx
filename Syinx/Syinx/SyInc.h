#include <iostream>
#include <fstream>
#include <istream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <random>

#include <list>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#ifdef POSIX_PTHREAD
#include "pthread.h"
#endif

#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#include <mysql.h>
#ifdef LIBEVENT_IOCP
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/listener.h"
#include "event2/buffer.h"
#include "event2/thread.h"
#endif
#elif defined (__linux__)
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include <pthread.h>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <mysql/mysql.h>
#endif

#ifdef _DEBUG 
#pragma warning(push)
#pragma warning(disable:4018)
#pragma warning(disable:4819)

#endif

#ifdef LIBEVENT_HTTP
#include <event2/http>
#endif





