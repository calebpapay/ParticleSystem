//-----------------------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------- 

// --------------------------------
// ---      DO NOT MODIFY       ---
// --------------------------------

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

	#if WIN32

	LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);		// Declaration For WndProc

	class EventHandler
	{
	public:
		EventHandler();
		~EventHandler();
		static void ProcessEvents();
	private:
		EventHandler(const EventHandler &toCopy);
		EventHandler& operator=(const EventHandler& toCopy);
	
		static EventHandler& Instance();
		void processEvents();
	};

	#endif

#endif

// --- End of File ---
