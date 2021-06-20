#pragma once

#include <Windows.h>

class Mutex
{
private:
	HANDLE handle;

public:
	Mutex(LPCWSTR name);
	~Mutex();

	void Close();
	void Open();

};
