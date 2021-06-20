#include "Mutex.h"

Mutex::Mutex(LPCWSTR name)
{
	this->handle = CreateMutex(nullptr, false, name);

}

Mutex::~Mutex()
{
}

void Mutex::Close()
{
	WaitForSingleObject(this->handle, INFINITE);
}

void Mutex::Open()
{
	ReleaseMutex(this->handle);
}
