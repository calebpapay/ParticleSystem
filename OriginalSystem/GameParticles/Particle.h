//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#ifndef PARTICLE_H
#define PARTICLE_H

// include
#include "Vect4D.h"

class Particle
{
public:
	

	Particle();
	Particle(const Particle& t) = delete;
	Particle& operator = (const Particle & tmp) = delete;
	~Particle();
	void Update(const float& time_elapsed);
	
	void Execute( );
	
	

public:
	Particle* next;
	Particle* prev;

	Vect4D	*position;
	Vect4D	*velocity;
	Vect4D	*scale;

	float	life;
	float	rotation;

};

#endif 

// --- End of File ---