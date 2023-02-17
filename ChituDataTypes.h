#pragma once

#include <string>
#include <iostream>

#include "ChituFileInterpreter.h"

//possible ChituData types
enum DataType
{
	INT = 0,
	FLOAT = 1,
	ADDRESS = 2,
	SHORT = 3,
};

struct ChituData
{
	//varable to keep track
	//of the type
	DataType type;
	//size (in bytes)
	long int size = -1;
	//offset in the data block
	//this datum object belongs to
	long int offset = -1;
	//name of this datum
	std::string name = "";
	//pointer to the raw data for this
	//datum
	char* rawData;

	//constructor, takes a char array to read from, the offset into that char array to read at,
	//the type of data, and a human readable name
	ChituData(char* readFrom, long int offsetInBlock, DataType typeOfData, std::string humanReadableName = "UNKNOWN")
		: type{ typeOfData }, offset{ offsetInBlock }, name{ humanReadableName }, rawData{ readFrom }
	{
		//set the size of the data depending on the type
		switch (type)
		{
		//ints, floats, and addresses are all stored as
		//32 bits in the file, or 4 bytes.
		case INT:
		case FLOAT:
		case ADDRESS:
			size = 4;
			break;
		//might need to set this to 1 eventually
		//once I find out more information about the file
		//but it works for now.
		case SHORT:
			size = 2;
			break;
		}
	}

	//destructor
	~ChituData()
	{
		//again, probably overkill but it's good practice :)
		size = -1;
		offset = -1;
		rawData = nullptr;
	}
};


//ChituInt, which inherits from ChituData
struct ChituInt : public ChituData
{
	//value as a long int
	long int value = -1;

	//constructor, same as for ChituData BUT we indicate the type!
	ChituInt(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::INT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
	}
	
	//destructor
	~ChituInt()
	{
		value = -1;
	}
};

//ChituFloat, which inherits from ChituData
struct ChituFloat : public ChituData
{
	//value as a float
	float value = -1;

	//constructor, same as for ChituData BUT we indicate the type!
	ChituFloat(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::FLOAT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
	}

	//destructor
	~ChituFloat()
	{
		value = -1;
	}
};

//ChituAddress, which inherits from ChituData
struct ChituAddress : public ChituData
{
	//value as a long int
	long int value = -1;

	//constructor, same as for ChituData BUT we indicate the type!
	ChituAddress(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::ADDRESS, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
	}

	//destructor
	~ChituAddress()
	{
		value = -1;
	}
};

//ChituShort, which inherits from ChituData
struct ChituShort : public ChituData
{
	//value as a short int
	short int value = -1;

	//constructor, same as for ChituData BUT we indicate the type!
	ChituShort(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::SHORT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
	}

	//destructor
	~ChituShort()
	{
		value = -1;
	}
};