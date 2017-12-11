// Author: Dante Nardo
// Last Modified: 12/10/2017
// Purpose: Generates batches of data procedurally. Batches instantiated by AppClass.

#include "ProceduralGeneration.h"

#pragma region Constructors And Deconstructors

ProceduralGeneration::ProceduralGeneration()
{
	w = 5;
	h = 2;
	l = 160;
	wBuffer = 1;
	hBuffer = 1;
	lBuffer = 40;
	difficulty = 15;

	lanes = new int[l];
	for (int i = 0; i < l; i++) {
		lanes[i] = 0;
	}
	batches = std::vector<Batch*>();

	Generate(EASY);
	Generate(MEDIUM);
}

ProceduralGeneration::ProceduralGeneration(int w, int h, int l, int wBuffer, int hBuffer, int lBuffer, int difficulty)
{
	this->w = w;
	this->h = h;
	this->l = l;
	this->wBuffer = wBuffer;
	this->hBuffer = hBuffer;
	this->lBuffer = lBuffer;
	this->difficulty = difficulty;

	lanes = new int[l];
	for (int i = 0; i < l; i++) {
		lanes[i] = 0;
	}
	batches = std::vector<Batch*>();
}

ProceduralGeneration::~ProceduralGeneration()
{
	// Free lanes array
	delete[] lanes;
	lanes = nullptr;

	// Free all Batch structs (which free a std::vector<int*>)
	for (int i = 0; i < batches.size(); i++) {
		delete batches[i];
	}
}

#pragma endregion

#pragma region Accessors

Batch* ProceduralGeneration::GetBatch()
{
	return batches[0];
}

std::vector<Batch*> ProceduralGeneration::GetBatches()
{
	return batches;
}

int ProceduralGeneration::GetWidth()
{
	return w;
}

int ProceduralGeneration::GetHeight()
{
	return h;
}

int ProceduralGeneration::GetLength()
{
	return l;
}


int ProceduralGeneration::GetLengthBuffer()
{
	return lBuffer;
}
#pragma endregion

#pragma region Public Creation And Iteration

void ProceduralGeneration::Generate(int range)
{
	// Generate enum values for each lane in the batch
	GenerateLanes(range, difficulty);

	// Generate coffee enum values for some random lanes
	GenerateCoffee();

	// Add the batch created based on enums to the batches vector
	AddBatch(GenerateBatch());
}

void ProceduralGeneration::NextBatch()
{
	// Erase earliest batch and add new batch to end
	batches.erase(batches.begin());
	difficulty += DIFFICULTY_INCREASE;
	Generate(HARD);
}

#pragma endregion

#pragma region Private Creation Of Batches

void ProceduralGeneration::GenerateLanes(int range, int difficulty)
{
	// Seed random numbers
	srand(static_cast<unsigned int>(time(NULL)));

	// Generate enum value for each lane
	int r = 0;
	while (true) {
		for (int i = 0; i < l; i += lBuffer) {

			// Create a random enum value
			// The lower the difficulty, the less 'points' to spend on each lane.
			while (true) {
				r = (rand() % range) + 1;
				if (lanes[i] <= r) {
					difficulty -= r;
					break;
				}
			}
			lanes[i] = r;

			// Return when we run out of points
			if (difficulty <= 0) {
				return;
			}
		}
	}
}

void ProceduralGeneration::GenerateCoffee()
{
	// Seed random numbers
	srand(static_cast<unsigned int>(time(NULL)));

	for (int i = 0; i < l; i++) {
		if (rand() % 2 == 0) {
			lanes[i] = COFFEE;
		}
	}
}

std::vector<int*> ProceduralGeneration::GenerateBatch()
{
	std::vector<int*> batch = std::vector<int*>();
	for (int i = 0; i < l; i++) {
		batch.push_back(GenerateLane(i));
	}
	return batch;
}

int* ProceduralGeneration::GenerateLane(int index)
{
	// TODO: Replace all instances of 2 with h
	// This requires the bottom portion of the algorithm to be adapted
	// to accomodate multiple height values for each lane. Currently
	// hardcoded to have a max height of 2.

	// 1D Array that represents the width and height of one lane
	// This array stores 2D information since:
	// index % width = x value
	// index / width = y value
	int* lane = new int[w * 2];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < w; j++) {
			lane[(i * w) + j] = 0;
		}
	}

	// Seed random numbers
	//srand(static_cast<unsigned int>(time(NULL)));
	int r = 0;

	// Determine how many blocks will be placed in this lane based on enum
	int count = 0;
	switch (lanes[index]) {
		case NONE:
			return lane;
			break;

		// A third of the total possible count
		case EASY:
			count = (w * 2) / 3;
			break;

		// Half of the total possible count
		case MEDIUM:
			count = (w * 2) / 2;
			break;

		// Most of the total possible count
		case HARD:
			count = (w * 2) / 1.2f;
			count = count == w * 2 ? count - 1 : count;
			break;

		// Special coffee case
		case COFFEE:
			r = rand() % w;
			lane[r] = 2;
			return lane;
	}

	// Fill bottom row if large count
	if (count >= w) {
		count -= w;
		for (int k = 0; k < w; k++) {
			lane[k] = 1;
		}

		// Fill top row with remaining small count
		while (count > 0) {
			r = rand() % w;
			if (lane[r + w] == 0) {
				lane[r + w] = 1;
				count--;
			}
		}
	}
	// Fill parts of bottom row if small count
	else {
		while (count > 0) {
			r = rand() % w;
			if (lane[r] == 0) {
				lane[r] = 1;
				count--;
			}
		}
	}
	

	return lane;
}

void ProceduralGeneration::AddBatch(std::vector<int*> data)
{
	batches.push_back(new Batch(data));
}

#pragma endregion