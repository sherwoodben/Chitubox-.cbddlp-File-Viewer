#pragma once

#include "ChituDataBlock.h"

struct ChituCopyRightData : public ChituDataBlock
{
	//the copyright data as a string
	//(empty until read)
	std::string dataAsString = "";

	//constructor, takes the length of the copyright string and the offset
	//into the file, then loads the string
	ChituCopyRightData(std::FILE* cFile, long int readOffset, long int bytesToRead) :
		ChituDataBlock(cFile, readOffset, bytesToRead)
	{
		Load();
	}

	//just reads the data as a string!
	void Load()
	{
		dataAsString = std::string(rawData, size);
	}

	//special report function that overrides the ChituDataBlock one.
	//simply outputs the string.
	void ReportData(std::ostream* targetStream, int tabLevel = 0) override
	{
		//builds the tab string for organizational purposes
		std::string tabString = "";
		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		//and outputs the string with no other formatting
		*targetStream << tabString << dataAsString << "\n";
	}
};