#ifndef _SYINX_LOG_H_
#define _SYINX_LOG_H_
#include <cstdarg>
/************************************************************************/
/*
const char *file：文件名称
int line：文件行号
int level：错误级别
		0 -- 没有日志
		1 -- debug级别
		2 -- info级别
		3 -- warning级别
		4 -- err级别
int status：错误码
const char *fmt：可变参数
*/
/************************************************************************/
// 日志类


class SyinxLog
{
public:
	enum LogLevel {
		NOLOG,
		DEBUG,
		INFO,
		WARNING,
		LOGERROR,
		EVENT,
	};
	void Log(const char* file, int line, int level, int status, const char* fmt, ...);
	SyinxLog();
	~SyinxLog();
	static SyinxLog mLog;

private:
	int SYINX_Error_GetCurTime(char* strTime);
	int SYINX_Error_OpenFile(int* pf);
	void SYINX_Error_Core(const char* file, int line, int level, int status, const char* fmt, va_list args);

};

extern SyinxLog mLog;
#endif
