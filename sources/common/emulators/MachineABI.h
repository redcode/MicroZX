/* mZX - MachineABI.h
	      __	   __
  _______ ___/ /______ ___/ /__
 / __/ -_) _  / __/ _ \ _  / -_)
/_/  \__/\_,_/\__/\___/_,_/\__/
Copyright © 2013 RedCode Software. All rights reserved. */

#ifndef mZX_MachineABI_h
#define mZX_MachineABI_h

#include <Z/types/generic functions.h>

Z_C_SYMBOLS_BEGIN

typedef struct {
	ZString* file_name;
	zsize	 base_address;
	zsize	 size;
} ROM;

typedef struct {
	ZString* model_name;
	zsize	 context_size;
	zsize	 context_cpu_context_offset;
	zsize	 memory_size;
	zsize	 rom_count;
	ROM*	 roms;
	ZDo	 initialize;
	ZSwitch	 power;
	ZDo	 reset;
	ZDo	 run_one_frame;
	ZDo	 run_one_scanline;
} MachineABI;

extern MachineABI machine_abi_table[];
extern zsize	  machine_abi_count;

Z_C_SYMBOLS_END

#endif
