#include "ObstacleGenerator.h"
using namespace Simplex;

ObstacleGenerator* ObstacleGenerator::o_pInstance = nullptr;
//Simple 1D Perlin noise procedural generation
void ObstacleGenerator::Init(void)
{

}

void ObstacleGenerator::Release(void)
{
	//delete generated obstacles from list
}

ObstacleGenerator* ObstacleGenerator::GetInstance()
{
	if (o_pInstance == nullptr)
	{
		o_pInstance = new ObstacleGenerator();
	}
	return o_pInstance;
}
void ObstacleGenerator::ReleaseInstance()
{
	if (o_pInstance != nullptr)
	{
		delete o_pInstance;
		o_pInstance = nullptr;
	}
}

//The big 3
ObstacleGenerator::ObstacleGenerator() { Init(); }
ObstacleGenerator::ObstacleGenerator(ObstacleGenerator const& other) { }
ObstacleGenerator& ObstacleGenerator::operator=(ObstacleGenerator const& other) { return *this; }
ObstacleGenerator::~ObstacleGenerator() { Release(); };

//basic helper functions for pseudo rng and interpolation
//-----------------------------------
//linear congruential pseudo-random number generator
float ObstacleGenerator::PseudoRNG(void)
{
	std::srand(std::time(0)); // use current time as seed for random generator
	uint result = std::rand() * mMod; //glm::floor((uint)std::rand() * mMod); //seed

	result = (aMult * result + cInc) % mMod;

	return (result / mMod) - 0.5f;
};

//simple cosine interpolation
float ObstacleGenerator::Interpolate(float pa, float pb, float px)
{
	float ft = px * PI;
	float f = (1 - cos(ft)) * 0.5;

	return pa * (1 - f) + pb * f;
}
//-----------------------------------

void ObstacleGenerator::Perlin(int ampl, int wl, int width)
{
	int x = 0;
	int fq = 1 / wl;

	float a = PseudoRNG();
	float b = PseudoRNG();

	while (x < width) {
		if (x % wl == 0) {

			a = b;
			b = PseudoRNG();

			lanes[x] = (a * ampl);
		}
		else {

			lanes[x] = (Interpolate(a, b, (x % wl) / wl) * ampl);
		}
		x++;
	}
}

//generate obstacles from perlin noise
//first generate noise forward, 
void ObstacleGenerator::GenerateObstacles(int h, int w)
{
	Perlin(5, 5, 20);

	// obstacle checking here




}

// helper to just return specified block
int ObstacleGenerator::GetLanes(int ind)
{
	return lanes[ind];
}