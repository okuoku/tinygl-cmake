#include "zgl.h"

void glGetIntegerv(int pname,int *params)
{
  GLContext *c=gl_get_context();

  switch(pname) {
  case GL_VIEWPORT:
    params[0]=c->viewport.xmin;
    params[1]=c->viewport.ymin;
    params[2]=c->viewport.xsize;
    params[3]=c->viewport.ysize;
    break;
  default:
    gl_fatal_error("glGet: option not implemented");
    break;
  }
}
