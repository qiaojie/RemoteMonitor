#pragma once
#include "Error.h"

class BinaryStreamOut
{
	BYTE* _data;
	int _index;
	int _size;

	Error GetValue(void* data, int size)
	{
		VALIDATE(_index + size > _size, E_StreamOverflow);

		memcpy(data, _data + _index, size);
		_index += size;

		return E_OK;
	}

public:
	BinaryStreamOut(BYTE* data, int size)
		: _data(data), _size(size), _index(0)
	{
	}

	void Reset()
	{
		_index = 0;
	}

	BYTE* Buffer()
	{
		return _data;
	}

	int Position()
	{
		return _index;
	}

	Error SetPosition(int newPos)
	{
		VALIDATE(_index > _size, E_StreamOverflow);
		_index = newPos;
		return E_OK;
	}

	template<class T>
	Error GetValue(T* val)
	{
		CHECK(GetValue(val, sizeof(T)));
		return E_OK;
	}

	Error GetValue(string* val)
	{
		int size;
		CHECK(GetValue(&size));

		VALIDATE(_index + size > _size, E_StreamOverflow);

		val->clear();
		val->assign((const char*)(_data + _index), size);
		_index += size;
		return E_OK;
	}
};



class BinaryStreamIn
{
	BYTE* _data;
	int _index;
	int _capacity;
	BYTE _buffer[256];

	void CheckCapacity(int size)
	{
		if(_index + size > _capacity)
		{
			_capacity = (_capacity * 2 + size);
			if(_data == _buffer)
			{
				_data = (BYTE*)malloc(_capacity);
				memcpy(_data, _buffer, GetSize());
			}
			else
			{
				BYTE* newData = (BYTE*)realloc(_data, _capacity);
				if(newData == 0)
				{
					newData = (BYTE*)malloc(_capacity);
					memcpy(newData, _data, GetSize());
					free(_data);
				}
				_data = newData;
			}
		}
	}

	void AddValue(const void* data, int size)
	{
		CheckCapacity(size);
		memcpy(_data + _index, data, size);
		_index += size;
	}

public:
	BinaryStreamIn()
	{
		_capacity = sizeof(_buffer);
		_data = _buffer;//(BYTE*)malloc(_capacity);
		_index = 0;
	}

	~BinaryStreamIn()
	{
		if(_data != _buffer)
			free(_data);
	}

	BYTE* Detach()
	{
		if(_data == _buffer)
		{
			BYTE* data = (BYTE*)malloc(GetSize());
			memcpy(data, _data, GetSize());
			return data;
		}
		else
		{
			BYTE* data = _data;
			_data = _buffer;
			_capacity = sizeof(_buffer);
			return data;
		}
	}

	void Reset()
	{
		_index = 0;
	}

	int GetSize()
	{
		return _index;
	}

	BYTE* GetBuffer()
	{
		return _data;
	}

	template<class T>
	void AddValue(const T& val)
	{
		AddValue(&val, sizeof(T));
	}

	void AddValue(const string& val)
	{
		int size = val.size();
		AddValue(size);
		CheckCapacity(size);
		memcpy(_data + _index, val.c_str(), size);
		_index += size;
	}
};

