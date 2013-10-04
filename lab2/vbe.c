#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	phys_bytes buf;
	struct reg86u rg;

	rg.u.w.ax = 0x4F01; // VBE get mode info - function 1

	rg.u.w.es = PB2BASE(buf);
	rg.u.w.di = PB2OFF(buf);
	rg.u.w.cx = mode;
	rg.u.b.intno = 0x10;

	if( sys_int86(&rg) != OK ) // fills the buffer with the info, right?
		return 1;

	struct mmap_t map; // this will have the virtual address to the struct vbe mode info

	lm_alloc(sizeof(struct vbe_mode_info_t), &map); // inicialize map



	return 1;
}


