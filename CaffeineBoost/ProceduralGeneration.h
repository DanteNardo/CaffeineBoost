// Author: Dante Nardo
// Last Modified: 12/8/2017
// Purpose: Generates batches of data procedurally. Batches instantiated by AppClass.

#pragma once
#include <vector>
#include <time.h>

// A container for a std::vector<int*>
// Instantiates and releases dynamic vector memory
struct Batch
{
	std::vector<int*> data;

	Batch() {
		data = std::vector<int*>();
	};

	Batch(std::vector<int*> data) {
		this->data = data;
	}

	~Batch() {
		for (int i = 0; i < data.size(); i++) {
			delete[] data[i];
			data[i] = nullptr;
		}
	}
};

// Enumerator of lanetype for sugar syntax
enum LaneType
{
	NONE,
	EASY,
	MEDIUM,
	HARD,
	COFFEE
};

class ProceduralGeneration
{
private:
	int w = 0, h = 0, l = 0;						// width, height, and length of batches
	int wBuffer = 0, hBuffer = 0, lBuffer = 0;		// width buffer, height buffer, and length buffer of batches
	int difficulty = 0;								// difficulty of batch generated
	const int DIFFICULTY_INCREASE = 1;				// const value to increase difficulty by each iteration
	int* lanes = nullptr;							// dynamic array of lane values (enum values for each lane)
	std::vector<Batch*> batches;					// list of batches created (currently always 2)

	void GenerateLanes(int range, int difficulty);	// generates the enum value for each lane
	void GenerateCoffee();							// generates coffee values in random empty lanes
	std::vector<int*> GenerateBatch();				// generates a single batch's data
	int* GenerateLane(int index, bool coffee);		// generates the 1D array that acts like a 2D array for each lane
	void AddBatch(std::vector<int*> batch);			// creates and adds a batch to batches based off of param data

public:
	
	// Constructors and Deconstructors
	ProceduralGeneration();
	ProceduralGeneration(int w, int h, int l, int wBuffer, int hBuffer, int lBuffer, int difficulty);
	~ProceduralGeneration();

	// Accessors
	Batch* GetBatch();
	std::vector<Batch*> GetBatches();
	int GetWidth();
	int GetHeight();
	int GetLength();

	int GetLengthBuffer();

	// Generate and Iteratioon
	void Generate(int range);
	void NextBatch();
};