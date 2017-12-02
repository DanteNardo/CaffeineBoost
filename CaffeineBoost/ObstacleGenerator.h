#ifndef __ObstacleGenerator_H_
#define __ObstacleGenerator_H_

#include "Definitions.h"

namespace Simplex
{
	class ObstacleGenerator
	{

		//values to set up seed
		uint mMod = 134456; //modulus
		uint aMult = 8121; // multiplier
		uint cInc = 28411; //increment

		int lanes[20]; //starting with 20 as a base view distance
		static ObstacleGenerator* o_pInstance; // Singleton pointer

	public:
		//Singleton Methods
		/*
		USAGE: Gets/Constructs the singleton pointer
		ARGUMENTS:
		OUTPUT:
		*/
		static ObstacleGenerator* GetInstance();
		/*
		USAGE: Destroys the singleton
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		static void ReleaseInstance(void);

		void GenerateObstacles(int h, int w);
		int GetLanes(int index);

	private:
		//Rule of 3
		/*
		USAGE: Constructor
		ARGUMENTS: ---
		OUTPUT:
		*/
		ObstacleGenerator(void);
		/*
		USAGE: Copy Constructor
		ARGUMENTS:
		OUTPUT:
		*/
		ObstacleGenerator(ObstacleGenerator const& other);
		/*
		USAGE: Copy Assignment Operator
		ARGUMENTS:
		OUTPUT:
		*/
		ObstacleGenerator& operator=(ObstacleGenerator const& other);
		/*
		USAGE: Destructor
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		~ObstacleGenerator(void);
		//Other methods
		/*
		USAGE: Initializes the objects fields
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Init(void);
		/*
		USAGE: Releases the objects memory
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Release(void);

		float PseudoRNG(void);
		float Interpolate(float a, float b, float c);
		void Perlin(int a, int b, int c);

	};
}

#endif //__OBSTACLEGENERATOR_H_