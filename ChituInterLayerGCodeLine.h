#pragma once

#include "ChituDataBlock.h"

struct ChituInterLayerGCodeLine : public ChituDataBlock
{
	//offsets to data within the
	//inter-layer(?) g-code line block for .cbddlp file type
	enum DataOffsets
	{
		LAYER_Z = 0x0000,
		EXPOSURE_TIME = 0x0004,
		LIGHT_OFF_DELAY = 0x0008,
		POINTER_TO_IMAGE = 0x000C,
		IMAGE_SIZE = 0x0010,
		LAYER_CODE_SIZE = 0x0018,
		SIZE_OF_NEXT_DATA = 0x0024,
		LIFTING_DISTANCE = 0x0028,
		LIFTING_SPEED = 0x002C,
		RETRACT_SPEED = 0x0038,
		REST_BEFORE_LIFT = 0x0044,
		REST_AFTER_LIFT = 0x0048,
		REST_AFTER_RETRACT = 0x004C,
		LIGHT_PWM = 0x0050
	};

	//since the image offset isn't necessarily
	//the same for all of these data blocks, we
	//need to store it
	long int imgOffset = -1;

	//constructor, takes a char array since we've already read the
	//data into the larger "g-code" object. Also calls the Init function
	//and sets the image offset
	ChituInterLayerGCodeLine(char* readFrom, long int offset, long int bytesToRead, long int imgOffset)
		: ChituDataBlock(readFrom, offset, bytesToRead), imgOffset{ imgOffset }
	{
		InitInterLayerGCodeLine();
	}

	//sets up all the variables associated with the inter-layer (?) g-code line
	void InitInterLayerGCodeLine()
	{
		RegisterData(new ChituFloat(rawData, LAYER_Z, "Layer Z Position (mm)"), "LAYER_Z_MM");
		RegisterData(new ChituFloat(rawData, EXPOSURE_TIME, "Layer Exposure Time (s)"), "LAYER_EXPOSURE_TIME");
		RegisterData(new ChituFloat(rawData, LIGHT_OFF_DELAY, "Light Off Delay (s)"), "LIGHT_OFF_DELAY");
		RegisterData(new ChituAddress(rawData, POINTER_TO_IMAGE, "Address of Image for Layer"), "LAYER_IMAGE_ADDRESS");
		RegisterData(new ChituInt(rawData, IMAGE_SIZE, "Size of Image (bytes)"), "IMAGE_SIZE");
		RegisterData(new ChituInt(rawData, LAYER_CODE_SIZE, "Size of Code Before Image (bytes)"), "SIZE_OF_CODE");
		RegisterData(new ChituInt(rawData, SIZE_OF_NEXT_DATA, "Size of Next Data Block (bytes)"), "NEXT_CHUNK_SIZE");
		RegisterData(new ChituFloat(rawData, LIFTING_DISTANCE, "Lifting Distance (mm)"), "LIFT_DISTANCE");
		RegisterData(new ChituFloat(rawData, LIFTING_SPEED, "Lifting Speed (mm/min)"), "LIFT_SPEED");
		RegisterData(new ChituFloat(rawData, RETRACT_SPEED, "Retract Speed (mm/min)"), "RETRACT_SPEED");
		RegisterData(new ChituFloat(rawData, REST_BEFORE_LIFT, "Rest Before Lift (s)"), "REST_BEFORE_LIFT");
		RegisterData(new ChituFloat(rawData, REST_AFTER_LIFT, "Rest After Lift (s)"), "REST_AFTER_LIFT");
		RegisterData(new ChituFloat(rawData, REST_AFTER_RETRACT, "Rest After Retract (s)"), "REST_AFTER_RETRACT");
		RegisterData(new ChituFloat(rawData, LIGHT_PWM, "Light PWM"), "LIGHT_PWM");

	}

	//extra functions that make getting some of the values easier (so we don't need to remember
	//key words all of the time. Note, we still can access value by key if we want!)
	long int GetImageAddress() { return GetValueByKey<long int>("LAYER_IMAGE_ADDRESS"); }
	long int GetImageOffsetInChunk() { return imgOffset; }
	long int GetImageSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
};