
typedef struct {
  GLContext *gl_context;
  Display *display;
  XVisualInfo visual_info;
  int xsize,ysize;
  XImage *ximage;
  GC gc;
  Colormap cmap;
  Drawable drawable;

  /* shared memory */
  int shm_use;
  XShmSegmentInfo *shm_info;
} TinyGLXContext;
