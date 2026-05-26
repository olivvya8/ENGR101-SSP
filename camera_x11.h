#ifndef CAMERA_X11_H
#define CAMERA_X11_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define DEVICE "/dev/video0"
#define WIDTH  640
#define HEIGHT 480

typedef unsigned char uchar;

struct buffer {
    void   *start;
    size_t length;
};

// Global state
static int cam_fd = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static struct v4l2_format fmt;
static Display *dpy = NULL;
static Window win;
static GC gc;
static XImage *image = NULL;

// Clamp helper
static inline uchar clamp_int(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uchar)v;
}

// Convert YUYV -> XRGB32 (BGRA order for little-endian X11)
static void yuyv_to_xrgb32(const uchar *yuyv, uchar *xrgb, int width, int height) {
    const int stride_yuyv = width * 2;
    const int stride_xrgb = width * 4;
    for (int y = 0; y < height; y++) {
        const uchar *src = yuyv + y * stride_yuyv;
        uchar *dst = xrgb + y * stride_xrgb;
        for (int x = 0; x < width; x += 2) {
            int Y0 = src[0];
            int U  = src[1];
            int Y1 = src[2];
            int V  = src[3];
            src += 4;

            int C0 = Y0 - 16;
            int C1 = Y1 - 16;
            int D  = U  - 128;
            int E  = V  - 128;

            int R0 = (298*C0 + 409*E + 128) >> 8;
            int G0 = (298*C0 - 100*D - 208*E + 128) >> 8;
            int B0 = (298*C0 + 516*D + 128) >> 8;

            int R1 = (298*C1 + 409*E + 128) >> 8;
            int G1 = (298*C1 - 100*D - 208*E + 128) >> 8;
            int B1 = (298*C1 + 516*D + 128) >> 8;

            // Pixel 0 (BGRA)
            dst[0] = clamp_int(B0);
            dst[1] = clamp_int(G0);
            dst[2] = clamp_int(R0);
            dst[3] = 0;

            // Pixel 1
            dst[4] = clamp_int(B1);
            dst[5] = clamp_int(G1);
            dst[6] = clamp_int(R1);
            dst[7] = 0;

            dst += 8;
        }
    }
}

// --- API functions ---

static int start_camera() {
    cam_fd = open(DEVICE, O_RDWR);
    if (cam_fd == -1) { perror("Opening video device"); return -1; }

    struct v4l2_capability cap;
    if (ioctl(cam_fd, VIDIOC_QUERYCAP, &cap) == -1) { perror("Querying Capabilities"); return -1; }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(cam_fd, VIDIOC_S_FMT, &fmt) == -1) { perror("Setting Pixel Format"); return -1; }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam_fd, VIDIOC_REQBUFS, &req) == -1) { perror("Requesting Buffer"); return -1; }

    buffers = calloc(req.count, sizeof(*buffers));
    n_buffers = req.count;
    for (unsigned int i = 0; i < n_buffers; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(cam_fd, VIDIOC_QUERYBUF, &buf) == -1) { perror("Querying Buffer"); return -1; }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) { perror("mmap"); return -1; }
    }
    for (unsigned int i = 0; i < n_buffers; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(cam_fd, VIDIOC_QBUF, &buf) == -1) { perror("Queue Buffer"); return -1; }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam_fd, VIDIOC_STREAMON, &type) == -1) { perror("Start Capture"); return -1; }

    dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Cannot open X display\n"); return -1; }
    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    win = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HEIGHT, 1,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    gc = XCreateGC(dpy, win, 0, NULL);

    char *img_data = (char*)malloc(WIDTH * HEIGHT * 4);
    image = XCreateImage(dpy, DefaultVisual(dpy, screen),
                         DefaultDepth(dpy, screen), ZPixmap, 0,
                         img_data, WIDTH, HEIGHT, 32, WIDTH*4);
    return 0;
}

static int take_picture() {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam_fd, VIDIOC_DQBUF, &buf) == -1) { perror("Retrieving Frame"); return -1; }

    yuyv_to_xrgb32((uchar*)buffers[buf.index].start,
                   (uchar*)image->data,
                   fmt.fmt.pix.width, fmt.fmt.pix.height);

    if (ioctl(cam_fd, VIDIOC_QBUF, &buf) == -1) { perror("Requeue Buffer"); return -1; }
    return 0;
}

// New function to redraw the frame
static void display_picture() {
    if (!image) return;
    XPutImage(dpy, win, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
    XFlush(dpy);
}

// Pixel accessors
static void get_pixel(int row, int col, uchar *r, uchar *g, uchar *b) {
    if (!image) return;
    uchar *px = (uchar*)image->data + (row*WIDTH + col)*4;
    *b = px[0];
    *g = px[1];
    *r = px[2];
}

static void set_pixel(int row, int col, uchar r, uchar g, uchar b) {
    if (!image) return;
    uchar *px = (uchar*)image->data + (row*WIDTH + col)*4;
    px[0] = b;
    px[1] = g;
    px[2] = r;
    px[3] = 0;
}


int save_picture(char fn[5])
{
   //save image into ppm file
   FILE *fp;
   char fname[9];
   sprintf(fname,"%s.ppm",fn);
   printf("Saving picture. %s\n",fname);
   fp = fopen(fname,"wb");
   if ( !fp){
       printf("Unable to open the file\n");
       return -1;
   }
   // write file header
   fprintf(fp,"P6\n %d %d %d\n",WIDTH , HEIGHT,255);
   int ind = 0;
   int row = 0;
   int col = 0;
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   for ( row = 0 ; row < HEIGHT; row++)  {
       for ( col = 0 ; col < WIDTH; col++) {
		     get_pixel(row, col, &red, &green , &blue);
		     fprintf(fp,"%c%c%c",red,green,blue);
		     ind = ind + 4;
       }
   }
   fflush(fp);
   fclose(fp);
        //system("timeout 1s gpicview im1.ppm");
   return 0;

}


#endif // CAMERA_X11_H
