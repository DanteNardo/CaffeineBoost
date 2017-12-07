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
		vector3(0.0f, 3.0f, 20.0f), //Where my eyes are
		vector3(0.0f, 3.0f, 19.0f), //where what I'm looking at is
		AXIS_Y);					//what is up

//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);
	clockMove = m_pSystem->GenClock();

	m_pGen = new ProceduralGeneration();

	m_pTable = new Model();
	m_pChest = new Model();
	m_pCoffee = new Model();
	m_pHallway = new Model();

	m_pTable->Load("Models\\Table.obj");
	//m_pChest->Load("Chest.obj");
	//m_pCoffee->Load("CoffeeCup.obj");
	//m_pHallway->Load("HallwaySegment.obj");


}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	float fDelta = m_pSystem->GetDeltaTime(clockMove);

	//m_pCamera->moveForward(fDelta);

	//m_pCamera->fall(fDelta);

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Add objects to the Manager
	for (int j = -50; j < 50; j += 2)
	{
		for (int i = -50; i < 50; i += 2)
		{
			m_pMyMeshMngr->AddConeToRenderList(glm::translate(vector3(i, 0.0f, j)));
		}
	}


	// Iterate through a 3D Batch (std::vector of int*)
	// Each int* is a single dimension array that represents a 2D array
	Batch* b = m_pGen->GetBatch();
	for (int i = 0; i < b->data.size(); i++) {
		for (int j = 0; j < m_pGen->GetWidth() * m_pGen->GetHeight(); j++) {

			// Spawn an obstacle whenever the array value is 1
			if (b->data[i][j] == 1)
			{
				float genToWorld = 1.0f;
				int x = (j + 0) % m_pGen->GetWidth();
				int y = (j + 0) / m_pGen->GetWidth();
				m_pMyMeshMngr->AddCubeToRenderList(glm::translate(vector3(x * genToWorld, y * genToWorld + 2, i)));
			}
		}
	}
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

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
	//release the singleton
	MyMeshManager::ReleaseInstance();

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}
