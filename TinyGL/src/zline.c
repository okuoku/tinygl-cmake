#include <stdlib.h>
#include "zbuffer.h"

#define ZCMP(z,zpix) ((z) >= (zpix))
    
void ZB_plot(ZBuffer *zb,ZBufferPoint *p)
{
  unsigned short *pz,*pp;
  int adr,zz;
  
  adr=p->y*zb->xsize+p->x;
  pz=zb->zbuf + adr;
  pp=zb->pbuf + adr;
  zz= p-> z >> ZB_POINT_Z_FRAC_BITS;
  if (ZCMP(zz,*pz)) {
    *pp = RGB_TO_5R6G5B(p->r,p->g,p->b);
    *pz = zz;
  }
}

void ZB_line(ZBuffer *zb,ZBufferPoint *p1,ZBufferPoint *p2)
{
  int n,adr,zinc,dx,dy,sx,color;
  register int a, z, zz;
  register unsigned short *pp,*pz;
  
  if (p1->y > p2->y || (p1->y == p2->y && p1->x > p2->x)) {
    ZBufferPoint *tmp;
    tmp=p1;
    p1=p2;
    p2=tmp;
  }
  sx=zb->xsize;
  adr=(p1->y * sx + p1->x);
  pp = zb->pbuf + adr;
  pz = zb->zbuf + adr;
  
  dx = p2->x - p1->x;
  dy = p2->y - p1->y;
  z = p1->z;
  color = RGB_TO_5R6G5B(p2->r,p2->g,p2->b);

#define PUTPIXEL() 				\
  {						\
    zz=z >> ZB_POINT_Z_FRAC_BITS;		\
    if (ZCMP(zz,*pz))  { 			\
      *pp=color; 				\
      *pz=zz; 					\
    }						\
  }

#define DRAWLINE(dx,dy,inc_1,inc_2) \
    zinc=(p2->z-p1->z)/dx;\
    n=dx;\
    a=2*dy-dx;\
    dy=2*dy;\
    dx=2*dx-dy;\
	 do {\
      PUTPIXEL();\
			z+=zinc;\
			if (a>0) { pp+=(inc_1); pz+=(inc_1);  a-=dx; }\
			else { pp+=(inc_2); pz+=(inc_2); a+=dy; }\
	 } while (--n >= 0);

/* fin macro */

  if (dx == 0 && dy == 0) {
    PUTPIXEL();
  } else if (dx > 0) {
    if (dx >= dy) {
      DRAWLINE(dx, dy, sx + 1, 1);
    } else {
      DRAWLINE(dy, dx, sx + 1, sx);
    }
  } else {
    dx = -dx;
    if (dx >= dy) {
      DRAWLINE(dx, dy, sx - 1, -1);
    } else {
      DRAWLINE(dy, dx, sx - 1, sx);
    }
  }


#undef DRAWLINE
#undef PUTPIXEL
}
