#pragma once
#include <assert.h>

class psx_mdec
{
	unsigned int ctrl,status,
		max_rldata;
	int iq_y[64*64],
			iq_uv[64*64],
			costab[64],
			icostab[64],
			rlhead,
			rltail,
			rlsz;
	unsigned short *rldata;

	void init_iqtab(int *iq, unsigned char *sp);
	void init_costab(signed short *sp);
	void decode_rl(int *blk);
	void yuv2rgb15(int *blk, unsigned short *dp);
	void yuv2rgb24(int *blk, unsigned char *dp);
	void idct(int *block, int used_col);
	unsigned short get_rldata();
	void skip_nops();

public:
	psx_mdec();
	~psx_mdec();

	void start_in_dma(const unsigned int madr,
									 	const unsigned int bcr,
									 	const unsigned int chcr);
	void start_out_dma(const unsigned int madr,
									 		const unsigned int bcr,
									 		const unsigned int chcr);

	unsigned char read_byte(const unsigned int addr) { assert(0); return 0; }
	unsigned short read_word(const unsigned int addr) { assert(0); return 0; }
	unsigned int read_long(const unsigned int addr);
	void write_byte(const unsigned int addr, const unsigned char byte) { assert(0); }
	void write_word(const unsigned int addr, const unsigned short word) { assert(0); }
	void write_long(const unsigned int addr, const unsigned int v);
};
