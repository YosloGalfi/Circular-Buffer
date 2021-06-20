#pragma once

#include <string>
#include "Memory.h"
#include "Mutex.h"

using namespace std;

enum TYPE { PRODUCER, CONSUMER };

class Comlib
{
private:

	// Header for sending information about next message
	struct MessageHeader
	{
		size_t messageID;
		size_t messageLength;
		size_t padding;
	};

	// Buffer for message and control
	char* messageData;

	// Pointers for storing head, tail and free memory
	size_t* head;
	size_t* tail;
	size_t* freeMemory;

	// Shared memory
	Memory* sharedMemory;

	// Mutex object
	Mutex* myMutex;

public:
	Comlib(LPCWSTR buffername, size_t& bufferSize, TYPE type, size_t& chunkSize);
	~Comlib();

	bool Send(const char* message, size_t messageLength, size_t chunkSize);
	bool Recieve(char* message, size_t& length);

};