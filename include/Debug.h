#ifndef DEBUG_H
#define DEBUG_H

// debug on/off
#ifdef _DEBUG_
#define dbgprt(fmt,...)  do{printf("DBG-PRINT:");printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(fmt,...)
#endif

#ifdef _DEBUG_MORE_
#define dbgmoreprt(fmt,...)  do{printf("DBG-MORE-PRINT:");printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgmoreprt(fmt,...)
#endif

#define ERROR(fmt, ...) do{printf("ERROR:");printf(fmt, ##__VA_ARGS__);exit(-1);}while(0)

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif // DEBUG_H
