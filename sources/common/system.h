/* ZSystem API - time.c
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2006-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __ZSystem_time_H__
#define __ZSystem_time_H__

#include <Z/types/base.h>

Z_C_SYMBOLS_BEGIN

#define Z_SYSTEM_API

Z_SYSTEM_API void    z_initialize_ticks (void);

Z_SYSTEM_API zuint64 z_ticks		(void);

Z_SYSTEM_API void    z_wait		(zuint64 time);

Z_C_SYMBOLS_END

#endif /* __ZSystem_time_H__ */
