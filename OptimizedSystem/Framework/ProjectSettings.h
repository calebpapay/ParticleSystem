//----------------------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------

#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

// -----------------------------------------------------------------------------
// DO NOT MODIFY this file
// -----------------------------------------------------------------------------
// Warning levels turned on for Wall... that Should be supressed globally
//     Set here to avoid surpressing them in the settings
// -----------------------------------------------------------------------------

// --- none ----

// --------------------------------------
// Environment check
// --------------------------------------

// minimum versions
#define TEST_WINSDK_VER "10.0.20348.0"
#define TEST_TOOLS_VER "14.29.30133"
#define TEST_COMPILER_VER "192930133"

#define IDEWinSDKCheck { char  winSDK_ver[13]; \
						 sprintf_s(winSDK_ver, 13, "%s", WINDOWS_TARGET_PLATFORM); \
						 CHECK(strcmp(winSDK_ver, TEST_WINSDK_VER) >= 0); }

#define IDEToolsCheck { char tools_ver[12]; \
						sprintf_s(tools_ver, 12, "%s", TOOLS_VERSION); \
						CHECK(strcmp(tools_ver, TEST_TOOLS_VER) >= 0); }

#define IDECompilerCheck { char compiler_ver[10]; \
						   sprintf_s(compiler_ver, 10, "%d", _MSC_FULL_VER); \
						   CHECK(strcmp(compiler_ver, TEST_COMPILER_VER) >= 0); }


#endif 

// ---  End of File ---------------
