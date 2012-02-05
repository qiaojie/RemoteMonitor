#pragma once

enum LogCategory
{
	Log_Info,
	Log_Debug,
	Log_Warning,
	Log_Error,
	Log_Fatal
};

struct ILogEventHandler
{
	virtual ~ILogEventHandler(){}
	virtual bool OnLog(LogCategory level, const SYSTEMTIME& time, const string& content) = 0;
};

void LogInfo2(LPCSTR format, ...);
void LogDebug2(LPCSTR format, ...);
void LogError2(LPCSTR format, ...);
void LogWarning2(LPCSTR format, ...);
void LogFatal2(LPCSTR format, ...);
void LogAppend2(LPCSTR format, ...);

void LogInfo(LPCSTR content);
void LogDebug(LPCSTR content);
void LogError(LPCSTR content);
void LogWarning(LPCSTR content);
void LogFatal(LPCSTR content);
void LogAppend(LPCSTR content);

void LogEnable(LogCategory log, bool enable);
void LogFlush();
void SetLogEventHandler(ILogEventHandler* handler);