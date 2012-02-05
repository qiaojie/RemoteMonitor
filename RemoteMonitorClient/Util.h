#pragma once
#include "BaseType.h"
#include "FixedArray.h"

template<class T>
inline void XChange(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}
wstring FromUtf8(LPCSTR str);
string ToUtf8(LPCWSTR str);

inline string FormatIP(DWORD ip)
{
	char buf[256];
	sprintf_s(buf, 256, "%d.%d.%d.%d", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, ip >> 24);
	return string(buf);
}

void parse_cmdline(char *cmdstart, char **argv, char *args, int *numargs, int *numchars);
void SetThreadName(DWORD dwThreadID, char* threadName);
Int64 GetTime();

template<int Size>
struct ByteStream
{
	BYTE buf[Size];
	int  index;

	ByteStream()
	{
		index = 0;
	}

	int FreeSize()
	{
		return Size - index;
	}

	Error Write(void* data, int size)
	{
		if(index + size > Size)
			return E_OutOfMemory;

		memcpy(buf + index, data, size);
		index += size;

		return E_OK;
	}

	void UncheckWrite(void* data, int size)
	{
		memcpy(buf + index, data, size);
		index += size;
	}

	void Clear()
	{
		index = 0;
	}
};
