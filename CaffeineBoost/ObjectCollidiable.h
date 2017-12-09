/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __OBJECTCOLLIDIABLECLASS_H_
#define __OBJECTCOLLIDIABLECLASS_H_

#include "Definitions.h"

namespace Simplex
{

	class ObjectCollidiable
	{
		vector3 m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where the object is located

	public:
		/*
		USAGE: Constructor
		ARGUMENTS: ---
		OUTPUT: the class object
		*/
		ObjectCollidiable();

		/*
		USAGE: constructor
		ARGUMENTS:
		-	vector3 a_v3Position -> Where the object is located
		OUTPUT: ---
		*/
		ObjectCollidiable(vector3 a_v3Position);

		/*
		USAGE: Copy constructor
		ARGUMENTS: ObjectCollidiable const& other -> object to copy
		OUTPUT: ---
		*/
		ObjectCollidiable(ObjectCollidiable const& other);

		/*
		USAGE: Copy Assignment operator
		ARGUMENTS: ObjectCollidiable const& other -> object to copy
		OUTPUT: ---
		*/
		ObjectCollidiable& operator=(ObjectCollidiable const& other);

		/*
		USAGE: Initialize the class
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Init(void);

		/*
		USAGE: Release pointers in the class
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Release(void);

		/*
		USAGE: swap the content with an incoming object
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Swap(ObjectCollidiable& other);

		/*
		USAGE: Destructor
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		~ObjectCollidiable(void);

		/*
		USAGE: Sets the position of the object
		ARGUMENTS: vector3 a_v3Position -> Position where we want the object to be
		OUTPUT: ---
		*/
		void SetPosition(vector3 a_v3Position);

		/*
		USAGE: Gets the position of the object
		ARGUMENTS: ---
		OUTPUT: position of the object
		*/
		vector3 GetPosition(void);

	};

} //namespace Simplex

#endif //__OBJECTCOLLIDIABLECLASS_H_

  /*
  USAGE:
  ARGUMENTS: ---
  OUTPUT: ---
  */