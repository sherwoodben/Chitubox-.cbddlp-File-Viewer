#pragma once

#include "ChituDataBlock.h"

struct ChituPreviewImageHeader : public ChituDataBlock
{
	//offsets to data within the
	//preview image header block for .cbddlp file type
	enum DataOffsets
	{
		IMAGE_WIDTH = 0x00000000,
		IMAGE_HEIGHT = 0x00000004,
		IMAGE_DATA_ADDRESS = 0x00000008,
		IMAGE_DATA_SIZE = 0x0000000C
	};

	//constructor calls the Init function
	ChituPreviewImageHeader(std::FILE* readFrom, long int offset, long int bytesToRead)
		: ChituDataBlock(readFrom, offset, bytesToRead)
	{
		InitImageHeader();
	}

	//sets up all the variables associated with the preview image headers
	void InitImageHeader()
	{
		RegisterData(new ChituInt(rawData, IMAGE_WIDTH, "Image Width (px)"), "IMAGE_X_PX");
		RegisterData(new ChituInt(rawData, IMAGE_HEIGHT, "Image Height (px)"), "IMAGE_Y_PX");
		RegisterData(new ChituInt(rawData, IMAGE_DATA_ADDRESS, "Image Data Address"), "IMAGE_ADDRESS");
		RegisterData(new ChituInt(rawData, IMAGE_DATA_SIZE, "Image Size (bytes)"), "IMAGE_SIZE");
	}

	//extra functions that make getting some of the values easier (so we don't need to remember
	//key words all of the time. Note, we still can access value by key if we want!)
	long int GetWidth() { return GetValueByKey<long int>("IMAGE_X_PX"); }
	long int GetHeight() { return GetValueByKey<long int>("IMAGE_Y_PX"); }
	long int GetAddress() { return GetValueByKey<long int>("IMAGE_ADDRESS"); }
	long int GetSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
};