//-----------------------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

// --------------------------------
// ---      DO NOT MODIFY       ---
// --------------------------------

#ifdef WIN32

#include "WinEventHandler.h"
#include "OpenGLDevice.h"

LRESULT CALLBACK WndProc(	HWND	hWnd,		// Handle For This Window
							UINT	uMsg,		// Message For This Window
							WPARAM	wParam,		// Additional Message Information
							LPARAM	lParam)		// Additional Message Information
{
	switch(uMsg)
	{
		case WM_CLOSE:				// Did We Receive A Close Message?
		{
			PostQuitMessage(0);		// Send A Quit Message
			return 0;				// Jump Back
		}
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

EventHandler::EventHandler()
{
	// do nothing
}

EventHandler::~EventHandler()
{
	// do nothing
}

void EventHandler::ProcessEvents()
{
	EventHandler::Instance().processEvents();
}

EventHandler::EventHandler(const EventHandler &)
{

	// do nothing
}

EventHandler& EventHandler::operator=(const EventHandler&)
{
	
	// do nothing
	return *this;
}

EventHandler& EventHandler::Instance()
{
	static EventHandler handler;
	return handler;
}

void EventHandler::processEvents()
{
	MSG	msg;										// Windows Message Structure

	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))		// Is There A Message Waiting?
	{
		if(WM_QUIT == msg.message)
		{
			OpenGLDevice::Quit();
		}
		else
		{
			TranslateMessage(&msg);					// Translate The Message
			DispatchMessage(&msg);
		}
	}
}

#endif

// --- End of File ---
