//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

// -----------------------------------------------------------
// ASSUME: MMX - SSE4.1 are allowed on the test machine
//         Do NOT use any setting above this level, grade->0
// -----------------------------------------------------------
#include "OpenGLDevice.h"
#include "Settings.h"
#include "ParticleEmitter.h"

int main()
{
	Trace::out("Num Particle: %.1e time:%.1f\n",(float)NUM_PARTICLES,MAX_LIFE);

	srand(1);

	// initialize timers:------------------------------

		// Create a timer objects
		PerformanceTimer updateTimer;
		PerformanceTimer drawTimer;

	// create an emitter:-------------------------------
		ParticleEmitter emitter;


		// counter for printing
		int i = 0;

		Vect4D tester(1.0f, 0.0f, 0.0f);
		const float* test = (const float*)&tester;

		OpenGLDevice::SetCameraMatrixFloat(test);
	
	// main update loop... do this forever or until some breaks 
	while(OpenGLDevice::IsRunning())
	{
		// start update timer ---------------------------------------
		updateTimer.Tic();

			// update the emitter
			emitter.update();

		// stop update timer: -----------------------------------------
		updateTimer.Toc();

		// start draw timer: ----------------------------------------
		drawTimer.Tic();

			// draw particles
			//emitter.draw();
		
		// stop draw timer: -----------------------------------------
		drawTimer.Toc();

		// update ouput every 25 times
		i++;
		if( i > 25 ) 
		{
			i = 0;
			float updateTime = updateTimer.TimeInSeconds();
			float drawTime = drawTimer.TimeInSeconds();
			printf("LoopTime: update:%f ms  draw:%f ms  tot:%f\n",updateTime * 1000.0f, drawTime * 1000.0f, (updateTime + drawTime) *1000.0f);
		}
	}
	
    return 0;
}

// WinMain required for all win32 applications (Windows entry point)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	OpenGLDevice::Initialize(hInstance);
	main();
}

// --- End of File ---
