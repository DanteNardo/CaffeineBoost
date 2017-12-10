#include "MyCamera.h"
using namespace Simplex;

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }

vector3 Simplex::MyCamera::GetPosition() { return m_v3Position; }

void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }

void Simplex::MyCamera::SetUp(vector3 a_v3Up) { m_v3Up = a_v3Up; }

void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }

void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }

void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }

void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }

void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }

void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }

matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }

matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

float drag = 0.9f;

float acceleration = 3;

float initHeight;


Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUp(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and up
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	m_v3Up = other.m_v3Up;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUp(other.m_v3Position, other.m_v3Target, other.m_v3Up);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	

	//sets values of all player movement variables

	//the speed the object moves forwards at
	velocity = 0;

	//the speed the object moves sideways at
	sideSpeed = 0.25;

	//amount speed is increased by
	speedIncrease = .5;

	//amount speed is decreased by
	speedDecrease = .5;

	//amoun of vertical movement
	verticalVelocity = 0;

	//amount vertical velocity changes
	verticalDelta = -3;

	//maximum fall speed
	terminalVelocity = -9.8f;

	//jump height
	jumpImpulse = .3;

	//height to check against for falling
	initHeight = 3;

	leftLimit = 0;

	rightLimit = 4;
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate yet
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(m_v3Up, other.m_v3Up);

	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	m_v3Up = vector3(0.0f, 1.0f, 0.0f); //What is up

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Ortographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Ortographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUp(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;
	m_v3Up =  a_v3Upward;
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//Calculate the look at
	m_m4View = glm::lookAt(m_v3Position, m_v3Target, m_v3Up);
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	//perspective
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective)
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else
	{
		m_m4Projection = glm::ortho(	m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
										m_v2Vertical.x, m_v2Vertical.y, //vertical
										m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}

void Simplex::MyCamera::moveForward(float deltaTime)
{

	velocity += acceleration;

	velocity = velocity * drag;

	//moves camera position by velocity amount
	SetPosition(m_v3Position + vector3(0, 0, -velocity * deltaTime));

	//moves target so camera doesn't turn
	SetTarget(vector3(m_v3Target.x , m_v3Target.y, m_v3Target.z - (velocity* deltaTime)));
}

void Simplex::MyCamera::moveSideways(bool direction, float deltaTime)
{
	//if direction is true, move right
	if (direction)
	{
		if (m_v3Target.x - (sideSpeed) < rightLimit) {
			//moves camera position by velocity amount
			SetPosition(m_v3Position + vector3(sideSpeed, 0, 0));

			//moves target so camera doesn't turn
			SetTarget(vector3(m_v3Target.x + (sideSpeed), m_v3Target.y, m_v3Target.z));
		}
	}
	//if false, move left
	else
	{
		if (m_v3Target.x - (sideSpeed) > leftLimit) {
			//moves camera position by velocity amount
			SetPosition(m_v3Position - vector3(sideSpeed, 0, 0));

			//moves target so camera doesn't turn
			SetTarget(vector3(m_v3Target.x - (sideSpeed), m_v3Target.y, m_v3Target.z));
		}
	}
}


//applys a drag force to slow down
void Simplex::MyCamera::slowDown()
{
	acceleration -= speedDecrease;
}

//speeds up based on an acceleration
void Simplex::MyCamera::speedUp()
{
	acceleration += speedIncrease;
}

void Simplex::MyCamera::jump()
{
	if (m_v3Position.y = initHeight) {

		verticalVelocity = jumpImpulse;

	}
}

void Simplex::MyCamera::fall(float deltaTime)
{
 	SetPosition(vector3(m_v3Position.x, m_v3Position.y + verticalVelocity, m_v3Position.z));
	//moves target so camera doesn't turn
	SetTarget(vector3(m_v3Target.x, m_v3Position.y, m_v3Target.z));
	verticalVelocity = verticalVelocity - (3.0f * deltaTime);
	if (m_v3Position.y < 1) {
		SetPosition(vector3(m_v3Position.x, 1, m_v3Position.z));
		SetTarget(vector3(m_v3Target.x, 1, m_v3Target.z));
	}
}

//getters for player movement variables

//returns velocity
float Simplex::MyCamera::Velocity(){	return velocity;}

//returns sideways movement speed
float Simplex::MyCamera::SideSpeed(){	return sideSpeed;}

//returns speed increase variable
float Simplex::MyCamera::SpeedIncrease(){	return speedIncrease;}

//returns speed decrease variable
float Simplex::MyCamera::SpeedDecrease(){	return speedDecrease;}

//returns vertical movement speed
float Simplex::MyCamera::VerticalVelocity(){	return verticalVelocity;}

//returns change in vertical velocity per second
float Simplex::MyCamera::VerticalDelta(){	return verticalDelta;}

//returns max fall speed
float Simplex::MyCamera::TerminalVelocity(){	return terminalVelocity;}

//returns how much verticalvelocity jump adds
float Simplex::MyCamera::JumpImpulse(){	return jumpImpulse;}

void Simplex::MyCamera::collide(vector3 position)
{

	float difference = (position.z + 1) - m_v3Position.z;
	velocity = -difference * velocity * 10;
	//slowDown();

}

void Simplex::MyCamera::coffeCollide()
{
	speedUp();
}
