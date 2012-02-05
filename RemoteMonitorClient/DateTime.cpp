#include "StdAfx.h"
#include "BaseType.h"
#include "BinaryStream.h"

Error DateTime::Read(BinaryStreamOut& stream, DateTime* data)
{
	return stream.GetValue(&data->time);	
}

void DateTime::Write(BinaryStreamIn& stream, const DateTime& data)
{
	stream.AddValue(data.time);
}
