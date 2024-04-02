#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#include "utils.h"

static volatile bool is_running = true;


static void
sig_handler(int sig)
{
    is_running = false;
}


void
print_help(const char *argv[])
{
    printf(
        "Usage: <png-data> | %s <flags>\n\n"
        "Shows an image from stdin above everything\n\n"
        "Flags:\n"
        "    --help       - display help\n\n"
        "        XORG PROPERTIES\n"
        "    -Xn <name>   - window name,  *\n"
        "    -Xc <class>  - window class, *\n\n",
        argv[0]
    );
}


int
main (int argc, const char *argv[])
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGKILL, sig_handler);

    const char * cur_arg = NULL;
    const char * window_name = NULL;
    const char * window_class = NULL;
   
    for (int i = 1; i < argc; i++) {
        cur_arg = argv[i];
        
        if (cur_arg[0] == '-') switch (cur_arg[1]) {
            // --<x>
            case '-':
                if (strcmp(cur_arg+2, "help") == 0) {
                    print_help(argv);
                    exit(0);
                }
                break;
            // -X<x>
            case 'X':
                switch (cur_arg[2]) {
                    case 'n':
                        window_name = (char*)argv[++i];
                        break;
                    case 'c':
                        window_class = (char*)argv[++i];
                        break;
                }
                break;
        }
    }
    if (!window_name || !window_class) {
        fprintf(stderr, "Window name and class must be set\n");
        print_help(argv);
        return 1;
    }
    
    uint16_t w, h;
    uint8_t * img_data = NULL;

    if (!read_image(stdin, &img_data, &w, &h)) {
        fprintf(stderr, "Failed to read image\n");
        return 1;
    }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Could not open display.\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    Visual *visual = DefaultVisual(dpy, screen);
    int depth  = DefaultDepth(dpy, screen);
    Window root_window = DefaultRootWindow(dpy);

    XSetWindowAttributes window_attributes = {
        .override_redirect = True,
        .event_mask = KeyReleaseMask | ExposureMask,
    };
    Window window = XCreateWindow(
        dpy,
        root_window,  // parent
        0, 0,
        w, h,
        0,  // border width
        depth,  // depth
        InputOutput,  // class
        visual,  // visual
        CWOverrideRedirect | CWEventMask,  // value mask
        &window_attributes
    );
    
    /* set the name and class hints for the window manager to use */
    XStoreName(dpy, window, window_name);
    XClassHint * class_hint = XAllocClassHint();
    if (class_hint) {
        class_hint->res_name = (char*)window_name;
        class_hint->res_class = (char*)window_class;
    }
    Atom win_type_atom = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom win_utility_atom = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_UTILITY", False);
    XSetClassHint(dpy, window, class_hint);
    XChangeProperty(
        dpy, window,
        win_type_atom, XA_ATOM, 32,
        PropModeReplace,
        (unsigned char *)&win_utility_atom, 1
    );

    XEvent xev;
    int x11_fd = ConnectionNumber(dpy);
    fd_set in_fds;
    struct timeval xev_timeout = {0, 500000};

    XMapWindow(dpy, window);
    XImage *image = XCreateImage(
        dpy, visual,
        depth,
        ZPixmap,
        0,
        (char*)img_data,
        w, h,
        32, 0
    );

    GC gc = DefaultGC(dpy, screen);
    XPutImage(
        dpy, window, gc,
        image,
        0, 0,
        0, 0,
        w, h
    );

    while (is_running) {
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        if (select(x11_fd + 1, &in_fds, 0, 0, &xev_timeout) <= 0) {
            continue;
        }

        XNextEvent(dpy, &xev);

        switch (xev.type) {
            case Expose:
                XPutImage(
                    dpy, window, gc,
                    image,
                    0, 0,
                    0, 0,
                    w, h
                );
                break;

            case KeyRelease:
                is_running = false;
                break;
        }

        XFlush(dpy);
    }

    XFree(image);
    free(img_data);
    XDestroyWindow(dpy, window);
    XCloseDisplay(dpy);

    return 0;
}
