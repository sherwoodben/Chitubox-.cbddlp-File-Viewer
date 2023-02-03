#pragma once

#include <iostream>

#include "ChituDataBlock.h"

struct NewFileHeader : public ChituDataBlock
{
	static enum DataOffsets
	{
		t_SCREEN_X_MM = 0x00000008,
		t_SCREEN_Y_MM = 0x0000000C,
		t_MACHINE_Z_MM = 0x00000010,
		t_MODEL_HEIGHT_MM = 0x0000001C,
		t_LAYER_HEIGHT = 0x00000020,
		t_LAYER_EXPOSURE_TIME = 0x00000024,
		t_BOTTOM_LAYER_EXPOSURE_TIME = 0x00000028,
		t_NUM_FIRST_LAYERS = 0x00000030,
		t_SCREEN_X_PX = 0x00000034,
		t_SCREEN_Y_PX = 0x00000038,
		t_LARGE_IMAGE_PREVIEW = 0x0000003C,
		t_G_CODE_POINTER = 0x00000040,
		t_NUM_LAYERS = 0x00000044,
		t_SMALL_IMAGE_PREVIEW = 0x00000048,
		t_NEW_DATA_POINTER = 0x00000054,
		t_LIGHT_PWM = 0x00000060,
		t_BOTTOM_LIGHT_PWM = 0x00000062,
		t_COPYRIGHT_POINTER = 0x00000068,
		t_COPYRIGHT_OFFSET = 0x0000006C
	};

	NewFileHeader(std::FILE* readFrom, long int bytesToRead) 
		: ChituDataBlock(readFrom, 0, bytesToRead)
	{
		for (int i = 0; i < size; i++)
		{
			std::cout << rawData[i];
		}
		std::cout << std::endl;
	}

	void InitFileHeader()
	{
		RegisterData(new ChituFloat(rawData, t_SCREEN_X_MM, "Screen X Size (mm)"), "SCREEN_X_MM");
		RegisterData(new ChituFloat(rawData, t_SCREEN_Y_MM, "Screen Y Size (mm)"), "SCREEN_Y_MM");
		RegisterData(new ChituFloat(rawData, t_MACHINE_Z_MM, "Machine Z Size (mm)"), "MACHINE_Z_MM");
		RegisterData(new ChituFloat(rawData, t_MODEL_HEIGHT_MM, "Model Height (mm)"), "MODEL_HEIGHT_MM");
		RegisterData(new ChituFloat(rawData, t_LAYER_HEIGHT, "Layer Height (mm)"), "LAYER_HEIGHT_MM");
		RegisterData(new ChituFloat(rawData, t_LAYER_EXPOSURE_TIME, "Layer Exposure Time (s)"), "LAYER_EXPOSURE_TIME_MM");
		RegisterData(new ChituFloat(rawData, t_BOTTOM_LAYER_EXPOSURE_TIME, "Bottom Layer Exposure Time (s)"), "B_LAYER_EXPOSURE_TIME_MM");
		RegisterData(new ChituInt(rawData, t_NUM_FIRST_LAYERS, "Number of Bottom Layers"), "NUM_B_LAYERS");
		RegisterData(new ChituInt(rawData, t_SCREEN_X_PX, "Screen X Pixels (px)"), "SCREEN_X_PX");
		RegisterData(new ChituInt(rawData, t_SCREEN_Y_PX, "Screen Y Pixels (px)"),"SCREEN_Y_PX");
		RegisterData(new ChituAddress(rawData, t_LARGE_IMAGE_PREVIEW, "Large Image Preview Address"), "LARGE_PREVIEW_ADDRESS");
		RegisterData(new ChituAddress(rawData, t_G_CODE_POINTER, "G-Code Address"), "G_CODE_ADDRESS");
		RegisterData(new ChituInt(rawData, t_NUM_LAYERS, "Number of Layers"), "NUMBER_OF_LAYERS");
		RegisterData(new ChituAddress(rawData, t_SMALL_IMAGE_PREVIEW, "Small Image Preview Address"), "SMALL_PREVIEW_ADDRESS");
		RegisterData(new ChituAddress(rawData, t_NEW_DATA_POINTER, "Address of Unknown Data"), "UNKNOWN_DATA_ADDRESS");
		RegisterData(new ChituShort(rawData, t_LIGHT_PWM, "Light PWM"), "LIGHT_PWM");
		RegisterData(new ChituShort(rawData, t_BOTTOM_LIGHT_PWM, "Bottom Light PWM"), "B_LIGHT_PWM");
		RegisterData(new ChituAddress(rawData, t_COPYRIGHT_POINTER, "Address of Copyright Notice"), "COPYRIGHT_ADDRESS");
		RegisterData(new ChituInt(rawData, t_COPYRIGHT_OFFSET, "Offset for Copyright Notice"), "COPYRIGHT_OFFSET");
	}
};