#include "zgl.h"

void gl_print_matrix( const float *m)
{
   int i;

   for (i=0;i<4;i++) {
      fprintf(stderr,"%f %f %f %f\n", m[i], m[4+i], m[8+i], m[12+i] );
   }
}

static inline void gl_matrix_update(GLContext *c)
{
  c->matrix_model_projection_updated=(c->matrix_mode<=1);
}


void glopMatrixMode(GLContext *c,GLParam *p)
{
  int mode=p[1].i;
  switch(mode) {
  case GL_MODELVIEW:
    c->matrix_mode=0;
    break;
  case GL_PROJECTION:
    c->matrix_mode=1;
    break;
  case GL_TEXTURE:
    c->matrix_mode=2;
    break;
  default:
    assert(0);
  }
}

void glopLoadMatrix(GLContext *c,GLParam *p)
{
  M4 *m;
  int i;
  GLParam *q;

  m=c->matrix_stack_ptr[c->matrix_mode];
  q=p+1;

  for(i=0;i<4;i++) {
    m->m[0][i]=q[0].f;
    m->m[1][i]=q[1].f;
    m->m[2][i]=q[2].f;
    m->m[3][i]=q[3].f;
    q+=4;
  }

  gl_matrix_update(c);
}

void glopLoadIdentity(GLContext *c,GLParam *p)
{

  gl_M4_Id(c->matrix_stack_ptr[c->matrix_mode]);

  gl_matrix_update(c);
}

void glopMultMatrix(GLContext *c,GLParam *p)
{
  GLParam *q;
  M4 m;
  int i;

  q=p+1;

  for(i=0;i<4;i++) {
    m.m[0][i]=q[0].f;
    m.m[1][i]=q[1].f;
    m.m[2][i]=q[2].f;
    m.m[3][i]=q[3].f;
    q+=4;
  }

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}


void glopPushMatrix(GLContext *c,GLParam *p)
{
  int n=c->matrix_mode;
  M4 *m;

  assert( (c->matrix_stack_ptr[n] - c->matrix_stack[n] + 1 )
	   < c->matrix_stack_depth_max[n] );

  m=++c->matrix_stack_ptr[n];
  
  gl_M4_Move(&m[0],&m[-1]);

  gl_matrix_update(c);
}

void glopPopMatrix(GLContext *c,GLParam *p)
{
  int n=c->matrix_mode;

  assert( c->matrix_stack_ptr[n] > c->matrix_stack[n] );
  c->matrix_stack_ptr[n]--;
  gl_matrix_update(c);
}


void glopRotate(GLContext *c,GLParam *p)
{
  M4 m;
  V3 dir;
  float angle;
  int dir_code;

  angle = p[1].f * M_PI / 180.0;
  dir.X=p[2].f;
  dir.Y=p[3].f;
  dir.Z=p[4].f;

  /* simple case detection */
  dir_code = ((dir.X != 0)<<2) | ((dir.Y != 0)<<1) | (dir.Z != 0);

  switch(dir_code) {
  case 0:
    gl_M4_Id(&m);
    break;
  case 4:
    if (dir.X < 0) angle=-angle;
    gl_M4_Rotate(&m,angle,0);
    break;
  case 2:
    if (dir.Y < 0) angle=-angle;
    gl_M4_Rotate(&m,angle,1);
    break;
  case 1:
    if (dir.Z < 0) angle=-angle;
    gl_M4_Rotate(&m,angle,2);
    break;
  default:
    gl_fatal_error("glRotate: general rotation not implemented");
  }

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}

void glopScale(GLContext *c,GLParam *p)
{
  float *m;
  float x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_stack_ptr[c->matrix_mode]->m[0][0];

  m[0] *= x;   m[1] *= y;   m[2]  *= z;
  m[4] *= x;   m[5] *= y;   m[6]  *= z;
  m[8] *= x;   m[9] *= y;   m[10] *= z;
  m[12] *= x;   m[13] *= y;   m[14] *= z;
  gl_matrix_update(c);
}

void glopTranslate(GLContext *c,GLParam *p)
{
  float *m;
  float x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_stack_ptr[c->matrix_mode]->m[0][0];

  m[3] = m[0] * x + m[1] * y + m[2]  * z + m[3];
  m[7] = m[4] * x + m[5] * y + m[6]  * z + m[7];
  m[11] = m[8] * x + m[9] * y + m[10] * z + m[11];
  m[15] = m[12] * x + m[13] * y + m[14] * z + m[15];

  gl_matrix_update(c);
}


void glopFrustum(GLContext *c,GLParam *p)
{
  float *r;
  M4 m;
  float left=p[1].f;
  float right=p[2].f;
  float bottom=p[3].f;
  float top=p[4].f;
  float near=p[5].f;
  float far=p[6].f;
  float x,y,A,B,C,D;

  x = (2.0*near) / (right-left);
  y = (2.0*near) / (top-bottom);
  A = (right+left) / (right-left);
  B = (top+bottom) / (top-bottom);
  C = -(far+near) / ( far-near);
  D = -(2.0*far*near) / (far-near);

  r=&m.m[0][0];
  r[0]= x; r[1]=0; r[2]=A; r[3]=0;
  r[4]= 0; r[5]=y; r[6]=B; r[7]=0;
  r[8]= 0; r[9]=0; r[10]=C; r[11]=D;
  r[12]= 0; r[13]=0; r[14]=-1; r[15]=0;

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}
  
