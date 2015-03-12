/*	xnelson.c

	The graphical routines for xnelson.
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
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

	screen = DefaultScreen(disp);

	if ((win = XCreateSimpleWindow(disp, RootWindow(disp, screen), x, y,
		width, height, 0, BlackPixel(disp, screen), WhitePixel(disp, screen))))
	{
		XMapWindow(disp, win);
		XFlush(disp);
		return win;
	}
	else
	{
		fprintf(stderr, "Fatal: Could not create a window at %d in %s.\n",
			__LINE__, __FILE__);
		exit(-1);
	}
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
		fprintf(stderr, "Fatal: Could not create a GC at %d in %s.\n",
			__LINE__, __FILE__);
		exit(-2);
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
			fprintf(stderr, "Fatal: Could not open the bitmap at %d in %s.\n",
				__LINE__, __FILE__);
			exit(-4);
		}
	}
	else
	{
		fprintf(stderr, "Fatal: Could not open an X display.\n");
		exit(-3);
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
