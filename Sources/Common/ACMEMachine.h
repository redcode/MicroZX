/* ACME - ACMEMachine.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __ACME_ACMEMachine_H__
#define __ACME_ACMEMachine_H__

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void* acme_machine_run	      (ACMEMachine* object);

void  acme_machine_reset      (ACMEMachine* object);

void  acme_machine_set_power  (ACMEMachine* object,
			       qboolean	    state);

void  acme_machine_add_device (ACMEMachine* object,
			       ACMEDevice*  device);

#ifdef __cplusplus
}
#endif

#endif /* __ACME_ACMEMachine_H__ */
