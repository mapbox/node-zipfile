#ifndef SIZEOF_OFF_T
	#if defined(_WIN64)
		#define SIZEOF_OFF_T 8
	#else if defined(_WIN32)
		#define SIZEOF_OFF_T 4
	#endif
#endif

#ifndef EXCLUDE_WIN_FIXES
	#if defined(_MSC_VER)
		#include <BaseTsd.h>
		typedef SSIZE_T ssize_t;
	#endif
#endif