//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Matrix.h"
#include "Vect4D.h"
#include "Particle.h"

#include <list>

class ParticleEmitter
{
public:
	ParticleEmitter();
	ParticleEmitter(const ParticleEmitter& t) = delete;
	ParticleEmitter& operator = (const ParticleEmitter & tmp) = delete;
	~ParticleEmitter();

	void SpawnParticle();
	void update();
	void draw();

	void addParticleToList(Particle * const p);
	void removeParticleFromList(Particle * const p);


	void initShellBuffer();
	

private:

	Particle* freeHead;
	Particle* usedHead; 
	Particle* mallocdmem;
	Vect4D* vectData;

	float	last_spawn;
	float	last_loop;
	int		last_active_particle;

};

#endif 

// --- End of File ---

