/*	xnelson.c

	The graphical routines for xnelson.
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "xnelson.h"
#include "nelson.xbm"

/*	xn_create_win
	create a new Window tied to the given display with width and height at
	coordinates x, y.
*/
static Window xn_create_win(Display *disp, int width, int height, int x, int y)
{
	int screen;
	Window win;
	XSizeHints *win_size;

	screen = DefaultScreen(disp);

	if ((win = XCreateSimpleWindow(disp, RootWindow(disp, screen), x, y,
		width, height, 0, BlackPixel(disp, screen), WhitePixel(disp, screen))))
	{
		if ((win_size = XAllocSizeHints()))
		{

			win_size->flags = PSize | PMinSize | PMaxSize;
			win_size->min_width = win_size->width = win_size->max_width = nelson_width;
			win_size->min_height = win_size->height = win_size->max_height = nelson_height;
			XSetWMNormalHints(disp, win, win_size);
			XFree(win_size);

			XMapWindow(disp, win);
			XFlush(disp);
			return win;
		}
		else
		{
			FATAL_ERROR("Could not allocate memory for window sizing.", -1);
		}
	}
	else
	{
		FATAL_ERROR("Could not create a window.", -1);
	}
}

/*	xn_set_win_above
	uses EWMH to make the xnelson window stay on top.
*/
static void xn_set_win_above(Display *disp, Window win)
{
	Atom wm_state, wm_state_above;
	XEvent event;

	if ((wm_state = XInternAtom(disp, "_NET_WM_STATE", False)) != None)
	{
		if ((wm_state_above = XInternAtom(disp, "_NET_WM_STATE_ABOVE", False))
			!= None)
		{
			event.xclient.type = ClientMessage;
			event.xclient.serial = 0;
			event.xclient.send_event = True;
			event.xclient.display = disp;
			event.xclient.window = win;
			event.xclient.message_type = wm_state;
			event.xclient.format = 32;
			event.xclient.data.l[0] = 1; /* _NET_WM_STATE_ADD */
			event.xclient.data.l[1] = wm_state_above;
			event.xclient.data.l[2] = 0;
			event.xclient.data.l[3] = 0;
			event.xclient.data.l[4] = 0;

			if (!(XSendEvent(disp, DefaultRootWindow(disp), False,
				SubstructureRedirectMask | SubstructureNotifyMask, &event)))
			{
				FATAL_ERROR("Could not send an X event for window level.", -1);
			}
		}
		else
		{
			FATAL_ERROR("Could not access _NET_WM_STATE_ABOVE.", -1);
		}
	}
	else
	{
		FATAL_ERROR("Could not access _NET_WM_STATE.", -1);
	}
}

/*	xn_set_win_name
	sets the window's name to "xnelson".
*/
static void xn_set_win_name(Display *disp, Window win)
{
	char *name = "xnelson";
	XTextProperty name_prop;


	name_prop.value = (unsigned char *) name;
	name_prop.encoding = XA_STRING;
	name_prop.format = 8;
	name_prop.nitems = strlen(name);

	XSetWMName(disp, win, &name_prop);
}

/*	xn_create_gc
	create and return a new graphics context for the given display in the given
	window.
*/
static GC xn_create_gc(Display *disp, Window win)
{
	int screen;
	GC ctx;
	XGCValues gc_values;

	screen = DefaultScreen(disp);

	if ((ctx = XCreateGC(disp, win, 0, &gc_values)) >= 0)
	{
		XSetForeground(disp, ctx, BlackPixel(disp, screen));
		XSetBackground(disp, ctx, WhitePixel(disp, screen));

		XSetLineAttributes(disp, ctx, 2, LineSolid, CapButt, JoinBevel);
		XSetFillStyle(disp, ctx, FillSolid);

		return ctx;
	}
	else
	{
		FATAL_ERROR("Could not create a graphics context.", -1);
	}
}

/*	xnelson
	summon nelson.
*/
int xnelson(void)
{
	int screen, i, j;
	struct timespec sleep_spec;
	Display *disp;
	Window win;
	GC ctx;
	Pixmap bmp;
	XEvent evt;

	sleep_spec.tv_sec = 0;
	sleep_spec.tv_nsec = 125000;

	if ((disp = XOpenDisplay(NULL)))
	{
		screen = DefaultScreen(disp);

		win = xn_create_win(disp, nelson_width, nelson_height, 0, 0);

		xn_set_win_name(disp, win);
		xn_set_win_above(disp, win);

		ctx = xn_create_gc(disp, win);

		XSync(disp, False);

		if ((bmp = XCreateBitmapFromData(disp, win, nelson_bits, nelson_width,
			nelson_height)) != None)
		{
			for (i = 0; i < 10; i++)
			{
				for (j = 0; j < 10; j++)
				{
					XCopyPlane(disp, bmp, win, ctx, 0, 0, nelson_width,
						nelson_height, 0, 0, 1);
					XSync(disp, False);
					nanosleep(&sleep_spec, NULL);
				}
			}

			XFlush(disp);
		}
		else
		{
			FATAL_ERROR("Could not open the nelson bitmap.", -1);
		}
	}
	else
	{
		FATAL_ERROR("Could not open a connection to the X server.", -1);
	}

	while(1)
	{
		XNextEvent(disp, &evt);
	}

	/* cleanup */
	XFreePixmap(disp, bmp);
	XFreeGC(disp, ctx);
	XDestroyWindow(disp, win);
	XCloseDisplay(disp);

	return 0;
}
