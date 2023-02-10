#pragma once

#include <iostream>

#include "ChituDataBlock.h"

struct ChituFileHeader : public ChituDataBlock
{
	//found by poking around in a hex editor;
	//slight possibility I've made the wrong
	//conclusion
	enum DataOffsets
	{
		SCREEN_X_MM = 0x00000008,
		SCREEN_Y_MM = 0x0000000C,
		MACHINE_Z_MM = 0x00000010,
		MODEL_HEIGHT_MM = 0x0000001C,
		LAYER_HEIGHT = 0x00000020,
		LAYER_EXPOSURE_TIME = 0x00000024,
		BOTTOM_LAYER_EXPOSURE_TIME = 0x00000028,
		NUM_FIRST_LAYERS = 0x00000030,
		SCREEN_X_PX = 0x00000034,
		SCREEN_Y_PX = 0x00000038,
		LARGE_IMAGE_PREVIEW = 0x0000003C,
		G_CODE_POINTER = 0x00000040,
		NUM_LAYERS = 0x00000044,
		SMALL_IMAGE_PREVIEW = 0x00000048,
		TIME_ESTIMATE = 0x0000004C,
		MYSTERY_INT_1 = 0x00000050, //what is this???
		SUBDATA_1_ADDRESS = 0x00000054,
		SIZE_OF_SUBDATA_1 = 0x00000058,
		MYSTERY_INT_3 = 0x0000005C, //what is this???
		LIGHT_PWM = 0x00000060,
		BOTTOM_LIGHT_PWM = 0x00000062,
		SUBDATA_2_ADDRESS = 0x00000068,
		SUBDATA_2_SIZE = 0x0000006C
	};

	ChituFileHeader(std::FILE* readFrom, long int bytesToRead) 
		: ChituDataBlock(readFrom, 0, bytesToRead)
	{
		InitFileHeader();
	}

	//the RegisterData function makes it easy to associate
	//data with a human-readable/understandable name and key!
	void InitFileHeader()
	{
		RegisterData(new ChituFloat(rawData, SCREEN_X_MM, "Screen X Size (mm)"), "SCREEN_X_MM");
		RegisterData(new ChituFloat(rawData, SCREEN_Y_MM, "Screen Y Size (mm)"), "SCREEN_Y_MM");
		RegisterData(new ChituFloat(rawData, MACHINE_Z_MM, "Machine Z Size (mm)"), "MACHINE_Z_MM");
		RegisterData(new ChituFloat(rawData, MODEL_HEIGHT_MM, "Model Height (mm)"), "MODEL_HEIGHT_MM");
		RegisterData(new ChituFloat(rawData, LAYER_HEIGHT, "Layer Height (mm)"), "LAYER_HEIGHT_MM");
		RegisterData(new ChituFloat(rawData, LAYER_EXPOSURE_TIME, "Layer Exposure Time (s)"), "LAYER_EXPOSURE_TIME_MM");
		RegisterData(new ChituFloat(rawData, BOTTOM_LAYER_EXPOSURE_TIME, "Bottom Layer Exposure Time (s)"), "B_LAYER_EXPOSURE_TIME_MM");
		RegisterData(new ChituInt(rawData, NUM_FIRST_LAYERS, "Number of Bottom Layers"), "NUM_B_LAYERS");
		RegisterData(new ChituInt(rawData, SCREEN_X_PX, "Screen X Pixels (px)"), "SCREEN_X_PX");
		RegisterData(new ChituInt(rawData, SCREEN_Y_PX, "Screen Y Pixels (px)"),"SCREEN_Y_PX");
		RegisterData(new ChituAddress(rawData, LARGE_IMAGE_PREVIEW, "Large Image Preview Address"), "LARGE_PREVIEW_ADDRESS");
		RegisterData(new ChituAddress(rawData, G_CODE_POINTER, "G-Code Address"), "G_CODE_ADDRESS");
		RegisterData(new ChituInt(rawData, NUM_LAYERS, "Number of Layers"), "NUMBER_OF_LAYERS");
		RegisterData(new ChituAddress(rawData, SMALL_IMAGE_PREVIEW, "Small Image Preview Address"), "SMALL_PREVIEW_ADDRESS");
		RegisterData(new ChituInt(rawData, TIME_ESTIMATE, "Time Estimate (s)"), "TIME_ESTIMATE");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_1), "MYSTERY_INT_1");
		RegisterData(new ChituAddress(rawData, SUBDATA_1_ADDRESS, "Subdata 1 Address"), "SUBDATA_1_ADDRESS");
		RegisterData(new ChituInt(rawData, SIZE_OF_SUBDATA_1, "Subdata 1 Size (bytes)"), "SUBDATA_1_SIZE");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_3), "MYSTERY_INT_3");
		RegisterData(new ChituShort(rawData, LIGHT_PWM, "Light PWM"), "LIGHT_PWM");
		RegisterData(new ChituShort(rawData, BOTTOM_LIGHT_PWM, "Bottom Light PWM"), "B_LIGHT_PWM");
		RegisterData(new ChituAddress(rawData, SUBDATA_2_ADDRESS, "Subdata 2 Address"), "SUBDATA_2_ADDRESS");
		RegisterData(new ChituInt(rawData, SUBDATA_2_SIZE, "Subdata 2 Size (bytes)"), "SUBDATA_2_SIZE");
		
	}

	
	long int GetLargePreviewHeaderAddress() { return GetValueByKey<long int>("LARGE_PREVIEW_ADDRESS");}
	long int GetSmallPreviewHeaderAddress() { return GetValueByKey<long int>("SMALL_PREVIEW_ADDRESS");}
	long int GetSubdata1Address() { return GetValueByKey<long int>("SUBDATA_1_ADDRESS"); }
	long int GetSubdata1Size() { return GetValueByKey<long int>("SUBDATA_1_SIZE"); }
	long int GetSubdata2Address() { return GetValueByKey<long int>("SUBDATA_2_ADDRESS");}
	long int GetSubdata2Size() { return GetValueByKey<long int>("SUBDATA_2_SIZE");}
	long int GetGCodeAddress() { return GetValueByKey<long int>("G_CODE_ADDRESS");}
	long int GetNumLayers() { return GetValueByKey<long int>("NUMBER_OF_LAYERS");}
	long int GetScreenX_PX() { return GetValueByKey<long int>("SCREEN_X_PX");}
	long int GetScreenY_PX() { return GetValueByKey<long int>("SCREEN_Y_PX");}
};