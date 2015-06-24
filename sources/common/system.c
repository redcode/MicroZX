#include <sys/time.h>
#include <mach/mach_time.h>
#include <errno.h>
#include <Q/types/base.h>


static uint64_t		  start_mach;
mach_timebase_info_data_t mach_base_info;
static qboolean		  has_monotonic_time = FALSE;
static struct timeval	  start_tv;
static qboolean		  ticks_started = FALSE;


void q_initialize_ticks(void)
	{
	if (ticks_started) return;
	ticks_started = TRUE;

	/* Set first ticks value */

	if (mach_timebase_info(&mach_base_info))
		gettimeofday(&start_tv, NULL);

	else	{
		has_monotonic_time = TRUE;
		start_mach = mach_absolute_time();
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

	return (ticks);
	}


void q_wait(quint64 time)
	{
	int was_error;
	struct timespec elapsed, tv;

	/* Set the timeout interval */
	elapsed.tv_sec = time / 1000000000;
	elapsed.tv_nsec = time % 1000000000;

	do	{
		errno = 0;
		tv.tv_sec = elapsed.tv_sec;
		tv.tv_nsec = elapsed.tv_nsec;
		was_error = nanosleep(&tv, &elapsed);
		}
	while (was_error && (errno == EINTR));
	}
