#include "flashfs.h"

bool ICACHE_FLASH_ATTR GetFirstFile( FlashFile *fileData, uint32 startAddress) {
	//even though we only want the first byte, we have to read 4 bytes at a time
	uint8 first_block[4];
	spi_flash_read(startAddress, (uint32*)first_block, 4);
	uint8 headerSize = first_block[0];
	if(headerSize == 0) return false;
	fileData->headerStart = startAddress;
	fileData->fileStart = startAddress + headerSize;

	//Copy header to buffer
	uint8 header[MAX_HEADER_SIZE];
	spi_flash_read(startAddress,(uint32*)header,headerSize);
	os_strncpy(fileData->name,(const char *)(header+1),127);
	fileData->name[127] = 0; //ensure it is terminated
	int next_byte_pos = PadTo4ByteAligned(os_strlen(fileData->name) + 2);

	fileData->fileLength = *((uint32*)(header + next_byte_pos));

	return true;
}

bool ICACHE_FLASH_ATTR GetNextFile( FlashFile *fileData) {
	uint32 nextStart = PadTo4ByteAligned(fileData->fileStart + fileData->fileLength);
	return GetFirstFile(fileData, nextStart);
}

bool ICACHE_FLASH_ATTR FindFile(const char *filename, FlashFile* fileData, uint32 startAddress) {
	bool status = GetFirstFile(fileData, startAddress);
	while(status) {
		os_printf("Found %128s, looking for %s.\n",fileData->name,filename);
		if(os_strncmp(fileData->name,filename,128) == 0) {
			return true;
		}
		status = GetNextFile(fileData);
	}
	return false;
}

bool ICACHE_FLASH_ATTR ReadFromFile(FlashFile *file, uint32 offset, uint32 length, char *buffer) {
	spi_flash_read(file->fileStart + offset,(uint32*)buffer, length);
	return true;
}


uint32 ICACHE_FLASH_ATTR PadTo4ByteAligned(uint32 val) {
	uint32 result = val;
	switch(val % 4) {
	case 1:
		result += 3;
		break;
	case 2:
		result += 2;
		break;
	case 3:
		result += 1;
		break;
	}
	return result;
}
