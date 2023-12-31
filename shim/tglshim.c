#include <GL/gl.h>
#include "zgl.h"
#include "tglshim.h"

static int
resize_viewport(GLContext* ctx, int *x, int* y){
    /* Adopt same behaviour of TinyGL's nglx */
    int xsize, ysize;

    xsize = *x;
    ysize = *y;

    /* Adjust size */
    xsize &= ~3;
    ysize &= ~3;

    *x = xsize;
    *y = ysize;

    ZB_resize(ctx->zb, NULL, xsize, ysize);
    return 0;
}

int
tglshim_capture(void** out_pixels, int* out_x, int* out_y){
    GLContext* ctx;

    ctx = gl_get_context();
    *out_x = ctx->zb->xsize;
    *out_y = ctx->zb->ysize;
    *out_pixels = ctx->zb->pbuf;
}


int /* zero for success */
tglshim_alloc(int xsize, int ysize){
    ZBuffer* zb;
    GLContext* ctx;
    zb = ZB_open(xsize, ysize, ZB_MODE_5R6G5B, 0, NULL, NULL, NULL);
    if(! zb){
        return -1;
    }
    glInit(zb);
    ctx = gl_get_context();
    ctx->gl_resize_viewport = resize_viewport;

    ctx->viewport.xsize = ctx->viewport.ysize = -1;
    glViewport(0, 0, xsize, ysize);

    return 0;
}


