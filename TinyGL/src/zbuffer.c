/*
 * 
 * Z buffer: 16 bits Z / 16 bits color
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "zbuffer.h"

ZBuffer *ZB_open(int xsize,int ysize,int mode,
		 int nb_colors,
		 unsigned char *color_indexes,
		 int *color_table,
		 void *frame_buffer)
{
  ZBuffer *zb;
  int size;
  
  zb=malloc(sizeof(ZBuffer));
  
  zb->xsize=xsize;
  zb->ysize=ysize;
  zb->mode=mode;
  switch(mode) {
   case ZB_MODE_INDEX:
    ZB_initDither(zb,nb_colors,color_indexes,color_table);
    break;
   case ZB_MODE_5R6G5B:
    zb->nb_colors=0;
    break;
   default:
    assert(0);
  }
  
  size=zb->xsize*zb->ysize*sizeof(unsigned short);
  
  zb->zbuf=malloc(size);

  if (frame_buffer == NULL) {
    zb->pbuf=malloc(size);
    zb->frame_buffer_allocated=1;
  } else {
    zb->frame_buffer_allocated=0;
    zb->pbuf=frame_buffer;
  }
  
  zb->current_texture=NULL;

  return zb;
}

void ZB_close(ZBuffer *zb)
{
  if (zb->mode == ZB_MODE_INDEX) 
    ZB_closeDither(zb);
  
  if (zb->frame_buffer_allocated) 
    free(zb->pbuf);

  free(zb->zbuf);
  free(zb);
}

void ZB_resize(ZBuffer *zb,void *frame_buffer,int xsize,int ysize)
{
  int size;

  zb->xsize=xsize;
  zb->ysize=ysize;

  size=zb->xsize*zb->ysize*sizeof(unsigned short);

  free(zb->zbuf);
  zb->zbuf=malloc(size);

  if (zb->frame_buffer_allocated) free(zb->pbuf);
  
  if (frame_buffer == NULL) {
    zb->pbuf=malloc(size);
    zb->frame_buffer_allocated=1;
  } else {
    zb->pbuf=frame_buffer;
    zb->frame_buffer_allocated=0;
  }
}

void ZB_copyFrameBuffer5R6G5B(ZBuffer *zb,
			      unsigned short *buf,
			      int size_x,int size_y,int min_x,int min_y)
{
  unsigned short *p,*p1,*q;
  int x,y,n;
	 
  q=zb->pbuf;
  p1=buf+min_y*size_x+min_x;
  
  for(y=0;y<zb->ysize;y++) {
    p=p1;
    n=zb->xsize>>2;
    for(x=0;x<n;x++) {
      p[0]=q[0];
      p[1]=q[1];
      p[2]=q[2];
      p[3]=q[3];
      p+=4;
      q+=4;
    }
    n=zb->xsize & 3;
    for(x=0;x<n;x++) *p++=*q++;

    p1+=size_x;
  }
}


void ZB_copyFrameBuffer(ZBuffer *zb,void *buf,
			int size_x,int size_y,int min_x,int min_y)
{
  switch(zb->mode) {
   case ZB_MODE_INDEX:
    ZB_ditherFrameBuffer(zb,buf,size_x,size_y,min_x,min_y);
    break;
   case ZB_MODE_5R6G5B:
    ZB_copyFrameBuffer5R6G5B(zb,buf,size_x,size_y,min_x,min_y);
    break;
   default:
    assert(0);
  }
}

/*
 * adr must be aligned on an 'int'
 */
void memset_s(void *adr,int val,int count)
{
  int i,n,v;
  unsigned int *p;
  unsigned short *q;

  p=adr;
  v=val | (val << 16);

  n=count >> 3;
  for(i=0;i<n;i++) {
    p[0]=v;
    p[1]=v;
    p[2]=v;
    p[3]=v;
    p+=4;
  }

  q=(unsigned short *)p;
  n=count & 7;
  for(i=0;i<n;i++) *q++=val;
}

void ZB_clear(ZBuffer *zb,int clear_z,int z,
	      int clear_color,int r,int g,int b)
{
  int size,color;
 
  size=zb->xsize*zb->ysize;

  if (clear_z) {
    memset_s(zb->zbuf,z,size);
  }

  if (clear_color) {
    color=RGB_TO_5R6G5B(r,g,b);
    memset_s(zb->pbuf,color,size);
  }
}



