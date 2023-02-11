#pragma once

#include "stb_image.h"
#include "stb_image_write.h"

#include "ChituGCode.h"
#include "ChituDataBlock.h"

struct ChituLayerImage
{
	char* rawData = nullptr;

	long int layerNumber = -1;

	long int encodedImgSize = -1;

	long int decodedImgWidth = -1;
	long int decodedImgHeight = -1;
	long int decodedImgSize = -1;

	char* decodedImg = nullptr;

	ChituLayerImage(ChituInterLayerGCodeLine* readFrom, long int layerNumber, long int IMAGE_OFFSET, long int ENCODED_IMAGE_SIZE, long int DECODED_WIDTH, long int DECODED_HEIGHT)
		: layerNumber(layerNumber), encodedImgSize(ENCODED_IMAGE_SIZE), decodedImgWidth(DECODED_WIDTH), decodedImgHeight(DECODED_HEIGHT)
	{
		InitLayerImage(readFrom, IMAGE_OFFSET);
		decodedImgSize = decodedImgWidth * decodedImgHeight;
	}

	~ChituLayerImage()
	{
		delete[] rawData;
		rawData = nullptr;

		if (decodedImg)
		{
			delete[] decodedImg;
		}
		decodedImg = nullptr;
	}

	ChituLayerImage(const ChituLayerImage& copyFrom)
	{
		encodedImgSize = copyFrom.encodedImgSize;
		decodedImgWidth = copyFrom.decodedImgWidth;
		decodedImgHeight = copyFrom.decodedImgHeight;

		rawData = new char[encodedImgSize];

		std::memcpy(rawData, copyFrom.rawData, sizeof(char) * encodedImgSize);
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

	void DecodeImage()
	{
		//Images are encoded using what appears to be runtime length
		//encoding. Since pixels are either on or off, the MOST significant
		//bit (either a 1 or a 0) say whether the pixel is on (1) or off (0).
		//the remaining 7 bits in the byte is how many pixels the color repeats for.

		//	FOR EXAMPLE:
		//	In the small 16px x 16px screen example, an "empty screen" is represented
		//	as
		//		7D 7D 06
		//
		//	Note, three bytes. As bits, this is
		//
		//		01111101 01111101 00000110
		//
		//	This is a 0 (black pixel) repeated 1111101 (125) times
		//	then its a 0 (black pixel) repeated 1111101 (125) times
		//	then its a 0 (black pixel) repeated 0000110 (6) times
		//	for a total of 256 black pixels, as desired!

		//first create the boolean array of the appropriate size
		decodedImg = new char[decodedImgSize];

		//now we need to loop over every byte in the encoded image,
		//and add the appropriate pixels to the bool array
		long int decodedIndex = 0;
		for (long int encodedByte = 0; encodedByte < encodedImgSize; encodedByte++)
		{
			char byte = rawData[encodedByte];
			char pixelOn = (byte & 0b10000000) >> 7;
			char repeatFor = (byte & 0b01111111);
			for (char repeatIndex = 0; repeatIndex < repeatFor; repeatIndex++)
			{	
				if (decodedIndex + repeatIndex < decodedImgSize) decodedImg[decodedIndex + repeatIndex] =  pixelOn * 255;
			}
			decodedIndex += repeatFor;
		}
	}

	bool SaveImage()
	{
		std::string fileName = "OUTPUT\\LAYER IMAGES\\Layer " + std::to_string(layerNumber) + ".bmp";

		return stbi_write_bmp(fileName.c_str(), decodedImgWidth, decodedImgHeight, 1, decodedImg);
	}
};

struct ChituLayerImageManager
{
	std::vector<ChituInterLayerGCodeLine*>* interLayerGCodeLines = nullptr;
	std::vector<ChituLayerImage*> layerImages;

	long int imageWidth = -1;
	long int imageHeight = -1;

	ChituLayerImageManager(std::vector<ChituInterLayerGCodeLine*>* lines, long int imageWidth, long int imageHeight)
		: interLayerGCodeLines(lines), imageWidth(imageWidth), imageHeight(imageHeight)
	{
		//LoadImages(imageWidth, imageHeight);
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

	void LoadImages()
	{
		long int layerCounter = 1;
		for (std::vector<ChituInterLayerGCodeLine*>::iterator line = (*interLayerGCodeLines).begin(); line != (*interLayerGCodeLines).end(); line++)
		{
			layerImages.push_back(new ChituLayerImage((*line), layerCounter, (*line)->GetImageOffsetInChunk(), (*line)->GetImageSize(), imageWidth, imageHeight));
			layerCounter++;
		}
	}

	//function that is used to output information about the layer images
	//to the log file (or console if that happens to be the target stream)

	/* WARNING:	for large screens, this can make the program slow. It does work, but I
	recommend disabling it to speed up development. After all, it's not likely
	the user will need to poke around the hex representation of the images so
	it's probably not crucial to include it in the log file. Useful for small images
	(16px x 16px) to see what the compression algorithm looks like though!*/
	void ReportImages(std::ofstream* targetStream, int tabLevel = 0)
	{
		std::string tabString = "";

		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		for (int i = 0; i < layerImages.size(); i++)
		{
			*targetStream << tabString << "Layer " << i + 1 << " Image:" << std::endl;
			RawDataToStream((char*)layerImages.at(i)->decodedImg, layerImages.at(i)->decodedImgSize, targetStream, tabLevel + 1);
		}
	}

	void DecodeImages()
	{
		for (int i = 0; i < layerImages.size(); i++)
		{
			layerImages.at(i)->DecodeImage();
		}
	}

	void SaveImages()
	{
		bool success = true;
		for (int i = 0; i < layerImages.size(); i++)
		{
			success = layerImages.at(i)->SaveImage() && success;
		}

		if (success) std::cout << "\t> All layer images successfully saved." << std::endl;
		else std::cout << "\t> Error saving at least one layer image." << std::endl;
	}
};