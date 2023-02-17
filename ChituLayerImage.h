#pragma once

#include "stb_image.h"
#include "stb_image_write.h"

#include "ChituInterLayerGCodeLine.h"
#include "ChituDataBlock.h"

struct ChituLayerImage
{
	//char array for the raw data
	char* rawData = nullptr;
	//to keep track of which layer this
	//image is for
	long int layerNumber = -1;
	//size (bytes) of the encoded image
	long int encodedImgSize = -1;
	//decoded image properties
	long int decodedImgWidth = -1;
	long int decodedImgHeight = -1;
	//decoded image size (bytes)
	long int decodedImgSize = -1;

	//sticking with char* because it was way faster
	//when compared to something like a std::vector<bool>
	//--like way way way faster. Also allowed for nice multithreading
	//optimization :) Drawback: should in theory take up more space on the
	//heap (8 bytes per pixel instead of 1, which is sad)
	char* decodedImg = nullptr;

	//constructor, takes a pointer to an inter-layer g-code line (where the image is located), the layer number,
	//the offset (in bytes) for the image into the inter-layer g-code raw data, the size of the encoded image (in bytes),
	//and the dimensions in px of the decoded image
	ChituLayerImage(ChituInterLayerGCodeLine* readFrom, long int layerNumber, long int IMAGE_OFFSET, long int ENCODED_IMAGE_SIZE, long int DECODED_WIDTH, long int DECODED_HEIGHT)
		: layerNumber(layerNumber), encodedImgSize(ENCODED_IMAGE_SIZE), decodedImgWidth(DECODED_WIDTH), decodedImgHeight(DECODED_HEIGHT)
	{
		//initialize the layer image, passing
		//the pointer to the inter-layer g-code
		//and the offset into that raw data
		InitLayerImage(readFrom, IMAGE_OFFSET);
	}

	//destructor
	~ChituLayerImage()
	{
		////if we actually allocated new memory
		////for the raw data, we need to delete it
		//if (rawData != nullptr)
		//{
		//	delete[] rawData;
		//}
		//then set the pointer to null
		rawData = nullptr;

		//if we actually allocated new memory
		//for the decoded image, we need to delete
		//it
		if (decodedImg)
		{
			delete[] decodedImg;
		}
		//then set the pointer to null
		decodedImg = nullptr;
	}

	//Initialize the layer image by getting things ready to decode
	void InitLayerImage(ChituInterLayerGCodeLine* readFrom, long int IMAGE_OFFSET)
	{
		//we already read the data to a char array, so we just need
		//the pointer to that data
		rawData = &readFrom->rawData[IMAGE_OFFSET];

		//allocate the memory for the decoded image
		//(first calculate how much memory we need!)
		decodedImgSize = decodedImgWidth * decodedImgHeight;
		decodedImg = new char[decodedImgSize];
	}

	//actually decode the images!
	void DecodeImage(long long int& pixelCounter)
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

		//we need to loop over every byte in the encoded image,
		//and add the appropriate pixels to the bool array
		
		//so we start at decoded index 0
		long long int decodedIndex = 0;

		long long int tempPixelCounter = 0;
		
		for (long int encodedByte = 0; encodedByte < encodedImgSize; encodedByte++)
		{
			//get the actual byte we want to analyze in the
			//encoded data
			char byte = rawData[encodedByte];
			//get our pixel value (1 or 0)
			bool pixelOn = (byte & 0b10000000) >> 7;
			//get our repeat value
			char repeatFor = (byte & 0b01111111);
			//then set values accordingly
			for (char repeatIndex = 0; repeatIndex < repeatFor; repeatIndex++)
			{	
				if (decodedIndex + repeatIndex < decodedImgSize) decodedImg[decodedIndex + repeatIndex] =  pixelOn * 255;
				if (pixelOn) tempPixelCounter++;
				//if (decodedIndex + repeatIndex < decodedBits.size()) decodedBits[decodedIndex + repeatIndex] = pixelOn;
			}
			//increment the decoded index
			decodedIndex += repeatFor;
		}

		//was getting some strange errors if I added every single time in the above loop,
		//so created a temporary accumulator to account for that. Seems to work
		//consistently now.
		pixelCounter += tempPixelCounter;
	}

	//saves the image to disk using stbi_write implementation
	bool SaveImage()
	{
		//name for the file, built from the layer number and a
		//hard coded (relative)directory for now
		std::string fileName = "OUTPUT\\LAYER IMAGES\\Layer " + std::to_string(layerNumber) + ".bmp";

		//returns false if there's an error saving the file
		return stbi_write_bmp(fileName.c_str(), decodedImgWidth, decodedImgHeight, 1, decodedImg);
	}
};

