#pragma once

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

enum ChituImageDataOffsets
{
	IMAGE_WIDTH = 0x00000000,
	IMAGE_HEIGHT = 0x00000004,
	IMAGE_DATA_ADDRESS = 0x00000008,
	IMAGE_DATA_SIZE = 0x0000000C
};

struct ChituImage
{
	std::FILE* file= nullptr;
	char* imageHeader = nullptr;
	char* imageData = nullptr;
	long int* imageAsRGBA = nullptr;
	ChituPointer imageHeaderStart;

	long int pixelsWide = 0;
	long int pixelsTall = 0;

	ChituPointer imageDataStart;
	long imageDataSize = 0;

	int imageID;

	ChituImage(std::FILE* file, ChituPointer startOfHeader, int IMG_HEADER_SIZE, int imgID = -1)
	{
		imageHeaderStart = startOfHeader;
		imageID = imgID;
		imageHeader = new char[IMG_HEADER_SIZE];
		fseek(file, imageHeaderStart.chituAddress, SEEK_SET);
		fread(imageHeader, sizeof(char), IMG_HEADER_SIZE, file);

		ReadFromBinary(pixelsWide, imageHeader, IMAGE_WIDTH);
		ReadFromBinary(pixelsTall, imageHeader, IMAGE_HEIGHT);
		ReadFromBinary(imageDataStart, imageHeader, IMAGE_DATA_ADDRESS);
		ReadFromBinary(imageDataSize, imageHeader, IMAGE_DATA_SIZE);

		imageData = new char[imageDataSize];
		imageAsRGBA = new long int[pixelsWide * pixelsTall];

		fseek(file, imageDataStart.chituAddress, SEEK_SET);
		fread(imageData, sizeof(char), imageDataSize, file);
	}

	~ChituImage()
	{
		delete[] imageHeader;
		delete[] imageData;
		delete[] imageAsRGBA;
	}

	void DecodeImage()
	{
		int d = 0;
		for (long i = 0; i < imageDataSize; i++)
		{
			long int b1, b2;
			ReadFromBinary(b1, imageData, i);
			ReadFromBinary(b2, imageData, i + 1);
			long int dot = b1 & 0xFF | ((b2 & 0xFF) << 8);
			i++;
		
			long int red = ((dot >> 11) & 0x1F) << 3;
			long int green = ((dot >> 6) & 0x1F) << 3;
			long int blue = (dot & 0x1F) << 3;
			long int alpha = 0xFF;
			long color = alpha<<24 | blue<<16 | green<<8 | red;
		
			int repeat = 1;
			if ((dot & 0x0020) == 0x0020)
			{
				i++;
				ReadFromBinary(b1, imageData, i);
				ReadFromBinary(b2, imageData, i + 1);
				repeat += b1 & 0xFF | ((b2 & 0x0F) << 8);
				i++;
			}
		
			while (repeat > 0)
			{
				imageAsRGBA[d++] = color;
				repeat--;
			}
		}
	}

	int SaveDecodedImage(std::string imgName)
	{
		std::string fileName = imgName + ".bmp";
		return stbi_write_bmp(fileName.c_str(), pixelsWide, pixelsTall, 4, imageAsRGBA);
	}

	void Report(std::ostream* targetStream = &std::cout, int tabLevel = 0)
	{
		std::string tabString;
		for (int i = 0; i < tabLevel; i++) tabString.append("\t");
		*targetStream << tabString << "Image " << imageID << " (located at  " << imageHeaderStart.chituAddress << "):" << std::endl;
		tabString.append("\t");
		*targetStream << tabString << "Image Width (px): " << pixelsWide << std::endl;
		*targetStream << tabString << "Image Height (px): " << pixelsTall << std::endl;
		*targetStream << std::endl;
		*targetStream << tabString << "Image Data Address: " << imageDataStart.chituAddress << std::endl;
		*targetStream << tabString << "Image Size (bytes): " << imageDataSize << std::endl;
		*targetStream << std::endl;
		*targetStream << tabString << "Image Header Raw Data (located at  " << imageHeaderStart.chituAddress << "):" << std::endl;

		tabString.append(1, '\t');

		std::string hexAsString = BinToHex(imageHeader, 16);

		int charactersPerGroup = 16;
		for (int i = 0; i < 16; i++)
		{
			std::string temp;

			if ((i % charactersPerGroup) == 0)
			{
				*targetStream << "\n" << tabString;
			}
			else if ((i % 8) == 0)
			{
				*targetStream << "| ";
			}
			*targetStream << hexAsString.substr(i * 2, 2) << " ";
		}
		*targetStream << "\n" << std::endl;
	}
};