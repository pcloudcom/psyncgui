#ifndef MYDEBUG_H
#define MYDEBUG_H
#include<stdarg.h>
#include "plibs.h"

#define mydebug(level, ...) do {if (level<=DEBUG_LEVEL) psync_debug(__FILE__, __FUNCTION__, __LINE__, level, ##__VA_ARGS__);} while (0)

#endif // MYDEBUG_H
