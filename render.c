#define PLANE_NEAR 0x0010
#define PLANE_FAR  0x00040000

//define SLOWDIV

int render_page = 0;

int player_x = (43<<12)+(1<<11);
int player_y = (33<<12)+(1<<11);
int player_ang = 0;
int player_ax = 724;
int player_ay = -724;

int render_pillar(kdtree *ktree, int rnode, int tmin, int tmax,
	int sx, int camx, int camy, int icamax, int icamay)
{
	uint16_t *pillar = VRAM0D + render_page*0x5000 + sx;

	for(;;)
	{
		// Skip degenerates
		if(tmin > tmax) return -1;

		if(rnode < 0)
		{
			//if(rnode == -1) return -1;

			// Draw span
			uint16_t v = (uint16_t)(-1-rnode);
			v *= 0x0101;

			// TODO: work out proper Y value
#ifdef SLOWDIV
			int yb = 0x40000/tmin;
#else
			int yb = tmin >= 1024
				? (tmin >= (1024<<6)
					? (izlut[(tmin>>12)]>>9)
					: (izlut[(tmin>>6)]>>3))
				: (izlut[tmin]<<3);
#endif
			int y;
			yb = MAX(0, MIN(79-1, yb));
			int y1 = 80-yb;
			int y2 = 80+yb;
			if(rnode == -1)
			{
				pillar[120*y1] = 0xFFFF;
				pillar[120*y2] = 0xFFFF;

			} else {
				for(y = y1; y < y2; y++)
				{
					pillar[120*y] = v;//0x0202;
				}
			}

			if(rnode == -1) return -1;

			// Return
			return rnode;
		}

		// Scan k-d tree
		kdnode *kd = &ktree->data[rnode];

		// Get offsets
		int offsc = (kd->axis == 0 ? camx : camy);
		int offsia = (kd->axis == 0 ? icamax : icamay);

		// Get times to plane
		int time = (((kd->offs<<12)-offsc)<<8);
		if(offsia == 0)
			time <<= 1;
		else
			time = (((int64_t)time)*((int64_t)offsia))>>17;

		// Get nodes
		int near_node = ((offsc < kd->offs) != (offsia >= 0) ? kd->ineg : kd->ipos);
		int  far_node = ((offsc < kd->offs) == (offsia >= 0) ? kd->ineg : kd->ipos);

		// Do nearest node if time in range
		// Otherwise do farthest node if in range
		// Otherwise return air
		if(time >= tmin + 0x10)
		{
			// Check if we also have a far node
			if(time < tmax - 0x10)
			{
				// Do near node first
				int ret = render_pillar(ktree, near_node,
					tmin, MIN(time, tmax),
					sx, camx, camy,
					icamax, icamay);

				if(ret < -1) return ret;

				// Continue onto far node
				rnode = far_node;
				tmin = MAX(time, tmin);
			} else {
				// Continue onto near node
				rnode = near_node;
				tmax = MIN(time, tmax);
			}
		} else if(time < tmax - 0x10) {
			// Continue onto near node
			rnode = far_node;
			tmin = MAX(time, tmin);
		} else {
			return -1;
		}
	}
}

void putpixel(int x, int y, uint16_t c)
{
	if(x < 0 || x >= 120 || y < 0 || y >= 160)
		return;

	VRAM0D[y*120+x + render_page*0x5000] = c;
}

void render_frame(kdtree *ktree)
{
	// Get camera
	int camx = player_x;
	int camy = player_y;
	int camax = player_ax;
	int camay = player_ay;

	// Calculate frustum (using 90FOV because easy)
	int camax1 = camax + camay;
	int camay1 = camay - camax;
	int camax2 = camax - camay;
	int camay2 = camay + camax;
	int ocamax1 = camax1;
	int ocamay1 = camay1;

	// Draw span (clamping 90FOV to something slightly smaller)
	int x;
	int camdx = (camax2 - camax1);
	int camdy = (camay2 - camay1);
	camax1 <<= 7;
	camay1 <<= 7;
	camax1 += camdx*4;
	camay1 += camdy*4;
	for(x = 0; x < 120; x++)
	{
		int y;

		// Clear pillar
		uint16_t *pillar = VRAM0D + render_page*0x5000 + x;
		for(y = 0; y < 160; y++)
			pillar[y*120] = 0x0000;

		// TODO: proper span engine
		// (we're just raycasting for now)

		int icamax1 = camax1;
		int icamay1 = camay1;
		int signax1 = icamax1 < 0;
		int signay1 = icamay1 < 0;

		if(signax1) icamax1 = -icamax1;
#ifdef SLOWDIV
		icamax1 = 0x1000000/icamax1;
#else
		if(icamax1 >= 1024)
		{
			if(icamax1 >= (1024<<6))
			{
				icamax1 = izlut[icamax1>>12]>>2;
			} else {
				icamax1 = izlut[icamax1>>6]<<4;
			}
		} else {
			if(icamax1 != 0)
				icamax1 = izlut[icamax1]<<10;
		}
#endif
		if(signax1) icamax1 = -icamax1;

		if(signay1) icamay1 = -icamay1;
#ifdef SLOWDIV
		icamay1 = 0x1000000/icamay1;
#else
		if(icamay1 >= 1024)
		{
			if(icamay1 >= (1024<<6))
			{
				icamay1 = izlut[icamay1>>12]>>2;
			} else {
				icamay1 = izlut[icamay1>>6]<<4;
			}
		} else {
			if(icamay1 != 0)
				icamay1 = izlut[icamay1]<<10;
		}
#endif
		if(signay1) icamay1 = -icamay1;

		render_pillar(ktree, 0,
			PLANE_NEAR, PLANE_FAR,
			x,
			camx, camy, icamax1, icamay1);

		camax1 += camdx;
		camay1 += camdy;
	}

	//VPAL0[0]++;

	// DEBUG: Draw camera info
	putpixel((camx>>12), (camy>>12), 0xFFFF);
	putpixel((camx+ocamax1*8)>>12, (camy+ocamay1*8)>>12, 0xE0E0);
	putpixel((camx+camax*8)>>12, (camy+camay*8)>>12, 0x1C1C);
	putpixel((camx+camax2*8)>>12, (camy+camay2*8)>>12, 0x0303);

	// Flip
	DISPCNT = 0x0404 | (render_page<<4);
	render_page = 1 - render_page;
}

void render_loop(kdtree *ktree)
{
	for(;;)
	{
		player_ax = lsintab[(-(player_ang>>4)+0x00)&255]>>2;
		player_ay = lsintab[((player_ang>>4)-0x40)&255]>>2;
		render_frame(ktree);

		int keys = KEYINPUT;

		if(keys & KEY_L) player_ang -= 4<<4;
		if(keys & KEY_R) player_ang += 4<<4;

		int mf = 0;
		int mr = 0;
		if(keys & KEY_UP)    mf++;
		if(keys & KEY_DOWN)  mf--;
		if(keys & KEY_RIGHT) mr++;
		if(keys & KEY_LEFT)  mr--;

		int ms = lsintab[(-(player_ang>>4)+0x00)&255]>>2;
		int mc = lsintab[((player_ang>>4)+0x40)&255]>>2;

		mf *= (100<<12)/120;
		mr *= (100<<12)/120;

		int mx = (-mc*mr - ms*mf + (1<<11))>>12;
		int my = ( mc*mf - ms*mr + (1<<11))>>12;

		player_x += mx;
		player_y += my;
	}
}
