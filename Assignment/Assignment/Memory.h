#pragma once
#include <Windows.h>
#include <iostream>

class Memory
{
private:
	// Buffer for message and control
	char* messageData;
	size_t* controlData;

	// Buffer size
	size_t size;

	// Handles for filemap and controller
	HANDLE filemap;
	HANDLE controlFilemap;

public:
	Memory(LPCWSTR name, size_t size);
	~Memory();

	void InitializeFilemap(LPCWSTR name);
	void InitializeFileview();

	char* GetMemoryBuffer();
	size_t* GetControlBuffer();
	size_t GetBufferSize();

};