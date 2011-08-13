#include <GL/glx.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
int main(int argc,char**argv){
	Display*dpy=XOpenDisplay(0);
	XVisualInfo*vi=glXChooseVisual(dpy,DefaultScreen(dpy),(int[]){GLX_RGBA,GLX_DOUBLEBUFFER,None});
	Window Wdo=XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,512,512,0,vi->depth,InputOutput,vi->visual,CWColormap|CWEventMask,(XSetWindowAttributes[]){{.colormap=XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone),.event_mask=PointerMotionMask}});
	XMapWindow(dpy,Wdo);
	glXMakeCurrent(dpy,Wdo,glXCreateContext(dpy,vi,0,GL_TRUE));
	srand(time(0));
	glOrtho(0,511,511,0,1,-1);
	glColor3ub(255,255,255);
	restart:;
	printf("!\n");
	int mx,my,bias=256;
	unsigned char rv[32768]={},g;
	for(int i=512;i>=0;i--){
		bias+=(rand()&1?1:-1)*(8+(rand()&15));
		if(bias<0)bias=0;
		if(bias>512-128)bias=512-128;
		for(int j=bias;j<bias+128;j++)rv[i<<6|j>>3]|=1<<(j&7);
	}
	for(;;){
		glXSwapBuffers(dpy,Wdo);
		glClear(GL_COLOR_BUFFER_BIT);
		while(XPending(dpy)){
			XEvent ev;
			XNextEvent(dpy,&ev);
			if(ev.type==MotionNotify){
				mx=ev.xmotion.x;
				my=ev.xmotion.y;
			}
		}
		glBegin(GL_POINTS);
		for(int i=0;i<512;i++)
			for(int j=0;j<512;j++)
				if(rv[i<<6|j>>3]&(1<<(j&7)))glVertex2i(256-(i>>1)+(i*j)/512,i);
		glEnd();
		glReadPixels(mx,512-my,1,1,GL_RED,GL_UNSIGNED_BYTE,&g);
		if(!g)goto restart;
		memmove(rv+64,rv,32704);
		memset(rv,0,64);
		bias+=(rand()&1?1:-1)*(12+(rand()&15));
		if(bias<0)bias=0;
		if(bias>512-96)bias=512-96;
		for(int i=bias;i<bias+96;i++)rv[i>>3]|=1<<(i&7);
		usleep(30000);
	}
}