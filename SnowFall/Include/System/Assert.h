#pragma once

// Triggered when assertion fails
void FailedAssert(const char* file, int line, const char* expression);

//--DEBUG MODE ASSERT--
#if defined(DEBUG)
	#if defined(WIN32)
		#include <intrin.h>
		#define debugBreak() __debugbreak()
	#elif // branch Win32
		#define DebugBreak() // Does nothing
	#endif // end Win32

	#undef assert // overide other asserts
	// Debug Breaks if available, else message logged, ends program
	#define assert(x) { \
		if(x){ \
		} else { \
			FailedAssert(__FILE__, __LINE__, #x);\
			debugBreak(); \
		} \
	}

	// Validates but doesnt terminate
	#define verify(x) ((x) ? true : FailedAssert(__FILE__, __LINE__, #x))

#else // DEBUG

	#define DebugBreak() // Does nothing.

	#undef assert

	// Does nothing in release for performance reasons
	#define assert(x) {((void)0);}

	// Validates but doesnt terminate
	#define verify(x) ((x)? true :  FailedAssert(__FILE__, __LINE__, #x))

#endif // DEBUG