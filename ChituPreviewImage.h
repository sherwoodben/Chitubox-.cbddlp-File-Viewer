#pragma once

#include <string>

#include "ChituDataBlock.h"

struct ChituPreviewImage
{
	//values for image properties that will
	//be read in
	long int decodedWidth = -1;
	long int decodedHeight = -1;
	long int imgStartAddress = -1;
	long int encodedImgSize = -1;

	//the raw encoded image data
	char* encodedData = nullptr;

	//the decoded image in RGBA form
	long int* imageAsRGBA = nullptr;

	//a bool to keep track of if the image is
	//converted or not
	bool converted = false;
	
	//constructor
	ChituPreviewImage(std::FILE* readFrom, long int width, long int height, long int address, long int size)
		: decodedWidth(width), decodedHeight(height), imgStartAddress(address), encodedImgSize(size)
	{
		//make the char array of the correct size
		encodedData = new char[encodedImgSize];
		//output some info to the console for debugging purposes
		std::cout << "\t> reading from address " << imgStartAddress << " to " << imgStartAddress + encodedImgSize << "\n";
		//actually read the file into the encoded data
		//array
		fseek(readFrom, address, SEEK_SET);
		fread(encodedData, sizeof(char), size, readFrom);
	}

	//destructor
	~ChituPreviewImage()
	{
		//only delete the imageAsRGBA if we actually
		//created it (i.e. it's been converted)
		if (converted) delete[] imageAsRGBA;

		//delete the encoded raw data
		delete[] encodedData;
	}

	//take the image from raw encoded data
	//to a long int RBGA representation
	void DecodeImage();

	//save the image to disk
	int SaveImage(std::string imgName = "");
};