#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "ChituGCodeLine.h"
#include "ChituInterLayerGCodeLine.h"
#include "ChituDataBlock.h"

struct ChituGCodeManager : public ChituDataBlock
{
	//a vector of pointers to g-code lines
	//and a vector of pointers to "inter-layer" g-code lines
	std::vector<ChituGCodeLine*> gCodeLines;
	std::vector<ChituInterLayerGCodeLine*> interLayerGCodeLines;

	//usefule values to have; seems the size
	//of a g-code line is always 36 bytes
	long int SIZE_OF_GCODE_LINE = 36;

	//we set the number of layers
	long int numberOfLayers = -1;

	//constructor for a top-level data block; reads straight from the file.
	//runs Init functions for the g-code and inter-layer gcode lines
	ChituGCodeManager(std::FILE* readFrom, long int readOffset, long int bytesToRead, long int numLayers)
		: ChituDataBlock(readFrom, readOffset, bytesToRead), numberOfLayers(numLayers)
	{
		InitGCodeLines();
		InitInterLayerGCodeLines();
	}

	//destructor
	~ChituGCodeManager()
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

	//gets the g-code lines ready
	void InitGCodeLines()
	{
		//reserve enough space up front
		gCodeLines.reserve(numberOfLayers);

		//read every g-code from layer 0 to the last layer
		for (int i = 0; i < numberOfLayers; i++)
		{
			//build the g code line for each layer
			gCodeLines.push_back(new ChituGCodeLine(rawData, i * SIZE_OF_GCODE_LINE, SIZE_OF_GCODE_LINE));
		}
	}

	//gets the inter-layer g-code lines ready
	void InitInterLayerGCodeLines()
	{
		//reserve enough space up front
		interLayerGCodeLines.reserve(numberOfLayers);

		//read every g-code in the vector of g-code pointers
		for (std::vector<ChituGCodeLine*>::iterator line = gCodeLines.begin(); line != gCodeLines.end(); line++)
		{
			//get the values we need to be able to read each line:
			long int pointerToImage = (*line)->GetImageAddress();
			long int sizeOfImage = (*line)->GetImageSize();
			long int sizeOfCode = (*line)->GetSizeOfCode();

			//use those values to build the inter layer g code line for each layer:
			interLayerGCodeLines.push_back(new ChituInterLayerGCodeLine(rawData, pointerToImage - sizeOfCode - offset, sizeOfCode + sizeOfImage, sizeOfCode));
		}
	}

	//we have a special ReportData function for the G-Code because
	//it's a collection of other data chunks, which each need to call
	//their own ReportData function-- so, we define a custom one instead of
	//the one in ChituDataBlock:
	void ReportData(std::ostream* targetStream, int tabLevel = 0) override
	{
		//for every layer
		for (int layer = 0; layer < numberOfLayers; layer++)
		{
			//we build a tab string for formatting purposes;
			//we change the tab level to assist with organization.
			std::string tabString = "";
			for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

			//output what layer we are getting g-code from (and add one since we
			//want to start with layer 1 not 0)
			*targetStream << tabString << "Layer " << layer + 1 << ":\n";

			//then, call the standard report function on the g-code lines
			//with a tab level one greater than we have now
			gCodeLines[layer]->ReportData(targetStream, tabLevel + 1);

			//output what layer we are getting inter-layer g-code from (and add one
			//since we want to start with layer 1 not 0)
			*targetStream << tabString << "Inter Layer " << layer + 1 << ":\n";

			//then, call the standard report function on the inter-layer g-code lines
			//with a tab level one greater than we have now
			interLayerGCodeLines[layer]->ReportData(targetStream, tabLevel + 1);
		}
	}

};