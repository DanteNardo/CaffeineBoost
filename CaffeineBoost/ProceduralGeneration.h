#pragma once
#include <vector>
#include <random>
#include <time.h>

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

enum LaneType
{
	NONE,
	EASY,
	MEDIUM,
	HARD
};

class ProceduralGeneration
{
private:
	int w = 0, h = 0, l = 0;
	int wBuffer = 0, hBuffer = 0, lBuffer = 0;
	int difficulty = 0;
	const int DIFFICULTY_INCREASE = 5;
	std::vector<Batch*> batches;

	void GenerateLanes(int range, int difficulty);
	std::vector<int*> GenerateBatch();
	int* GenerateLane(int index);
	void AddBatch(std::vector<int*> batch);

public:
	ProceduralGeneration();
	ProceduralGeneration(int w, int h, int l, int wBuffer, int hBuffer, int lBuffer, int difficulty);
	~ProceduralGeneration();

	Batch* GetBatch();
	int GetWidth();
	int GetHeight();
	int GetLength();
	int* lanes = nullptr;

	void NextBatch();
	void Generate(int range);
};