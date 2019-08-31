#pragma once
#include <cstdint>
#include <Windows.h>

class Sector
{
public:
	static const unsigned long long SECTOR_BYTE_SIZE = 512;
	static const int SUCCESS = 0;
	static const int ERR_READ = 1;
	static const int ERR_READ_SIZE_MISMATCH = 2;

private:
	DWORD64 mSectorNum;
	BYTE mSectorData [SECTOR_BYTE_SIZE];
	int mReadStatus;
	int mReadBytes;

public:
	Sector(HANDLE deviceHandle, unsigned long long sectorNum);
	~Sector();

	DWORD64 getSectorNumber();
	BYTE* getSectorDataPtr();
	int getReadStatus();
	int getReadBytes();
	bool isEntirelyNull();
};

