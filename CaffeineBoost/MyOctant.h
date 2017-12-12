// Author: Dante Nardo
// Last Modified: 12/10/2017
// Purpose: Creates an Octree.

#pragma once

#include "MyEntityManager.h"
#include <vector>

namespace Simplex
{
	struct AABB
	{
		vector3 m_min;
		vector3 m_max;

		AABB()
		{
			m_min = vector3(0, 0, 0);
			m_max = vector3(0, 0, 0);
		}

		AABB(vector3 min, vector3 max)
		{
			m_min = min;
			m_max = max;
		}

		~AABB() { }

		// Determines whether a AABB has another AABB completely inside of it
		// Returns false if 'other' param has any point outside of this AABB
		bool Contains(AABB* other) 
		{
			return (m_min.x < other->m_min.x &&
					m_min.y < other->m_min.y &&
					m_min.z < other->m_min.z &&
					m_max.x > other->m_max.x &&
					m_max.y > other->m_max.y &&
					m_max.z > other->m_max.z);
		}

		vector3 GetCenter()
		{
			return vector3(m_min.x + (m_max.x - m_min.x) / 2,
						   m_min.y + (m_max.y - m_min.y) / 2,
						   m_min.z + (m_max.z - m_min.z) / 2);
		}

		vector3 GetSize()
		{
			return vector3((m_max.x - m_min.x) * 1,
						   (m_max.y - m_min.y) * 1,
						   (m_max.z - m_min.z) * 1);
		}
	};

	class MyOctant
	{
	private:
		const int MINIMUM_ENTITIES = 4;
		int m_depth;
		int m_childCount = 0;
		AABB* m_pAABB = nullptr;
		MyOctant* m_pParent = nullptr;
		MyOctant* m_pChildren[8];
		MyEntityManager* m_pEntityMngr = nullptr;
		MeshManager* m_pMeshMngr = nullptr;
		std::vector<MyEntity*> m_vEntityList;
		MyEntity* m_pPlayerEntity = nullptr;

		void Display(int depth);
		void CreateTree(int depth);
		void CheckForCollisions(int depth);
		AABB* CreateSubdivision(int index);
		void MoveEntitiesToLowerDivision();

	public:
		MyOctant(int depth, AABB* aabb, MyEntityManager* entityManager, MeshManager* meshManager);
		~MyOctant();

		void Display();
		void CreateTree();
		void CheckForCollisions();
	};
}