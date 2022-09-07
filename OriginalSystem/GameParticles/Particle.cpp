//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#include "Particle.h"
#include "Matrix.h"



Particle::Particle()
	:next(nullptr),
	prev(nullptr),
	position(nullptr),
	velocity(nullptr),
	scale(nullptr),
	life(0.0f),
	rotation(0.0f)
{
	// construtor
	this->life = 0.0f;
	this->rotation = 0.0f;
	this->next = nullptr;
	this->prev = nullptr;
}

Particle::~Particle()
{
	// nothing to do
}

void Particle::Execute()
{
	// Ses it's ugly - I didn't write this so don't bitch at me
	// Sometimes code like this is inside real commerical code ( so know you now how it feels )
	Vect4D pos_variance(1.50f, 0.50f, 10.0f);
	Vect4D vel_variance(15.0f, 0.70f, -1.0f);


	// x - variance
	Vect4D varHolder;
	float var = (float)(rand() % 1000) * 0.001f;
	int sign = (rand() % 2);
	if (sign == 0)
	{
		var *= -1.0f;
	}
	varHolder.x = var;


	// y - variance
	var = (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);
	if (sign == 0)
	{
		var *= -1.0f;
	}
	varHolder.y = var;


	// z - variance
	var = (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);

	if (sign == 0/*.0f*/)
	{
		var *= -1.0f;
	}
	varHolder.z = var;

	Vect4D temp;
	temp.x = pos_variance.x * varHolder.x;
	temp.y = pos_variance.y * varHolder.y;
	temp.z = pos_variance.z * varHolder.z;


	this->position->x += temp.x;
	this->position->y += temp.y;
	this->position->z += temp.z;


	var = (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);
	// x  - add velocity
	if (sign == 0)
	{
		var *= -1.0f;
	}
	varHolder.x = var;


	// y - add velocity
	var = (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);
	if (sign == 0)
	{
		var *= -2.0;
	}
	varHolder.y = var;

	// z - add velocity
	var = (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);

	if (sign == 0)
	{
		var *= -2.0f;
	}
	varHolder.z = var;

	temp.x = vel_variance.x * varHolder.x;
	temp.y = vel_variance.y * varHolder.y;
	temp.z = vel_variance.z * varHolder.z;


	this->velocity->x += temp.x;
	this->velocity->y += temp.y;
	this->velocity->z += temp.z;

	// correct the sign
	var = 2.0f * (float)(rand() % 1000) * 0.001f;
	sign = (rand() % 2);

	if (sign == 0)
	{
		var *= -2.0f;
	}
	*this->scale = *this->scale * var;
}

void Particle::Update(const float& time_elapsed)
{

	// serious math below - magic secret sauce
	life += time_elapsed;
	Vect4D temp = (*velocity * time_elapsed);
	*position = *position + temp;

	Vect4D z_axis(0.0f, 0.0f, 3.0f);
	
	temp = position->Cross(z_axis);
	temp = temp.norm(temp);
	*position = *position + temp * 0.05f * life;

	rotation = rotation + -0.25f * time_elapsed * 2.0f;
}



// --- End of File ---
