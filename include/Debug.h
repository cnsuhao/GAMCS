#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>

// debug on/off
#ifdef _DEBUG_
#define dbgprt(pref, fmt,...)  do{printf("%s:", pref);printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(pref, fmt,...)
#endif

#ifdef _DEBUG_MORE_
#define dbgmoreprt(pref, fmt,...)  do{printf("%s:", pref);printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgmoreprt(pref, fmt,...)
#endif

#define ERROR(fmt, ...) do{printf("ERROR:");printf(fmt, ##__VA_ARGS__);exit(-1);}while(0)
#define WARNNING(fmt, ...) do{printf("WARNNING:");printf(fmt, ##__VA_ARGS__);}while(0)

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif // DEBUG_H
