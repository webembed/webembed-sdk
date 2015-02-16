/*
 * Part of the WebEmbed project
 * Simple flash-based filesystem
 * See LICENSE for details
 */

/*
 * This is basically the simplest possible, readonly filesystem for a SPI flash device
 * It is only really useful for simple situations will small numbers of files.
 * It is effectively organised as a series of files stored sequentially
 * At the start of each file, there are the following
 * 1 byte: header length
 * nullterminated string: file path and name
 * followed by the correct number of 0s to ensure start of file is 4-byte aligned
 * uint32: file length
 * then the file data, then enough bytes so that the next header is 4-byte aligned, then the next file header
 * A header length of zero means end of fs
 */

#ifndef FLASHFS_H
#define FLASHFS_H

#include <c_types.h>

extern "C" {
#include <ets_sys.h>
#include <osapi.h>
#include <spi_flash.h>
}

#include <misc_includes.h>


#define DEFAULT_START 0x20000

#define MAX_HEADER_SIZE 256

struct FlashFile {
	uint32 headerStart; //first byte of header
	uint32 fileStart; //first byte of file data
	uint32 fileLength; //size of file
	char name[128]; //file name and path
};

bool GetFirstFile( FlashFile *fileData, uint32 startAddress = DEFAULT_START); //Sets FileData to the data of the first file [false=fail, true = OK]
bool GetNextFile(FlashFile* fileData); //Sets FileData to data of next file relative to original FileData [false=end of fs, true = OK]

bool FindFile(const char *filename, FlashFile* fileData, uint32 startAddress = DEFAULT_START); //If a file is found, copy data into FileData, otherwise returns false

//Offset and Length --MUST-- be 4-byte aligned at the moment
bool ReadFromFile(FlashFile *file, uint32 offset, uint32 length, char *buffer); //Reads a number of bytes from a file into a buffer. false=fail, true=ok

//Compensates for padding to make value 4-byte aligned
uint32 PadTo4ByteAligned(uint32 val);

#endif
