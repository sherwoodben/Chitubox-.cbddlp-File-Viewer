#pragma once

#include "ChituDataBlock.h"

struct ChituCopyRightData : public ChituDataBlock
{
	std::string dataAsString = "";

	ChituCopyRightData(std::FILE* cFile, long int readOffset, long int bytesToRead) :
		ChituDataBlock(cFile, readOffset, bytesToRead)
	{
		Load();
	}

	void Load()
	{
		dataAsString = std::string(rawData, size);
	}

	void ReportData(std::ostream* targetStream, int tabLevel = 0) override
	{
		std::string tabString = "";

		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		*targetStream << tabString << dataAsString << std::endl;
	}
};