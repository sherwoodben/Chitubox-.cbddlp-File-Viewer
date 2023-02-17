#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "ChituFileInterpreter.h"
#include"ChituDataTypes.h"

struct ChituDataBlock
{
	//size of the data block
	long int size = -1;
	//offset in the file
	long int offset = -1;
	
	//this is useful for the case where we
	//want to create a bunch of smaller objects
	//from a large block of raw data, say for
	//G-Code lines
	bool isSubsetOfLarger = false;

	//array for the raw data (will be created with new)
	char* rawData = nullptr;

	//a map to store a pointer to a datum in, accessed by a string for
	//the name of the datum (should really rename that "ChituDatum" but
	//oh well)-- we also keep track of the insertion order with a simple
	//vector of the strings, as well as the number of data entries
	//----NOTE: ChituData is NOT a "true" class (can't think of the name
	//			right now). We're making use of polymorphism here (I think?
	//			it's late and I can't keep my terminology straight) to store
	//			ints and floats and addresses and shorts as a single data type,
	//			which we then handle based on which specific type it is.
	//			Inheritance is fun :)
	std::map<std::string, ChituData*> data;
	std::vector<std::string> dataInsertionOrder;
	int dataEntries = 0;

	//constructor for a top-level data block; reads straight from the file
	ChituDataBlock(std::FILE* readFrom, long int readOffset, long int bytesToRead) : size{bytesToRead}, offset{readOffset}
	{
		//since we want to store the raw data in the top-level data block
		//object, we need to create space for it
		rawData = new char[size];

		//just some console stuff, will be removed later
		std::cout << "\t> reading from address " << readOffset << " to " << readOffset + bytesToRead << "\n";

		//actually read the contents of the file to the
		//char array we just made
		fseek(readFrom, readOffset, SEEK_SET);
		fread(rawData, sizeof(char), size, readFrom);
	}

	//constructor for a sub-level data block; reads from a char array
	ChituDataBlock(char* readFrom, long int readOffset, long int bytesToRead) : size{ bytesToRead }
	{
		//Note the lack of "new" for the rawData array--
		//we've (presumably) already done that for this
		//raw data since we're providing it in the form of
		//a char*; we just add the readOffset to the pointer
		//and have a pointer to the raw data we want to read!
		isSubsetOfLarger = true;
		rawData = readFrom + readOffset;
	}

	~ChituDataBlock()
	{
		//we ONLY want to delete the raw data IF we are a top-level data block
		//(and we actually allocated the memory in the first place)
		if (rawData != nullptr && !isSubsetOfLarger)
		{
			delete[] rawData;
		}

		//deletes all of the ChituData objects we created when we registered them
		//(this may actually not be needed actually, since we never
		//allocate any memory in creating a new ChituData object. Will look into this,
		//but for now it's gonna stay because I'm hesitant to remove something that could
		//lead to bad memory management.
		for (std::map<std::string, ChituData*>::iterator mapEntry = data.begin(); mapEntry != data.end(); mapEntry++)
		{
			delete mapEntry->second;
		}

		//just clears the vector
		dataInsertionOrder.clear();

		//set the rawData pointer to null and clean up, some of this is probably
		//a bit excessive
		rawData = nullptr;
		size = -1;
		dataEntries = 0;
	}

	//The most important part of the data block class-- the ability to
	//"register" an "arbitrary" data type which can be referred to by
	//a string as the key-- I say "arbitrary" because it's limited to
	//only those types defined by "ChituDataTypes.h"
	//see specializations for more notes.
	template <typename T>
	void RegisterData(const T& dataToRegister, std::string key)
	{
		data[key] = static_cast<ChituData*>(dataToRegister);
		dataInsertionOrder.push_back(key);
		dataEntries++;
	}

	//Possibly the second most important part of the data block class--
	//the ability to recall an "arbitrary" data type by simply
	//passing the key as a string! Returns the appropriate ChituDataType
	//depending on the type that the key-accessed datum indicates
	template <typename T>
	T GetValueByKey(std::string key)
	{
		//but we always want the unspecialized version
		//to not really work; does that mean templates aren't
		//the best way to do this? Not really sure.

		return -1;
	}

	//specialization for <long int>
	template <>
	long int GetValueByKey<long int>(std::string key)
	{
		try
		{
			//get a ChituData pointer
			ChituData* datum = data.at(key);

			//cast and return based on the type indicated
			//by the pointer we just got:
			//if it's a ChituInt
			if (datum->type == DataType::INT)
			{
				return (static_cast<ChituInt*>(datum))->value;
			}
			//if it's a ChituAddress
			else if (datum->type == DataType::ADDRESS)
			{
				return (static_cast<ChituAddress*>(datum))->value;
			}

			//return -1 if we somehow got here-- this isn't undefined
			//behavior because whenever we call GetValueByKey we also
			//provide a return type we expect. So it's all good. But really--
			//if we somehow get here it's not all good.
			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			//return -1 if we provide an invalid key
			return -1;
		}
	}

	//specialization for <float> ... basically same as <long int>!
	template <>
	float GetValueByKey<float>(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::FLOAT)
			{
				return (static_cast<ChituFloat*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	//specialization for <short int> ... basically same as <long int>!
	template <>
	short int GetValueByKey<short int>(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::SHORT)
			{
				return (static_cast<ChituShort*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	//a function that reports the data in the data block-- can be overridden
	//if a specific data block wants to implement its own report function
	//(image data blocks, for example)
	virtual void ReportData(std::ostream* targetStream, int tabLevel = 0)
	{
		//Note: we take a "target stream" here, which means we could
		//use the same function to report data to the console /or/ a file
		//which is pretty convenient

		//For every key stored in "dataInsertionOrder" we should
		//report that value
		for (std::vector<std::string>::iterator dataEntry = dataInsertionOrder.begin(); dataEntry != dataInsertionOrder.end(); dataEntry++)
		{
			//we build a tab string for formatting purposes;
			//we change the tab level to assist with organization.
			std::string tabString = "";
			for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

			//get a pointer to the datum
			ChituData* datum = data[*dataEntry];

			//if something went wrong, we don't even bother reporting
			//this datum
			if (datum == nullptr)
			{
				break;
			}

			//otherwise we get the name (which we provide when we register
			//the datum)
			*targetStream << tabString << datum->name << ": ";

			//and depending on the type of datum, we ask for a different
			//return type
			switch (datum->type)
			{
			//for ChituInts and ChituAddresses, which
			//we both want a long int from
			case DataType::INT:
			case DataType::ADDRESS:
				*targetStream << GetValueByKey<long int>(*dataEntry) << "\n";
				break;
			//for ChituFloats, which we want a float from
			case DataType::FLOAT:
				*targetStream << GetValueByKey<float>(*dataEntry) << "\n";
				break;
			//for ChituShorts, which we want a short int from (maybe even a char,
			//but we'll update that later (maybe))
			case DataType::SHORT:
				*targetStream << GetValueByKey<short int>(*dataEntry) << "\n";
				break;
			//should never get here but if we do then at least give an error indication
			default:
				*targetStream << "ERROR\n";
				break;
			}
		}
	}
};