/* QSystem API - time.c
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2006-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __QSystem_time_H__
#define __QSystem_time_H__

#include <Q/types/base.h>

Q_C_SYMBOLS_BEGIN

#define Q_SYSTEM_API

Q_SYSTEM_API void    q_initialize_ticks (void);

Q_SYSTEM_API quint64 q_ticks		(void);

Q_SYSTEM_API void    q_wait		(quint64 time);

Q_C_SYMBOLS_END

#endif /* __QSystem_time_H__ */
