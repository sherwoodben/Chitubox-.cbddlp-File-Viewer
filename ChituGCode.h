#pragma once

#include <vector>
#include <iostream>

#include "FileInterpreter.h"

struct ChituDataLine
{
	ChituPointer lineAddress;
	char* rawData = nullptr;
	int sizeOfCode = 0;
};

struct GCodeLine : public ChituDataLine
{
	enum GCodeLineDataOffsets
	{
		LAYER_Z = 0x0000,
		EXPOSURE_TIME = 0x0004,
		LIGHT_OFF_DELAY = 0x0008,
		POINTER_TO_IMAGE = 0x000C,
		IMAGE_SIZE = 0x0010,
		SIZE_OF_CODE_BEFORE_IMAGE = 0x0018,

	};

	float layerZPos = 0;
	float layerExposureTime = 0;
	ChituPointer PointerToImage;
	long int imageSize = 0;
	long int sizeOfCodeBeforeImage = 0;

	GCodeLine() : ChituDataLine() {};

	~GCodeLine()
	{
		if (rawData != nullptr)
		{
			delete[] rawData;
		}
		rawData = nullptr;
	}

	void LoadData()
	{
		ReadFromBinary(layerZPos, rawData, GCodeLineDataOffsets::LAYER_Z);
		ReadFromBinary(layerExposureTime, rawData, GCodeLineDataOffsets::EXPOSURE_TIME);
		ReadFromBinary(PointerToImage, rawData, GCodeLineDataOffsets::POINTER_TO_IMAGE);
		ReadFromBinary(imageSize, rawData, GCodeLineDataOffsets::IMAGE_SIZE);
		ReadFromBinary(sizeOfCodeBeforeImage, rawData, GCodeLineDataOffsets::SIZE_OF_CODE_BEFORE_IMAGE);
	}
};

struct InterLayerGCodeLine : public ChituDataLine
{
	enum InterLayerGCodeLineDataOffsets
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

	float layerZPos = 0;
	float layerExposureTime = 0;
	ChituPointer PointerToImage;
	long int imageSize = 0;
	long int sizeOfCode = 0;
	long int totalSize = 0;
	float liftingDistance = 0;
	float liftingSpeed = 0;
	float retractSpeed = 0;
	float restBeforeLift = 0;
	float restAfterLift = 0;
	float restAfterRetract = 0;
	float lightPWM = 0;

	InterLayerGCodeLine() : ChituDataLine() {};

	~InterLayerGCodeLine()
	{
		if (rawData != nullptr)
		{
			delete[] rawData;
		}
		rawData = nullptr;
	}

	void LoadData()
	{
		ReadFromBinary(layerZPos, rawData, InterLayerGCodeLineDataOffsets::LAYER_Z);
		ReadFromBinary(layerExposureTime, rawData, InterLayerGCodeLineDataOffsets::EXPOSURE_TIME);
		ReadFromBinary(PointerToImage, rawData, InterLayerGCodeLineDataOffsets::POINTER_TO_IMAGE);
		ReadFromBinary(imageSize, rawData, InterLayerGCodeLineDataOffsets::IMAGE_SIZE);
		ReadFromBinary(sizeOfCode, rawData, InterLayerGCodeLineDataOffsets::LAYER_CODE_SIZE);
		ReadFromBinary(totalSize, rawData, InterLayerGCodeLineDataOffsets::SIZE_OF_NEXT_DATA);
		ReadFromBinary(liftingDistance, rawData, InterLayerGCodeLineDataOffsets::LIFTING_DISTANCE);
		ReadFromBinary(liftingSpeed, rawData, InterLayerGCodeLineDataOffsets::LIFTING_SPEED);
		ReadFromBinary(retractSpeed, rawData, InterLayerGCodeLineDataOffsets::RETRACT_SPEED);
		ReadFromBinary(restBeforeLift, rawData, InterLayerGCodeLineDataOffsets::REST_BEFORE_LIFT);
		ReadFromBinary(restAfterLift, rawData, InterLayerGCodeLineDataOffsets::REST_AFTER_LIFT);
		ReadFromBinary(restAfterRetract, rawData, InterLayerGCodeLineDataOffsets::REST_AFTER_RETRACT);
		ReadFromBinary(lightPWM, rawData, InterLayerGCodeLineDataOffsets::LIGHT_PWM);
	}
};



struct ChituGCode
{
	int SIZE_OF_G_CODE_LINE = 36;
	int numLines = 0;
	std::vector<GCodeLine*> gCodeLines;
	std::vector<InterLayerGCodeLine*> dataLines;

	ChituGCode(std::FILE* file, int _numLines, unsigned long int GCodeOffset)
	{
		numLines = _numLines;
		gCodeLines.clear();
		dataLines.clear();

		for (int i = 0; i < numLines; i++)
		{
			gCodeLines.push_back(ReadIntoGCodeLine(file, GCodeOffset + (i * SIZE_OF_G_CODE_LINE), SIZE_OF_G_CODE_LINE));
			dataLines.push_back(ReadIntoDataLine(file, gCodeLines.back()->PointerToImage.chituAddress, gCodeLines.back()->imageSize, gCodeLines.back()->sizeOfCodeBeforeImage));
		}
	}

	GCodeLine* ReadIntoGCodeLine(std::FILE* file, unsigned long int startLocation, int bytesToRead)
	{
		GCodeLine* lineToReturn = new GCodeLine();

		lineToReturn->rawData = new char[bytesToRead];
		lineToReturn->sizeOfCode = bytesToRead;
		lineToReturn->lineAddress.chituAddress = startLocation;

		fseek(file, startLocation, SEEK_SET);
		fread(lineToReturn->rawData, sizeof(char), lineToReturn->sizeOfCode, file);

		lineToReturn->LoadData();

		return lineToReturn;
	}

	InterLayerGCodeLine* ReadIntoDataLine(std::FILE* file, unsigned long int addressPointedTo, int SIZE_OF_IMAGE, int SIZE_OF_LINE)
	{
		InterLayerGCodeLine* lineToReturn = new InterLayerGCodeLine();
		lineToReturn->imageSize = SIZE_OF_IMAGE;
		lineToReturn->sizeOfCode = SIZE_OF_LINE;
		lineToReturn->rawData = new char[lineToReturn->sizeOfCode + lineToReturn->imageSize];

		lineToReturn->lineAddress.chituAddress = addressPointedTo - lineToReturn->sizeOfCode;
		

		fseek(file, lineToReturn->lineAddress.chituAddress, SEEK_SET);
		fread(lineToReturn->rawData, sizeof(char), lineToReturn->sizeOfCode + lineToReturn->imageSize, file);

		lineToReturn->LoadData();

		return lineToReturn;
	}

	void Report(std::ostream* targetStream = &std::cout, int tabLevel = 0)
	{
		std::string tabString;
		for (int i = 0; i < tabLevel; i++) tabString.append("\t");
		*targetStream << tabString << "Number of Layers: " << numLines << ":" << std::endl;
		*targetStream << std::endl;
		for (int i = 0; i < numLines; i++)
		{
			tabString.append("\t");
			*targetStream << tabString << "G-Code Line " << i << " (located at " << gCodeLines[i]->lineAddress.chituAddress << "):" << std::endl;
			tabString.append("\t");
			*targetStream << tabString << "Layer Z Pos (mm): " << gCodeLines[i]->layerZPos << std::endl;
			*targetStream << tabString << "Layer Exposure Time (s): " << gCodeLines[i]->layerExposureTime << std::endl;
			*targetStream << tabString << "Pointer to Layer's Image: " << gCodeLines[i]->PointerToImage.chituAddress << std::endl;
			*targetStream << tabString << "Layer's Image Size: " << gCodeLines[i]->imageSize << std::endl;
			*targetStream << tabString << "Size of Code Before Layer's Image: " << gCodeLines[i]->sizeOfCodeBeforeImage << std::endl;
			*targetStream << std::endl;

			*targetStream << tabString << "Raw Data (located at " << gCodeLines[i]->lineAddress.chituAddress << "):" << std::endl;
			RawDataToStream(gCodeLines[i]->rawData, gCodeLines[i]->sizeOfCode, targetStream, tabLevel + 3);
			tabString.clear();
			for (int j = 0; j < tabLevel; j++) tabString.append("\t");

			tabString.append("\t");
			*targetStream << tabString << "Other Data Line " << i << " (located at " << dataLines[i]->lineAddress.chituAddress << "):" << std::endl;
			tabString.append("\t");
			*targetStream << tabString << "Layer Z Pos (mm): " << dataLines[i]->layerZPos << std::endl;
			*targetStream << tabString << "Layer Exposure Time (s): " << dataLines[i]->layerExposureTime << std::endl;
			*targetStream << tabString << "Pointer to Layer's Image: " << dataLines[i]->PointerToImage.chituAddress << std::endl;
			*targetStream << tabString << "Image Size: " << dataLines[i]->imageSize << std::endl;
			*targetStream << tabString << "Size of Code Before Image: " << dataLines[i]->sizeOfCode << std::endl;
			*targetStream << tabString << "Total Size: " << dataLines[i]->totalSize << std::endl;
			*targetStream << tabString << "Lifting Distance (mm): " << dataLines[i]->liftingDistance << std::endl;
			*targetStream << tabString << "Lifting Speed (mm/min): " << dataLines[i]->liftingSpeed << std::endl;
			*targetStream << tabString << "Retract Speed (mm/min): " << dataLines[i]->retractSpeed << std::endl;
			*targetStream << tabString << "Rest Before Lift (s): " << dataLines[i]->restBeforeLift << std::endl;
			*targetStream << tabString << "Rest After Lift (s): " << dataLines[i]->restAfterLift << std::endl;
			*targetStream << tabString << "Rest After Retract (s): " << dataLines[i]->restAfterRetract << std::endl;
			*targetStream << tabString << "Light PWM: " << dataLines[i]->lightPWM << std::endl;
			*targetStream << std::endl;

			*targetStream << tabString << "Raw Data (located at " << dataLines[i]->lineAddress.chituAddress << "):" << std::endl;
			RawDataToStream(dataLines[i]->rawData, dataLines[i]->sizeOfCode + dataLines[i]->imageSize, targetStream, tabLevel + 3);
			tabString.clear();
			for (int j = 0; j < tabLevel; j++) tabString.append("\t");
			
		}
		*targetStream << "\n" << std::endl;
	}

};
