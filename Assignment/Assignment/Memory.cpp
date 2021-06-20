#include "Memory.h"

Memory::Memory(LPCWSTR name, size_t size)
{
    this->size = size;

    InitializeFilemap(name);
    InitializeFileview();
}

Memory::~Memory()
{
    // Unmap and release the filemap and views
    UnmapViewOfFile(messageData);
    UnmapViewOfFile(controlData);
    CloseHandle(controlFilemap);
    CloseHandle(filemap);
}

void Memory::InitializeFilemap(LPCWSTR name)
{
    // File map
    filemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, this->size, name);
    if (!filemap)
    {
        std::cout << "Couldnt create filemap" << std::endl;
    }

    // Filemap for control of head and tail
    controlFilemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, this->size, L"controlmap");
    if (!controlFilemap)
    {
        std::cout << "Couldnt open control filemap" << std::endl;
    }
}

void Memory::InitializeFileview()
{
    // File view
    messageData = (char*)MapViewOfFile(filemap, FILE_MAP_ALL_ACCESS, 0, 0,this->size);
    if (!messageData)
    {
        std::cout << "Couldnt open view of file" << std::endl;
    }

    // Fileview for control of head and view
    controlData = (size_t*)MapViewOfFile(controlFilemap, FILE_MAP_ALL_ACCESS, 0, 0, this->size);
    if (!controlData)
    {
        std::cout << "Couldnt open control view" << std::endl;
    }
}

char* Memory::GetMemoryBuffer()
{
    return messageData;
}

size_t* Memory::GetControlBuffer()
{
    return controlData;
}

size_t Memory::GetBufferSize()
{
    return size;
}
