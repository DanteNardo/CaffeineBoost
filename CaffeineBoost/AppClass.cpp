
#include "AppClass.h"
using namespace Simplex;

int score = 0;
unsigned int clockMove;
void Application::InitVariables(void)
{
	hallwayOffset = 0;
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 1.0f, ZERO_V3, AXIS_Y);

	//init the camera
	m_pCamera = new MyCamera();
	m_pCamera->SetPositionTargetAndUp(
		vector3(0.0f, 1.0f, 20.0f), //Where my eyes are
		vector3(0.0f, 1.0f, 19.0f), //where what I'm looking at is
		AXIS_Y);					//what is up
	
	m_v4ClearColor = vector4(0.380, 0.3411, 0.2823, 1.0);
	//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);

	m_pMyEntityMngr = MyEntityManager::GetInstance();

	clockMove = m_pSystem->GenClock();

	m_pGen = new ProceduralGeneration();

	m_pTable = new Simplex::Model();
	m_pChest = new Simplex::Model();
	m_pCoffee = new Simplex::Model();
	m_pHallway = new Simplex::Model();

	m_pTable->LoadOBJ("Minecraft\\Table.obj");
	m_pChest->LoadOBJ("Minecraft\\Chest.obj");
	m_pCoffee->LoadOBJ("Minecraft\\CoffeeCup.obj");
	m_pHallway->LoadOBJ("Minecraft\\HallwaySegment.obj");

	cubemap = { vector3(-0.5, -0.5, 0.5),
				vector3(0.5, -0.5, 0.5),
				vector3(0.5, 0.5, 0.5),
				vector3(-0.5, 0.5, 0.5),

				vector3(-0.5, -0.5, -0.5),
				vector3(0.5, -0.5, -0.5),
				vector3(0.5, 0.5, -0.5),
				vector3(-0.5, 0.5, -0.5) };



	// Initialize Player
	/*playermap = {	vector3(-0.35, -0.35, 0.35),
					vector3(0.35, -0.35, 0.35),
					vector3(0.35, 0.35, 0.25),
					vector3(-0.35, 0.35, 0.35),

					vector3(-0.35, -0.35, -0.35),
					vector3(0.35, -0.35, -0.35),
					vector3(0.35, 0.35, -0.35),
					vector3(-0.35, 0.35, -0.35) };*/

	vector3 camPos = vector3(0.0f, 1.0f, 20.0f);
	m_pMyEntityMngr->AddEntity("Minecraft\\Chest.obj", "player");
	m_pMyEntityMngr->GetEntity(m_pMyEntityMngr->GetEntityIndex("player"))->SetPlayer(m_pCamera);
	m_pMyEntityMngr->GetEntity(m_pMyEntityMngr->GetEntityIndex("player"))->SetModelMatrix(glm::translate(vector3(0, 0, 0)));
	m_pMyEntityMngr->GetEntity(m_pMyEntityMngr->GetEntityIndex("player"))->HidePlayer();

	//test coffee cup
	m_pMyEntityMngr->AddEntity("Minecraft\\CoffeeCup.obj", "coffee");


	m_pMyEntityMngr->AddEntity("Minecraft\\HallwaySegment.obj", "Hallway1");
	m_pMyEntityMngr->AddEntity("Minecraft\\HallwaySegment.obj", "Hallway2");

	// init music
	/*String filePath = m_pSystem->m_pFolder->GetFolderData();
	filePath += m_pSystem->m_pFolder->GetFolderAudio();
	// file path is now set to the directory that holds audio files
	m_soundBGM.openFromFile(filePath + "coffeerhythm.wav");
	m_soundBGM.setLoop(true);
	m_soundBGM.play();*/

	// sound effects
	//m_soundBuffer.loadFromFile(filePath + "jump.wav");
	//m_soundJump.setBuffer(m_soundBuffer);

	#pragma region Obstacle Generation
	// Spawn obstacles
	while (obstacles.size() < (m_pGen->GetLength() / m_pGen->GetLengthBuffer()) * m_pGen->GetWidth() * 4) {

		if (rand() % 2 == 0) {
			m_pMyEntityMngr->AddEntity("Minecraft\\Chest.obj", std::to_string(obstacles.size() + 1));
		}
		else {
			m_pMyEntityMngr->AddEntity("Minecraft\\Table.obj", std::to_string(obstacles.size() + 1));

		}
		obstacles.push_back(std::to_string(obstacles.size() + 1));
	}

	// set model matrix for (now) spawned entities
	for (int i = 0; i < obstacles.size(); i++) {
		//for now, we're setting way behind the player
		m_pMyEntityMngr->SetModelMatrix(glm::translate(vector3(-10, -10, -10)), obstacles[i]);
	}
	#pragma endregion

	#pragma region Octree Generation
	vector3 min;
	vector3 max;
	m_rootAABB = new AABB(min, max);
	m_pRoot = new MyOctant(4, m_rootAABB, m_pMyEntityMngr, m_pMeshMngr);
	m_pRoot->CreateTree();
	m_pMyEntityMngr->Update();
	m_pRoot->CheckForCollisions();
	#pragma endregion

	oldCameraPosition = m_pCamera->GetPosition().z;
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	float fDelta = m_pSystem->GetDeltaTime(clockMove);

	//comment to disable automatic movement for testing
	m_pCamera->moveForward(fDelta);
	m_pCamera->fall(fDelta);

	#pragma region Player Updating
	vector3 camPos = m_pCamera->GetPosition();
	matrix4 mPlayer = glm::translate(vector3(camPos.x, camPos.y, camPos.z));
	//m_pMyEntityMngr->SetModelMatrix(mPlayer, "player");
	#pragma endregion

    incrementScore(fDelta, m_pCamera->Velocity(), m_pCamera->GetPosition().z);

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	// Adds the procedurally generated world to the render list
	#pragma region Instantiating Batches

	float genToWorld = 1.0f;
	Batch* b = &Batch();
	
	int objectIndex = 1;

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
					int z = -(j + ((i + m_iBatchIterations - 1) * m_pGen->GetLength()));

					if (m_pMyEntityMngr->GetEntityIndex("player") == objectIndex) {
						objectIndex++;
					}
					m_pMyEntityMngr->SetModelMatrix(glm::translate(vector3(x * genToWorld, y * genToWorld, z + 10) - m_pCamera->GetPosition()), objectIndex);
					objectIndex++;
				}
				// Spawn coffee whenever the array value is 2
				else if (b->data[j][k] == 2) {

					int x = k % m_pGen->GetWidth();
					int y = k / m_pGen->GetWidth();
					int z = -(j + ((i + m_iBatchIterations - 1) * m_pGen->GetLength()));

					m_pMyEntityMngr->SetModelMatrix(glm::translate(vector3(x * genToWorld, y * genToWorld, z + 10) - m_pCamera->GetPosition()), "coffee");
				}
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

	// Check for collisions in Octree
	m_pMyEntityMngr->Update();
	m_pRoot->CheckForCollisions();

	//Hallways work endlessly
	#pragma region Hallway Setup
	m_pMyEntityMngr->SetModelMatrix(glm::translate(vector3(2, 0, hallwayOffset+40) - m_pCamera->GetPosition()), "Hallway1");
	m_pMyEntityMngr->SetModelMatrix(glm::translate(vector3(2, 0, hallwayOffset-40) - m_pCamera->GetPosition()), "Hallway2");
	
	if (abs(fmod(camPos.z, 80)) - abs(fmod(oldCameraPosition, 80)) < -20) {
		hallwayOffset -= 80;
	}
	oldCameraPosition = camPos.z;
	#pragma endregion
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	m_pMyEntityMngr->AddEntityToRenderList(-1, true);

	// Display the Octree
	m_pRoot->Display();

	//Render the list of MyMeshManager
	m_pMyMeshMngr->Render();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the MyMeshManager list
	//m_pMyMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// Release the singleton
	MyMeshManager::ReleaseInstance();
	EntityManager::ReleaseInstance();

	// Release the camera
	SafeDelete(m_pCamera);

	// Release the player
	SafeDelete(m_pPlayer);

	// Release the procedural generator
	SafeDelete(m_pGen);

	// Release the octree
	SafeDelete(m_pRoot);
	SafeDelete(m_rootAABB);

	// Release the models
	SafeDelete(m_pTable);
	SafeDelete(m_pChest);
	SafeDelete(m_pCoffee);
	SafeDelete(m_pHallway);

	// stop sounds
	//m_soundBGM.stop();
	//m_soundJump.stop();
	
	//release GUI
	ShutdownGUI();
}
