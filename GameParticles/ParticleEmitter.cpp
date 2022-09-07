//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#include <Math.h>
#include "OpenGLDevice.h"
#include "ParticleEmitter.h"
#include "Settings.h"

PerformanceTimer globalTimer;

ParticleEmitter::ParticleEmitter()
	: last_spawn(globalTimer.GetGlobalTime()),
	last_loop(globalTimer.GetGlobalTime()),
	last_active_particle(-1)
{
	this->initShellBuffer();
}

ParticleEmitter::~ParticleEmitter()
{
	_aligned_free(mallocdmem);
	_aligned_free(vectData);
}

void ParticleEmitter::SpawnParticle()
{
	// create another particle if there are ones free
	if (last_active_particle < NUM_PARTICLES - 1)
	{
		Particle* newParticle = freeHead;

		//remove from free list
		freeHead = freeHead->next;
		freeHead->prev = nullptr;

		newParticle->life = 0.0f;
		*(newParticle->position) = Vect4D(0.0f, 2.0f, 2.0f);
		*(newParticle->velocity) = Vect4D(-4.0f, 4.0f, 0.0f);
		*(newParticle->scale) = Vect4D(-1.0f, -1.0f, -1.0f, 1.0f);
		newParticle->rotation = 0.0f;
	
		//add to used list
		this->addParticleToList(newParticle); 
		
		newParticle->Execute();
	
		last_active_particle++;

	}
}

void ParticleEmitter::update()
{
	float current_time = globalTimer.GetGlobalTime();
	float time_elapsed = current_time - last_spawn;
	float spawn_freq = 0.00001f;

	// spawn partickes for a time
	while (spawn_freq < time_elapsed)
	{

		this->SpawnParticle();
		
		// adjust time
		time_elapsed -= spawn_freq;
		last_spawn = current_time;


	}
	// total elapsed
	time_elapsed = current_time - last_loop;

	//kill, time elapsed Particles, else draw the particle
	Particle* curr = this->usedHead;
	Matrix tmp;
	while (curr != nullptr)
	{


		// call every particle and update its position 
		curr->Update(time_elapsed);

		// if life is greater that the max_life 
		// remove particle
		if ((curr->life > MAX_LIFE))
		{
			Particle* s = curr;
			curr = curr->next;

			this->removeParticleFromList(s);

			last_active_particle--;
		}
		else
		{
			// increment to next point
			tmp.setGLMatrix(*curr->scale, *curr->position, curr->rotation);
			const float* test = &tmp.v0.x;
			// ------------------------------------------------
			OpenGLDevice::SetTransformMatrixFloat(test);
			curr = curr->next;
		}


	}
	
	last_loop = current_time;
}

void ParticleEmitter::draw()
{
	
	
	//Moved work to update

}



void ParticleEmitter::addParticleToList(Particle * const p)
{
	//assert(p);
	if (this->usedHead == nullptr)
	{ // first on list
		this->usedHead = p;

		p->next = 0;
		p->prev = 0;
	}
	else
	{ // add to front of list
		usedHead->prev = p;
		p->next = usedHead;
		p->prev = 0;
		usedHead = p;
	}

}




void ParticleEmitter::removeParticleFromList(Particle* const p)
{
	// make sure we are not screwed with a null pointer
	//assert(p);
	
	if (p->next == nullptr && p->prev == nullptr)
	{ // only one on the list
		this->usedHead = nullptr;
		
		p->next = freeHead;
		p->prev = nullptr;
		freeHead->prev = p;
		this->freeHead = p;	
	}
	else if (p->prev == nullptr )
	{ // first on the list
		p->next->prev = nullptr;
		this->usedHead = p->next;
		
		p->next = freeHead;
		p->prev = nullptr;
		freeHead->prev = p;
		this->freeHead = p;
	}
	else if (p->next == nullptr)
	{ // last on the list 
		p->prev->next = nullptr;

		p->next = freeHead;
		p->prev = nullptr;
		freeHead->prev = p;
		this->freeHead = p;
	}
	else//( p->next != nullptr  && p->prev !=nullptr )
	{ // middle of the list
		p->prev->next = p->next;
		p->next->prev = p->prev;
		
		p->next = freeHead;
		p->prev = nullptr;
		freeHead->prev = p;
		this->freeHead = p;
	}
}



void ParticleEmitter::initShellBuffer()
{
	mallocdmem = (Particle*)_aligned_malloc((sizeof(Particle) * (NUM_PARTICLES)),16);
	vectData = (Vect4D*)_aligned_malloc(((sizeof(Vect4D) * 3) * NUM_PARTICLES),16);
	
	Vect4D* save2 = vectData;
	freeHead = mallocdmem;
	usedHead = nullptr;

	Particle* currParticle = freeHead;

	currParticle->position = vectData;
	currParticle->velocity = vectData + 1;
	currParticle->scale = vectData + 2;

	vectData = vectData + 3;

	
	currParticle->next = currParticle + 1;
	currParticle->prev = nullptr;

	currParticle = currParticle->next;
	size_t i = 1;
	while (i < NUM_PARTICLES)
	{
		currParticle->position = vectData;
		currParticle->velocity = vectData + 1;
		currParticle->scale = vectData + 2;

		vectData = vectData + 3;

		currParticle->next = currParticle + 1;
		currParticle->prev = currParticle - 1;


		currParticle = currParticle->next;
		i++;
	}
	currParticle->position = vectData;
	currParticle->velocity = vectData + 1;
	currParticle->scale = vectData + 2;

	currParticle->next = nullptr;
	currParticle->prev = currParticle - 1;

	vectData = save2;
}



// --- End of File ---
