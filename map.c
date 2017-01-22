void map_apply_gravity() {
	for(int k=0;k<map_checked_voxels_index;k++) {
		unsigned int col = map_get(map_checked_voxels_x[k],map_checked_voxels_y[k],map_checked_voxels_z[k]);
		if(col!=0xFFFFFFFF) {
			map_set(map_checked_voxels_x[k],map_checked_voxels_y[k],map_checked_voxels_z[k],0xFFFFFFFF);
			//particle_create(col,map_checked_voxels_x[k]+0.5F,map_checked_voxels_y[k]+0.5F,map_checked_voxels_z[k]+0.5F,2.5F,1.0F,8,0.1F,0.25F);
		}
	}
}

boolean map_checked_voxels_contains(int x, int y, int z) {
	for(int k=map_checked_voxels_index-1;k>=0;k--) {
		if(map_checked_voxels_x[k]==x && map_checked_voxels_y[k]==y && map_checked_voxels_z[k]==z) {
			return true;
		}
	}
	return false;
}

void map_ground_connected_sub(int x, int y, int z, int depth) {
	map_checked_voxels_x[map_checked_voxels_index] = x;
	map_checked_voxels_y[map_checked_voxels_index] = y;
	map_checked_voxels_z[map_checked_voxels_index++] = z;
	
	if(y==0) {
		map_ground_connected_result = true;
		return;
	}
	
	if((map_get(x,y-1,z)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x,y-1,z)) {
		map_ground_connected_sub(x,y-1,z,depth+1);
	}
	if(map_ground_connected_result) {
		return;
	}
	if((map_get(x+1,y,z)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x+1,y,z)) {
		map_ground_connected_sub(x+1,y,z,depth+1);
	}
	if(map_ground_connected_result) {
		return;
	}
	if((map_get(x-1,y,z)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x-1,y,z)) {
		map_ground_connected_sub(x-1,y,z,depth+1);
	}
	if(map_ground_connected_result) {
		return;
	}
	if((map_get(x,y,z+1)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x,y,z+1)) {
		map_ground_connected_sub(x,y,z+1,depth+1);
	}
	if(map_ground_connected_result) {
		return;
	}
	if((map_get(x,y,z-1)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x,y,z-1)) {
		map_ground_connected_sub(x,y,z-1,depth+1);
	}
	if(map_ground_connected_result) {
		return;
	}
	if((map_get(x,y+1,z)&0xFFFFFFFF)!=0xFFFFFFFF && !map_checked_voxels_contains(x,y+1,z)) {
		map_ground_connected_sub(x,y+1,z,depth+1);
	}
}

boolean map_ground_connected(int x, int y, int z) {
	map_checked_voxels_index = 0;
	map_ground_connected_result = false;
	map_ground_connected_sub(x,y,z,0);
	return map_ground_connected_result;
}

unsigned long long map_get(int x, int y, int z) {
	if(x<0 || y<0 || z<0 || x>=map_size_x || y>=map_size_y || z>=map_size_z) {
		return 0xFFFFFFFF;
	}
	
	return map_colors[x+(y*map_size_z+z)*map_size_x];
}

void map_set(int x, int y, int z, unsigned long long color) {
	if(x<0 || y<0 || z<0 || x>=map_size_x || y>=map_size_y || z>=map_size_z) {
		return;
	}
	
	map_colors[x+(y*map_size_z+z)*map_size_x] = color;
	chunk_block_update(x,y,z);
	
	unsigned char x_off = x%CHUNK_SIZE;
	unsigned char z_off = z%CHUNK_SIZE;
	
	if(x>0 && x_off==0) {
		chunk_block_update(x-1,y,z);
	}
	if(z>0 && z_off==0) {
		chunk_block_update(x,y,z-1);
	}
	if(x<map_size_x-1 && x_off==CHUNK_SIZE-1) {
		chunk_block_update(x+1,y,z);
	}
	if(z<map_size_z-1 && z_off==CHUNK_SIZE-1) {
		chunk_block_update(x,y,z+1);
	}
	if(x>0 && z>0 && x_off==0 && z_off==0) {
		chunk_block_update(x-1,y,z-1);
	}
	if(x<map_size_x-1 && z<map_size_z-1 && x_off==CHUNK_SIZE-1 && z_off==CHUNK_SIZE-1) {
		chunk_block_update(x+1,y,z+1);
	}
	if(x>0 && z<map_size_z-1 && x_off==0 && z_off==CHUNK_SIZE-1) {
		chunk_block_update(x-1,y,z+1);
	}
	if(x<map_size_x-1 && z>0 && x_off==CHUNK_SIZE-1 && z_off==0) {
		chunk_block_update(x+1,y,z-1);
	}
}

void map_vxl_setgeom(int x, int y, int z, unsigned int t, unsigned long long* map) {
	if(x<0 || y<0 || z<0 || x>=map_size_x || y>=map_size_z || z>=map_size_y) {
		return;
	}
	
	map[x+((map_size_y-1-z)*map_size_z+y)*map_size_x] = t;
}

void map_vxl_setcolor(int x, int y, int z, unsigned int t, unsigned long long* map) {
	if(x<0 || y<0 || z<0 || x>=map_size_x || y>=map_size_z || z>=map_size_y) {
		return;
	}
	/*x *= 2;
	y *= 2;
	z *= 2;*/
	unsigned char r = t & 255;
	unsigned char g = (t>>8) & 255;
	unsigned char b = (t>>16) & 255;
	map[x+((map_size_y-1-z)*map_size_z+y)*map_size_x] = (r<<16)|(g<<8)|b;
	
	/*map[x+1+((map_size_y-1-z)*map_size_z+y)*map_size_x] = (r<<16)|(g<<8)|b;
	map[x+((map_size_y-1-z)*map_size_z+y+1)*map_size_x] = (r<<16)|(g<<8)|b;
	map[x+1+((map_size_y-1-z)*map_size_z+y+1)*map_size_x] = (r<<16)|(g<<8)|b;
	
	map[x+((map_size_y-1-z-1)*map_size_z+y)*map_size_x] = (r<<16)|(g<<8)|b;
	map[x+1+((map_size_y-1-z-1)*map_size_z+y)*map_size_x] = (r<<16)|(g<<8)|b;
	map[x+((map_size_y-1-z-1)*map_size_z+y+1)*map_size_x] = (r<<16)|(g<<8)|b;
	map[x+1+((map_size_y-1-z-1)*map_size_z+y+1)*map_size_x] = (r<<16)|(g<<8)|b;*/
}

void map_vxl_load_s(unsigned char* v) {
	map_vxl_load(v,map_colors);
}

void map_vxl_load(unsigned char* v, unsigned long long* map) {
   unsigned char* base = v;
   int x,y,z;
   for (y=0; y < 512; ++y) {
      for (x=0; x < 512; ++x) {
         for (z=0; z < 64; ++z) {
            map_vxl_setgeom(x,y,z,0x004080,map);
         }
         z = 0;
         for(;;) {
            unsigned int *color;
            int i;
            int number_4byte_chunks = v[0];
            int top_color_start = v[1];
            int top_color_end   = v[2]; // inclusive
            int bottom_color_start;
            int bottom_color_end; // exclusive
            int len_top;
            int len_bottom;

            for(i=z; i < top_color_start; i++)
               map_vxl_setgeom(x,y,i,0xFFFFFFFF,map);

            color = (unsigned int *) (v+4);
            for(z=top_color_start; z <= top_color_end; z++)
               map_vxl_setcolor(x,y,z,*color++,map);

            len_bottom = top_color_end - top_color_start + 1;

            // check for end of data marker
            if (number_4byte_chunks == 0) {
               // infer ACTUAL number of 4-byte chunks from the length of the color data
               v += 4 * (len_bottom + 1);
               break;
            }

            // infer the number of bottom colors in next span from chunk length
            len_top = (number_4byte_chunks-1) - len_bottom;

            // now skip the v pointer past the data to the beginning of the next span
            v += v[0]*4;

            bottom_color_end   = v[3]; // aka air start
            bottom_color_start = bottom_color_end - len_top;

            for(z=bottom_color_start; z < bottom_color_end; ++z) {
               map_vxl_setcolor(x,y,z,*color++,map);
            }
         }
      }
   }
   v = base;
}