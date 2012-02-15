/**\file Error.cpp
 * \brief system error handling
 */
#include "stdafx.h"
#include "Error.h"
#include "Logger.h"
#include "Util.h"

Error::CallStack Error::CallStack::pool[4096];
DWORD Error::CallStack::index = 0;

Error::Error()
{
	errCode = E_OK;
	checked = false;
	stack = 0;
}

Error::Error(ErrorCode error)
{
	errCode = error;
	checked = false;
	stack = 0;
}

Error::~Error()
{
	if(checked)
		return;
	if(errCode == E_OK)
		LogWarning("the error code is not checked!");
	else
	{
		LogCallStack();
		LogError2("the error code is not checked! error code: %d", errCode);
	}
}

Error::Error(const Error& e)
{
	stack = e.stack;
	errCode = const_cast<Error&>(e).Check();
	checked = false;
}

Error& Error::operator = (const Error& e)
{
	stack = e.stack;
	errCode = const_cast<Error&>(e).Check();
	checked = false;

	return *this;
}

ErrorCode Error::Check()
{
	checked = true;
	return errCode;
}

Error::Error(ErrorCode error, LPCSTR func, int lineNo)
{
	errCode = error;
	checked = false;
	stack = new CallStack();
	stack->func = func;
	stack->line = lineNo;
	stack->next = 0;
}

void Error::PushCall(LPCSTR func, int line)
{
	CallStack* c = new CallStack();
	c->func = func;
	c->line = line;
	c->next = stack;
	stack = c;
}

LPCSTR FormatErrorCode(DWORD err)
{
	static LPCSTR errMsg[] = {
		"OK",											//E_OK = 0,
		"the method is not implemented.",				//E_NotImpl,
		"general failed.",								//E_Fail,
		"unknown method called.",						//E_UnknownMethod,
		"win32 api error.",								//E_WIN32APIError,
		"socket error.",								//E_SocketError,
		"the remote socket is closed.",					//E_SocketClosed,
		"the stream is overflow.",						//E_StreamOverflow,
		"client tick timeout.",							//E_TickTimeout,
		"the packet is incomplete.",					//E_PacketIncomplete,
		"the protocol is not match.",					//E_ProtocolNotMatch
		"out of memory"									//E_OutOfMemory
	};

	return errMsg[err];
}

LPCSTR Error::ErrorMessage()
{
	return FormatErrorCode(errCode);
}

void Error::LogCallStack(CallStack* s)
{
	if(!s)
		return;
	LogCallStack(s->next);
	LogAppend2("	%s line %d\n", s->func, s->line);
}

void Error::LogCallStack()
{
	LogAppend("Call Stack:\n");
	LogCallStack(stack);
}

string FormatAPIError(DWORD err)
{
	WCHAR* msgBuf;
	::FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		0, err, 0, (LPWSTR)&msgBuf, 0, 0);
	string msg = ToUtf8(msgBuf);
	LocalFree(msgBuf);
	return msg;
}