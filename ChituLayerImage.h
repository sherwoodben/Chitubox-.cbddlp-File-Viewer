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
		//if we actually allocated new memory
		//for the raw data, we need to delete it
		if (rawData != nullptr)
		{
			delete[] rawData;
		}
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
		//HEY! THIS IS REALLY INEFFICIENT-- WE ALREADY HAVE
		//THE RAW DATA IN A CHAR ARRAY, SO WHY ALLOCATE MORE
		//MEMORY JUST TO COPY IT?
		//need to refactor this!
		rawData = new char[encodedImgSize];

		//like for real doing something like this should have been
		//a major red flag what was I thinking
		char* readCharArray = &readFrom->rawData[IMAGE_OFFSET];

		//this really pains me, I'm doing everything in my power to just
		//keep documenting the code instead of fixing this right now
		for (int i = 0; i < encodedImgSize; i++)
		{
			rawData[i] = readCharArray[i];
		}

		//okay at least this is acceptable; allocate the memory
		//for the decoded image (first calculate how much memory
		//we need!)
		decodedImgSize = decodedImgWidth * decodedImgHeight;

		decodedImg = new char[decodedImgSize];

		//decodedBits.resize(decodedImgSize);
	}

	//actually decode the images!
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

		//we need to loop over every byte in the encoded image,
		//and add the appropriate pixels to the bool array
		
		//so we start at decoded index 0
		long long int decodedIndex = 0;
		
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
				//if (decodedIndex + repeatIndex < decodedBits.size()) decodedBits[decodedIndex + repeatIndex] = pixelOn;
			}
			//increment the decoded index
			decodedIndex += repeatFor;
		}
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

//manages all the ChituLayerImage objects!
struct ChituLayerImageManager
{
	//a vector of pointers to inter-layer g-code line objects from
	//which we pull the raw image data
	std::vector<ChituInterLayerGCodeLine*> interLayerGCodeLines;
	//a vector of pointers to ChituLayerImage objects which
	//are the actual layer images
	std::vector<ChituLayerImage*> layerImages;
	//counters to keep track of how many images we've decoded
	long int imagesDecoded = 0;
	//and saved
	long int imagesSaved = 0;

	//decoded image dimensions
	long int imageWidth = -1;
	long int imageHeight = -1;

	//constructor, takes a vector of inter-layer g-code lines (PASSED BY REFERENCE SO IT DOESN'T GET COPIED!)
	//and the dimensions of the decoded image
	ChituLayerImageManager(std::vector<ChituInterLayerGCodeLine*>& lines, long int imageWidth, long int imageHeight)
		: interLayerGCodeLines(lines), imageWidth(imageWidth), imageHeight(imageHeight)
	{
	};

	//destructor
	~ChituLayerImageManager()
	{
		//only need to do this if we've actually allocated
		//memory
		if (!layerImages.empty())
		{
			//we have to call delete since we called new
			for (std::vector<ChituLayerImage*>::iterator layerImage = layerImages.begin(); layerImage != layerImages.end(); layerImage++)
			{
				delete* layerImage;
			}
		}
	}

	//function to initialize all of the layer images--NOT to decode them!
	//this just makes a layer image object for each image
	void InitImages()
	{
		//we start our layer counter at 1 since this doesn't
		//need to be zero indexed-- it makes more sense for the
		//layers to start at 1 in this case
		long int layerCounter = 1;

		//loop through the number of layers
		for (long int line = 0; line < interLayerGCodeLines.size(); line++)
		{
			//get a pointer to the inter-layer g-code line at the index
			ChituInterLayerGCodeLine* readFromLine = interLayerGCodeLines.at(line);

			//then, add to the vector of layer image pointers a new ChituLayerImage,
			//reading from the line we just got a pointer to, with the data offset
			//dictated by the line, and the encoded image size dictated by the line.
			//The decoded image dimensions are passed as well.
			layerImages.push_back(new ChituLayerImage(readFromLine,
				layerCounter, readFromLine->GetImageOffsetInChunk(),
				readFromLine->GetImageSize(), imageWidth, imageHeight));

			//increment the layer counter
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
		//build up the tab string for organizational
		//purposes
		std::string tabString = "";
		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		//then for every image just dump the decoded image to the
		//output stream
		for (int i = 0; i < interLayerGCodeLines.size(); i++)
		{
			*targetStream << tabString << "Layer " << i + 1 << " Image:" << std::endl;
			std::cout << tabString;

			//REALLY ONLY DO THIS FOR SMALL IMAGES OTHERWISE IT'S GONNA BE A BAD TIME
			RawDataToStream(layerImages.at(i)->decodedImg, layerImages.at(i)->decodedImgSize, targetStream, tabLevel + 1);
		}
	}

	//multithreaded decoding of images!
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

		//in case there are less layers than threads,
		//spawn just enough threads:
		if (NUM_THREADS > interLayerGCodeLines.size())
		{
			NUM_THREADS = interLayerGCodeLines.size();
		}

		//a vector to store all the threads in
		std::vector<std::thread> threads;

		//counter to keep the user updated
		long int prevDecoded = -1;
		//number of images each thread needs to decode
		long int binSize = interLayerGCodeLines.size() / NUM_THREADS;

		//create all the threads! need to use std::move since you can't
		//copy threads! Note this generates one less thread than required,
		//such that the last thread can handle any images that could not
		//be divided evenly amongst the others
		for (int i = 0; i < NUM_THREADS - 1; i++)
		{
			//since DecodeImagesInLayerRange is a member function, we also need to
			//pass a reference to this so it knows where to look for the data;
			//we pass in the appropriate range of layers based on the number of threads
			threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, &layerImages, i * binSize, ((i + 1) * binSize) - 1)));
		}

		//handle the creation of the last (or only!) thread
		threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, &layerImages, (NUM_THREADS - 1) * binSize, interLayerGCodeLines.size() - 1)));

		//just update the user with how many threads were created
		std::cout << "\t> Created " << threads.size() << " threads! Currently decoding..." << std::endl;

		//while threads are working, tell how many images have been decoded
		while (imagesDecoded < interLayerGCodeLines.size())
		{
			//but only update if the value has changed!
			if (imagesDecoded > prevDecoded)
			{
				std::cout << "\t\t> Decoded " << imagesDecoded << " out of " << interLayerGCodeLines.size() << std::endl;
				//increment the tracker
				prevDecoded = imagesDecoded;
			}
		}

		//wait for all the threads to join
		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
	}

	//decodes the images in a range of layers, usefule for multithreading
	void DecodeImagesInLayerRange(std::vector<ChituLayerImage*>* layerImages, long int startLayer, long int endLayer)
	{
		for (int i = startLayer; i <= endLayer; i++)
		{
			(*layerImages)[i]->DecodeImage();
			imagesDecoded += 1;
		}
	}

	//saves the images in a range of layers, usefule for multithreading
	void SaveImagesInLayerRange(std::vector<ChituLayerImage*>* layerImages, long int startLayer, long int endLayer)
	{
		for (int i = startLayer; i <= endLayer; i++)
		{
			(*layerImages)[i]->SaveImage();
			imagesSaved += 1;
		}
	}

	void SaveImages()
	{
		//get the maximum number of threads the hardware will allow
		//to make this as fast as possible:
		int NUM_THREADS = std::thread::hardware_concurrency();
		if (!NUM_THREADS)
		{
			//in case of an error, just use a single thread:
			NUM_THREADS = 1;
		}

		//in case there are less layers than threads,
		//spawn just enough threads:
		if (NUM_THREADS > interLayerGCodeLines.size())
		{
			NUM_THREADS = interLayerGCodeLines.size();
		}

		//a vector to store all the threads in
		std::vector<std::thread> threads;

		//counter to keep the user updated
		long int prevSaved = -1;

		//number of images each thread needs to save
		long int binSize = interLayerGCodeLines.size() / NUM_THREADS;

		//create all the threads! need to use std::move since you can't
		//copy threads! Note this generates one less thread than required,
		//such that the last thread can handle any images that could not
		//be divided evenly amongst the others
		for (int i = 0; i < NUM_THREADS - 1; i++)
		{
			//since SaveImagesInLayerRange is a member function, we also need to
			//pass a reference to this so it knows where to look for the data;
			//we pass in the appropriate range of layers based on the number of threads
			threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::SaveImagesInLayerRange, this, &layerImages, i * binSize, ((i + 1) * binSize) - 1)));
		}

		//handle the creation of the last (or only!) thread
		threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::SaveImagesInLayerRange, this, &layerImages, (NUM_THREADS - 1) * binSize, interLayerGCodeLines.size() - 1)));

		//just update the user with how many threads were created
		std::cout << "\t> Created " << threads.size() << " threads! Currently saving layer images..." << std::endl;

		//while threads are working, tell how many images have been saved
		while (imagesSaved < interLayerGCodeLines.size())
		{
			//but only update if the value has changed!
			if (imagesSaved > prevSaved)
			{
				std::cout << "\t\t> Saved " << imagesSaved << " out of " << interLayerGCodeLines.size() << std::endl;
				//increment the tracker
				prevSaved = imagesSaved;
			}
		}

		//wait for all the threads to join
		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
	}
};