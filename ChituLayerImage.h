#pragma once

#include <bitset>
#include <thread>
#include <algorithm>

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


	//sticking with char* because it was way faster
	//when compared to something like a std::vector<bool>
	//--like way way way faster. Also allowed for nice multithreading
	//optimization :) Drawback: should in theory take up more space on the
	//heap (8 bytes per pixel instead of 1, which is sad)
	char* decodedImg = nullptr;

	ChituLayerImage(ChituInterLayerGCodeLine* readFrom, long int layerNumber, long int IMAGE_OFFSET, long int ENCODED_IMAGE_SIZE, long int DECODED_WIDTH, long int DECODED_HEIGHT)
		: layerNumber(layerNumber), encodedImgSize(ENCODED_IMAGE_SIZE), decodedImgWidth(DECODED_WIDTH), decodedImgHeight(DECODED_HEIGHT)
	{
		InitLayerImage(readFrom, IMAGE_OFFSET);
	}

	~ChituLayerImage()
	{
		if (rawData != nullptr)
		{
			delete[] rawData;
		}
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
		decodedImgSize = copyFrom.decodedImgSize;

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

		decodedImgSize = decodedImgWidth * decodedImgHeight;

		decodedImg = new char[decodedImgSize];

		//decodedBits.resize(decodedImgSize);
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
		//decodedImg = new char[decodedImgSize];

		//now we need to loop over every byte in the encoded image,
		//and add the appropriate pixels to the bool array
		long long int decodedIndex = 0;
		//std::cout << "Decoding Layer Image " << layerNumber << std::endl;
		for (long int encodedByte = 0; encodedByte < encodedImgSize; encodedByte++)
		{
			char byte = rawData[encodedByte];
			bool pixelOn = (byte & 0b10000000) >> 7;
			char repeatFor = (byte & 0b01111111);
			for (char repeatIndex = 0; repeatIndex < repeatFor; repeatIndex++)
			{	
				if (decodedIndex + repeatIndex < decodedImgSize) decodedImg[decodedIndex + repeatIndex] =  pixelOn * 255;
				//if (decodedIndex + repeatIndex < decodedBits.size()) decodedBits[decodedIndex + repeatIndex] = pixelOn;
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
	std::vector<ChituInterLayerGCodeLine*> interLayerGCodeLines;
	std::vector<ChituLayerImage*> layerImages;
	long int imagesDecoded = 0;
	long int imagesSaved = 0;

	long int imageWidth = -1;
	long int imageHeight = -1;

	ChituLayerImageManager(std::vector<ChituInterLayerGCodeLine*> lines, long int imageWidth, long int imageHeight)
		: interLayerGCodeLines(lines), imageWidth(imageWidth), imageHeight(imageHeight)
	{
	};

	~ChituLayerImageManager()
	{
		if (!layerImages.empty())
		{
			//we have to call delete since we called new
			for (std::vector<ChituLayerImage*>::iterator layerImage = layerImages.begin(); layerImage != layerImages.end(); layerImage++)
			{
				delete* layerImage;
			}
		}
	}

	void LoadImages()
	{
		long int layerCounter = 1;
		for (long int line = 0; line < interLayerGCodeLines.size(); line++)
		{
			ChituInterLayerGCodeLine* readFromLine = interLayerGCodeLines.at(line);

			layerImages.push_back(new ChituLayerImage(readFromLine,
				layerCounter, readFromLine->GetImageOffsetInChunk(),
				readFromLine->GetImageSize(), imageWidth, imageHeight));
			layerCounter++;
		}
	}

	//function that is used to output information about the layer images
	//to the log file (or console if that happens to be the target stream)

	/* WARNING:	for LCD large screens, this can make the program slow. It does work, but I
	recommend disabling it to speed up development. After all, it's not likely
	the user will need to poke around the hex representation of the images so
	it's probably not crucial to include it in the log file. Useful for small images
	(16px x 16px) to see what the compression algorithm looks like though!*/
	void ReportImages(std::ofstream* targetStream, int tabLevel = 0)
	{
		/*std::string tabString = "";

		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		for (int i = 0; i < interLayerGCodeLines.size(); i++)
		{
			*targetStream << tabString << "Layer " << i + 1 << " Image:" << std::endl;
			std::cout << tabString;
			for (int j = 0; j < layerImages.at(i)->decodedBits.capacity(); j += 4)
			{
				if ((j / layerImages.at(i)->decodedImgWidth) % 4 == 0)
				{
					if ((j) % imageWidth == 0)
					{
						std::cout << std::endl << tabString;
					}

					if (layerImages.at(i)->decodedBits.at(j)) std::cout << "1";
					else std::cout << "0";
				}
			}
			std::cout << std::endl;*/
			//RawDataToStream(layerImages.at(i)->decodedImg, layerImages.at(i)->decodedImgSize, targetStream, tabLevel + 1);
		//}
	}

	void DecodeImages()
	{
		//get the maximum number of threads the hardware will allow
		//to make this as fast as possible:
		int NUM_THREADS = std::thread::hardware_concurrency();
		if (!NUM_THREADS)
		{
			//in case of an error, just use a single thread:
			NUM_THREADS = 1;
		}

		if (NUM_THREADS > interLayerGCodeLines.size())
		{
			//in case there are less layers than threads,
			//spawn just enough threads:
			NUM_THREADS = interLayerGCodeLines.size();
		}
		std::vector<std::thread> threads;
		long int prevDecoded = -1;
		long int binSize = interLayerGCodeLines.size() / NUM_THREADS;

		for (int i = 0; i < NUM_THREADS - 1; i++)
		{
			threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, i, layerImages, i * binSize, ((i + 1) * binSize) - 1)));
		}

		threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, NUM_THREADS - 1, layerImages, (NUM_THREADS - 1) * binSize, interLayerGCodeLines.size() - 1)));

		std::cout << "\t> Created " << threads.size() << " threads! Currently decoding..." << std::endl;

		while (imagesDecoded < interLayerGCodeLines.size())
		{
			if (imagesDecoded > prevDecoded)
			{
				std::cout << "\t\t> Decoded " << imagesDecoded << " out of " << interLayerGCodeLines.size() << std::endl;
				prevDecoded = imagesDecoded;
			}
		}

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
	}

	void DecodeImagesInLayerRange(int thread, std::vector<ChituLayerImage*> layerImages, long int startLayer, long int endLayer)
	{
		for (int i = startLayer; i <= endLayer; i++)
		{
			layerImages[i]->DecodeImage();
			imagesDecoded += 1;
		}
	}

	void SaveImagesInLayerRange(int thread, std::vector<ChituLayerImage*> layerImages, long int startLayer, long int endLayer)
	{
		for (int i = startLayer; i <= endLayer; i++)
		{
			layerImages[i]->SaveImage();
			imagesSaved += 1;
		}
	}

	void SaveImages()
	{
		//bool success = true;
		/*for (int i = 0; i < interLayerGCodeLines.size(); i++)
		{
			success = layerImages.at(i)->SaveImage() && success;
		}*/

		//if (success) std::cout << "\t> All layer images successfully saved." << std::endl;
		//else std::cout << "\t> Error saving at least one layer image." << std::endl;

		//get the maximum number of threads the hardware will allow
		//to make this as fast as possible:
		int NUM_THREADS = std::thread::hardware_concurrency();
		if (!NUM_THREADS)
		{
			//in case of an error, just use a single thread:
			NUM_THREADS = 1;
		}

		if (NUM_THREADS > interLayerGCodeLines.size())
		{
			//in case there are less layers than threads,
			//spawn just enough threads:
			NUM_THREADS = interLayerGCodeLines.size();
		}
		std::vector<std::thread> threads;
		long int prevSaved = -1;
		long int binSize = interLayerGCodeLines.size() / NUM_THREADS;

		for (int i = 0; i < NUM_THREADS - 1; i++)
		{
			threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::SaveImagesInLayerRange, this, i, layerImages, i * binSize, ((i + 1) * binSize) - 1)));
		}

		threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::SaveImagesInLayerRange, this, NUM_THREADS - 1, layerImages, (NUM_THREADS - 1) * binSize, interLayerGCodeLines.size() - 1)));

		std::cout << "\t> Created " << threads.size() << " threads! Currently saving layer images..." << std::endl;

		while (imagesSaved < interLayerGCodeLines.size())
		{
			if (imagesSaved > prevSaved)
			{
				std::cout << "\t\t> Saved " << imagesSaved << " out of " << interLayerGCodeLines.size() << std::endl;
				prevSaved = imagesSaved;
			}
		}

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
	}
};