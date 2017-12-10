#include "AppClass.h"
using namespace Simplex;

unsigned int clockMove;
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//init the camera
	m_pCamera = new MyCamera();
	m_pCamera->SetPositionTargetAndUp(
		vector3(0.0f, 1.0f, 20.0f), //Where my eyes are
		vector3(0.0f, 1.0f, 19.0f), //where what I'm looking at is
		AXIS_Y);					//what is up


	//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);

	m_pMyEntityMngr = EntityManager::GetInstance();

	clockMove = m_pSystem->GenClock();

	m_pGen = new ProceduralGeneration();

	//m_pSteve = new Simplex::Model();
	//m_pTable = new Simplex::Model();
	//m_pChest = new Simplex::Model();
	//m_pCoffee = new Simplex::Model();
	//m_pHallway = new Simplex::Model();

	//m_pTable->LoadOBJ("Minecraft\\Table.obj");
	//m_pChest->LoadOBJ("Minecraft\\Chest.obj");
	//m_pCoffee->LoadOBJ("Minecraft\\CoffeeCup.obj");
	//m_pHallway->LoadOBJ("Minecraft\\HallwaySegment.obj");


	
	

	cubemap = { vector3(-1, -1, 1),
		vector3(1, -1, 1),
		vector3(1, 1, 1),
		vector3(-1, 1, 1),

		vector3(-1, -1, -1),
		vector3(1, -1, -1),
		vector3(1, 1, -1),
		vector3(-1, 1, -1) };
	playermap = { vector3(-0.25, -0.25, 0.25),
		vector3(0.25, -0.25, 0.25),
		vector3(0.25, 0.25, 0.25),
		vector3(-0.25, 0.25, 0.25),

		vector3(-0.25, -0.25, -0.25),
		vector3(0.25, -0.25, -0.25),
		vector3(0.25, 0.25, -0.25),
		vector3(-0.25, 0.25, -0.25) };

	m_pPlayer = new RigidBody(playermap); 
	// m_pTable->GetVertexList());

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	float fDelta = m_pSystem->GetDeltaTime(clockMove);

	//comment to disable automatic movement for testing
	m_pCamera->moveForward(fDelta);
	m_pCamera->fall(fDelta);
	

	#pragma region player rigid body updating

	//get position for camera collisions
	vector3 campos = m_pCamera->GetPosition();
	matrix4 mPlayer = glm::translate(vector3(campos.x, campos.y - 1, campos.z));

	m_pPlayer->SetModelMatrix(mPlayer);

	#pragma endregion player rigid body updating


	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();


	

	// Adds the procedurally generated world to the render list
	#pragma region Instantiating Batches

	float genToWorld = 1.0f;
	Batch* b = &Batch();

	collisionReg = false;

	// Iterate through every 3D Batch (std::vector of Batch*)
	for (int i = 0; i < m_pGen->GetBatches().size(); i++) {
		b = m_pGen->GetBatches()[i];

		// Iterate through current 3D Batch (std::vector of int*)
		for (int j = 0; j < b->data.size(); j++) {

			// Each int* is a single dimension array that represents a 2D array
			for (int k = 0; k < m_pGen->GetWidth() * m_pGen->GetHeight(); k++) {

				// Spawn an obstacle whenever the array value is 1
				if (b->data[j][k] == 1) {
					int x = k % m_pGen->GetWidth();
					int y = k / m_pGen->GetWidth();
					int z = -(j +((i + m_iBatchIterations - 1) * m_pGen->GetLength()));

					//build obstacle as a rigid body and check collisions against player
					matrix4 mObstacle = glm::translate(vector3(x * genToWorld, y * genToWorld, z));

					RigidBody* rigidObs = new RigidBody(cubemap); // m_pTable->GetVertexList());


					rigidObs->SetModelMatrix(mObstacle);

					m_pMyMeshMngr->AddCubeToRenderList(mObstacle);

					//Adds the table to render list, with the identifier "table jk"
					//m_pMyEntityMngr->AddEntity("Minecraft\\Chest.obj");
					//m_pMyEntityMngr->SetModelMatrix(mObstacle);
				
					//collision check
					if (!collisionReg && m_pPlayer->IsColliding(rigidObs)) {

						m_pCamera->collide(vector3(x * genToWorld, y * genToWorld, z));
						collisionReg = true;
					}

					//rigidObs->~RigidBody();
				}

				//todo: add coffee generation at intervals
			}
		}
	}
	#pragma endregion

	// Creates a new batch when necessary
	#pragma region Iterate Batches
	
	// Check if the player has passed the current batch and if so iterate
	if (m_pCamera->GetPosition().z < -(m_iBatchIterations * m_pGen->GetLength())) {
		m_pGen->NextBatch();
		m_iBatchIterations++;
	}

	#pragma endregion

	//m_pMyEntityMngr->Update();
	
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	


	//clear the render list
	m_pMeshMngr->ClearRenderList();

	m_pMyEntityMngr->AddEntityToRenderList(-1, false);

	//Render the list of MyMeshManager
	m_pMyMeshMngr->Render();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the MyMeshManager list
	m_pMyMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// Release the singleton
	MyMeshManager::ReleaseInstance();

	// Release the camera
	SafeDelete(m_pCamera);

	// Release the procedural generator
	SafeDelete(m_pGen);

	// Release the models
	SafeDelete(m_pTable);
	SafeDelete(m_pChest);
	SafeDelete(m_pCoffee);
	SafeDelete(m_pHallway);

	//release GUI
	ShutdownGUI();
}
