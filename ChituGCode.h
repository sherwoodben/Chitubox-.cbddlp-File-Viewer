#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "FileInterpreter.h"
#include "ChituDataBlock.h"

struct ChituGCodeLine : public ChituDataBlock
{
	//found by poking around in a hex editor;
	//slight possibility I've made the wrong
	//conclusion
	enum DataOffsets
	{
		LAYER_Z = 0x0000,
		EXPOSURE_TIME = 0x0004,
		LIGHT_OFF_DELAY = 0x0008,
		POINTER_TO_IMAGE = 0x000C,
		IMAGE_SIZE = 0x0010,
		SIZE_OF_CODE_BEFORE_IMAGE = 0x0018
	};

	ChituGCodeLine(char* readFrom, long int offset, long int bytesToRead)
		: ChituDataBlock(readFrom, offset, bytesToRead)
	{
		InitGCodeLine();
	}

	ChituGCodeLine(const ChituGCodeLine& copyFrom) : ChituDataBlock(copyFrom)
	{
		InitGCodeLine();
	}

	void InitGCodeLine()
	{
		RegisterData(new ChituFloat(rawData, LAYER_Z, "Layer Z Position (mm)"), "LAYER_Z_MM");
		RegisterData(new ChituFloat(rawData, EXPOSURE_TIME, "Layer Exposure Time (s)"), "LAYER_EXPOSURE_TIME");
		RegisterData(new ChituFloat(rawData, LIGHT_OFF_DELAY, "Light Off Delay (s)"), "LIGHT_OFF_DELAY");
		RegisterData(new ChituAddress(rawData, POINTER_TO_IMAGE, "Address of Image for Layer"), "LAYER_IMAGE_ADDRESS");
		RegisterData(new ChituInt(rawData, IMAGE_SIZE, "Size of Image (bytes)"), "IMAGE_SIZE");
		RegisterData(new ChituInt(rawData, SIZE_OF_CODE_BEFORE_IMAGE, "Size of Code Before Image (bytes)"), "SIZE_OF_CODE");
	}

	//"helper" functions to get values we may need to access
	//more frequently
	long int GetImageAddress() { return GetValueByKey<long int>("LAYER_IMAGE_ADDRESS"); }
	long int GetImageSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
	long int GetSizeOfCode() { return GetValueByKey<long int>("SIZE_OF_CODE"); }

};

struct ChituInterLayerGCodeLine : public ChituDataBlock
{
	//found by poking around in a hex editor;
	//slight possibility I've made the wrong
	//conclusion
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

	long int imgOffset = -1;

	ChituInterLayerGCodeLine(char* readFrom, long int offset, long int bytesToRead)
		: ChituDataBlock(readFrom, offset, bytesToRead)
	{
		InitInterLayerGCodeLine();
	}

	ChituInterLayerGCodeLine(const ChituInterLayerGCodeLine& copyFrom) : ChituDataBlock(copyFrom)
	{
		InitInterLayerGCodeLine();
	}

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

	long int GetImageAddress() { return GetValueByKey<long int>("LAYER_IMAGE_ADDRESS"); }
	long int GetImageOffsetInChunk() { return imgOffset; }
	long int GetImageSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
};

struct ChituGCode : public ChituDataBlock
{

	std::vector<ChituGCodeLine*> gCodeLines;
	std::vector<ChituInterLayerGCodeLine*> interLayerGCodeLines;

	long int SIZE_OF_GCODE_LINE = 36;
	long int numberOfLayers = -1;

	ChituGCode(std::FILE* readFrom, long int readOffset, long int bytesToRead, long int numLayers)
		: ChituDataBlock(readFrom, readOffset, bytesToRead), numberOfLayers(numLayers)
	{
		InitGCodeLines();
		InitInterLayerGCodeLines();
	}

	~ChituGCode()
	{
		//since we allocated with new, we need to delete each
		//object in the g-code/inter layer g-code vectors
		for (std::vector<ChituGCodeLine*>::iterator line = gCodeLines.begin(); line != gCodeLines.end(); line++)
		{
			delete *line;
		}
		for (std::vector<ChituInterLayerGCodeLine*>::iterator line = interLayerGCodeLines.begin(); line != interLayerGCodeLines.end(); line++)
		{
			delete* line;
		}

		//clear the vectors
		gCodeLines.clear();
		interLayerGCodeLines.clear();
	}

	void InitGCodeLines()
	{
		gCodeLines.reserve(numberOfLayers);
		for (int i = 0; i < numberOfLayers; i++)
		{
			//build the g code line for each layer
			gCodeLines.push_back(new ChituGCodeLine(rawData, i * SIZE_OF_GCODE_LINE, SIZE_OF_GCODE_LINE));
		}
	}

	void InitInterLayerGCodeLines()
	{
		interLayerGCodeLines.reserve(numberOfLayers);
		for (std::vector<ChituGCodeLine*>::iterator line = gCodeLines.begin(); line != gCodeLines.end(); line++)
		{
			//get the values we need to be able to read each line:
			long int pointerToImage = (*line)->GetImageAddress();
			long int sizeOfImage = (*line)->GetImageSize();
			long int sizeOfCode = (*line)->GetSizeOfCode();

			//use those values to build the inter layer g code line for each layer:
			interLayerGCodeLines.push_back(new ChituInterLayerGCodeLine(rawData, pointerToImage - sizeOfCode - offset, sizeOfCode + sizeOfImage));
			interLayerGCodeLines.back()->imgOffset = sizeOfCode;
		}
	}

	//we have a special ReportData function for the G-Code because
	//it's a collection of other data chunks, which each need to call
	//their own ReportData function-- so, we define a custom instead of
	//the one in ChituDataBlock:
	void ReportData(std::ostream* targetStream, int tabLevel = 0) override
	{
		for (int layer = 0; layer < numberOfLayers; layer++)
		{
			std::string tabString = "";

			for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

			*targetStream << tabString << "Layer " << layer + 1 << ":" << std::endl;

			gCodeLines[layer]->ReportData(targetStream, tabLevel + 1);

			*targetStream << tabString << "Inter Layer " << layer + 1 << ":" << std::endl;

			interLayerGCodeLines[layer]->ReportData(targetStream, tabLevel + 1);
		}
	}

};