/**\file Logger.cpp
 * \brief the Log module
 */
#include "stdafx.h"
#include "Error.h"
#include "Logger.h"

enum
{
	Info =		1 << Log_Info,
	Debug =		1 << Log_Debug,
	Warning =	1 << Log_Warning,
	Error =		1 << Log_Error,
	Fatal =		1 << Log_Fatal
};

int LogFlag = Info | Debug | Warning | Error | Fatal;
DWORD LogTlsIndex = 1;
//LogCategory lastLog;

FILE* logFile = stdout;
/*
static void output(const char * _Format)
{
	//fprintf(logFile, _Format);
}

static void output(const char* _Format, va_list _ArgList)
{
	//vfprintf(logFile, _Format, _ArgList);
	//fflush(logFile);
}
*/
struct InitLog
{
	InitLog()
	{
		LogTlsIndex = ::TlsAlloc();
	}
};

static InitLog _initLog;

static void output(LogCategory category, const char* content)
{
	switch(category)
	{
		case Log_Info:		fprintf(logFile, "[Info] %s\n", content); break;
		case Log_Debug:		fprintf(logFile, "[Debug] %s\n", content); break;
		case Log_Error:		fprintf(logFile, "[Error] %s\n", content); break;
		case Log_Warning:	fprintf(logFile, "[Warning] %s\n", content); break;
		case Log_Fatal:		fprintf(logFile, "[Fatal] %s\n", content); break;
	}
	fflush(logFile);
}

struct LogData
{
	LogCategory category;
	SYSTEMTIME time;
	string content;
	ILogEventHandler* logEventHandler;
};

void SetLogEventHandler(ILogEventHandler* handler)
{
	LogData* data = (LogData*)TlsGetValue(LogTlsIndex);
	if(data == 0)
	{
		data = new LogData();
		TlsSetValue(LogTlsIndex, data);
		data->category = Log_Info;
	}
	data->logEventHandler = handler;
}

static void _LogAppend(LPCSTR content)
{
	LogData* data = (LogData*)TlsGetValue(LogTlsIndex);
	data->content.append(content);
}

static void Log(LogCategory category, LPCSTR content)
{
	LogData* data = (LogData*)TlsGetValue(LogTlsIndex);
	if(data == 0)
	{
		data = new LogData();
		TlsSetValue(LogTlsIndex, data);
		data->category = category;
		::GetSystemTime(&data->time);
		if(content)
			data->content = content;
		data->logEventHandler = 0;
		return;
	}

	if(data->logEventHandler)
	{
		if(!data->logEventHandler->OnLog(data->category, data->time, data->content))
		{
			if(!content)
				data->content.clear();
			else
			{
				data->category = category;
				::GetSystemTime(&data->time);
				data->content = content;
			}
			return;
		}
	}
	if(data->content.size() > 0)
		output(data->category, data->content.c_str());

	if(!content)
		data->content.clear();
	else
	{
		data->category = category;
		::GetSystemTime(&data->time);
		data->content = content;
	}
}

void LogEnable(LogCategory log, bool enable)
{
	if(enable)
		LogFlag |= 1 << log;
	else
		LogFlag &= ~(1 << log);
}

void LogInfo(LPCSTR content)
{
	Log(Log_Info, content);
}

void LogDebug(LPCSTR content)
{
	Log(Log_Debug, content);
}

void LogError(LPCSTR content)
{
	Log(Log_Error, content);
}

void LogWarning(LPCSTR content)
{
	Log(Log_Warning, content);
}

void LogFatal(LPCSTR content)
{
	Log(Log_Fatal, content);
}

void LogAppend(LPCSTR content)
{
	_LogAppend(content);
}

void LogInfo2(LPCSTR format, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	Log(Log_Info, buf);
}

void LogDebug2(LPCSTR format, ...)
{
#ifdef _DEBUG
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	Log(Log_Debug, buf);
#endif
}

void LogError2(LPCSTR format, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	Log(Log_Error, buf);
}

void LogWarning2(LPCSTR format, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	Log(Log_Warning, buf);
}

void LogFatal2(LPCSTR format, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	Log(Log_Fatal, buf);
}

void LogAppend2(LPCSTR format, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buf, sizeof(buf), format, args);
	va_end(args);
	_LogAppend(buf);
}

void LogFlush()
{
	Log(Log_Info, 0);
}

void LogEnable(LogCategory log, bool enable);