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
		NEW_DATA_POINTER = 0x00000054,
		LIGHT_PWM = 0x00000060,
		BOTTOM_LIGHT_PWM = 0x00000062,
		COPYRIGHT_POINTER = 0x00000068,
		COPYRIGHT_OFFSET = 0x0000006C
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
		RegisterData(new ChituAddress(rawData, NEW_DATA_POINTER, "Address of Unknown Data"), "UNKNOWN_DATA_ADDRESS");
		RegisterData(new ChituShort(rawData, LIGHT_PWM, "Light PWM"), "LIGHT_PWM");
		RegisterData(new ChituShort(rawData, BOTTOM_LIGHT_PWM, "Bottom Light PWM"), "B_LIGHT_PWM");
		RegisterData(new ChituAddress(rawData, COPYRIGHT_POINTER, "Address of Copyright Notice"), "COPYRIGHT_ADDRESS");
		RegisterData(new ChituInt(rawData, COPYRIGHT_OFFSET, "Offset for Copyright Notice"), "COPYRIGHT_OFFSET");
	}

	
	long int GetLargePreviewHeaderAddress() { return GetValueByKey<long int>("LARGE_PREVIEW_ADDRESS");}
	long int GetSmallPreviewHeaderAddress() { return GetValueByKey<long int>("SMALL_PREVIEW_ADDRESS");}
	long int GetUnknownDataAddress() { return GetValueByKey<long int>("UNKNOWN_DATA_ADDRESS"); }
	long int GetCopyrightAddress() { return GetValueByKey<long int>("COPYRIGHT_ADDRESS");}
	long int GetCopyrightOffset() { return GetValueByKey<long int>("COPYRIGHT_OFFSET");}
	long int GetGCodeAddress() { return GetValueByKey<long int>("G_CODE_ADDRESS");}
	long int GetNumLayers() { return GetValueByKey<long int>("NUMBER_OF_LAYERS");}
	long int GetScreenX_PX() { return GetValueByKey<long int>("SCREEN_X_PX");}
	long int GetScreenY_PX() { return GetValueByKey<long int>("SCREEN_Y_PX");}
};