#pragma once
#include "Error.h"
#include "Logger.h"

typedef unsigned char		Byte;
typedef unsigned short		UInt16;
typedef unsigned int		UInt32;
typedef unsigned __int64	UInt64;

typedef bool	Boolean;
typedef char	SByte;
typedef __int64	Int64;
typedef int		Int32;
typedef short	Int16;
typedef float	Single;
typedef double	Double;
typedef std::string	String;


struct DateTime
{
	time_t time;

	static Error Read(class BinaryStreamOut& stream, DateTime* data);
	static void Write(class BinaryStreamIn& stream, const DateTime& data);
};

#ifndef interface 
#define interface struct
#endif

struct ProtocolHead
{
	int ID;
	int Version;
};