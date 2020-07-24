/************************************************************************

  Routines for measuring time.

  $Id: time.cxx,v 1.1 2000/04/26 18:56:26 garland Exp $

 ************************************************************************/

#include <gfx.h>
#include <windows.h>

// Only Windows NT supports getting proper time usage information.
// In Windows 95, we have to settle for measuring real time.
double get_cpu_time()
{
    FILETIME start, end, kernel, user;

    if( !GetThreadTimes(GetCurrentThread(), &start, &end, &kernel, &user) )
    {
	// We're running under Windows 95 instead of NT.
	// Just get the current time and be done with it.
	SYSTEMTIME now;
	GetSystemTime(&now);
	SystemTimeToFileTime(&now, &user);
    }

    // Convert into something we can do math on
    LARGE_INTEGER i;
    i.LowPart = user.dwLowDateTime;
    i.HighPart = user.dwHighDateTime;

    // Convert to seconds and return
    return (double)(i.QuadPart) / 1e7;
}

