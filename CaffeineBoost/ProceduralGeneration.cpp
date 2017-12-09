#include "ProceduralGeneration.h"

ProceduralGeneration::ProceduralGeneration()
{
	w = 5;
	h = 2;
	l = 30;
	wBuffer = 1;
	hBuffer = 1;
	lBuffer = 10;
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
	delete[] lanes;
	lanes = nullptr;

	for (int i = 0; i < batches.size(); i++) {
		delete batches[i];
	}
}

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

void ProceduralGeneration::NextBatch()
{
	batches.erase(batches.begin());
	difficulty += DIFFICULTY_INCREASE;
	Generate(HARD);
}

void ProceduralGeneration::Generate(int range)
{
	GenerateLanes(range, difficulty);
	AddBatch(GenerateBatch());
}

void ProceduralGeneration::GenerateLanes(int range, int difficulty)
{
	// Seed random numbers
	srand(static_cast<unsigned int>(time(NULL)));

	// Generate enum value for each lane
	int r = 0;
	while (true) {
		for (int i = 0; i < l; i += lBuffer) {
			while (true) {
				r = (rand() % range) + 1;
				if (lanes[i] <= r) {
					difficulty -= r;
					break;
				}
			}
			lanes[i] = r;
			if (difficulty <= 0) {
				return;
			}
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
	// This requires the bottom portion of the algorithm to be adopted
	// to accomodate multiple height values for each lane. Currently
	// hardcoded to have a max height of 2.

	// 2D Array that represents the width and height of one lane
	int* lane = new int[w * 2];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < w; j++) {
			lane[(i * w) + j] = 0;
		}
	}

	// Determine how many blocks will be placed in this lane
	int count = 0;
	switch (lanes[index]) {
	case NONE:
		return lane;
		break;
	case EASY:
		count = (w * 2) / 3;
		break;
	case MEDIUM:
		count = (w * 2) / 2;
		break;
	case HARD:
		count = (w * 2) / 1.05f;
		count = count == w * 2 ? count - 1 : count;
		break;
	}

	// Seed random numbers
	srand(static_cast<unsigned int>(time(NULL)));
	int r = 0;

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