#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>

#ifdef _DEBUG
# define DEBUGOUT printf
#else
# define DEBUGOUT nothing
#endif

inline void nothing(...) { }

#endif
