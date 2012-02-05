#pragma once

#include <string>
using namespace std;

#pragma warning(error: 4715)
#pragma warning(disable:4018)

enum ErrorCode
{
	E_OK = 0,
	E_NotImpl,
	E_Fail,
	E_UnknownMethod,
	E_WIN32APIError,
	E_SocketError,
	E_SocketClosed,
	E_StreamOverflow,
	E_TickTimeout,
	E_PacketIncomplete,
	E_ProtocolNotMatch,
	E_OutOfMemory
};

class Error
{
	struct CallStack
	{
		LPCSTR func;
		int line;
		CallStack* next;
		
		static CallStack pool[4096];
		static DWORD index;

		static void* operator new(size_t size)
		{
			DWORD i = ::InterlockedIncrement((LONG*)&index);
			return pool + (i % 4096);
		}

		static void operator delete(void* object)
		{
		}
	};

	ErrorCode errCode;
	CallStack* stack;
	bool checked;
	static void LogCallStack(CallStack* s);
	
public:
	Error();
	Error(ErrorCode error);
	Error(ErrorCode error, LPCSTR func, int lineNo);
	~Error();
	Error(const Error& e);
	ErrorCode Check();

	Error& operator = (const Error& e);

	void PushCall(LPCSTR func, int line);
	LPCSTR ErrorMessage();
	void LogCallStack();
};

string FormatAPIError(DWORD err);
LPCSTR FormatErrorCode(DWORD err);

#define PUSHCALL(e) e.PushCall(__FUNCSIG__, __LINE__);
#define THROW(x) return Error(x, __FUNCSIG__, __LINE__);
#define CHECK(x) {Error e = x; if(e.Check()){e.PushCall(__FUNCSIG__, __LINE__); return e;}}

#define VALIDATE(x, e) if(x){ return Error(e, __FUNCSIG__, __LINE__); }
