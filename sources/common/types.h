/* ACME API - types.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#ifndef __ACME_types_H__
#define __ACME_types_H__

#include <Q/types/generic functions.h>
#include <Q/macros/node.h>
#include "MachineABI.h"

Q_DEFINE_LIST_NODE(ACMEIOBuffer,
	void*  buffer;
	void** frame;
	QDo    step;
);

typedef struct {
	quint		  frames_per_second;
	volatile quint	  maximum_frame_skip;
	volatile qboolean must_stop;
	ACMEIOBuffer*	  inputs;
	ACMEIOBuffer*	  outputs;
	void*		  context;
	MachineABI*	  abi;
	QSwitch		  power;
	QDo		  reset;
	QDo		  run_one_frame;
} ACMEMachine;

typedef struct {
} ACMEDevice;

#ifdef __cplusplus
extern "C" {
#endif

void *acme_machine_run(ACMEMachine *object);

#ifdef __cplusplus
}
#endif

#endif /* __ACME_types_H__ */
