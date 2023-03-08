/*
 * =====================================================================
 *
 *						XplicitNgin C++ Game Engine
 *			Copyright XPX Technologies all rights reserved.
 *
 *			File: Foundation.cpp
 *			Purpose: Xplicit Foundation Header
 *
 * =====================================================================
 */

#include "Foundation.h"

static FILE* stdlog{ nullptr };

XPLICIT_API FILE* get_logger(void) 
{
	return stdlog;
}

size_t fstrlen(const char* buffer) 
{
	if (!buffer) return 0;
	if (*buffer == END_OF_BUFFER) return 0;

	size_t index = 0;
	while (buffer[index] != END_OF_BUFFER) ++index;

	return index;
}

time_t get_epoch() 
{
	static time_t curtime{};
	curtime = time(&curtime);
	// do not check anything here, it is source of race conditions.
	
	return curtime;
}

void log(const char* msg) 
{
#ifndef NDEBUG
	char buf[sizeof(time_t)];
	snprintf(buf, sizeof(time_t), "%llu", get_epoch());
	fprintf(stdlog, "[%s - LOG] %s", buf, msg);
#endif // !NDEBUG

}

char dbg_filename[256];

bool open_logger() 
{
#ifndef NDEBUG
	snprintf(dbg_filename, 256, "%llu_xplicit.log", get_epoch());

	if (fopen_s(&stdlog, dbg_filename, "w+") != EXIT_SUCCESS) 
	{
		assert(false);
		exit(EXIT_FAILURE);

		return false;
	}

#endif // !NDEBUG

	return true;
}