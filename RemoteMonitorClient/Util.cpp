/**\file Util.cpp
 * \brief Utility functions
 */
#include "stdafx.h"
#include "Util.h"

/*
size_t Utf8ToWideString(LPCSTR utf8, WCHAR* utf16, int sz)
{
	while(((BYTE)*utf8 & 0xc0) == 0x80)
		++utf8;
	
	if(sz == 0)
	{
		int n = 0;
		while(*utf8)
		{
			signed char c=*utf8;
			if((c & 0xc0)!=0x80)
				++n;
			++utf8;
		}
		return n;
	}
	if (*utf8 == '\0')
	{
		utf16[0] = 0;
		return 0;
	}
	WCHAR* ptr = utf16 - 1;
	while(*utf8)
	{
		char c = *utf8;
		if((c & 0xc0) == 0x80)
			*ptr = *ptr << 6 | (c & 0x3f);
		else
		{
			++ptr;
			if (ptr - utf16 >= sz)
			{
				*(ptr - 1) = 0;
				return sz - 1;
			}
			*ptr = c & (0x0f | (~(c >> 1) & 0x1f) | ~(c >> 7));
		}
		++utf8;
	}
	if(ptr - utf16 < sz)
		++ptr;
	*ptr = 0;
	return ptr - utf16;
}

size_t Utf8FromWideString(LPCWSTR utf16, char* utf8, int sz)
{
	BYTE* ptr = (BYTE*)utf8;
	if(sz < 3)
	{
		while(*utf16)
		{
			WCHAR c = *utf16;
			if(c > 0x7F)
			{
				if(c > 0x7ff)
					++ptr;
				++ptr;
			}
			++ptr;
			++utf16;
		}
		return (char*)ptr - utf8;
	}
	sz -= 2;
	BYTE* last;
	while(*utf16)
	{
		WCHAR c = *utf16;
		last = ptr;
		if(c <= 0x7F)
			*ptr = (BYTE)c;
		else
		{
			if(c <= 0x7ff)
				*ptr = (BYTE)((c >> 6) | 0xc0);
			else
			{
				*ptr = (BYTE)((c >> 12) | 0xe0);
				++ptr;
				*ptr = (BYTE)(((c >> 6) & 0x3f) | 0x80);
			}
			++ptr;
			*ptr = (BYTE)((c&0x3f) | 0x80);
		}
		++ptr;
		if((char*)ptr - utf8 >= sz + 2)
		{
			ptr = last;
			break;
		}
		++utf16;
	}
	*ptr = 0;
	return (char*)ptr - utf8;
}*/

wstring FromUtf8(LPCSTR utf8)
{
	while(((BYTE)*utf8 & 0xc0) == 0x80)
		++utf8;

	wstring str;
	WCHAR ch = 0;
	while(*utf8)
	{
		char c = *utf8;
		if((c & 0xc0) == 0x80)
			ch = (ch << 6) | (c & 0x3f);
		else
		{
			if(ch != 0)
				str.append(1, ch);
			ch = c & (0x0f | (~(c >> 1) & 0x1f) | ~(c >> 7));
		}
		++utf8;
	}
	if(ch != 0)
		str.append(1, ch);
	return str;
}

string ToUtf8(LPCWSTR utf16)
{
	string str;
	while(*utf16)
	{
		WCHAR c = *utf16;
		if(c <= 0x7f)
			str.append(1, c);
		else
		{
			if(c <= 0x7ff)
				str.append(1, (BYTE)((c >> 6) | 0xc0));
			else
			{
				str.append(1, (BYTE)((c >> 12) | 0xe0));
				str.append(1, (BYTE)(((c >> 6) & 0x3f) | 0x80));
			}
			str.append(1, (BYTE)((c & 0x3f) | 0x80));
		}
		++utf16;
	}
	return str;
}
/*
wstring FromUtf8(LPCSTR str)
{
	int slen = ::strlen(str);
	WCHAR buf[65536];
	int len = ::MultiByteToWideChar(CP_UTF8, 0, str, slen, buf, 65536);
	buf[len] = 0;
	return wstring(buf);
}

string ToUtf8(LPCWSTR str)
{
	int slen = ::wcslen(str);
	char buf[65536];
	int len = ::WideCharToMultiByte(CP_UTF8, 0, str, slen, buf, 65536, 0, 0);
	buf[len] = 0;
	return string(buf);
}*/