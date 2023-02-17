#pragma once

#include <vector>
#include <thread>
#include <algorithm>

#include "ChituLayerImage.h"

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

	long long int numWhitePixels = 0;

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
			*targetStream << tabString << "Layer " << i + 1 << " Image:\n";
			std::cout << tabString;

			//REALLY ONLY DO THIS FOR SMALL IMAGES OTHERWISE IT'S GONNA BE A BAD TIME
			RawDataToStream(layerImages.at(i)->decodedImg, layerImages.at(i)->decodedImgSize, targetStream, tabLevel + 1);
		}
	}

	//multithreaded decoding of images!
	void DecodeImages()
	{
		//get the maximum number of threads the hardware will allow
		//to make this as fast as possible OR the number of layers in
		//the print-- no sense spawning 20 threads if only 15 are needed!
		int NUM_THREADS = std::min(std::thread::hardware_concurrency(), (unsigned int)interLayerGCodeLines.size());

		//if the hardware concurrency call fails it returns a 0, so this
		//evalueates to true
		if (!NUM_THREADS)
		{
			//in case of an error, just use a single thread:
			NUM_THREADS = 1;
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
			threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, &layerImages, i * binSize, ((i + 1) * binSize) - 1, std::ref(numWhitePixels))));
		}

		//handle the creation of the last (or only!) thread
		threads.emplace_back(std::move(std::thread(&ChituLayerImageManager::DecodeImagesInLayerRange, this, &layerImages, (NUM_THREADS - 1) * binSize, interLayerGCodeLines.size() - 1, std::ref(numWhitePixels))));

		//just update the user with how many threads were created
		std::cout << "\t> Created " << threads.size() << " threads! Currently decoding...\n";

		//while threads are working, tell how many images have been decoded
		while (imagesDecoded < interLayerGCodeLines.size())
		{
			//but only update if the value has changed!
			if (imagesDecoded > prevDecoded)
			{
				std::cout << "\t\t> Decoded " << imagesDecoded << " out of " << interLayerGCodeLines.size() << "\n";
				//increment the tracker
				prevDecoded = imagesDecoded;
			}
		}

		//wait for all the threads to join
		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}

		//output the number of white pixels for dubugging purposes
		std::cout << "\tTotal Number of Exposed Voxels: " << numWhitePixels << "\n";
	}

	//decodes the images in a range of layers, useful for multithreading
	void DecodeImagesInLayerRange(std::vector<ChituLayerImage*>* layerImages, long int startLayer, long int endLayer, long long int& pixelCounter)
	{
		for (int i = startLayer; i <= endLayer; i++)
		{
			(*layerImages)[i]->DecodeImage(pixelCounter);
			imagesDecoded += 1;
		}
	}

	//saves the images in a range of layers, useful for multithreading
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
		//to make this as fast as possible OR the number of layers in
		//the print-- no sense spawning 20 threads if only 15 are needed!
		int NUM_THREADS = std::min(std::thread::hardware_concurrency(), (unsigned int)interLayerGCodeLines.size());

		//if the hardware concurrency call fails it returns a 0, so this
		//evalueates to true
		if (!NUM_THREADS)
		{
			//in case of an error, just use a single thread:
			NUM_THREADS = 1;
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
		std::cout << "\t> Created " << threads.size() << " threads! Currently saving layer images...\n";

		//while threads are working, tell how many images have been saved
		while (imagesSaved < interLayerGCodeLines.size())
		{
			//but only update if the value has changed!
			if (imagesSaved > prevSaved)
			{
				std::cout << "\t\t> Saved " << imagesSaved << " out of " << interLayerGCodeLines.size() << "\n";
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