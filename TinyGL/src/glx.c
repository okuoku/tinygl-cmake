/* simple glx driver for TinyGL */
#include <GL/glx.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include "zgl.h"
#include "zglx.h"

Bool glXQueryExtension( Display *dpy, int *errorb, int *event )
{
  return True;
}


XVisualInfo* glXChooseVisual( Display *dpy, int screen,
                              int *attribList )
{
	 XVisualInfo vinfo;
   int n;

  /* the attribList is ignored : we consider only RGBA rendering (no
     direct color) */

  if (XMatchVisualInfo (dpy, screen, 16, TrueColor, &vinfo)) {
    /* 16 bit visual */
  } else if (XMatchVisualInfo (dpy, screen, 8, PseudoColor, &vinfo)) {
    /* 8 bit visual */
  } else {
    /* no suitable visual */
    return NULL;
  }

  return XGetVisualInfo(dpy,VisualAllMask,&vinfo,&n);
}



GLXContext glXCreateContext( Display *dpy, XVisualInfo *vis,
                             GLXContext shareList, Bool direct )
{
  TinyGLXContext *ctx;

  if (shareList != NULL) {
    gl_fatal_error("No sharing available in TinyGL");
  }
  ctx=malloc(sizeof(TinyGLXContext));
  ctx->gl_context=NULL;
  ctx->visual_info=*vis;
  return (GLXContext) ctx;
}


void glXDestroyContext( Display *dpy, GLXContext ctx1 )
{
  TinyGLXContext *ctx = (TinyGLXContext *) ctx1;
  if (ctx->gl_context != NULL) {
    glClose();
  }
  free(ctx);
}


static int glxXErrorFlag=0;

static int glxHandleXError(Display *dpy,XErrorEvent *event)
{
  glxXErrorFlag=1;
  return 0;
}

static int create_ximage(TinyGLXContext *ctx,
                         int xsize,int ysize,int depth)
{
  int major,minor;
  Bool pixmaps;
  unsigned char *framebuffer;
  int (*old_handler)(Display *,XErrorEvent *);

  if (XShmQueryVersion(ctx->display,&major,&minor,&pixmaps))
    ctx->shm_use=1;
  else
    ctx->shm_use=0;

  if (!ctx->shm_use) goto no_shm;

  ctx->shm_info=malloc(sizeof(XShmSegmentInfo));
  ctx->ximage=XShmCreateImage(ctx->display,None,depth,ZPixmap,NULL,
                              ctx->shm_info,xsize,ysize);
  if (ctx->ximage == NULL) {
    fprintf(stderr,"XShm: error: XShmCreateImage\n");
    ctx->shm_use=0;
    free(ctx->shm_info);
    goto no_shm;
  }
  ctx->shm_info->shmid=shmget(IPC_PRIVATE,
                              xsize*ysize*(depth/8),
                              IPC_CREAT | 0777);
  if (ctx->shm_info->shmid < 0) {
    fprintf(stderr,"XShm: error: shmget\n");
  no_shm1:
    ctx->shm_use=0;
    XDestroyImage(ctx->ximage);
    goto no_shm;
  }
  ctx->ximage->data=shmat(ctx->shm_info->shmid,0,0);
  if (ctx->ximage->data == (char *) -1) {
    fprintf(stderr,"XShm: error: shmat\n");
  no_shm2:
    shmctl(ctx->shm_info->shmid,IPC_RMID,0);
    goto no_shm1;
  }
  ctx->shm_info->shmaddr=ctx->ximage->data;
  
  ctx->shm_info->readOnly=False;

  /* attach & test X errors */

  glxXErrorFlag=0;
  old_handler=XSetErrorHandler(glxHandleXError);
  XShmAttach(ctx->display,ctx->shm_info);
  XSync(ctx->display, False);
  
  if (glxXErrorFlag) {
    XFlush(ctx->display);
    shmdt(ctx->shm_info->shmaddr);
    XSetErrorHandler(old_handler);
    goto no_shm2;
  }

  /* the shared memory will be automatically deleted */
  shmctl(ctx->shm_info->shmid,IPC_RMID,0);

  /* test with a dummy XShmPutImage */
  XShmPutImage(ctx->display,ctx->drawable,ctx->gc,
               ctx->ximage,0,0,0,0,1,1,
               False);

  XSync(ctx->display, False);
  XSetErrorHandler(old_handler);
  
  if (glxXErrorFlag) {
    fprintf(stderr,"XShm: error: XShmPutImage\n");
    XFlush(ctx->display);
    shmdt(ctx->shm_info->shmaddr);
    goto no_shm2;
  }
  
  /* shared memory is OK !! */

  return 0;

  no_shm:
    framebuffer=malloc(xsize*ysize*(depth/8));
    ctx->ximage=XCreateImage(ctx->display, None, depth, ZPixmap, 0, 
                             framebuffer,xsize,ysize, 8, 0);
    return 0;
}

static void free_ximage(TinyGLXContext *ctx)
{
  if (ctx->shm_use)
  {
    XShmDetach(ctx->display, ctx->shm_info);
    XDestroyImage(ctx->ximage);
    shmdt(ctx->shm_info->shmaddr);
    free(ctx->shm_info);
  } else {
    free(ctx->ximage->data);
    XDestroyImage(ctx->ximage);
  }
}

/* resize the glx viewport : we try to use the xsize and ysize
   given. We return the effective size which is guaranted to be smaller */

int glX_resize_viewport(GLContext *c,int *xsize_ptr,int *ysize_ptr)
{
  TinyGLXContext *ctx;
  int xsize,ysize;

  ctx=(TinyGLXContext *)c->opaque;

  xsize=*xsize_ptr;
  ysize=*ysize_ptr;

  /* we ensure that xsize and ysize are multiples of 2 for the zbuffer. 
     TODO: suppress it ! */
  xsize&=~3;
  ysize&=~3;

  if (xsize == 0 || ysize == 0) return -1;

  *xsize_ptr=xsize;
  *ysize_ptr=ysize;

  if (ctx->ximage != NULL) free_ximage(ctx);
  
  ctx->xsize=xsize;
  ctx->ysize=ysize;

  if (create_ximage(ctx,ctx->xsize,ctx->ysize,ctx->visual_info.depth) != 0) 
    return -1;

  /* resize the Z buffer */
  if (ctx->visual_info.depth == 8) {
    ZB_resize(c->zb,NULL,xsize,ysize);
  } else {
    ZB_resize(c->zb,ctx->ximage->data,xsize,ysize);
  }
  return 0;
}

/* we assume here that drawable is a window */
Bool glXMakeCurrent( Display *dpy, GLXDrawable drawable,
                     GLXContext ctx1)
{
  TinyGLXContext *ctx = (TinyGLXContext *) ctx1;
  XWindowAttributes attr;
  int i,xsize,ysize;
  unsigned int palette[ZB_NB_COLORS];
  unsigned char color_indexes[ZB_NB_COLORS];
  ZBuffer *zb;
  XColor xcolor;
  unsigned long pixel[ZB_NB_COLORS],tmp_plane;
  
  if (ctx->gl_context == NULL) {
    /* create the TinyGL context */

    ctx->display=dpy;
    ctx->drawable=drawable;

    XGetWindowAttributes(ctx->display,drawable,&attr);

    xsize=attr.width;
    ysize=attr.height;

    if (attr.depth != ctx->visual_info.depth) return False;

    /* ximage structure */
    ctx->ximage=NULL;
    ctx->shm_use=1; /* use shm */

    if (attr.depth == 8) {
      /* get the colormap from the window */
      ctx->cmap = attr.colormap;

      if ( XAllocColorCells(ctx->display,ctx->cmap,True,&tmp_plane,0,
                            pixel,ZB_NB_COLORS) == 0) {
        /* private cmap */
        ctx->cmap = XCreateColormap(ctx->display, drawable,  
                                    ctx->visual_info.visual, AllocAll);
        XSetWindowColormap(ctx->display, drawable, ctx->cmap);
        for(i=0;i<ZB_NB_COLORS;i++) pixel[i]=i;
      }

      for(i=0;i<ZB_NB_COLORS;i++) color_indexes[i]=pixel[i];

      /* Open the Z Buffer - 256 colors */
      zb=ZB_open(xsize,ysize,ZB_MODE_INDEX,ZB_NB_COLORS,
                 color_indexes,palette,NULL);
      if (zb == NULL) {
        fprintf(stderr, "Error while initializing Z buffer\n");
        exit(1);
      }

      for (i=0; i<ZB_NB_COLORS; i++) {
        xcolor.flags = DoRed | DoGreen | DoBlue;
        
        xcolor.red = (palette[i]>>8) & 0xFF00;
        xcolor.green = (palette[i] & 0xFF00);
        xcolor.blue = (palette[i] << 8) & 0xFF00;
        xcolor.pixel = pixel[i];
        XStoreColor(ctx->display,ctx->cmap,&xcolor);
      }

    } else {
      /* RGB 16 */
      
      zb=ZB_open(xsize,ysize,ZB_MODE_5R6G5B,0,NULL,NULL,NULL);
      if (zb == NULL) {
        fprintf(stderr, "Error while initializing Z buffer\n");
        exit(1);
      }
    }

    /* create a gc */
    ctx->gc = XCreateGC(ctx->display, drawable, 0, 0);

    /* initialisation of the TinyGL interpreter */
    glInit(zb);
    ctx->gl_context=gl_get_context();
    ctx->gl_context->opaque=(void *) ctx;
    ctx->gl_context->gl_resize_viewport=glX_resize_viewport;

    /* set the viewport : we force a call to glX_resize_viewport */
    ctx->gl_context->viewport.xsize=-1;
    ctx->gl_context->viewport.ysize=-1;

    glViewport(0, 0, xsize, ysize);
  }

  return True;
}

void glXSwapBuffers( Display *dpy, GLXDrawable drawable )
{
  GLContext *gl_context;
  TinyGLXContext *ctx;

  /* retrieve the current GLXContext */
  gl_context=gl_get_context();
  ctx=(TinyGLXContext *)gl_context->opaque;

  /* 8 bit dithering if needed */
  if (ctx->visual_info.depth == 8) {
    ZB_copyFrameBuffer(ctx->gl_context->zb,
                       ctx->ximage->data,
                       ctx->xsize,ctx->ysize,0,0);

  }

  /* draw the ximage */
  if (ctx->shm_use) {
    XShmPutImage(dpy,drawable,ctx->gc,
                 ctx->ximage,0,0,0,0,ctx->ximage->width, ctx->ximage->height,
                 False);
  } else {
    XPutImage(dpy, drawable, ctx->gc, 
              ctx->ximage, 0, 0, 0, 0, ctx->ximage->width, ctx->ximage->height);
  }
  XFlush(dpy);
}


void glXWaitGL( void )
{
}

void glXWaitX( void )
{
}

