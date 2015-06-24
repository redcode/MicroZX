/* mZX - MachineABI.h
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#ifndef mZX_MachineABI_h
#define mZX_MachineABI_h

#include <Q/types/generic functions.h>

typedef struct {
	QString*	file_name;
	qsize		base_address;
	qsize		size;
} ROM;

typedef struct {

	QString*	model_name;
	qsize		context_size;
	qsize		context_cpu_context_offset;
	qsize		memory_size;
	qsize		rom_count;
	ROM*		roms;
	QDo		initialize;
	QSwitch		power;
	QDo		reset;
	QDo		run_one_frame;
	QDo		run_one_scanline;

} MachineABI;

extern MachineABI machine_abi_table[];
extern qsize	  machine_abi_count;

#endif
