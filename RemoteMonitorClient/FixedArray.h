#pragma once


template<class T, int SIZE>
class FixedArray
{
	char data[sizeof(T) * SIZE];
	int count;

	T* GetElement(int i)
	{
		return (T*)(data + sizeof(T) * i);
	}

	const T* GetElementConst(int i) const
	{
		return (T*)(data + sizeof(T) * i);
	}

public:
	FixedArray()
	{
		count = 0;
	}

	void Push(const T& e)
	{
		assert(count < SIZE);
		new(GetElement(count)) T(e);
		count++;
	}

	void Pop()
	{
		assert(count > 0);
		count--;
		data[count]->~T();
	}

	void Clear()
	{
		for(int i = 0; i < count; ++i)
			GetElement(i)->~T();
		count = 0;
	}

	T& operator[](int i)
	{
		assert(i >= 0 && i < count);
		return *GetElement(i);
	}

	const T& operator[](int i) const
	{
		assert(i >= 0 && i < count);
		return *GetElementConst(i);
	}

	int Count() const
	{
		return count;
	}

	bool IsFull() const
	{
		return (count == SIZE);
	}
};