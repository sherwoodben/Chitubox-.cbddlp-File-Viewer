#pragma once

#include <fstream>
#include <string>

#include "FileInterpreter.h"

/*	The following is provided to understand where
	the data in the file is located. Starting at
	hex offset 0x00000000:

	[8 bytes of unknown data (constant across files)]			-- 0x00000000
	[4 bytes of screen x dimension (float)]						-- 0x00000008
	[4 bytes of screen y dimension (float)]						-- 0x0000000C
	[4 bytes of machine z dimension (float)]					-- 0x00000010
	[8 bytes of empty (?) data]									-- 0x00000014
	[4 bytes of model height (float)]							-- 0x0000001C
	[4 bytes of layer height dimension (float)]					-- 0x00000020
	[4 bytes of exposure time (float)]							-- 0x00000024
	[4 bytes of first layer(s) exposure time (float)]			-- 0x00000028
	[4 bytes of empty (?) data]									-- 0x0000002C
	[4 bytes of number of first layers (long)]					-- 0x00000030
	[4 bytes of screen x pixels (long)]							-- 0x00000034
	[4 bytes of screen y pixels (long)]							-- 0x00000038
	[4 bytes pointing to the large preview image (pointer)]		-- 0x0000003C
	[4 bytes pointing to the g-code (pointer)]					-- 0x00000040
	[4 bytes of (the number of layers) (long)]					-- 0x00000044
	[4 bytes pointing to the small preview image (pointer)]		-- 0x00000048
	[4 bytes of ????? data]										-- 0x0000004C

	[1 byte of light PWM data (short)]							-- 0x00000060
	[1 byte of empty (?) data]									-- 0x00000061
	[1 byte of bottom light PWM data (short)]					-- 0x00000062
	[1 byte of empty (?) data]									-- 0x00000063
	[4 bytes of empty (?) data]									-- 0x00000064
	[4 bytes pointing to the Chitubox copyright notice]			-- 0x00000068
	[4 bytes for offset of start of notice from pointer]		-- 0x0000006C

	END OF FILE HEADER
*/

enum HeaderDataOffsets
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

struct FileHeader {

	ChituPointer fileHeaderPointer;
	char* rawData = nullptr;
	int headerSize = 0;

	float screenXDimensions = 0;
	float screenYDimensions = 0;
	float machineZDimensions = 0;
	float modelHeight = 0;

	float layerHeight = 0;
	float layerExposureTime = 0;
	float bottomLayerExposureTime = 0;

	long int numBottomLayers = 0;
	long int screenXPixels = 0;
	long int screenYPixels = 0;
	ChituPointer largePreviewImage;

	ChituPointer gCodePointer;
	long int numLayers = 0;
	ChituPointer smallPreviewImage;

	ChituPointer newDataPointer;

	short int lightPWM = 0;

	short int bottomLightPWM = 0;

	ChituPointer copyrightPointer;
	long int copyrightOffset = 0;


	FileHeader(std::FILE* file, int HEADER_SIZE)
	{
		headerSize = HEADER_SIZE;
		rawData = new char[headerSize];

		fseek(file, 0, SEEK_SET);
		fread(rawData, sizeof(char), headerSize, file);

		for (int i = 0; i < headerSize; i++)
		{
			std::cout << rawData[i];
		}
		std::cout << std::endl;

	}

	~FileHeader()
	{
		delete[] rawData;
		rawData = nullptr;
	}

	void ParseRawData()
	{

		ReadFromBinary(screenXDimensions, rawData, SCREEN_X_MM);
		ReadFromBinary(screenYDimensions, rawData, SCREEN_Y_MM);
		ReadFromBinary(machineZDimensions, rawData, MACHINE_Z_MM);
		ReadFromBinary(modelHeight, rawData, MODEL_HEIGHT_MM);

		ReadFromBinary(layerHeight, rawData, LAYER_HEIGHT);
		ReadFromBinary(layerExposureTime, rawData, LAYER_EXPOSURE_TIME);
		ReadFromBinary(bottomLayerExposureTime, rawData, BOTTOM_LAYER_EXPOSURE_TIME);

		ReadFromBinary(numBottomLayers, rawData, NUM_FIRST_LAYERS);
		ReadFromBinary(screenXPixels, rawData, SCREEN_X_PX);
		ReadFromBinary(screenYPixels, rawData, SCREEN_Y_PX);
		ReadFromBinary(largePreviewImage, rawData, LARGE_IMAGE_PREVIEW);

		ReadFromBinary(gCodePointer, rawData, G_CODE_POINTER);
		ReadFromBinary(numLayers, rawData, NUM_LAYERS);
		ReadFromBinary(smallPreviewImage, rawData, SMALL_IMAGE_PREVIEW);

		ReadFromBinary(newDataPointer, rawData, NEW_DATA_POINTER);

		ReadFromBinary(lightPWM, rawData, LIGHT_PWM);

		ReadFromBinary(bottomLightPWM, rawData, BOTTOM_LIGHT_PWM);

		ReadFromBinary(copyrightPointer, rawData, COPYRIGHT_POINTER);
		ReadFromBinary(copyrightOffset, rawData, COPYRIGHT_OFFSET);
	}

	void Report(std::ostream* targetStream = &std::cout, int tabLevel = 0)
	{
		std::string tabString;

		for (int i = 0; i < tabLevel; i++) tabString.append("\t");

		*targetStream << tabString << "Screen X Dimensions (mm): " << screenXDimensions << std::endl;
		*targetStream << tabString << "Screen Y Dimensions (mm): " << screenYDimensions << std::endl;
		*targetStream << tabString << "Machine Z Dimensions (mm): " << machineZDimensions << std::endl;
		*targetStream << tabString << "Model Height (mm): " << modelHeight << std::endl;
		*targetStream << tabString << "Layer Height (mm): " << layerHeight << std::endl;
		*targetStream << tabString << "Layer Exposure Time (s): " << layerExposureTime << std::endl;
		*targetStream << tabString << "Bottom Layer Exposure Time (s): " << bottomLayerExposureTime << std::endl;
		*targetStream << tabString << "Number of bottom layers: " << numBottomLayers << std::endl;
		*targetStream << tabString << "Screen X Pixels: " << screenXPixels << std::endl;
		*targetStream << tabString << "Screen Y Pixels: " << screenYPixels << std::endl;
		*targetStream << tabString << "Number of Layers: " << numLayers << std::endl;
		*targetStream << tabString << "Light PWM Value: " << lightPWM << std::endl;
		*targetStream << tabString << "Bottom Light PWM Value: " << bottomLightPWM << std::endl;
		*targetStream << std::endl;
		*targetStream << tabString << "Large Preview Image Address: " << largePreviewImage.chituAddress << std::endl;
		*targetStream << tabString << "Small Preview Image Address: " << smallPreviewImage.chituAddress << std::endl;
		*targetStream << tabString << "New Data Address: " << newDataPointer.chituAddress << std::endl;
		*targetStream << tabString << "G Code Address: " << gCodePointer.chituAddress << std::endl;
		*targetStream << tabString << "Copyright Address: " << copyrightPointer.chituAddress << std::endl;
		*targetStream << std::endl;
		*targetStream << tabString << "Raw Data (located at " << fileHeaderPointer.chituAddress << "):" << std::endl;

		RawDataToStream(rawData, headerSize, targetStream, tabLevel + 1);
	}

};