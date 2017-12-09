#include "ObjectCollidiable.h"
using namespace Simplex;

//Accessors
void Simplex::ObjectCollidiable::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }

vector3 Simplex::ObjectCollidiable::GetPosition() { return m_v3Position; }

Simplex::ObjectCollidiable::ObjectCollidiable()
{
	Init(); //Init the object with default values
}

Simplex::ObjectCollidiable::ObjectCollidiable(vector3 a_v3Position)
{
	Init(); //Initialize the object
	SetPosition(a_v3Position); //set the position
}

Simplex::ObjectCollidiable::ObjectCollidiable(ObjectCollidiable const& other)
{
	m_v3Position = other.m_v3Position;
}

ObjectCollidiable& Simplex::ObjectCollidiable::operator=(ObjectCollidiable const& other)
{
	if (this != &other)
	{
		Release();
		SetPosition(other.m_v3Position);
		ObjectCollidiable temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::ObjectCollidiable::Init(void)
{

}

void Simplex::ObjectCollidiable::Release(void)
{
	//No pointers to deallocate yet
}

void Simplex::ObjectCollidiable::Swap(ObjectCollidiable & other)
{
	std::swap(m_v3Position, other.m_v3Position);
}

Simplex::ObjectCollidiable::~ObjectCollidiable(void)
{
	Release();
}