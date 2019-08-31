
# DiskNullJumper
*A Windows command-line utility for searching out non-null sectors*

## Purpose
I developed this simple tool due to the astonishing lack of a "jump to next non-null sector" in every hex/disk editor I could find. This tool will allow you to easily ensure you have seen every byte on a drive if you use it from start to finish, or will help will finding a pesky single sector data chunk that sits between a large number of null sectors if you aren't sure of its exact position.

Given how much data can be stored on a drive and taking into account that quick formats do not zero a drive, this tool may not seem that useful; however, I personally have found it to be incredibly useful when studying the meta-data of drives that have been fully formatted or when trying to find small bits of RAW data on lightly used disks.

At the moment this tool only works on Windows (should work on Vista and up) and is not fully optimized nor polished (error messages/conditions aren't as verbose as they could be, search speed could be slightly improved, etc.), but it works very well and has no functional issues as far as I can tell. The implementation is quite basic and a tad crude, but I needed the functionality this tool offers posthaste and given that it is a rather simple utility the source didn't need to be particularly organized. 

## Usage
This tool is not argument based. Simply launch the executable from Windows Explorer, a Command Prompt, the Run dialog, etc. In order to work whatsoever the program must be run as an administrator.

You select which drive you want to search using numbers 0-(n-1) where **n** is the number of physical disks installed in your system, upon which the program will return basic information about the disk so that you can be sure you have selected the correct drive. Then enter at which LBA sector you wish to begin the search as an integer value (spaces are ignored). The search will then commence and update you on its progress as it passes each 1% of the total drive capacity. Note that this can take quite some time for larger disks (i.e. a couple to several hours for disks that are 1TB and up) as it has to search the drive byte-by-byte. Once the tool finds a hit, a sector that contains any non-null data, the search will cease and the console will alert you of which sector contained said data. You can then enter a new sector offset to begin the search again from, including sectors that lie before your previous search. If the search reaches the end of the disk without finding any non-null data after the start position provided the search will end and the program will inform you that this has occurred.

![Usage Example 1](https://i.imgur.com/HRDSCY2.jpg)

![Usage Example 2](https://i.imgur.com/xxX4oai.jpg)

In addition to standard usage via the on-screen prompts, the following commands are available:
"change" - Re-select which drive to search
"quit/exit" - Exit the program
ESC (Key) - Halt an in-progress search. The program will let you know what sector the search stopped at.

## Releases
The provided release was compiled using MSVC++ in Visual Studio 2019 and the Windows 10 SDK

## License
I distribute this software and source under the MIT licence (see licence file). There is no guaranteed support for this software but feel free to improve upon it, use it for research, post any issues you find, ask a question, or use it however you see fit.
