#pragma once

//NOT DOCUMENTING THIS ONE AS WELL AS THE OTHERS
//BECAUSE EVENTUALLY THESE WILL BE IDENTIFIED
//AND PUT INTO A DIFFERENT FILE. LOOK AT THE OTHER
//DATABLOCK INHERITED STRUCTURES TO FIGURE OUT THIS,
//IT'S THE SAME THING!

#include "ChituDataBlock.h"

struct ChituUnknownDataBlock : public ChituDataBlock
{
	enum DataOffsets
	{
		BOTTOM_LIFT_DISTANCE = 0x00,
		BOTTOM_LIFT_SPEED = 0x04,
		LIFT_DISTANCE = 0x08,
		LIFT_SPEED = 0x0C,
		RETRACT_SPEED = 0x10,
		PRINT_VOLUME = 0x14,
		PRINT_WEIGHT = 0x18,
		PRINT_PRICE = 0x1C,

		MYSTERY_FLOAT_9 = 0x54,
		MACHINE_TYPE_ADDRESS = 0x58,
		MACHINE_TYPE_LENGTH = 0x5C,
		MYSTERY_FLOAT_10 = 0x64, //really a float?
		MYSTERY_INT_2 = 0x68,
		MYSTERY_FLOAT_11 = 0x6C,
		MYSTERY_FLOAT_12 = 0x70,
		MYSTERY_FLOAT_13 = 0x74,
		TRANSITION_LAYERS = 0x78,
		MYSTERY_DATA_ADDRESS = 0x7C
	};

	std::string machineType = "";


	ChituUnknownDataBlock(std::FILE* cFile, long int readOffset, long int bytesToRead)
		: ChituDataBlock(cFile, readOffset, bytesToRead)
	{
		Init();
		
		int size = GetMachineTypeStringLength();
		char* temp = new char[size];
		fseek(cFile, GetMachineTypeString(), SEEK_SET);
		fread(temp, sizeof(char), size, cFile);
		machineType = std::string(temp, size);
		delete[] temp;
	}

	~ChituUnknownDataBlock() {};

	void Init()
	{
		RegisterData(new ChituFloat(rawData, BOTTOM_LIFT_DISTANCE, "Bottom Lift Distance (mm)"), "BOTTOM_LIFT_DISTANCE");
		RegisterData(new ChituFloat(rawData, BOTTOM_LIFT_SPEED, "Bottom Lift Speed (mm/min)"), "BOTTOM_LIFT_SPEED");
		RegisterData(new ChituFloat(rawData, LIFT_DISTANCE, "Lift Distance (mm)"), "LIFT_DISTANCE");
		RegisterData(new ChituFloat(rawData, LIFT_SPEED, "Lift Speed (mm/min)"), "LIFT_SPEED");
		RegisterData(new ChituFloat(rawData, RETRACT_SPEED, "Retract Speed (mm/min)"), "RETRACT_SPEED");
		RegisterData(new ChituFloat(rawData, PRINT_VOLUME, "Print Volume (ml)"), "PRINT_VOLUME");
		RegisterData(new ChituFloat(rawData, PRINT_WEIGHT, "Print Weight (g)"), "PRINT_WEIGHT");
		RegisterData(new ChituFloat(rawData, PRINT_PRICE, "Print Price ($)"), "PRINT_PRICE");

		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_9), "MYSTERY_FLOAT_9");
		RegisterData(new ChituAddress(rawData, MACHINE_TYPE_ADDRESS, "Machine Type Address"), "MACHINE_TYPE_ADDRESS");
		RegisterData(new ChituInt(rawData, MACHINE_TYPE_LENGTH, "Length of Machine Type String"), "MACHINE_TYPE_LENGTH");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_10), "MYSTERY_FLOAT_10");
		RegisterData(new ChituInt(rawData, MYSTERY_INT_2), "MYSTERY_INT_2");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_11), "MYSTERY_FLOAT_11");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_12), "MYSTERY_FLOAT_12");
		RegisterData(new ChituFloat(rawData, MYSTERY_FLOAT_13), "MYSTERY_FLOAT_13");
		RegisterData(new ChituInt(rawData, TRANSITION_LAYERS, "Number of Transition Layers"), "TRANSITION_LAYERS");
		RegisterData(new ChituAddress(rawData, MYSTERY_DATA_ADDRESS, "Address of More Mystery Data"), "MYSTERY_DATA_ADDRESS");
	}

	long int GetMachineTypeString() { return GetValueByKey<long int>("MACHINE_TYPE_ADDRESS"); }
	long int GetMachineTypeStringLength() { return GetValueByKey<long int>("MACHINE_TYPE_LENGTH"); }
	long int GetAddressOfMysteryData() { return GetValueByKey<long int>("MYSTERY_DATA_ADDRESS"); }

	void ReportData(std::ostream* targetStream, int tabLevel = 0)
	{
		
		ChituDataBlock::ReportData(targetStream, tabLevel);
		std::string tabString = "";
		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');
		*targetStream << tabString << "Machine Type: " << machineType << "\n";

	}
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

		COPYRIGHT_TEXT_ADDRESS = 0x48,
		COPYRIGHT_SIZE = 0x4C
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
		RegisterData(new ChituAddress(rawData, COPYRIGHT_TEXT_ADDRESS, "Address of Copyright Text"), "COPYRIGHT_POINTER");
		RegisterData(new ChituInt(rawData, COPYRIGHT_SIZE, "Size of Copyright Text (bytes)"), "COPYRIGHT_SIZE");
	}

	long int GetCopyrightAddress() { return GetValueByKey<long int>("COPYRIGHT_POINTER"); }
	long int GetCopyrightDataSize() { return GetValueByKey<long int>("COPYRIGHT_SIZE"); }
};