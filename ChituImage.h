#pragma once

#include <string>

#include "ChituDataBlock.h"

struct ChituPreviewImageHeader : public ChituDataBlock
{
	//found by poking around in a hex editor;
	//slight possibility I've made the wrong
	//conclusion
	enum DataOffsets
	{
		IMAGE_WIDTH = 0x00000000,
		IMAGE_HEIGHT = 0x00000004,
		IMAGE_DATA_ADDRESS = 0x00000008,
		IMAGE_DATA_SIZE = 0x0000000C
	};

	ChituPreviewImageHeader(std::FILE* readFrom, long int offset, long int bytesToRead)
		: ChituDataBlock(readFrom, offset, bytesToRead)
	{
		InitImageHeader();
	}

	//gets the information from the image headers
	void InitImageHeader()
	{
		RegisterData(new ChituInt(rawData, IMAGE_WIDTH, "Image Width (px)"), "IMAGE_X_PX");
		RegisterData(new ChituInt(rawData, IMAGE_HEIGHT, "Image Height (px)"), "IMAGE_Y_PX");
		RegisterData(new ChituInt(rawData, IMAGE_DATA_ADDRESS, "Image Data Address"), "IMAGE_ADDRESS");
		RegisterData(new ChituInt(rawData, IMAGE_DATA_SIZE, "Image Size (bytes)"), "IMAGE_SIZE");
	}

	~ChituPreviewImageHeader() {};

	long int GetWidth() { return GetValueByKey<long int>("IMAGE_X_PX"); }
	long int GetHeight() { return GetValueByKey<long int>("IMAGE_Y_PX"); }
	long int GetAddress() { return GetValueByKey<long int>("IMAGE_ADDRESS"); }
	long int GetSize() { return GetValueByKey<long int>("IMAGE_SIZE"); }
};

struct ChituPreviewImage
{
	long int decodedWidth = -1;
	long int decodedHeight = -1;
	long int imgStartAddress = -1;
	long int encodedImgSize = -1;

	char* encodedData = nullptr;

	bool converted = false;
	long int* imageAsRGBA = nullptr;

	ChituPreviewImage(std::FILE* cFile, long int width, long int height, long int address, long int size)
		: decodedWidth(width), decodedHeight(height), imgStartAddress(address), encodedImgSize(size)
	{
		encodedData = new char[encodedImgSize];
		fseek(cFile, address, SEEK_SET);
		fread(encodedData, sizeof(char), size, cFile);
	}

	~ChituPreviewImage()
	{
		if (converted) delete[] imageAsRGBA;
		delete[] encodedData;
	}

	void DecodeImage();

	int SaveImage(std::string imgName = "");
};