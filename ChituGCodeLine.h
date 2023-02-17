#pragma once

#include "ChituDataBlock.h"

struct ChituGCodeLine : public ChituDataBlock
{
	//offsets to data within the
	//g-code line block for .cbddlp file type
	enum DataOffsets
	{
		LAYER_Z = 0x0000,
		EXPOSURE_TIME = 0x0004,
		LIGHT_OFF_DELAY = 0x0008,
		POINTER_TO_IMAGE = 0x000C,
		IMAGE_SIZE = 0x0010,
		SIZE_OF_CODE_BEFORE_IMAGE = 0x0018
	};

	//constructor, takes a char array since we've already read the
	//data into the larger "g-code" object. Also calls the Init function
	ChituGCodeLine(char* readFrom, long int offset, long int bytesToRead)
		: ChituDataBlock(readFrom, offset, bytesToRead)
	{
		InitGCodeLine();
	}

	//sets up all the variables associated with the g-code line
	void InitGCodeLine()
	{
		RegisterData(new ChituFloat(rawData, LAYER_Z, "Layer Z Position (mm)"), "LAYER_Z_MM");
		RegisterData(new ChituFloat(rawData, EXPOSURE_TIME, "Layer Exposure Time (s)"), "LAYER_EXPOSURE_TIME");
		RegisterData(new ChituFloat(rawData, LIGHT_OFF_DELAY, "Light Off Delay (s)"), "LIGHT_OFF_DELAY");
		RegisterData(new ChituAddress(rawData, POINTER_TO_IMAGE, "Address of Image for Layer"), "LAYER_IMAGE_ADDRESS");
		RegisterData(new ChituInt(rawData, IMAGE_SIZE, "Size of Image (bytes)"), "IMAGE_SIZE");
		RegisterData(new ChituInt(rawData, SIZE_OF_CODE_BEFORE_IMAGE, "Size of Code Before Image (bytes)"), "SIZE_OF_CODE");
	}

	//extra functions that make getting some of the values easier (so we don't need to remember
	//key words all of the time. Note, we still can access value by key if we want!)
	long int GetImageAddress() { return GetValueByKey<long int>("LAYER_IMAGE_ADDRESS"); }
	long int GetImageSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
	long int GetSizeOfCode() { return GetValueByKey<long int>("SIZE_OF_CODE"); }

};