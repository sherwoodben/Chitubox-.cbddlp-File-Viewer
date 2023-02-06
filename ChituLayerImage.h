#pragma once

#include "ChituGCode.h"
#include "ChituDataBlock.h"

struct ChituLayerImage
{
	char* rawData = nullptr;

	long int encodedImgSize;

	long int decodedImgWidth;
	long int decodedImgHeight;

	ChituLayerImage(ChituInterLayerGCodeLine* readFrom, long int IMAGE_OFFSET, long int ENCODED_IMAGE_SIZE, long int DECODED_WIDTH, long int DECODED_HEIGHT)
		: encodedImgSize(ENCODED_IMAGE_SIZE), decodedImgWidth(DECODED_WIDTH), decodedImgHeight(DECODED_HEIGHT)
	{
		InitLayerImage(readFrom, IMAGE_OFFSET);
	}

	~ChituLayerImage()
	{
		delete[] rawData;
	}

	ChituLayerImage(const ChituLayerImage& copyFrom)
	{
		encodedImgSize = copyFrom.encodedImgSize;
		decodedImgWidth = copyFrom.decodedImgWidth;
		decodedImgHeight = copyFrom.decodedImgHeight;

		rawData = new char[encodedImgSize];

		std::memcpy(rawData, copyFrom.rawData, sizeof(char) * encodedImgSize);

		/*for (int i = 0; i < encodedImgSize; i++)
		{
			rawData[i] = copyFrom.rawData[i];
		}*/
	}

	void InitLayerImage(ChituInterLayerGCodeLine* readFrom, long int IMAGE_OFFSET)
	{
		rawData = new char[encodedImgSize];

		char* readCharArray = &readFrom->rawData[IMAGE_OFFSET];

		for (int i = 0; i < encodedImgSize; i++)
		{
			rawData[i] = readCharArray[i];
		}
	}
};

struct ChituLayerImageManager
{
	std::vector<ChituInterLayerGCodeLine*>* interLayerGCodeLines = nullptr;
	std::vector<ChituLayerImage*> layerImages;

	ChituLayerImageManager(std::vector<ChituInterLayerGCodeLine*>* lines, long int imageWidth, long int imageHeight)
		: interLayerGCodeLines(lines)
	{
		LoadImages(imageWidth, imageHeight);
	};

	~ChituLayerImageManager()
	{
		if (!layerImages.empty())
		{
			//we have to call delete since we called new!
			for (std::vector<ChituLayerImage*>::iterator image = layerImages.begin(); image != layerImages.end(); image++)
			{
				delete *image;
			}
		}

		layerImages.clear();
		interLayerGCodeLines = nullptr;
	}

	void LoadImages(long int imageWidth, long int imageHeight)
	{
		for (std::vector<ChituInterLayerGCodeLine*>::iterator line = (*interLayerGCodeLines).begin(); line != (*interLayerGCodeLines).end(); line++)
		{
			layerImages.push_back(new ChituLayerImage((*line), (*line)->GetImageOffsetInChunk(), (*line)->GetImageSize(), imageWidth, imageHeight));
		}
	}

	void ReportImages(std::ofstream* targetStream, int tabLevel = 0)
	{
		std::string tabString = "";

		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		for (int i = 0; i < layerImages.size(); i++)
		{
			*targetStream << tabString << "Layer " << i + 1 << " Image:" << std::endl;

			/* WARNING:	for large screens, this can make the program slow. It does work, but I'm
						disabling it for now to speed up development. After all, it's not likely
						the user will need to poke around the hex representation of the images so
						it's probably not crucial to include it in the log file. Useful for small images
						(16px x 16px) to see what the compression algorithm looks like though!*/

			//RawDataToStream(layerImages.at(i)->rawData, layerImages.at(i)->encodedImgSize, targetStream, tabLevel + 1);
		}
	}
};