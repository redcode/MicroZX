/* QSystem - time.c
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
#include <Q/inspection/OS.h>
#include <Q/types/base.h>

static qboolean has_monotonic_time = FALSE;
static qboolean ticks_started = FALSE;
static struct timeval start_tv;


#if Q_OS == Q_OS_MAC_OS_X

#	include <mach/mach_time.h>

	static uint64_t		  start_mach;
	mach_timebase_info_data_t mach_base_info;


	void q_initialize_ticks(void)
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


	quint64 q_ticks(void)
		{
		quint64 ticks;

		if (!ticks_started) q_initialize_ticks();

		if (has_monotonic_time)
			{
			quint64 now = mach_absolute_time();
			ticks = ((now - start_mach) * mach_base_info.numer) / mach_base_info.denom;
			}

		else	{
			struct timeval now;

			gettimeofday(&now, NULL);
			ticks = (now.tv_sec - start_tv.tv_sec) * 1000000000 + (now.tv_usec - start_tv.tv_usec);
			}

		return ticks;
		}


#elif Q_OS_IS(POSIX)


	void q_initialize_ticks(void)
		{
		if (!ticks_started)
			{
			gettimeofday(&start_tv, NULL);
			ticks_started = TRUE;
			}
		}


	quint64 q_ticks(void)
		{
		struct timeval now;

		if (!ticks_started) q_initialize_ticks();
		gettimeofday(&now, NULL);

		return	(quint64)(now.tv_sec - start_tv.tv_sec) * 1000000000 +
			(now.tv_usec - start_tv.tv_usec)	* 1000;
		}


#endif


#if Q_OS_IS(POSIX)

	void q_wait(quint64 time)
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
