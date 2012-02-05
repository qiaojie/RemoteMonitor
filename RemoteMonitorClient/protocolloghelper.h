#pragma once

#include "Logger.h"

namespace Protocol
{
inline void LogData(Int64 val)
{
	LogAppend2("%I64d ", val);
}

inline void LogData(UInt64 val)
{
	LogAppend2("%I64u ", val);
}

inline void LogData(int val)
{
	LogAppend2("%d ", val);
}

inline void LogData(short val)
{
	LogAppend2("%d ", val);
}

inline void LogData(char val)
{
	LogAppend2("%d ", val);
}

inline void LogData(Byte val)
{
	LogAppend2("%u ", val);
}

inline void LogData(UInt16 val)
{
	LogAppend2("%u ", val);
}

inline void LogData(UInt32 val)
{
	LogAppend2("%u ", val);
}

inline void LogData(const string& val)
{
	LogAppend2("%s ", val.c_str());
}

inline void LogData(bool val)
{
	if(val)
		LogAppend("true ");
	else
		LogAppend("false ");
}

inline void LogData(float val)
{
	LogAppend2("%g ", val);
}

inline void LogData(double val)
{
	LogAppend2("%g ", val);
}

template<class T>
void LogArray(const T& arr)
{
	LogAppend("[ ");
	for(int i = 0; i < arr.size(); ++i)
		LogData(arr[i]);
	LogAppend("] ");
}

inline void LogData(DateTime val)
{
	LogAppend2("%d ", val.time);
}
}