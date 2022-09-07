//-----------------------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

// --------------------------------
// ---      DO NOT MODIFY       ---
// --------------------------------

// --------------------------------------------------------------
// OpenGLDevice - wrapper for Optimized C++
//                simplifies the project, abstracting graphics
// --------------------------------------------------------------

#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

	#ifdef WIN32

	#include <gl\gl.h>								// Header File For The OpenGL32 Library
	#include <gl\glu.h>								// Header File For The GLu32 Library

	struct sWH
	{
		int width;
		int height;
	};

	class OpenGLDevice
	{
	
	public:
		~OpenGLDevice();
	
		static void SetCameraMatrixDouble(const double *);
		static void SetCameraMatrixFloat(const float *);

		static void SetTransformMatrixDouble(const double *);
		static void SetTransformMatrixFloat(const float *);

		static bool IsRunning();
		static void Initialize(HINSTANCE hInst);

	private:	
		static bool InitWindow();
		static void EndDraw();
		static bool KillWindow();
		static void Quit();

		friend class EventHandler;

	private:
		OpenGLDevice();
		void privSetup();
		static OpenGLDevice& Instance();
	
		bool privInitWindow();
		bool privKillWindow();
		void privEndDraw();
		bool privIsRunning();
		void privQuit();
		void privSetHInstance(HINSTANCE hInst);

		HDC			hDC;
		HGLRC		hRC;
		HWND		hWnd;
		HINSTANCE	hInstance;
		WNDCLASS	_wc;
		DWORD		_dwExStyle;
		DWORD		_dwStyle;
		sWH			screenSize;
		int			depth;
		int			pixelsize;
		bool		fullscreen;
		bool		running;
		char		pad0;
		char		pad1;
	};

	#endif 

#endif

// --- End of File ---
