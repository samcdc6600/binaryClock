/*
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);

    XVisualInfo vinfo;

    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0x80ffffff;

    Window win = XCreateWindow(display, DefaultRootWindow(display), 0, 0, 300, 200, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
    XSelectInput(display, win, StructureNotifyMask);
    GC gc = XCreateGC(display, win, 0, 0);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, win, &wm_delete_window, 1);

    XMapWindow(display, win);

    int keep_running = 1;
    XEvent event;

    while (keep_running) {
        XNextEvent(display, &event);

        switch(event.type) {
            case ClientMessage:
                if (event.xclient.message_type == XInternAtom(display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", 1))
                    keep_running = 0;

                break;

            default:
                break;
        }
    }

    XDestroyWindow(display, win);
    XCloseDisplay(display);
    return 0;
}
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);

    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    //    attr.border_pixel = 0;
    unsigned char r {0xff}, g {0x00}, b {0x09};
    unsigned char alpha {0xff};
    //    int alphaDiv {256};
    unsigned int bgColor {};
    //    r *= alpha;
    //    g *= alpha;
    //    b *= alpha;
    bgColor = alpha;
    std::cout<<"    bgColor = alpha; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor<<=8;
    std::cout<<"    bgColor<<=2; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor += r;
    std::cout<<"    bgColor += r; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor<<=8;
    std::cout<<"    bgColor<<=2; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor += g;
    std::cout<<"    bgColor += g; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor<<=8;
    std::cout<<"    bgColor<<=2; = "<<std::bitset<32>(bgColor)<<std::endl;
    bgColor += b;
    std::cout<<"    bgColor += b; = "<<std::bitset<32>(bgColor)<<std::endl;
    //    bgColor<<=2;
    //    bgColor = 0x60000060;
    //bgColor /= alphaDiv;
    //    alpha

    std::cout<<std::hex<<bgColor<<std::endl;

    attr.background_pixel = bgColor;
    //    attr.background_pixel = 0x80808080;


    attr.override_redirect = 1; // Non boardered / decorated window.
    Window win = XCreateWindow(display, RootWindow(display, 0), 0, 0, 300, 200, 0, vinfo.depth, InputOutput,
			       vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);
    XSelectInput(display, win, StructureNotifyMask);
    GC gc = XCreateGC(display, win, 0, 0);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, win, &wm_delete_window, 1);

    XMapWindow(display, win);

    Colormap cmap;
    cmap = DefaultColormap(display, DefaultScreen(display));

    XColor cyan, purple, blue, green, yellow, orange, red, darkRed;
    std::vector<XColor *> xColors {&cyan, &purple, &blue, &green, &yellow, &orange, &red, &darkRed};

    std::vector<std::string> colors {"Cyan", "Purple", "Blue", "Green", "Yellow", "Orange", "Red", "Dark Red"};

    Status rc;
    for(int iter {}; iter < colors.size(); ++iter)
      {
	rc = XAllocNamedColor(display, cmap, colors[iter].c_str(), xColors[iter], xColors[iter]);
	if(rc == 0)
	  {
	    std::cerr<<"XAllocNamedColor - failed to allocated '"<<colors[iter].c_str()<<"' color.\n";
	    exit(1);
	  }
      }

    // The red green and blue members of XColor are of type unsigned short, scaled from 0 to 65535 inclusive.
    XColor color;
    XAllocColor(display, cmap, &color);
    color.red = 65535; color.green = 0; color.blue = 0;


    XSetForeground(display, gc, color.pixel);
    XDrawString(display, win, gc, 0, 10, "Hello", 5);
    

    int keep_running = 1;
    XEvent event;

    while (keep_running) {
      XNextEvent(display, &event);
      switch(event.type) {
      case ClientMessage:
	if (event.xclient.message_type == XInternAtom(display, "WM_PROTOCOLS", 1) &&
	    (Atom)event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", 1))
                    keep_running = 0;

                break;

            default:
                break;
        }
    }

    XDestroyWindow(display, win);
    XCloseDisplay(display);
    return 0;
}

