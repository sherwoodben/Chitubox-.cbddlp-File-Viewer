#pragma once


#include "ChituDataBlock.h"

struct ChituUnknownDataBlock : public ChituDataBlock
{
	enum DataOffsets
	{
		MYSTERY_FLOAT_1 = 0x00,
		MYSTERY_FLOAT_2 = 0x04,
		MYSTERY_FLOAT_3 = 0x08,
		MYSTERY_FLOAT_4 = 0x0C,
		MYSTERY_FLOAT_5 = 0x10,
		MYSTERY_FLOAT_6 = 0x14,
		MYSTERY_FLOAT_7 = 0x18,
		MYSTERY_FLOAT_8 = 0x1C,

		MYSTERY_FLOAT_9 = 0x54,
		COPYRIGHT_TEXT_ADDRESS = 0x58,
		MYSTERY_INT_1 = 0x5C,
		MYSTERY_FLOAT_10 = 0x64, //really a float?
		MYSTERY_INT_2 = 0x68,
		MYSTERY_FLOAT_11 = 0x6C,
		MYSTERY_FLOAT_12 = 0x70,
		MYSTERY_FLOAT_13 = 0x74,
		MYSTERY_DATA_ADDRESS = 0x7C
	};


	ChituUnknownDataBlock(std::FILE* cFile, long int readOffset, long int bytesToRead)
		: ChituDataBlock(cFile, readOffset, bytesToRead)
	{
		Init();
	}

	~ChituUnknownDataBlock() {};

	void Init()
	{
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_1), "MYSTERY_FLOAT_1");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_2), "MYSTERY_FLOAT_2");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_3), "MYSTERY_FLOAT_3");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_4), "MYSTERY_FLOAT_4");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_5), "MYSTERY_FLOAT_5");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_6), "MYSTERY_FLOAT_6");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_7), "MYSTERY_FLOAT_7");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_8), "MYSTERY_FLOAT_8");

		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_9), "MYSTERY_FLOAT_9");
		RegisterData(new ChituAddress(rawData, COPYRIGHT_TEXT_ADDRESS, "Copyright Text Address"), "COPYRIGHT_TEXT_ADDRESS");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_1), "MYSTERY_INT_1");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_10), "MYSTERY_FLOAT_10");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_2), "MYSTERY_INT_2");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_11), "MYSTERY_FLOAT_11");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_12), "MYSTERY_FLOAT_12");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_13), "MYSTERY_FLOAT_13");
		RegisterData(new ChituAddress(rawData, MYSTERY_DATA_ADDRESS, "Address of More Mystery Data"), "MYSTERY_DATA_ADDRESS");
	}

	long int GetCopyrightAddress() { return GetValueByKey<long int>("COPYRIGHT_TEXT_ADDRESS"); }
	long int GetAddressOfMysteryData() { return GetValueByKey<long int>("MYSTERY_DATA_ADDRESS"); }
	long int GetCopyrightDataSize() { return (GetAddressOfMysteryData() - GetCopyrightAddress()); }
};

struct ChituMysteryDataBlock : public ChituDataBlock
{
	enum DataOffsets
	{
		MYSTERY_FLOAT_1 = 0x00,
		MYSTERY_FLOAT_2 = 0x0C,
		MYSTERY_FLOAT_3 = 0x14,
		MYSTERY_FLOAT_4 = 0x18,
		MYSTERY_FLOAT_5 = 0x1C,
		MYSTERY_FLOAT_6 = 0x20,
		MYSTERY_FLOAT_7 = 0x28,
		MYSTERY_INT_1 = 0x2C,
		MYSTERY_INT_2 = 0x30,
		MYSTERY_INT_3 = 0x34,

		POINTER_TO_L_IN_COPYRIGHT_TEXT = 0x48,
		MYSTERY_INT_4 = 0x4C
	};

	ChituMysteryDataBlock(std::FILE* cFile, long int readOffset, long int bytesToRead)
		: ChituDataBlock(cFile, readOffset, bytesToRead)
	{
		Init();
	}

	~ChituMysteryDataBlock() {};

	void Init()
	{
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_1), "MYSTERY_FLOAT_1");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_2), "MYSTERY_FLOAT_2");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_3), "MYSTERY_FLOAT_3");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_4), "MYSTERY_FLOAT_4");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_5), "MYSTERY_FLOAT_5");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_6), "MYSTERY_FLOAT_6");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_7), "MYSTERY_FLOAT_7");

		RegisterData(new ChituInt(rawData, MYSTERY_INT_1), "MYSTERY_INT_1");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_2), "MYSTERY_INT_2");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_3), "MYSTERY_INT_3");
		RegisterData(new ChituAddress(rawData, POINTER_TO_L_IN_COPYRIGHT_TEXT, "Pointer to 'L' in Copyright Text"), "POINTER_TO_L");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_4), "MYSTERY_INT_4");
	}
};