//---------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//---------------------------------------------------------------

#ifndef SETTINGS_H
#define SETTINGS_H

// 2 coarse knobs that you can adjust while refactoring this amazing code
// Make sure you leave these setting to the default on final submission

#define NUM_PARTICLES	(125* 1000 ) //leave at 125K for final testing
#define MAX_LIFE		(20.0f)       //leave at 20 for final testing

#define CPU_WITH_GRAPHICS 0 // Set CPU_WITH_GRAPHICS: 1 to verify Graphics and CPU performance
                             //   Make sure graphics looks correct and shutdown doesn't leak
                             //        Memory Leak - 20% grade
                             //        Changed Graphics behavior - 50% grade 
                             
                             // Set CPU_WITH_GRAPHICS: 0 to verify CPU performance without graphics
                             //   Test ONLY CPU performance (used for final grading)

#endif 

// --- End of File ---
