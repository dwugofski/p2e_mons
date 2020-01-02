#pragma once

#ifndef __P2EM_MACROS_H__
#define __P2EM_MACROS_H__

#ifdef _DEBUG
// Assertion with message
#define _assertm(condition, message) \
do { \
	if (! (condition)) {\
		std::cerr << "Assertion (" #condition "} failed in " << __FILE__ << " (" << __LINE__ << "): "\
					<< message << std::endl;\
		std::terminate();\
	}\
} while (false)
#else
#define _assertm(condition, message) do{} while(false)
#endif

#endif