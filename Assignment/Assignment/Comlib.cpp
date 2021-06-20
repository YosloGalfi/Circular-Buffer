#include "Comlib.h"

Comlib::Comlib(LPCWSTR buffername, size_t& bufferSize, TYPE type, size_t& chunkSize)
{
    this->myMutex = new Mutex(L"myMutex");

    if (type == PRODUCER)
    {
        Sleep(10);
    }

    if (type == CONSUMER)
    {
        myMutex->Close();
    }

    this->sharedMemory = new Memory(buffername, bufferSize);

    messageData = sharedMemory->GetMemoryBuffer();

    // Set the memory adress of head to the same as the fileview for the control buffer
    // Head will point its adress to the start of the buffer
    // While tail will be 1 step ahead
    // And the amount of memory between them will be next in the buffer
    // This way, we wont need to use memcpy
    head = sharedMemory->GetControlBuffer();
    tail = head + 1;
    freeMemory = tail + 1;

    // Set the start of head and tail to 0
    // And the free memory to the buffer size
    *head = 0;
    *tail = 0;
    *freeMemory = bufferSize;

    myMutex->Open();
}

Comlib::~Comlib()
{
    // Deallocate
    if (sharedMemory)
    {
        delete sharedMemory;
    }
    
    if (myMutex)
    {
        delete myMutex;
    }
}

bool Comlib::Send(const char* message, size_t messageLength, size_t chunkSize)
{
    size_t padding = chunkSize - ((messageLength + sizeof(MessageHeader)) % chunkSize);
    size_t remainingMemory = sharedMemory->GetBufferSize() - *head;

    myMutex->Close();

    // Next message is to big
    if ((messageLength + sizeof(MessageHeader) + padding) >= remainingMemory)
    {
        // Make sure that the consumer isnt at position 0
        if (*tail != 0)
        {
            // Print a header with ID 0, making sure the consumer knows when to restart
            MessageHeader header;
            header.messageLength = remainingMemory - sizeof(MessageHeader);
            header.messageID = 0;
            header.padding = 0;

            memcpy(messageData + *head, &header, sizeof(MessageHeader));

            // Subtract the memory and restart head
            *freeMemory -= (header.messageLength + sizeof(MessageHeader) + header.padding);
            *head = 0;

            myMutex->Open();
            return false;
        }
        else // If the consumer is at 0
        {
            myMutex->Open();
            return false;
        }
    }

    // Next message will fit
    else if ((messageLength + sizeof(MessageHeader) + padding) < (*freeMemory - 1))
    {
        MessageHeader header;
        header.messageID = 1;
        header.messageLength = messageLength;
        header.padding = padding;

        // Print the header with appropriate data and the message
        memcpy(messageData + *head, &header, sizeof(MessageHeader));
        memcpy(messageData + *head + sizeof(MessageHeader), message, header.messageLength);

        // Subtract the free memory and place the head and current position
        *freeMemory -= (header.messageLength + sizeof(MessageHeader) + header.padding);
        myMutex->Open();
        *head = (*head + header.messageLength + sizeof(MessageHeader) + header.padding) % sharedMemory->GetBufferSize();

        return true;
    }
    else // Will most likely never happend
    {
        myMutex->Open();
        return false;
    }
}

bool Comlib::Recieve(char* message, size_t& length)
{

    myMutex->Close();

    // Make sure that we are within the buffersize and not printing to scrap value or at the end of the buffer
    if (*freeMemory < sharedMemory->GetBufferSize())
    {
        // Make sure that the producer and consumer isnt on the same place
        if (*head != *tail)
        {
            // Read the next header
            MessageHeader* header = ((MessageHeader*)&messageData[*tail]);
            length = header->messageLength;

            // If the ID is 0, place tail at 0 and restart
            if (header->messageID == 0)
            {
                *freeMemory += (length + sizeof(MessageHeader) + header->padding);
                *tail = 0;

                myMutex->Open();
                return false;
            }
            else
            {
                // Read the next message
                memcpy(message, &messageData[*tail + sizeof(MessageHeader)], length);

                // Replace tail and add to the memory
                *tail = (*tail + (length + sizeof(MessageHeader) + header->padding)) % sharedMemory->GetBufferSize();
                *freeMemory += (length + sizeof(MessageHeader) + header->padding);

                myMutex->Open();
                return true;
            }
        }

        else // If they are on the same position
        {
            myMutex->Open();
            return false;
        }
    }
    else // If we are outside of the buffersize, could happend if consumer reads scrap value
    {
        myMutex->Open();
        return false;
    }

}
