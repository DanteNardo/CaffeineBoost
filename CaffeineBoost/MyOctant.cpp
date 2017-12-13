#include "MyOctant.h"

using namespace Simplex;

MyOctant::MyOctant(int depth, AABB* aabb, MyEntityManager* entityManager, MeshManager* meshManager)
{
	m_depth = depth;
	m_pEntityMngr = entityManager;
	m_pMeshMngr = meshManager;
	m_pAABB = aabb;
}

MyOctant::~MyOctant()
{
	// Release children
	for (int i = 0; i < m_childCount; i++) {
		SafeDelete(m_pChildren[i]);
	}

	// Release AABB
	SafeDelete(m_pAABB);

	// Release EntityList
	for (int j = 0; j < m_vEntityList.size(); j++) {
		SafeDelete(m_vEntityList[j]);
	}
}

// Public function that displays the octree.
// Nonrecursive and sets up the recursive display function.
void MyOctant::Display()
{
	// Search through octree and draw all Octant AABB
	int depth = 1;

	if (depth < m_depth) {
		Display(depth);
	}
}

// Private recursive function that displays a single Octant
// and then calls itself on all of that Octant's children.
void MyOctant::Display(int depth)
{
	// Display all elements in tree
	if (depth < m_depth) {

		// Draw this AABB
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_pAABB->GetCenter()) * glm::scale(m_pAABB->GetSize()), C_YELLOW);

		// Call display on children
		for (int i = 0; i < m_childCount; i++) {
			m_pChildren[i]->Display(depth + 1);
		}
	}
}

// Public function that creates the octree.
// Nonrecursive and sets up the recursive creation function.
void MyOctant::CreateTree()
{
	int depth = 1;
	m_vEntityList = std::vector<MyEntity*>();
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
		m_vEntityList.push_back(m_pEntityMngr->GetEntity(i));
	}

	if (depth < m_depth) {
		CreateTree(depth);
	}
}

// Private recursive function that divides a single Octant
// into eight child sections, moving all entities from this
// level into the child sections.
void MyOctant::CreateTree(int depth)
{
	// Continue subdividing if necessary
	if (depth < m_depth) {

		// Create children and continue iteration
		m_childCount = 8;
		for (int i = 0; i < 8; i++) {
			m_pChildren[i] = new MyOctant(m_depth, CreateSubdivision(i), m_pEntityMngr, m_pMeshMngr);
			m_pChildren[i]->m_pParent = this;

			// Only continue subdividing if this child contains too many entities
			m_pChildren[i]->MoveEntitiesToLowerDivision();
			if (m_pChildren[i]->m_vEntityList.size() > MINIMUM_ENTITIES) {
				m_pChildren[i]->CreateTree(depth + 1);
			}
		}
	}
	else {
		m_childCount = 0;
		m_pParent = nullptr;
	}
}

// Public function that checks for collisions using octree
// Nonrecursive and sets up the recursive collisions function.
void Simplex::MyOctant::CheckForCollisions()
{
	int depth = 1;
	if (depth < m_depth) {
		CheckForCollisions(depth);
	}
}

// Private recursive function that checks for all collisions with
// entities saved in this octant. Calls this function on all children.
void Simplex::MyOctant::CheckForCollisions(int depth)
{
	// Continue checking through tree
	if (depth < m_depth) {

		// Check for collisions with everything in this partition
		if (m_vEntityList.size() > 0) {
			for (int i = 0; i < m_vEntityList.size() - 1; i++) {
				for (int j = i + 1; j < m_vEntityList.size(); j++) {

					// Ignore hallways
					if (m_vEntityList[i]->GetUniqueID() == "Hallway1" ||
						m_vEntityList[i]->GetUniqueID() == "Hallway2" ||
						m_vEntityList[i]->GetUniqueID() == "coffee"   ||
						m_vEntityList[j]->GetUniqueID() == "Hallway1" ||
						m_vEntityList[j]->GetUniqueID() == "Hallway2" ||
						m_vEntityList[j]->GetUniqueID() == "coffee") {
						continue;
					}

					// If the colliding object is the player, react accordingly
					else if (m_vEntityList[i]->GetUniqueID() == "player" && 
							 m_vEntityList[i]->IsColliding(m_vEntityList[j])) {
						vector3 objectCenter = m_vEntityList[j]->GetRigidBody()->GetCenterGlobal();
						m_vEntityList[i]->GetPlayer()->collide(objectCenter);
					}

					// All other collisions
					else if (m_vEntityList[i]->IsColliding(m_vEntityList[j])) {
						m_vEntityList[i]->ResolveCollision(m_vEntityList[j]);
					}
				}
			}
		}

		// Check children's collisions
		for (int k = 0; k < m_childCount; k++) {
			m_pChildren[k]->CheckForCollisions(depth + 1);
		}
	}
}

// Separates a single AABB into eight equal cubes.
// Index param determines cube subdivision.
AABB* MyOctant::CreateSubdivision(int index)
{
	//   2----3 
	//  /|   /|
	// 6----7 |
	// | |  | |
	// | 0--|-1
	// |/   |/
	// 4----5

	vector3 min;
	vector3 max;
	vector3 half = m_pAABB->m_max - m_pAABB->m_min;
	half.x = half.x / 2;
	half.y = half.y / 2;
	half.z = half.z / 2;

	switch (index)
	{
		// Bottom back left
	case 0:
		min = m_pAABB->m_min;
		max = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z + half.z);
		break;


		// Bottom back right
	case 1:
		min = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y,
			m_pAABB->m_min.z);
		max = vector3(m_pAABB->m_max.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z + half.z);
		break;

		// Top back left
	case 2:
		min = vector3(m_pAABB->m_min.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z);
		max = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_max.y,
			m_pAABB->m_min.z + half.z);
		break;

		// Top back right
	case 3:
		min = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z);
		max = vector3(m_pAABB->m_max.x,
			m_pAABB->m_max.y,
			m_pAABB->m_min.z + half.z);
		break;

		// Bottom front left
	case 4:
		min = vector3(m_pAABB->m_min.x,
			m_pAABB->m_min.y,
			m_pAABB->m_min.z + half.z);
		max = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_max.z);
		break;

		// Bottom front right
	case 5:
		min = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y,
			m_pAABB->m_min.z + half.z);
		max = vector3(m_pAABB->m_max.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_max.z);
		break;

		// Top front left
	case 6:
		min = vector3(m_pAABB->m_min.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z + half.z);
		max = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_max.y,
			m_pAABB->m_max.z);
		break;

		// Top front right
	case 7:
		min = vector3(m_pAABB->m_min.x + half.x,
			m_pAABB->m_min.y + half.y,
			m_pAABB->m_min.z + half.z);
		max = m_pAABB->m_max;
		break;

		// This shouldn't occur
	default:
		min = vector3(0, 0, 0);
		max = vector3(0, 0, 0);
		break;
	}

	return new AABB(min, max);
}

// Moves entities from parent to child subdivision
// if the child subdivision contains them.
void MyOctant::MoveEntitiesToLowerDivision()
{
	// Iterate through parent's entityList and move entities from
	// that subdivision if they belong in this subdivision.
	// Necessary to dereference Parent's EntityList for [] operation.
	AABB aabb = AABB();
	for (int i = 0; i < m_pParent->m_vEntityList.size(); i++) {

		// Create AABB for entity
		aabb = AABB(m_pParent->m_vEntityList[i]->GetRigidBody()->GetMinGlobal(),
			m_pParent->m_vEntityList[i]->GetRigidBody()->GetMaxGlobal());

		// Move entity to this subdivision if it contains it
		if (m_pAABB->Contains(&aabb)) {
			m_vEntityList.push_back(m_pParent->m_vEntityList[i]);
			m_pParent->m_vEntityList.erase(m_pParent->m_vEntityList.begin() + i);
			i--;
		}
	}
}