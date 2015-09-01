/* ZSystem - time.c
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2006-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <Z/inspection/OS.h>
#include <Z/types/base.h>

static zboolean has_monotonic_time = FALSE;
static zboolean ticks_started = FALSE;
static struct timeval start_tv;


#if Z_OS == Z_OS_MAC_OS_X

#	include <mach/mach_time.h>

	static uint64_t		  start_mach;
	mach_timebase_info_data_t mach_base_info;


	void z_initialize_ticks(void)
		{
		if (!ticks_started)
			{
			if (mach_timebase_info(&mach_base_info))
				gettimeofday(&start_tv, NULL);

			else	{
				has_monotonic_time = TRUE;
				start_mach = mach_absolute_time();
				}

			ticks_started = TRUE;
			}
		}


	zuint64 z_ticks(void)
		{
		zuint64 ticks;

		if (!ticks_started) z_initialize_ticks();

		if (has_monotonic_time)
			{
			zuint64 now = mach_absolute_time();
			ticks = ((now - start_mach) * mach_base_info.numer) / mach_base_info.denom;
			}

		else	{
			struct timeval now;

			gettimeofday(&now, NULL);
			ticks = (now.tv_sec - start_tv.tv_sec) * 1000000000 + (now.tv_usec - start_tv.tv_usec);
			}

		return ticks;
		}


#elif Z_OS_IS(POSIX)


	void z_initialize_ticks(void)
		{
		if (!ticks_started)
			{
			gettimeofday(&start_tv, NULL);
			ticks_started = TRUE;
			}
		}


	zuint64 z_ticks(void)
		{
		struct timeval now;

		if (!ticks_started) z_initialize_ticks();
		gettimeofday(&now, NULL);

		return	(zuint64)(now.tv_sec - start_tv.tv_sec) * 1000000000 +
			(now.tv_usec - start_tv.tv_usec)	* 1000;
		}


#endif


#if Z_OS_IS(POSIX)

	void z_wait(zuint64 time)
		{
		int was_error;
		struct timespec tv;
		//exit(0);
		/* Set the timeout interval */
		tv.tv_sec  = (long)(time / 1000000000);
		tv.tv_nsec = (long)(time % 1000000000);

		do	{
			errno = 0;
			was_error = nanosleep(&tv, NULL);
			}
		while (was_error && (errno == EINTR));
		}

#endif
