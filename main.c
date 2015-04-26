#ifdef TARGET_GBA
#include <stdint.h>
#include <sys/types.h>
#include "gba.h"

#define printf(...)
#define assert(v) if(!(v)) { ((volatile uint16_t *)VPAL0)[0] = 0xFFF; for(;;) {} }
#else
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <assert.h>
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

const int16_t lsintab[256] = {
0,100,200,301,401,501,601,700,799,897,995,1092,1189,1284,1379,1474,1567,1659,1751,1841,1930,2018,2105,2191,2275,2358,2439,2519,2598,2675,2750,2824,2896,2966,3034,3101,3166,3229,3289,3348,3405,3460,3513,3563,3612,3658,3702,3744,3784,3821,3856,3889,3919,3947,3973,3996,4017,4035,4051,4065,4076,4084,4091,4094,4096,4094,4091,4084,4076,4065,4051,4035,4017,3996,3973,3947,3919,3889,3856,3821,3784,3744,3702,3658,3612,3563,3513,3460,3405,3348,3289,3229,3166,3101,3034,2966,2896,2824,2750,2675,2598,2519,2439,2358,2275,2191,2105,2018,1930,1841,1751,1659,1567,1474,1379,1284,1189,1092,995,897,799,700,601,501,401,301,200,100,0,-100,-200,-301,-401,-501,-601,-700,-799,-897,-995,-1092,-1189,-1284,-1379,-1474,-1567,-1659,-1751,-1841,-1930,-2018,-2105,-2191,-2275,-2358,-2439,-2519,-2598,-2675,-2750,-2824,-2896,-2966,-3034,-3101,-3166,-3229,-3289,-3348,-3405,-3460,-3513,-3563,-3612,-3658,-3702,-3744,-3784,-3821,-3856,-3889,-3919,-3947,-3973,-3996,-4017,-4035,-4051,-4065,-4076,-4084,-4091,-4094,-4096,-4094,-4091,-4084,-4076,-4065,-4051,-4035,-4017,-3996,-3973,-3947,-3919,-3889,-3856,-3821,-3784,-3744,-3702,-3658,-3612,-3563,-3513,-3460,-3405,-3348,-3289,-3229,-3166,-3101,-3034,-2966,-2896,-2824,-2750,-2675,-2598,-2519,-2439,-2358,-2275,-2191,-2105,-2018,-1930,-1841,-1751,-1659,-1567,-1474,-1379,-1284,-1189,-1092,-995,-897,-799,-700,-601,-501,-401,-301,-200,-100
};

uint16_t izlut[1024];

typedef struct kdnode_s kdnode;
struct kdnode_s
{
	int32_t offs;
	int16_t iparent;
	int16_t ipos;
	int16_t ineg;
	uint8_t axis;
	uint8_t PAD[1]; // knived
};

typedef struct kdtree_s kdtree;
struct kdtree_s
{
	size_t size;
	size_t maxsize;
	kdnode *data;
};

#ifdef TARGET_GBA
#include "render.c"
#endif

void kd_expand(kdtree *ktree, int needed_size)
{
	if(needed_size <= ktree->maxsize)
		return;

#ifdef TARGET_GBA
	assert(0);
#else
	needed_size += needed_size>>1;
	needed_size += 1;
	ktree->maxsize = needed_size;
	ktree->data = realloc(ktree->data, sizeof(kdnode)*needed_size);
#endif
}

int kd_peek(kdtree *ktree, int x, int y)
{
	// Get root
	int rnode = 0;

	// Scan
	while(rnode >= 0)
	{
		// Get node
		kdnode *kd = &ktree->data[rnode];

		// Get offset
		int offs = (kd->axis == 0 ? x : y);

		// Compare and enter
		if(offs < kd->offs)
			rnode = kd->ineg;
		else
			rnode = kd->ipos;

	}

	// Return
	return rnode;
}

void kd_print(kdtree *ktree, int rnode, int tabs, char mode)
{
	int i;

	// Get root
	if(rnode < 0)
		rnode = 0;

	// Get node
	kdnode *kd = &ktree->data[rnode];

	// Print tabs
	for(i = 0; i < tabs; i++)
		printf(" ");

	// Print info
	printf("%c: %i: p=%i, a=%i, o=%i, c=(%i, %i)\n",
		mode, rnode,
		kd->iparent, kd->axis, kd->offs, kd->ineg, kd->ipos);
	
	if(kd->ineg >= 0)
		kd_print(ktree, kd->ineg, tabs+2, '-');
	if(kd->ipos >= 0)
		kd_print(ktree, kd->ipos, tabs+2, '+');
}

int kd_add_node(kdtree *ktree, int axis, int offs, int iparent, int ineg, int ipos)
{
	int idx = ktree->size;
	kd_expand(ktree, ktree->size+1);
	ktree->size++;

	kdnode *kd = &ktree->data[idx];
	kd->axis = axis;
	kd->offs = offs;
	kd->iparent = iparent;
	kd->ipos = ipos;
	kd->ineg = ineg;

	return idx;
}

void kd_add_box_direct(kdtree *ktree, int parent, int rnode,
	int x1, int y1, int x2, int y2,
	int rx1, int ry1, int rx2, int ry2, int icontain)
{
	// xy12 = subbox
	// rxy12 = real box
	// TODO: have a range limit box to prevent duplicate nodes
	printf("add %i %i (%i,%i)-(%i,%i) = %i\n", parent, rnode, x1, y1, x2, y2, icontain);

	// If we have an empty tree, create root
	if(ktree->size == 0)
	{
		int idx0 = kd_add_node(ktree, 0, x1,   -1, -1, icontain);
		int idx1 = kd_add_node(ktree, 1, y1, idx0, -1, icontain);
		int idx2 = kd_add_node(ktree, 0, x2, idx1, icontain, -1);
		int idx3 = kd_add_node(ktree, 1, y2, idx2, icontain, -1);

		ktree->data[idx0].ipos = idx1;
		ktree->data[idx1].ipos = idx2;
		ktree->data[idx2].ineg = idx3;

		return;
	}

	// If rnode is negative, check parent
	if(rnode < 0)
	{
		if(parent == -1)
		{
			// Get root
			rnode = 0;

		} else {
			// Get parent
			kdnode *kd = &ktree->data[parent];

			// Find side to split onto
			assert(kd->axis == 0 || kd->axis == 1);

			int16_t *nodeloc = NULL;
			int o1 = (kd->axis == 0 ? x1 : y1);
			int o2 = (kd->axis == 0 ? x2 : y2);
			int ro1 = (kd->axis == 0 ? rx1 : ry1);
			int ro2 = (kd->axis == 0 ? rx2 : ry2);
			assert(o1 < o2); // strictly less

			if(o1 >= kd->offs && o2 >= kd->offs)
			{
				nodeloc = &kd->ipos;
			} else {
				nodeloc = &kd->ineg;
			}

			// Apply perpendicular splits
			int node_bg = *nodeloc;
			int lidx = parent;
			int nidx;

			switch(kd->axis)
			{
				case 0:
					if(y1 == ry1)
					{
						nidx = kd_add_node(ktree, 1, y1, lidx, node_bg, icontain);
						*nodeloc = nidx;
						lidx = nidx;
						nodeloc = &ktree->data[lidx].ipos;
					}

					if(y2 == ry2)
					{
						nidx = kd_add_node(ktree, 1, y2, lidx, icontain, node_bg);
						*nodeloc = nidx;
						lidx = nidx;
						nodeloc = &ktree->data[lidx].ineg;
					}

					break;

				case 1:
					if(x1 == rx1)
					{
						nidx = kd_add_node(ktree, 0, x1, lidx, node_bg, icontain);
						*nodeloc = nidx;
						lidx = nidx;
						nodeloc = &ktree->data[lidx].ipos;
					}

					if(x2 == rx2)
					{
						nidx = kd_add_node(ktree, 0, x2, lidx, icontain, node_bg);
						*nodeloc = nidx;
						lidx = nidx;
						nodeloc = &ktree->data[lidx].ineg;
					}

					break;
			}

			// Add neg node
			if(o1 != kd->offs && o1 == ro1)
			{
				nidx = kd_add_node(ktree, kd->axis, o1, lidx, node_bg, icontain);
				*nodeloc = nidx;
				lidx = nidx;
				nodeloc = &ktree->data[lidx].ipos;
			}

			// Add pos node
			if(o2 != kd->offs && o2 == ro2)
			{
				nidx = kd_add_node(ktree, kd->axis, o2, lidx, icontain, node_bg);
				*nodeloc = nidx;
				lidx = nidx;
				nodeloc = &ktree->data[lidx].ineg;
			}

			// Done! Return.
			return;
		}
	}

	// Get rnode structure
	kdnode *kd = &ktree->data[rnode];

	// Get offsets according to node axis
	assert(kd->axis == 0 || kd->axis == 1);
	int offsn = (kd->axis == 0 ? x1 : y1);
	int offsp = (kd->axis == 0 ? x2 : y2);

	printf("offs %i < %i < %i\n", offsn, kd->offs, offsp);

	// Split against negative
	if(offsn < kd->offs)
	switch(kd->axis)
	{
		case 0:
			kd_add_box_direct(ktree, rnode, kd->ineg,
				x1, y1, MIN(x2, kd->offs), y2,
				rx1, ry1, rx2, ry2,
				icontain);
			break;
		case 1:
			kd_add_box_direct(ktree, rnode, kd->ineg,
				x1, y1, x2, MIN(y2, kd->offs),
				rx1, ry1, rx2, ry2,
				icontain);
			break;
	}

	// Split against positive
	if(offsp > kd->offs)
	switch(kd->axis)
	{
		case 0:
			kd_add_box_direct(ktree, rnode, kd->ipos,
				MAX(x1, kd->offs), y1, x2, y2,
				rx1, ry1, rx2, ry2,
				icontain);
			break;
		case 1:
			kd_add_box_direct(ktree, rnode, kd->ipos,
				x1, MAX(y1, kd->offs), x2, y2,
				rx1, ry1, rx2, ry2,
				icontain);
			break;
	}
}

void kd_add_conv_poly(kdtree *ktree, int icontain, int ptcount, int *ptlist)
{
	int i;
	int p = ptcount-1;

	for(i = 0; i < ptcount; i++)
	{
		// Get coord pair
		int x1 = ptlist[2*p+0];
		int y1 = ptlist[2*p+1];
		int x2 = ptlist[2*i+0];
		int y2 = ptlist[2*i+1];

		// TODO: handle diagonal cases
		assert((x1 == x2 ?1:0) != (y1 == y2 ?1:0));

		// Add line segment(s)
		// TODO!

		// Advance
		p = i;
	}
}

void kd_add_box(kdtree *ktree, int icontain, int x1, int y1, int x2, int y2)
{
	/*
	int ptlist[4*2];

	ptlist[0] = x1; ptlist[1] = y1;
	ptlist[2] = x1; ptlist[3] = y2;
	ptlist[4] = x2; ptlist[5] = y2;
	ptlist[6] = x2; ptlist[7] = y1;

	kd_add_conv_poly(ktree, icontain, 4, ptlist);
	*/

	kd_add_box_direct(ktree, -1, -1, x1, y1, x2, y2, x1, y1, x2, y2, icontain);
}

#ifndef TARGET_GBA
kdtree *kd_new(int init_size)
{
	kdtree *ktree = malloc(sizeof(kdtree));

	ktree->size = 0;
	ktree->maxsize = init_size;
	ktree->data = malloc(sizeof(kdnode)*init_size);

	return ktree;
}
#endif

#ifdef TARGET_GBA
#define MAX_KNODES 1024
kdtree base_tree;
__attribute__((section(".slowmem")))
kdnode base_nodes[MAX_KNODES];
#endif

int main(int argc, char *argv[])
{
	int x, y, i;

#ifdef TARGET_GBA
	kdtree *ktree = &base_tree;
	ktree->size = 0;
	ktree->maxsize = MAX_KNODES;
	ktree->data = base_nodes;
#else
	kdtree *ktree = kd_new(128);
#endif
	printf("kd tree %p\n", ktree);

	// fill
	kd_add_box(ktree, -6, 45, 28, 65, 30);
	kd_add_box(ktree, -2, 10, 5, 50, 25);
	kd_add_box(ktree, -4, 60, 10, 70, 25);
	kd_add_box(ktree, -5, 48, 14, 62, 18);

	// cut
	kd_add_box(ktree, -1, 25, 23, 35, 25);
	kd_add_box(ktree, -1, 12, 7, 48, 23);
	kd_add_box(ktree, -1, 62, 12, 68, 23);
	kd_add_box(ktree, -1, 51, 14, 54, 18);
	kd_add_box(ktree, -1, 56, 14, 59, 18);
	kd_add_box(ktree, -1, 48, 15, 62, 17);
	kd_add_box(ktree, -1, 68, 21, 70, 23);

	// subfill -2
	kd_add_box(ktree, -3, 20, 10, 35, 15);

	printf("\nOptimising\n");
	for(i = ktree->size-1; i >= 0; i--)
	{
		kdnode *kd = &ktree->data[i];
		if(kd->ineg == kd->ipos && kd->iparent >= 0)
		{
			printf("merging %i->%i->(%i, %i)\n",
				kd->iparent, i, kd->ineg, kd->ipos);
			kdnode *kpar = &ktree->data[kd->iparent];
			if(kpar->ineg == i) kpar->ineg = kd->ineg;
			if(kpar->ipos == i) kpar->ipos = kd->ipos;
		}
	}

	printf("\nTree:\n");
	kd_print(ktree, -1, 0, '=');
	printf("\n");

	for(y = 0; y < 35; y++)
	{
		for(x = 0; x < 80; x++)
		{
			int v = -(kd_peek(ktree, x, y)+1);
#ifdef TARGET_GBA
			VRAM0D[y*120+x] = v*0x0101;
#endif
			printf("%c", '0'+v);
		}

		printf("\n");
	}

	printf("\nSplits:\n");
	for(y = 0; y < 35; y++)
	{
		for(x = 0; x < 80; x++)
		{
			int rnode = 0;
			char c = ' ';

			while(rnode >= 0)
			{
				kdnode *kd = &ktree->data[rnode];
				int offs = (kd->axis == 0 ? x : y);

				if(offs < kd->offs)
				{
					rnode = kd->ineg;

				} else if(offs > kd->offs) {
					rnode = kd->ipos;

				} else {
					c = (kd->axis == 0 ? '|' : '-');
#ifdef TARGET_GBA
					VRAM0D[y*120+x] = rnode+1;
#endif
					break;
				}
			}

			if(c == ' ' && rnode != -1) c = '0'+(-1-rnode);
			printf("%c", c);
		}

		printf("\n");
	}

	printf("\nNode count: %i\n", ktree->size);

#ifdef TARGET_GBA
	render_loop(ktree);
#endif

	return 0;
}

#ifdef TARGET_GBA
void _start(void)
{
	int i;

	IME = 0;
	IE = 0;
	DISPCNT = 0x0404;

	for(i = 0; i < 256; i++)
	{
		VPAL0[i] = 0
			| (((i>>0)&3)<<3)
			| (((i>>2)&7)<<7)
			| (((i>>5)&7)<<12);

	}

	izlut[0] = 0xFFFF;
	for(i = 1; i < 1024; i++)
		izlut[i] = 0x8000/i;

	main(0, NULL);

	((volatile uint16_t *)VPAL0)[0] = 0x1F;
	for(;;) {}
}

#endif


