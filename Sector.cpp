#include "stdafx.h"
#include "Sector.h"


Sector::Sector(HANDLE deviceHandle, unsigned long long sectorNum)
{
	mReadStatus = 0;
	DWORD bytesRead;
	unsigned long long pointerOffset = sectorNum * SECTOR_BYTE_SIZE;
	LARGE_INTEGER filePointer;
	filePointer.QuadPart = pointerOffset;
	SetFilePointerEx(deviceHandle, filePointer, NULL, FILE_BEGIN);

	if (!ReadFile(deviceHandle, mSectorData, SECTOR_BYTE_SIZE, &bytesRead, NULL))
		mReadStatus = ERR_READ;
	else if (bytesRead != SECTOR_BYTE_SIZE)
		mReadStatus = ERR_READ_SIZE_MISMATCH;

	mReadBytes = bytesRead;
}

Sector::~Sector()
{
}

uint64_t Sector::getSectorNumber() { return mSectorNum; }
BYTE* Sector::getSectorDataPtr() { return mSectorData; }
int Sector::getReadStatus() { return mReadStatus; }
int Sector::getReadBytes() { return mReadBytes; }
bool Sector::isEntirelyNull()
{
	for (int i = 0; i < SECTOR_BYTE_SIZE; i++)
		if (int(mSectorData[i]) != 0)
			return false;

	return true;
}
