// DiskNullJumper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Sector.h"
#include <winioctl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <conio.h>

long long sectorSearch(HANDLE diskHandle, unsigned long long startSector, unsigned long long totalSectors, int &readStatus, unsigned long long &finalSector)
{
	long long hit = -1;
	int progress = startSector * 100 / totalSectors;



	// Print first search report
	std::cout << "Initiating search. Press ESC to cancel." << std::endl;
	std::cout << "Searching... " << progress << "%\r";
	std::cout.flush();

	for (unsigned long long i = startSector; i < totalSectors; i++)
	{
		// Update progress
		if ((i * 100)/ totalSectors > progress)
		{
			progress++;
			std::cout << "Searching... " << progress << "%\r";
			std::cout.flush();
		}

		Sector currentSector(diskHandle, i);
		readStatus = currentSector.getReadStatus();
		finalSector = i;

		if (readStatus == Sector::SUCCESS)
		{
			if (!currentSector.isEntirelyNull())
			{
				hit = i;
				break;
			}
		}
		else
			break;

		if (_kbhit()) // Key press in buffer
			if (_getch() == 0x1B) // ESC key pressed
			{
				hit = -2;
				break;
			}
	}

	std::cout << std::endl << std::endl;
	return hit; // Return result
}

std::string padUIntWithChar(unsigned long long num, unsigned long long n, char c)
{
	std::string unformatedNum = std::to_string(num);
	std::string formattedNum;

	formattedNum = unformatedNum[unformatedNum.size() - 1];

	for (int i = unformatedNum.size() - 2; i > -1; i--)
	{
		if ((unformatedNum.size() - 1 - i) % n == 0)
			formattedNum = c + formattedNum;
		formattedNum = unformatedNum[i] + formattedNum;
	}

	return formattedNum;
}

int main()
{
	// Vars
	std::wstring diskString = L"\\\\.\\PhysicalDrive";
	int BYTES_PER_LINE = 16;

	bool getInput = false;
	bool quit = false;
	bool start = true;
	bool first = true;
	bool firstSearch = true;

	std::string userInput;
	unsigned int diskNum;
	HANDLE diskHandle = NULL;
	unsigned long long byteCount = 0;
	unsigned long long sectorCount = 0;
	unsigned long long startSector;
	unsigned long long hitSector;

	enum progStage { DriveSel, Search};
	progStage execStage = DriveSel;

	// Get console handle
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Disable mouse selection
	//DWORD prev_mode;
	//GetConsoleMode(consoleHandle, &prev_mode);
	//SetConsoleMode(consoleHandle, ENABLE_EXTENDED_FLAGS |
	//	(prev_mode & ~ENABLE_QUICK_EDIT_MODE));

	// Set double print precision
	std::cout << std::fixed;
	std::cout << std::setprecision(2);

	// Print opener
	std::cout << "Disk Null Jumper - 0.21" << std::endl;
	std::cout << "Chris H" << std::endl;
	std::cout << "MIT License, 2019" << std::endl << std::endl;

	// Print commands
	std::cout << "At any time the following commands are available:" << std::endl;
	std::cout << "change - Select a new disk to sweep" << std::endl;
	std::cout << "exit | quit - Quit the program" << std::endl << std::endl;

	// Start input loop
	while (!quit)
	{
		if (userInput == "" && !first)
		{
			first = true;
			getInput = true;
		}
		if (userInput == "change")
		{
			execStage = DriveSel;
			first = true;
			userInput = "";
		}
		else if (userInput == "exit" || userInput == "quit")
			quit = true;
		else if (std::all_of(userInput.begin(), userInput.end(), ::isdigit))
		{
			switch (execStage)
			{
			case DriveSel:
				if (first)
				{
					std::cout << "Enter the number of the disk to search:" << std::endl;
					getInput = true;
					first = false;
					start = false;
				}
				else
				{
					diskNum = std::stoi(userInput);
					std::wstring diskAccessToken = diskString + std::to_wstring(diskNum);
					diskHandle = CreateFile(diskAccessToken.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					if (diskHandle == INVALID_HANDLE_VALUE)
						std::cout << "Error: Could not open disk " << diskNum << ". Make sure the device exists and that this utility is running as administrator" << std::endl << std::endl;
					else
					{
						DISK_GEOMETRY_EX diskInfo;
						DWORD junk = 0;
						bool geoRead = DeviceIoControl(diskHandle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &diskInfo, sizeof(diskInfo), &junk, (LPOVERLAPPED) NULL);
						if (geoRead)
						{
							sectorCount = diskInfo.DiskSize.QuadPart / Sector::SECTOR_BYTE_SIZE;
							std::cout << "Disk " << diskNum << ": " << (diskInfo.DiskSize.QuadPart) / (1024.0 * 1024.0 * 1024.0) << " GB" << std::endl;
							std::cout << "Disk " << diskNum << ": " << padUIntWithChar(sectorCount, 3, ' ') << " Sectors (LBA)" << std::endl << std::endl;

							execStage = Search;
						}
						else
							std::cout << "Error: Could not obtain " << diskNum << " geometry" << std::endl << std::endl;
					}

					first = true;
				}

				break;
			case Search:
				if (first)
				{
					if (firstSearch)
						std::cout << "Enter the sector from which the search should begin:" << std::endl;
					else
						std::cout << "Enter the sector from which to continue the search:" << std::endl;
					
					getInput = true;
					first = false;
				}
				else
				{
					startSector = std::stoull(userInput);

					int readStatus;
					unsigned long long finalSector;

					if (startSector >= sectorCount)
						std::cout << "Error: Start sector " << padUIntWithChar(startSector, 3, ' ')  << " exceeds disk bounds (" << padUIntWithChar(sectorCount, 3, ' ') << ")" << std::endl << std::endl;
					else
					{
						hitSector = sectorSearch(diskHandle, startSector, sectorCount, readStatus, finalSector);

						if (hitSector == -1)
						{
							if (readStatus == Sector::SUCCESS)
							{
								std::cout << "End of disk reached" << std::endl << std::endl;
								firstSearch = true;
							}
							else if (readStatus == Sector::ERR_READ)
								std::cout << "Error reading sector " << padUIntWithChar(finalSector, 3, ' ') << std::endl << std::endl;
							else if (readStatus == Sector::ERR_READ_SIZE_MISMATCH)
								std::cout << "Size read mismatch at sector " << padUIntWithChar(finalSector, 3, ' ') << std::endl << std::endl;
						}
						else if (hitSector == -2)
						{
							std::cout << "ABORT: Search halted at sector " << padUIntWithChar(finalSector, 3, ' ') << std::endl << std::endl;
							firstSearch = true;
						}
						else
						{
							std::cout << "Sector " << padUIntWithChar(hitSector, 3, ' ') << " contains non-null data" << std::endl << std::endl;
							firstSearch = false;
						}
					}

					first = true;
				}
				break;
			}
		}
		else
		{
			std::cout << "Invalid input. Try again with numbers only, or enter a command:" << std::endl << std::endl;
			getInput = true;
		}
		
		// Get next input and ignore spaces
		if (getInput)
		{
			std::getline(std::cin, userInput);
			userInput.erase(remove_if(userInput.begin(), userInput.end(), isspace), userInput.end());
			std::cout << std::endl;
			getInput = false;
		}

	}

    return 0;
}

//std::cout << "Sector Contents:" << std::endl;

//for (int i = 0; i < Sector::SECTOR_BYTE_SIZE / BYTES_PER_LINE; i++)
//{
//	// Alter to display hex best
//	std::cout << std::hex << std::uppercase;

//	// Output byte chunk
//	for (int j = 0; j < BYTES_PER_LINE; j++)
//		std::cout << int(testSector.getSectorDataPtr()[(i*BYTES_PER_LINE) + j]) << " ";

//	// Go back to standard display
//	std::cout << std::dec << std::nouppercase;

//	// End line
//	std::cout << std::endl;
//}