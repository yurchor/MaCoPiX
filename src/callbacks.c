//  MaCoPiX = Mascot Construnctive Pilot for X
//                                (ActX / Gtk+ Evolution)
//
//
//     callback.c
//     Callback functions
//
//                            Copyright 2002-2008  K.Chimari
//                                     http://rosegray.sakura.ne.jp/
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
//


#include "main.h"

#undef DEBUG

// *** GLOBAL ARGUMENT ***
// ���ץ饤�Ƚ����
static  typSprite sprite_void[MAX_PIXMAP+1];

void DrawPanelClock0();


// *** GLOBAL ARGUMENT in this code *** 
int pre_hour, pre_min, pre_sec;
gint start_x, start_y, clk_start_x, clk_start_y;
#ifdef USE_BIFF
gint biff_start_x, biff_start_y;
#endif
gboolean flag_anime, flag_clock;
int old_day;
int errflag;
int xpop,ypop;
gint FWinX,FWinWidth;
gint clk_window_x, clk_window_y;
#ifdef USE_BIFF
gint biff_window_x, biff_window_y;
#endif
#ifdef USE_WIN32
HWND hWnd_active;
HWND hWndTaskBar;
#endif


// Prototype of functions in this file
int weight_random();
int weight_click();

#ifndef USE_WIN32
int ehandler ();
#endif

#ifdef USE_BIFF
#endif

#ifdef USE_WIN32
gint dw_init_sdw();
#endif
gchar * ini_week();

#ifndef USE_WIN32
void ChildTerm();
#endif // USE_WIN32

void drag_begin();
void drag_end();
gint dw_configure_main();
gint dw_expose_main();
gint expose_main();
gboolean window_motion();
#ifdef USE_WIN32
gint dw_configure_sdw();
gint dw_expose_sdw();
#endif
void focus_in();
void focus_out();


#ifdef USE_WIN32
MyXY GetTaskbarHeight( );
gboolean IsWin32Root();
#endif

#ifndef USE_WIN32
gchar* Get_Window_Name();
gint Get_Window_Bar_Size();
MyXY GetAutoHomePos();
#endif
void InitMascot0();

// �ŤߤĤ�������ѥ����������
int weight_random(typMascot *mascot)
{
  int ptn=0,sum=0,random_end;

  random_end=RANDOM(mascot->random_total)+1;

  do{
    ptn++;
    sum+=mascot->random_weight[ptn];
  }while(sum<random_end);

  return(ptn);
}

// �ŤߤĤ�����å��ѥ����������
int weight_click(typMascot *mascot)
{
  int ptn=0,sum=0,random_end;

  random_end=RANDOM(mascot->click_total)+1;

  do{
    ptn++;
    sum+=mascot->click_weight[ptn];
  }while(sum<random_end);

  return(ptn);
}


// XEvent ��Ϣ�Υ��顼�ϥ�ɥ顼  MoveToFocus�ǻ��Ȥ���
#ifndef USE_WIN32
int ehandler (Display *d, XErrorEvent *e)
{
    errflag = 1;
    return(0);
}
#endif

// �ޥ����åȤΰ�ư
void MoveMascot(typMascot *mascot, gint x, gint y)
{

  mascot->x=x;
  mascot->y=y;

#ifdef USE_WIN32
  if((mascot->sdw_flag)&&(mascot->sdw_height>0)){
    if(mascot->move==MOVE_FOCUS){
      gdk_window_move(gtk_widget_get_window(mascot->win_sdw), 
		      x+mascot->sdw_x, 
		      y+mascot->height-mascot->yoff*((gfloat)(mascot->magnify)/100));
    }
    else{
      gdk_window_move(gtk_widget_get_window(mascot->win_sdw), x+mascot->sdw_x, y+mascot->sdw_y);
    }
  }
#endif

  gdk_window_move(gtk_widget_get_window(mascot->win_main), x, y);

  MoveBalloon(mascot, x, y);
  if(mascot->clkmode==CLOCK_PANEL){
#ifdef USE_WIN32
    gdk_window_move(gtk_widget_get_window(mascot->clock_fg), 
		    x+mascot->clk_x*((gfloat)(mascot->magnify)/100), 
		    y+mascot->clk_y*((gfloat)(mascot->magnify)/100));
#endif
    gdk_window_move(gtk_widget_get_window(mascot->clock_main), 
		    x+mascot->clk_x*((gfloat)(mascot->magnify)/100), 
		    y+mascot->clk_y*((gfloat)(mascot->magnify)/100));
  }
#ifdef USE_BIFF
  MoveBiffPix(mascot, x, y);
#endif
  
  raise_all(mascot);

  gdk_window_move(gtk_widget_get_window(mascot->win_main), x, y);

#ifdef USE_WIN32
  gtk_menu_popdown(GTK_MENU(mascot->PopupMenu));
#endif

#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}

#ifdef USE_BIFF
// Biff�����ΰ�ư
void MoveBiffPix(typMascot *mascot, gint x, gint y)
{
  GtkAllocation *allocation=g_new(GtkAllocation, 1);
  gtk_widget_get_allocation(mascot->biff_pix,allocation);
  
  if(mascot->mail.flag){
    if(mascot->mail.pix_pos==MAIL_PIX_LEFT){
      gdk_window_move(gtk_widget_get_window(mascot->biff_pix), 
		      x +mascot->mail.pix_x -allocation->width/2, 
		      y+mascot->mail.pix_y);
    }
    else{
      gdk_window_move(gtk_widget_get_window(mascot->biff_pix), 
		      x+mascot->mail.pix_x
		      +mascot->width -allocation->width/2, 
		      y+mascot->mail.pix_y);
    }
  }
  g_free(allocation);

#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}
#endif

// �Х롼��ΰ�ư
void MoveBalloon(typMascot *mascot, gint x, gint y)
{
  gint bal_y;
  bal_y=y+mascot->bal_lyoff[mascot->anime_ptn];

  if(bal_y<0) bal_y=0;

  if(mascot->bal_pos==BAL_POS_LEFT){
#ifdef USE_WIN32
    gdk_window_move(gtk_widget_get_window(mascot->balloon_fg), 
		    x-mascot->balwidth+mascot->bal_lxoff[mascot->anime_ptn],
		    bal_y);
#endif
    gdk_window_move(gtk_widget_get_window(mascot->balloon_main), 
		    x-mascot->balwidth+mascot->bal_lxoff[mascot->anime_ptn],
		    bal_y);
  }
  else{
#ifdef USE_WIN32
    gdk_window_move(gtk_widget_get_window(mascot->balloon_fg), 
		    x+mascot->width+mascot->bal_rxoff[mascot->anime_ptn],
		    bal_y);
#endif
    gdk_window_move(gtk_widget_get_window(mascot->balloon_main), 
		    x+mascot->width+mascot->bal_rxoff[mascot->anime_ptn],
		    bal_y);
  }

#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}

void ResizeMoveBalloon(typMascot *mascot, gint x, gint y, gint w, gint h)
{
  gint bal_y;
  bal_y=y+mascot->bal_lyoff[mascot->anime_ptn];

  if(bal_y<0) bal_y=0;

  if(mascot->bal_pos==BAL_POS_LEFT){
#ifdef USE_WIN32
    gdk_window_move_resize(gtk_widget_get_window(mascot->balloon_fg), 
			   x-w+mascot->bal_lxoff[mascot->anime_ptn],
			   bal_y,
			   w,h);
#endif
    gdk_window_move_resize(gtk_widget_get_window(mascot->balloon_main), 
			   x-w+mascot->bal_lxoff[mascot->anime_ptn],
			   bal_y,
			   w,h);
  }
  else{
#ifdef USE_WIN32
    gdk_window_move_resize(gtk_widget_get_window(mascot->balloon_fg), 
			   x+mascot->width+mascot->bal_rxoff[mascot->anime_ptn],
			   bal_y,
			   w,h);
#endif
    gdk_window_move_resize(gtk_widget_get_window(mascot->balloon_main), 
			   x+mascot->width+mascot->bal_rxoff[mascot->anime_ptn],
			   bal_y,
			   w,h);
  }

#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}

// �ե�������Window�ɽ�����ư
int MoveToFocus(typMascot *mascot, gboolean force_fl)
{
#ifdef USE_WIN32
  /* for Windows �Ȥꤢ��������ѥ�����̤� */
  HWND   hWnd;
  RECT   nRect;
  DWORD dwStyle;

  int sx,sy,x_root=0,y_root=0;
  int realXPos;
  unsigned int width;
#else
  Window rootwin,parent,*children,child,wf;
  int sx,sy,x,y,i,x_root=0,y_root=0;
  unsigned int border,depth;
  unsigned int nchildren;
  unsigned int width,height,width_root,height_root;
  int rx,ry,wx,wy,mask_return,realXPos;
  gint win_bar_size;
#endif
  gboolean eflag=FALSE;
  gint flag_homepos=HOMEPOS_NEVER;

  gint oxpop,oypop;

#ifdef USE_WIN32
  /* for Windows */
  hWnd = GetForegroundWindow();

  if(hWnd!=hWnd_active){ // for multiple running 
    char wtitle[256];    
    GetWindowText(hWnd, wtitle, 256);
    if(strcmp(wtitle,"MaCoPiX")==0){
      hWnd=hWnd_active;
    }
    else if(mascot->no_capbar){  // for windows w/o title bar
      DWORD wStyle;
      wStyle = GetWindowLong (hWnd,GWL_STYLE);
      if(!(wStyle&WS_CAPTION)){
	flag_homepos=mascot->homepos_nb;
	hWnd=hWnd_active;
      }
    }
  }
#else
  win_bar_size=0;
  XGetInputFocus(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		 &wf, &i);
  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
	       GDK_ROOT_WINDOW(),
	       &rootwin,
	       &x_root,
	       &y_root,
	       &width_root,
	       &height_root,
	       &border,
	       &depth);

  parent = 0;
#endif

  sx = 0;
  sy = 0;

#ifdef USE_WIN32
    /* for Windows */
  if(hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->win_main)) 
     && hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->balloon_fg))
     && hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->balloon_main))
     && hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->clock_fg))
     && hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->clock_main))
#ifdef USE_BIFF
     && hWnd!=GDK_WINDOW_HWND(gtk_widget_get_window(mascot->biff_pix))
#endif
     && (int)hWnd!=0){

    if(mascot->task_force){  // force to sit on the task bar
      if((hWnd!=hWnd_active)&&(hWnd!=hWndTaskBar)){
	  raise_all(mascot);
      }
      hWnd_active=hWnd;
      hWnd=hWndTaskBar;
    }
    else{
      hWnd_active=hWnd;
    }
      
    GetWindowRect(hWnd, &nRect);
    sx = nRect.left;
    sy = nRect.top;
    width = nRect.right-nRect.left;

#else  // UNIX X
  if(wf!=GDK_WINDOW_XID(gtk_widget_get_window(mascot->win_main)) 
     && wf!=GDK_WINDOW_XID(gtk_widget_get_window(mascot->balloon_main))
     && wf!=GDK_WINDOW_XID(gtk_widget_get_window(mascot->clock_main))
#ifdef USE_BIFF
     && wf!=GDK_WINDOW_XID(gtk_widget_get_window(mascot->biff_pix))
#endif
     && (int)wf!=0){

    if(mascot->focus_autobar!=AUTOBAR_MANUAL){ 
      // �����ȥ�С��Υ�������ư���� 
      if(errflag ==0){
	XQueryTree(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		   wf,&rootwin,&parent,&children,&nchildren);
	if(errflag ==0)  XFree(children);
      }
      else{
	errflag = 0;
	return(0);
      }
      
      if(wf == rootwin){
	if(errflag ==0){
	  XQueryTree(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		     wf,&rootwin,&parent,&children,&nchildren);
	  if(errflag ==0)  XFree(children);
	}
	else{
	  errflag = 0;
	  return(0);
	}
	
	if(errflag ==0){
	  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		       wf,&rootwin,&x,&y,&width,&height,
		       &border,&depth);
	}
	else{
	  errflag   = 0;
	  return(0);
	}
      }
      else{
	//Window wf_org;

	for(;;){
	  if(parent == rootwin){
	    if(win_bar_size==0){
	      win_bar_size
		=Get_Window_Bar_Size(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
				     wf);
	    }

	    if((win_bar_size==0)&&(mascot->no_capbar)){
	      // for windows w/o title bar 
	      flag_homepos=mascot->homepos_nb;
	      if(flag_homepos==HOMEPOS_NEVER) return(0);
	    }
	    break;
	  }
	  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		       parent,&rootwin,&x_root,&y_root,
		       &width,&height,&border,&depth);
	     
	  if((width==width_root)&&(height==height_root)){
	    eflag=TRUE;
	    break; // For Enlightenment
	  }

	  if(win_bar_size==0){
	    win_bar_size
	      =Get_Window_Bar_Size(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
				   wf);
	  }
	  
	  if(mascot->bar_offset!=0){
	    if(errflag ==0){
	      XQueryPointer(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
			    wf, &rootwin, &child, &rx, &ry,
			    &wx, &wy, &mask_return);
	    }
	    else{
	      errflag = 0;
	      return(0);
	    }
	  }

	  //wf_org = wf;
	  wf = parent;

	  if(errflag ==0){
	    XQueryTree(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
	    	       wf,&rootwin,&parent,&children,&nchildren);
	    if(errflag ==0)  XFree(children);
	  }
	  else{
	    errflag = 0;
	    return(0);
	  }
	  

	}

	if(errflag ==0){
	  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
	  	       wf,&rootwin,&x,&y,&width,&height,
	  	       &border,&depth);
	  // �ե�����������Window��¸�ߤ��ʤ� Offset"%"�ΤȤ�
	  //		if (mascot->flag_xp){
	  // if((x<0)&&(y<0)&&(depth==0)&&(border==0)){
	  //   x-=mascot->width; y-=mascot->height;
	  //}
	  //}
	}
	else{
	  errflag = 0;
	  return(0);
	}
      }

      sx=x;
#ifdef USE_WIN32
      if(mascot->bar_offset==0){
	sy=y;
      }
      else if((x==rx-wx)&&(y==ry-wy)){
	sy=y;  // ���ե��åȷ�¬ : �ޥ����åȤϥС�̵��Win�˾�äƤ���
      }
      else{    // ���ե��åȷ�¬ : �ޥ����åȤϥС�ͭ��Win�˾�äƤ���
	sy=y-mascot->bar_offset;
      }
#else
      if(mascot->focus_autobar==AUTOBAR_COMPIZ){
	y=y-win_bar_size;
      }

      if((mascot->bar_offset!=0)&&(win_bar_size!=0)){
	sy=y-mascot->bar_offset;
      }
      else{
	sy=y;
      }
#endif
    }
    else{  // �����ȥ�С��Υ�������ޥ˥奢������⡼��
      if(errflag ==0){
	XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		     wf,&rootwin,&x,&y,&width,&height,&border,&depth);
      }
      else{
	errflag = 0;
	return(0);
      }

      if(width==1){  // ľ�� Focused Window�Υ��������Ȥ�Ƥ��ʤ�
	XQueryTree(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		   wf,&rootwin,&parent,&children,&nchildren);
	wf=parent;
	if(errflag ==0){
	  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		       wf,&rootwin,&x,&y,&width,&height,&border,&depth);
	}
	else{
	  errflag = 0;
	  return(0);
	}
      }

      if(errflag ==0){
	XQueryPointer(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
		      wf, &rootwin, &child, &rx, &ry,
		      &wx, &wy, &mask_return);
      }
      else{
	errflag = 0;
	return(0);
	
      }


      sx=rx-wx;
      sy=ry-wy-mascot->bar_size;
      }
#endif

    // sx, sy �ϸ��ߤΥե�������Window�κ���(�ޥ����ȥ�С�)�����а���
      
    errflag = 0;
    
    oxpop=xpop;
    oypop=ypop;
    
    if(mascot->ff_side==FF_SIDE_RIGHT){
      if(mascot->flag_xp){
	realXPos=((int)width-mascot->width)*(1-(mascot->offsetp*0.01))
	  -mascot->xoff;
      }
      else{
	realXPos=(int)width-mascot->width-mascot->xoff-mascot->offset;
      }
    }
    else{
      if(mascot->flag_xp){
	realXPos=((int)width-mascot->width)*(mascot->offsetp*0.01)
	  -mascot->xoff;
      }
      else{
	realXPos=-mascot->xoff+mascot->offset;
      }
    }    
    
    if(eflag){
      xpop=sx+realXPos+x_root;
      ypop=sy+mascot->yoff*((gfloat)(mascot->magnify)/100)
	+y_root-mascot->height+mascot->sdw_y_int;
    }
    else{
      xpop=sx+realXPos;
      ypop=sy+mascot->yoff*((gfloat)(mascot->magnify)/100)
	-mascot->height+mascot->sdw_y_int;
    }

    if(mascot->focus_autobar!=AUTOBAR_MANUAL){
#ifdef USE_WIN32   // �ե����������ʤ� 
      if(IsWin32Root(hWnd, mascot->width_root, mascot->height_root)){
#else
      if((sx==ROOTOFF_X-width)
	 &&(sy==ROOTOFF_Y-mascot->bar_offset)){  // �ե����������ʤ� 
#endif
	flag_homepos=mascot->homepos_nf;
      }
      else if(ypop<ROOTOFF_Y){ //���̾�˥ޥ����åȤ��ϤߤǤ�
	flag_homepos=mascot->homepos_out;
      }
    }
    else{
#ifdef USE_WIN32   // �ե����������ʤ� 
      if(IsWin32Root(hWnd, mascot->width_root, mascot->height_root)){
#else
      if((sx==ROOTOFF_X) &&(sy==ROOTOFF_Y)){  // �ե����������ʤ� 
#endif
	flag_homepos=mascot->homepos_nf;
      }
      else if(ypop<ROOTOFF_Y){ //���̾�˥ޥ����åȤ��ϤߤǤ�
	flag_homepos=mascot->homepos_out;
      }
    }
       
    switch(flag_homepos){
    case HOMEPOS_BAR:
      if(mascot->ff_side==FF_SIDE_RIGHT){
	if(mascot->flag_xp){
	  xpop=((mascot->width_root-mascot->home_x)-mascot->width)
	    *(1-(mascot->offsetp*0.01)) -mascot->xoff;
	}
	else{
	  xpop=(mascot->width_root-mascot->home_x)
	    -mascot->width-mascot->xoff-mascot->offset;
	}
      }
      else{
	if(mascot->flag_xp){
	  xpop=((mascot->width_root-mascot->home_x)-mascot->width)
	    *(mascot->offsetp*0.01) -mascot->xoff;
	}
	else{
	  xpop=mascot->home_x-mascot->xoff+mascot->offset;
	}
      }
#ifdef USE_WIN32
      {
	MyXY xy;

	xy=GetTaskbarHeight();
	
	if(xy.flag){
	  mascot->home_x=mascot->width_root-xy.x;
	  mascot->home_y=xy.y;
	}
	if((!xy.flag)||(mascot->home_y==0)){
	  if(mascot->home_auto_vanish){
	    ypop=mascot->height_root;
	    break;
	  }
	}
      }
#else
      if(mascot->home_auto){  // Auto Taskbar Scaling for X
	MyXY xy;

	xy=GetAutoHomePos();
	if(xy.flag){
	  mascot->home_x=mascot->width_root-xy.x;
	  mascot->home_y=mascot->height_root-xy.y;
	}
	if((!xy.flag)||(mascot->home_y==0)){
	  if(mascot->home_auto_vanish){
	    ypop=mascot->height_root;
	    break;
	  }
	}
      }
#endif
      ypop=(mascot->height_root-mascot->home_y)
	+mascot->yoff*((gfloat)(mascot->magnify)/100)-mascot->height
	+mascot->sdw_y_int;
      break;
      
    case HOMEPOS_VANISH:
      ypop=mascot->height_root;
      break;

    default:
      break;
    }
    
    if((oxpop!=xpop)||(oypop!=ypop)){
      MoveMascot(mascot, xpop, ypop);
      if(flag_homepos){
	FWinWidth=mascot->width_root-mascot->home_x;
	FWinX=0;
      }
      else{
	FWinWidth=width;
	FWinX=sx;
      }
    }
    else if(force_fl){
      MoveMascot(mascot, xpop, ypop);
      if(flag_homepos){
	FWinWidth=mascot->width_root-mascot->home_x;
	FWinX=0;
      }
      else{
	FWinWidth=width;
	FWinX=sx;
      }
    }
  }

  while (my_main_iteration(FALSE));
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
  return(1); 
}


#ifndef USE_GTK3    
// ���Ĥ�GC�κ����ȥ��顼allocation
GdkGC *MPCreatePen(GdkPixmap *pixmap, GdkColor *color)
{
  GdkGC *gc;

  gdk_colormap_alloc_color(gdk_colormap_get_system(),color,FALSE,TRUE);

  if(pixmap){
    gc=gdk_gc_new(pixmap);
  
    gdk_gc_set_foreground(gc, color);
    return(gc);
  }
  else{
    return(NULL);
  }
}
#endif
 

// �ޥ����ܥ���򤪤����Ȥ��Υ��٥��Callback
#ifdef USE_GTK3
void drag_begin(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;

  mascot=(typMascot *)gdata; 

  if(mascot->flag_menu) return;
  
  if (event->button == 1) {
    mascot->drag = TRUE;
    gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.push);
    window_x = event->x;
    window_y = event->y;
    gdk_window_get_device_position(NULL, event->device, &start_x, &start_y, &modmask);
    gdk_seat_grab(gtk_widget_get_window(widget),
		  event->device,
		  FALSE, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
		  NULL, NULL, GDK_CURRENT_TIME);
    
    gdk_display_flush(gdk_display_get_default());
  }
  else if(event->button == 3){
    gtk_menu_popup (GTK_MENU(mascot->PopupMenu), NULL, NULL, NULL, NULL, 
		    event->button, event->time);
  }

}
#else
void drag_begin(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;

  mascot=(typMascot *)gdata; 

  if(mascot->flag_menu) return;
  
  if (event->button == 1) {
    mascot->drag = TRUE;
    gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.push);
    window_x = event->x;
    window_y = event->y;

    gdk_window_get_pointer(NULL, &start_x, &start_y, &modmask);
    gdk_pointer_grab(gtk_widget_get_window(widget), 
    		     FALSE, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
    		     NULL, NULL, GDK_CURRENT_TIME);
  }
  else if(event->button == 3){
    gtk_menu_popup (GTK_MENU(mascot->PopupMenu), NULL, NULL, NULL, NULL, 
		    event->button, event->time);
  }

}
#endif


// �ޥ����ܥ����Ϥʤ����Ȥ��Υ��٥��Callback
#ifdef USE_GTK3
void drag_end(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;
  gint end_x,end_y;
  int i_frm;
  GdkSeat    *seat;
  GdkDevice  *device;
  GdkDisplay *display;

  display = gtk_widget_get_display (widget);
  seat = gdk_display_get_default_seat (display);
  device = gdk_seat_get_pointer (seat);

  mascot=(typMascot *)gdata; 

  raise_all(mascot);
  
  if (mascot->drag){
    mascot->drag = FALSE;
    gdk_window_get_pointer(NULL, &end_x, &end_y, &modmask);
    if((end_x==start_x)&&(end_y==start_y)&&
       (mascot->click_total!=0)&&(flag_balloon==FALSE)){
      /////// ����å����˥� ///////
      for(i_frm=0;i_frm<mascot->frame_num[mascot->anime_ptn];i_frm++){
	// �֥��å��롼�פΥ���󥻥�
	mascot->frame_loop[mascot->anime_ptn][i_frm].seq=0;
      }
      mascot->anime_ptn=weight_click(mascot);
      mascot->anime_frm=-1;
      mascot->anime_seq=-1;
      mascot->anime_seqend=
	RANDOM(mascot->frame_max[mascot->anime_ptn][mascot->anime_frm]
	       -mascot->frame_min[mascot->anime_ptn][mascot->anime_frm]+1)
	+mascot->frame_min[mascot->anime_ptn][mascot->anime_frm];

      sound_play(mascot,mascot->click_sound[mascot->anime_ptn]);

      if(mascot->click_word[mascot->anime_ptn]) {
	mascot->balseq=0;
	mascot->bal_mode=BALLOON_NORMAL;
	DoBalloon(mascot); 
	flag_balloon=TRUE;
      }

#ifdef USE_SOCKMSG
      // Duet���˥�
      if(mascot->duet_use_click){
	mascot->duet_seq=mascot->duet_delay[mascot->anime_ptn];
	mascot->duet_mode=DUET_CLICK;
      }
#endif

      // flag_anime=TRUE;
    }
      

  }
  gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.normal);
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
  gdk_display_flush(gdk_display_get_default());
}
#else
void drag_end(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;
  gint end_x,end_y;
  int i_frm;

  mascot=(typMascot *)gdata; 

  raise_all(mascot);
  
  if (mascot->drag){
    mascot->drag = FALSE;
    gdk_window_get_pointer(NULL, &end_x, &end_y, &modmask);
    if((end_x==start_x)&&(end_y==start_y)&&
       (mascot->click_total!=0)&&(flag_balloon==FALSE)){
      /////// ����å����˥� ///////
      for(i_frm=0;i_frm<mascot->frame_num[mascot->anime_ptn];i_frm++){
	// �֥��å��롼�פΥ���󥻥�
	mascot->frame_loop[mascot->anime_ptn][i_frm].seq=0;
      }
      mascot->anime_ptn=weight_click(mascot);
      mascot->anime_frm=-1;
      mascot->anime_seq=-1;
      mascot->anime_seqend=
	RANDOM(mascot->frame_max[mascot->anime_ptn][mascot->anime_frm]
	       -mascot->frame_min[mascot->anime_ptn][mascot->anime_frm]+1)
	+mascot->frame_min[mascot->anime_ptn][mascot->anime_frm];

      sound_play(mascot,mascot->click_sound[mascot->anime_ptn]);

      if(mascot->click_word[mascot->anime_ptn]) {
	mascot->balseq=0;
	mascot->bal_mode=BALLOON_NORMAL;
	//DoBalloon(mascot); 
	flag_balloon=TRUE;
      }

#ifdef USE_SOCKMSG
      // Duet���˥�
      if(mascot->duet_use_click){
	mascot->duet_seq=mascot->duet_delay[mascot->anime_ptn];
	mascot->duet_mode=DUET_CLICK;
      }
#endif

      // flag_anime=TRUE;
    }
      

  }
  gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.normal);
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
  gdk_flush();
}
#endif

// ����  �ޥ����ܥ���򤪤����Ȥ��Υ��٥��Callback
void clk_drag_begin(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;

  mascot=(typMascot *)gdata; 

  if(mascot->flag_menu) return;
  
  if (event->button == 1) {
    mascot->clk_drag = TRUE;
    clk_window_x = event->x;
    clk_window_y = event->y;
    gdk_window_get_pointer(NULL, &clk_start_x, &clk_start_y, &modmask);
    gdk_pointer_grab(gtk_widget_get_window(widget), 
		     FALSE, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
		     NULL, NULL, GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
    gdk_display_flush(gdk_display_get_default());
#else	    
    gdk_flush();
#endif	    
  }
}


// ���� �ޥ����ܥ����Ϥʤ����Ȥ��Υ��٥��Callback
void clk_drag_end(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;
  gint end_x,end_y;
  int i_frm;


  mascot=(typMascot *)gdata; 

  raise_all(mascot);
  
  if (mascot->clk_drag){
    mascot->clk_drag = FALSE;
    gdk_window_get_pointer(NULL, &end_x, &end_y, &modmask);
  }
  
  gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.clk);
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}


#ifdef USE_BIFF
// Biff �ޥ����ܥ���򤪤����Ȥ��Υ��٥��Callback
void biff_drag_begin(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;

  mascot=(typMascot *)gdata; 

  if(mascot->flag_menu) return;
  
  if (event->button == 1) {
    mascot->mail.drag = TRUE;
    biff_window_x = event->x;
    biff_window_y = event->y;
    gdk_window_get_pointer(NULL, &biff_start_x, &biff_start_y, &modmask);
    gdk_pointer_grab(gtk_widget_get_window(widget), 
		     FALSE, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
		     NULL, NULL, GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
    gdk_display_flush(gdk_display_get_default());
#else	    
    gdk_flush();
#endif	    
  }
}


// Biff �ޥ����ܥ����Ϥʤ����Ȥ��Υ��٥��Callback
void biff_drag_end(GtkWidget * widget, GdkEventButton * event, gpointer gdata)
{
  typMascot *mascot;
  GdkModifierType modmask;
  gint end_x,end_y;
  int i_frm;


  mascot=(typMascot *)gdata; 

  raise_all(mascot);
  
  if (mascot->mail.drag){
    mascot->mail.drag = FALSE;
    gdk_window_get_pointer(NULL, &end_x, &end_y, &modmask);
    if((end_x==biff_start_x)&&(end_y==biff_start_y)){
      // From Subject Win ��ư
      create_biff_dialog(mascot);
    }
    else{
      //mascot->mail.pix_x=end_x-mascot->x;
      //mascot->mail.pix_y=end_y-mascot->y;
    }
  }
  
  gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.biff);
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}
#endif


// Mascot Window���������ѷ��ˤȤ�ʤ�Callback
gint dw_configure_main(GtkWidget *widget, GdkEventConfigure *event,
		       gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 

#ifdef DEBUG
  printf("Configure: \n");
#endif

  //DrawMascot0(mascot);
  {
    GtkAllocation *allocation=g_new(GtkAllocation, 1);
    GtkStyle *style=gtk_widget_get_style(widget);
    gtk_widget_get_allocation(widget,allocation);
    
    gdk_draw_drawable(gtk_widget_get_window(widget),
		      style->fg_gc[gtk_widget_get_state(widget)],
		      pixmap_main[mascot->pixmap_page],
		      0,0,0,0,
		      allocation->width,
		      allocation->height);
    g_free(allocation);
  }
  /*
  gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->win_main),
  			     pixmap_main[mascot->pixmap_page],
  			     FALSE);
  */

#ifdef DEBUG
  printf("End of Configure: \n");
#endif
  return FALSE;
}

#ifdef USE_WIN32
gint dw_configure_sdw(GtkWidget *widget, GdkEventConfigure *event,
		       gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 

  if((mascot->sdw_flag)&&(mascot->sdw_height>0)){
    {
      GtkAllocation *allocation=g_new(GtkAllocation, 1);
      GtkStyle *style=gtk_widget_get_style(widget);
      gtk_widget_get_allocation(widget,allocation);
      
      gdk_draw_drawable(gtk_widget_get_window(mascot->dw_main),
			style->fg_gc[gtk_widget_get_state(widget)],
			pixmap_sdw[mascot->pixmap_page],
			0,0,0,0,
			allocation->width,
			allocation->height);
      g_free(allocation);
    }
    /*
    gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->win_sdw),
			       pixmap_sdw[mascot->pixmap_page],
			       FALSE);
    */
  }
  return FALSE;
}
#endif

gint dw_init_main(GtkWidget *widget, GdkEventConfigure *event,
		  gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 
  gint i_work;
  gint i_frm=0;

#ifdef DEBUG
  printf("Init Main: \n");
#endif

  if(mascot->anime_frm==-1) i_frm=0;
  else i_frm=mascot->anime_frm;

#ifndef USE_GTK3  
  if(mascot->gc_main[0]==NULL){
    mascot->gc_main[0] = MPCreatePen(pixmap_main[0], mascot->colclk);
    mascot->gc_mainsd[0] = MPCreatePen(pixmap_main[0], mascot->colclksd);
    if(mascot->gc_main[1]==NULL){
      mascot->gc_main[1] = MPCreatePen(pixmap_main[1], mascot->colclk);
      mascot->gc_mainsd[1] = MPCreatePen(pixmap_main[1], mascot->colclksd);
    }
#ifndef USE_WIN32
    if(errflag==-1){
      XSetErrorHandler (ehandler);
    }
#endif
#ifdef DEBUG
    printf("Init GC Main\n");
#endif
  }
#endif // USE_GTK3
  
  for(i_work=0;i_work<2;i_work++){
    gdk_draw_drawable(pixmap_main[i_work],
		      mascot->gc_main[i_work],
		      mascot->sprites[mascot->frame_pix[0][0]].pixmap,
		      0, 0,
		      0, 0,
		      mascot->width, mascot->height);
  }
  
  //DrawMascot0(mascot);
  /*
  {
    GtkAllocation *allocation=g_new(GtkAllocation, 1);
    GtkStyle *style=gtk_widget_get_style(widget);
    gtk_widget_get_allocation(widget,allocation);
    
    gdk_draw_drawable(gtk_widget_get_window(widget),
		      style->fg_gc[gtk_widget_get_state(widget)],
		      pixmap_main[mascot->pixmap_page],
		      0,0,0,0,
		      allocation->width,
		      allocation->height);
    g_free(allocation);
  }
  */
  /*
  gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->win_main),
			     pixmap_main[mascot->pixmap_page],
			     FALSE);
  */

#ifdef USE_WIN32  // Get Handle of Task-Bar
  hWndTaskBar = FindWindow("Shell_TrayWnd", NULL);
#endif  

#ifdef DEBUG
  printf("End of Init Main: \n");
#endif
  return FALSE;
}



#ifdef USE_BIFF
// Biff Pixmap  Window���������ѷ��ˤȤ�ʤ�Callback
#ifdef USE_GTK3
gint dw_configure_biff_pix(GtkWidget *widget, cairo_t *cr, gpointer userdata){
  typMascot *mascot;
  mascot=(typMascot *)userdata;
  
  gdk_cairo_set_source_pixbuf(cr, mascot->mail.pixbuf, 0, 0);
  cairo_paint(cr);
  return(FALSE);
}
#else
gint dw_configure_biff_pix(GtkWidget *widget, GdkEventConfigure *event,
			   gpointer gdata)
{
  typMascot *mascot;
  GtkAllocation *allocation=g_new(GtkAllocation, 1);

  mascot=(typMascot *)gdata; 
  gtk_widget_get_allocation(widget,allocation);

  gdk_draw_drawable(gtk_widget_get_window(widget),
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  mascot->mail.pixmap,
		  0,0,0,0,
		  allocation->width,
		  allocation->height);
  g_free(allocation);
  return FALSE;
}
#endif
#endif

// Clock Panel���������ѷ��ˤȤ�ʤ�Callback
#ifdef USE_GTK3
gint dw_configure_clk(GtkWidget *widget, cairo_t *cr, gpointer userdata){
  typMascot *mascot;
  mascot=(typMascot *)userdata;
  
  gdk_cairo_set_source_pixbuf(cr, pixbuf_clk[mascot->clk_page], 0, 0);
  cairo_paint(cr);
  return(FALSE);
}
#else
gint dw_configure_clk(GtkWidget *widget, GdkEventConfigure *event,
		      gpointer gdata)
{
  typMascot *mascot;
  GtkAllocation *allocation=g_new(GtkAllocation, 1);

  mascot=(typMascot *)gdata; 
  gtk_widget_get_allocation(widget,allocation);

  if (!pixmap_clk[0]) {
    pixmap_clk[0] = gdk_pixmap_new(gtk_widget_get_window(widget),
				   allocation->width,
				   allocation->height,
				   -1);
  } 
  if (!pixmap_clk[1]) {
    pixmap_clk[1] = gdk_pixmap_new(gtk_widget_get_window(widget),
				   allocation->width,
				   allocation->height,
				   -1);
  }

  g_free(allocation);

  if(mascot->gc_clk[0]==NULL){
    mascot->gc_clk[0] = MPCreatePen(pixmap_clk[0], mascot->colclk);
    mascot->gc_clksd[0] = MPCreatePen(pixmap_clk[0], mascot->colclksd);
    mascot->gc_clkbg[0] = MPCreatePen(pixmap_clk[0], mascot->colclkbg);
    mascot->gc_clkbd[0] = MPCreatePen(pixmap_clk[0], mascot->colclkbd);
  }
  if(mascot->gc_clk[1]==NULL){
    mascot->gc_clk[1] = MPCreatePen(pixmap_clk[1], mascot->colclk);
    mascot->gc_clksd[1] = MPCreatePen(pixmap_clk[1], mascot->colclksd);
    mascot->gc_clkbg[1] = MPCreatePen(pixmap_clk[1], mascot->colclkbg);
    mascot->gc_clkbd[1] = MPCreatePen(pixmap_clk[1], mascot->colclkbd);
  }
  
  if(mascot->clkmode==CLOCK_PANEL){
    // **** Mascot���� window�ؤ�����
    // **** window�ؤ�����
#ifdef USE_WIN32
    if((mascot->flag_clkfg)&&(mascot->alpha_clk!=100)){
      {
	GtkAllocation *allocation=g_new(GtkAllocation, 1);
	GtkStyle *style=gtk_widget_get_style(mascot->dw_clkfg);
	gtk_widget_get_allocation(mascot->dw_clkfg,allocation);
	
	gdk_draw_drawable(gtk_widget_get_window(mascot->dw_clkfg),
			  style->fg_gc[gtk_widget_get_state(mascot->dw_clkfg)],
			  pixmap_clk[mascot->clk_page],
			  0,0,0,0,
			  allocation->width,
			  allocation->height);
	g_free(allocation);
      }
      /*
      gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_fg),
				 pixmap_clk[mascot->clk_page],
				 FALSE);
      */
    }
#endif
    {
      GtkAllocation *allocation=g_new(GtkAllocation, 1);
      GtkStyle *style=gtk_widget_get_style(mascot->dw_clock);
      gtk_widget_get_allocation(mascot->dw_clock,allocation);
      
      gdk_draw_drawable(gtk_widget_get_window(mascot->dw_clock),
			style->fg_gc[gtk_widget_get_state(mascot->dw_clock)],
			pixmap_clk[mascot->clk_page],
			0,0,0,0,
			allocation->width,
			allocation->height);
      g_free(allocation);
    }
    /*
    gdk_window_set_back_pixmap(gtk_widget_get_window(mascot->clock_main),
			       pixmap_clk[mascot->clk_page],
			       FALSE);
    */
  }
  return FALSE;
}
#endif

// Balloon Window���������ѷ��ˤȤ�ʤ�Callback
#ifdef USE_GTK3
gint dw_configure_balloon(GtkWidget *widget, cairo_t *cr, gpointer userdata){
  typMascot *mascot;
  mascot=(typMascot *)userdata;
  
  //gdk_cairo_set_source_pixbuf(cr, pixbuf_clk[mascot->clk_page], 0, 0);
  //cairo_paint(cr);
  return(FALSE);
}
#else
gint dw_configure_bal(GtkWidget *widget, GdkEventConfigure *event,
		      gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 


  if(pixmap_bal[0]==NULL){
    pixmap_bal[0] = gdk_pixmap_new(gtk_widget_get_window(widget),
				widget->allocation.width,
				widget->allocation.height,
				-1);
  }

  if(pixmap_bal[1]==NULL){
    pixmap_bal[1] = gdk_pixmap_new(gtk_widget_get_window(widget),
				widget->allocation.width,
				widget->allocation.height,
				-1);
  }

  if(mascot->gc_bal[0]==NULL){
    mascot->gc_bal[0] = MPCreatePen(pixmap_bal[0], mascot->colbal);
    mascot->gc_balbg[0] = MPCreatePen(pixmap_bal[0], mascot->colbalbg);
    mascot->gc_balbd[0] = MPCreatePen(pixmap_bal[0], mascot->colbalbd);
  }
  if(mascot->gc_bal[1]==NULL){
    mascot->gc_bal[1] = MPCreatePen(pixmap_bal[1], mascot->colbal);
    mascot->gc_balbg[1] = MPCreatePen(pixmap_bal[1], mascot->colbalbg);
    mascot->gc_balbd[1] = MPCreatePen(pixmap_bal[1], mascot->colbalbd);
  }

  return FALSE;
}
#endif

// Main (Mascot) ���Ťʤ����������ν���
// Work���Ƥ��ʤ�Buffer���Pixmap�������Ԥ���
#ifdef USE_GTK3
gint dw_expose_main(GtkWidget *widget, cairo_t *cr, gpointer userdata){
  typMascot *mascot;
  mascot=(typMascot *)userdata;
  
  //if(!pixbuf_fcbk) draw_fc_cairo(widget,hg);
  gdk_cairo_set_source_pixbuf(cr, pixbuf_main[mascot->pixmap_page], 0, 0);
  cairo_paint(cr);
  return(FALSE);
}
#else
gint dw_expose_main(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  typMascot *mascot;
#ifdef DEBUG
  printf("Expose in\n");
#endif

  mascot=(typMascot *)gdata; 
  
  // **** Mascot���� window�ؤ�����
  gdk_draw_drawable(gtk_widget_get_window(widget),
		  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		  pixmap_main[mascot->pixmap_page],
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);

#ifdef DEBUG
  printf("Expose: using %d\n",mascot->pixmap_page);
#endif
  return FALSE;
}

#endif

gint expose_main(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
  
  if (supports_alpha)
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
  else
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* opaque white */
  
  /* draw the background */
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  
  cairo_destroy(cr);
  
  return FALSE;
}


#ifdef USE_WIN32
gint dw_expose_sdw(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  typMascot *mascot;

  mascot=(typMascot *)gdata; 
  
  // **** Mascot���� window�ؤ�����
  if((mascot->sdw_flag)&&(mascot->sdw_height>0))
    gdk_draw_drawable(gtk_widget_get_window(widget),
		      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		      pixmap_sdw[mascot->pixmap_page],
		      event->area.x, event->area.y,
		      event->area.x, event->area.y,
		      event->area.width, event->area.height);

  return FALSE;
}
#endif

#ifdef USE_BIFF
// Biff Pixmap ���Ťʤ����������ν���
gint dw_expose_biff_pix(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  typMascot *mascot;

  mascot=(typMascot *)gdata; 

  if(mascot->mail.flag){
    gdk_draw_drawable(gtk_widget_get_window(widget),
		    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		    mascot->mail.pixmap,
		    0,0,0,0,
		    widget->allocation.width,
		    widget->allocation.height);
  }

  return FALSE;
}
#endif

gint expose_biff_pix(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
  
  if (supports_alpha)
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
  else
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* opaque white */
  
  /* draw the background */
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  
  cairo_destroy(cr);
  
  return FALSE;
}


// Clock Panel ���Ťʤ����������ν���
// Work���Ƥ��ʤ�Buffer���Pixmap�������Ԥ���
gint dw_expose_clk(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  typMascot *mascot;

  mascot=(typMascot *)gdata; 

  if(mascot->clkmode==CLOCK_PANEL){
    // **** Mascot���� window�ؤ�����
    // **** window�ؤ�����
    gdk_draw_drawable(gtk_widget_get_window(widget),
		      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		      pixmap_clk[mascot->clk_page],
		      0,0,0,0,
		      widget->allocation.width,
		      widget->allocation.height);
  }
  return FALSE;
}


gint expose_clk(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
  
  if (supports_alpha)
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
  else
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* opaque white */
  
  /* draw the background */
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  
  cairo_destroy(cr);
  
  return FALSE;
}


// Balloon ���Ťʤ����������ν���
// Work���Ƥ��ʤ�Buffer���Pixmap�������Ԥ���
// 1.6.1�Ǽ¼���� drawballoon�Τʤ��� back_pixmap����ꤹ��ΤߤȤ�����
gint dw_expose_bal(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 
  
  // **** Balloon�� window�ؤ�����

  gdk_draw_drawable(gtk_widget_get_window(widget),
		    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		    pixmap_bal[mascot->bal_page],
		    0,0,0,0,
		    widget->allocation.width,
		    widget->allocation.height);
  return FALSE;
}	   

gint expose_bal(GtkWidget *widget, GdkEventExpose *event,  gpointer gdata)
{
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
  
  if (supports_alpha)
    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
  else
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* opaque white */
  
  /* draw the background */
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  
  cairo_destroy(cr);
  
  return FALSE;
}



// �ɥ�å��ˤ���ư
gboolean window_motion(GtkWidget * widget, GdkEventMotion * event, gpointer gdata)
{
  typMascot *mascot;
  mascot=(typMascot *)gdata; 

  if (mascot->drag){
    gint mx, my, newx, newy;
    GdkModifierType modmask;

    gdk_window_get_pointer(NULL, &mx, &my, &modmask);
    
    if(mascot->move==MOVE_FIX){
      gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.drag_f);
      newx = mx - window_x;
      newy = my - window_y;
      MoveMascot(mascot, newx, newy);
    }
    else if(mascot->move==MOVE_FOCUS){
      gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.drag_h);
      newx = mx - window_x;
      if(mascot->ff_side==FF_SIDE_RIGHT){
	if(mascot->flag_xp){
	  mascot->offsetp=100-
	    (int)((double)(newx-FWinX)/(double)(FWinWidth-mascot->width)*100);
	  if(mascot->offsetp>100) mascot->offsetp=100;
	  if(mascot->offsetp<0) mascot->offsetp=0;
	}
	else{
	  mascot->offset=-newx+FWinWidth+FWinX-mascot->width-mascot->xoff;
	}
      }
      else{
	if(mascot->flag_xp){
	    mascot->offsetp=(int)((double)(newx-FWinX)/
				  (double)(FWinWidth-mascot->width)*100);
	  if(mascot->offsetp>100) mascot->offsetp=100;
	  if(mascot->offsetp<0) mascot->offsetp=0;
	}
	else{
	  mascot->offset=newx-FWinX-mascot->xoff;
	}
      }
      MoveToFocus(mascot,FALSE);
    }
  }

  gdk_window_get_pointer(gtk_widget_get_window(widget), NULL, NULL, NULL);
  return(FALSE);
}   


// ���� �ɥ�å��ˤ���ư
void clk_window_motion(GtkWidget * widget, GdkEventMotion * event, gpointer gdata)
{   
  typMascot *mascot;
  mascot=(typMascot *)gdata; 

  if (mascot->clk_drag){
    gint mx, my, newx, newy;
    GdkModifierType modmask;
    
    gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.drag_f);
    gdk_window_get_pointer(NULL, &mx, &my, &modmask);
    newx = mx - clk_window_x;
    newy = my - clk_window_y;

    mascot->clk_x = (newx - mascot->x)*100/mascot->magnify;
    mascot->clk_y = (newy - mascot->y)*100/mascot->magnify;
    MoveMascot(mascot, mascot->x, mascot->y);
  }
}   


#ifdef USE_BIFF
// Biff �ɥ�å��ˤ���ư
void biff_window_motion(GtkWidget * widget, GdkEventMotion * event, gpointer gdata)
{   
  typMascot *mascot;
  mascot=(typMascot *)gdata; 

  if (mascot->mail.drag){
    gint mx, my, newx, newy;
    GdkModifierType modmask;
    
    gdk_window_set_cursor(gtk_widget_get_window(widget),mascot->cursor.drag_f);
    gdk_window_get_pointer(NULL, &mx, &my, &modmask);
    newx = mx - biff_window_x;
    newy = my - biff_window_y;
    if(mascot->mail.pix_pos==MAIL_PIX_LEFT){
      mascot->mail.pix_x = newx - mascot->x + widget->allocation.width/2;
    }
    else{
      mascot->mail.pix_x = newx - mascot->x - mascot->width 
	+ widget->allocation.width/2;
    }
      mascot->mail.pix_y = newy - mascot->y;
    MoveBiffPix(mascot, mascot->x, mascot->y);
  }
}   
#endif


void focus_in(GtkWidget * widget, GdkEventMotion * event)
{
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}

void focus_out(GtkWidget * widget, GdkEventMotion * event)
{
  gdk_pointer_ungrab(GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
}


// ���˥᡼����󤪤�ӻ���update�Ѵؿ�
// timeout�����ʥ�����Ū�˸ƤӽФ���
gboolean time_update(gpointer gdata)
{
  typMascot *mascot;
#ifdef DEBUG
  static int pre_pix;
  gint work_page;
#endif
  SockMsgInitResult sockres;
  gchar *msg=NULL;

  flag_anime=FALSE;
  flag_clock=FALSE;
  mascot=(typMascot *)gdata; 

  // ��˥塼���ФƤ���
  if(mascot->flag_menu) return(TRUE);

  // Window Sitter
  if(mascot->move==MOVE_FOCUS){
    MoveToFocus(mascot, FALSE);
  }
  // ����ޥ����å�
  if(mascot->move==MOVE_FIX){
    mascot->xfix=mascot->x;
    mascot->yfix=mascot->y;
  }

#ifdef USE_BIFF
  // POP biff
  if(mascot->mail.pop_readed==TRUE){
      display_biff_balloon(mascot);
      mascot->mail.pop_readed=FALSE;
  }
#endif

  // �դ���������
  if(flag_balloon==TRUE){
    DoBalloon(mascot);
  }


#ifdef USE_SOCKMSG
  // Duet���˥�
  if(mascot->duet_seq>0){
    mascot->duet_seq--;
  }
  else if(mascot->duet_seq==0){
    mascot->duet_seq--;
    if (mascot->duet_tgt[mascot->anime_ptn]){
      if((0<mascot->duet_ptn[mascot->anime_ptn])&&
	 (mascot->duet_ptn[mascot->anime_ptn]<MAX_ANIME_PATTERN)){
	if((mascot->duet_mode==DUET_CLICK)
	   &&(mascot->duet_word[mascot->anime_ptn])){
	  msg = g_strdup_printf("%02d%02d%s",
				mascot->duet_mode,
				mascot->duet_ptn[mascot->anime_ptn],
				mascot->duet_word[mascot->anime_ptn]);
	}
	else{
	  msg = g_strdup_printf("%02d%02d",mascot->duet_mode,
				mascot->duet_ptn[mascot->anime_ptn]);
	}
	
	// Duet Anime Client ��ư
	sockres = duet_cl_init(mascot->duet_tgt[mascot->anime_ptn]);
	switch (sockres) {
	case SOCKMSG_ERROR:
	  break;
	case SOCKMSG_OPENED:
	  duet_send_msg(msg);
	  duet_cl_done(mascot->duet_tgt[mascot->anime_ptn],FALSE);
	  break;
	}
	g_free(msg);
      }
    }
  }
#endif  // USE_SOCKMSG

  // ���˥�
  mascot->anime_seq++;
  if((mascot->anime_seqend-mascot->anime_seq)<=0){
    //�ե졼��ν�λ
    mascot->anime_seq=0;
    // �֥��å��롼��
    if((mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].next>=0)){
      mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seq++;
      if(mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seq==1){
	// �֥��å��롼�׳���
	mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seqend=
	  RANDOM(mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].max
		 -mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].min+1)
	  +mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].min;
	       
	if(mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seqend==1){
	  mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seq=0;
	  mascot->anime_frm++;
	}
	else{
	  mascot->anime_frm=
	    mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].next;
	}
      }
      else if(mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seqend-
	      mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seq<=0){
	// �֥��å��롼�׽�λ
	mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].seq=0;
	mascot->anime_frm++;
      }
      else{
	// �֥��å��롼�׷�³
	mascot->anime_frm=
	  mascot->frame_loop[mascot->anime_ptn][mascot->anime_frm].next;
      }
    }
    else{
      mascot->anime_frm++;
    }
    if(mascot->anime_frm>=mascot->frame_num[mascot->anime_ptn]){
      //�ѥ�����ν�λ
      if(mascot->anime_ptn==0){
	if(mascot->random_total==0){
	  // �����ॢ�˥᤬�ʤ���� �١�������Ƭ�����
	  mascot->anime_frm=0;
	}
	else{
	  // �١����ν�λ -> ������ѥ�����ؤΰܹ�
	  mascot->anime_ptn=weight_random(mascot);
	  mascot->anime_frm=0;
	  if(mascot->duet_use_random){
	    if(mascot->duet_tgt[mascot->anime_ptn]){
	      // Duet���˥�
	      mascot->duet_seq=mascot->duet_delay[mascot->anime_ptn];
	      mascot->duet_mode=DUET_RANDOM;
	    }
	  }
	}
      }
      else{
	// ������֤����
	mascot->anime_ptn=0;
	mascot->anime_frm=0;
	if((flag_balloon)&&
	   ((mascot->bal_mode==BALLOON_NORMAL)
	    ||(mascot->bal_mode==BALLOON_DUET))){
	  // �Х롼�󤬤ǤƤ�������Ĥ���
	  // ����å����˥�ν�λ
#ifdef USE_WIN32
	    gtk_widget_unmap(mascot->balloon_fg);
#endif
	    gtk_widget_unmap(mascot->balloon_main);
	    flag_balloon=FALSE;
	}
      }
    }
    else{
      //���Υե졼��ذܹ�
    }
    mascot->anime_seqend=
      RANDOM(mascot->frame_max[mascot->anime_ptn][mascot->anime_frm]
	     -mascot->frame_min[mascot->anime_ptn][mascot->anime_frm]+1)
      +mascot->frame_min[mascot->anime_ptn][mascot->anime_frm];
    flag_anime=TRUE;
  }
  // �ե졼�ब�Ѥ�ä���shape���ѷ�
  //  if(pre_pix!=mascot->frame_pix[mascot->anime_ptn][mascot->anime_frm]){
  //   gtk_widget_shape_combine_mask( win_main, 
  //				  mascot->sprites[mascot->frame_pix[mascot->anime_ptn][mascot->anime_frm]].mask,
  //				   0, 0 ); 
  //pre_pix=mascot->frame_pix[mascot->anime_ptn][mascot->anime_frm];
    
  //}

  clock_update(mascot,FALSE);


  // ���˥᡼�����ˤ��ޥ����åȤ�����
  // �ޥ����åȾ�λ��פ�񤭴������Ȥ��Ϥ��ʤ�
  if((flag_clock==FALSE)&&(flag_anime==TRUE)){ 
#ifdef DEBUG 
    work_page=mascot->pixmap_page;
    work_page^=1;
    printf("Drawto %d\n",work_page);
#endif
  
    if(mascot->clkmode==CLOCK_PIXMAP){ 
      //���פ��Ѥ��ʤ������˥᤬�Ѥ�ä�
      DrawMascotWithDigit(mascot);
    }
    else{
      DrawMascot(mascot);
    }

#ifdef DEBUG
    printf("Draw finish %d\n",work_page);
#endif
  }

  if(mascot->raise_force) raise_all(mascot);
    
#ifdef USE_GTK3	    
  gdk_display_flush(gdk_display_get_default());
#else	    
  gdk_flush();
#endif	    
  
  return(TRUE);

}


// ���פΥ��åץǡ���
void clock_update(typMascot *mascot, gboolean force_flag){
  time_t t;
  struct tm *tmpt;
  gchar* caldigit;
  gchar *week;

  t = time(NULL);
  tmpt = localtime(&t);

  if(tmpt->tm_mday!=old_day){
    week=ini_week(tmpt->tm_wday);

    caldigit=g_strdup_printf(_("%d/%d/%d(%s)"), tmpt->tm_year+1900,tmpt->tm_mon+1,
                                      tmpt->tm_mday,week);
    g_free(week);

    gtk_widget_set_tooltip_text(mascot->clock_main,caldigit);

#ifdef USE_WIN32
    gtk_widget_set_tooltip_text(mascot->clock_fg,caldigit);
#endif

    g_free(caldigit);
    old_day=tmpt->tm_mday;
  }

  // �������
  if(pre_hour!=-1){
    if(pre_hour!=tmpt->tm_hour){
      pre_hour=tmpt->tm_hour;
      if((mascot->signal.flag)&&(!mascot->drag)){
	// �������ޥ��
	if(mascot->signal.com){
	  ext_play(mascot,mascot->signal.com);
	}

	// �ޥ����åȤ�ư��
	switch(mascot->signal.type){
	case SIGACT_CLICK:
	  if((mascot->click_total!=0)&&(flag_balloon==FALSE)){
	    gint i_frm;

	    // ����å����˥�
	    for(i_frm=0;i_frm<mascot->frame_num[mascot->anime_ptn];i_frm++){
	      // �֥��å��롼�פΥ���󥻥�
	      mascot->frame_loop[mascot->anime_ptn][i_frm].seq=0;
	    }
	    mascot->anime_ptn=weight_click(mascot);
	    mascot->anime_frm=-1;
	    mascot->anime_seq=-1;
	    mascot->anime_seqend=
	      RANDOM(mascot->frame_max[mascot->anime_ptn][mascot->anime_frm]
		     -mascot->frame_min[mascot->anime_ptn][mascot->anime_frm]+1)
	      +mascot->frame_min[mascot->anime_ptn][mascot->anime_frm];

	    sound_play(mascot,mascot->click_sound[mascot->anime_ptn]);

	    if(mascot->click_word[mascot->anime_ptn]) {
	      mascot->balseq=0;
	      mascot->bal_mode=BALLOON_NORMAL;
	      DoBalloon(mascot); 
	      flag_balloon=TRUE;
	    }
	  }
      	  gdk_pointer_ungrab(GDK_CURRENT_TIME);
#ifdef USE_GTK3	    
	  gdk_display_flush(gdk_display_get_default());
#else	    
	  gdk_flush();
#endif	    
	  break;
	case SIGACT_CHANGE:
	  if(mascot->menu_file){
	    AllRandomChangeMascotMenu(NULL);
	  }
	  break;
	}
      }
    }

  if(mascot->drag==FALSE){
    gboolean draw_flag=FALSE;

    switch(mascot->clktype){
    case CLOCK_TYPE_24S:
      if((tmpt->tm_sec!=pre_sec)||(force_flag)){
	sprintf(mascot->digit,"%2d:%02d:%02d",
		tmpt->tm_hour,tmpt->tm_min,tmpt->tm_sec);
	draw_flag=TRUE;
      }
      break;

    case CLOCK_TYPE_12S:
      if((tmpt->tm_sec!=pre_sec)||(force_flag)){
	if(tmpt->tm_hour>=12){
	  sprintf(mascot->digit,"%2d:%02d:%02d",
		  tmpt->tm_hour==12?12:tmpt->tm_hour-12,tmpt->tm_min,
		  tmpt->tm_sec);
	  mascot->clk_pm=TRUE;
	}
	else{
	  sprintf(mascot->digit,"%2d:%02d:%02d",
		  tmpt->tm_hour,tmpt->tm_min,tmpt->tm_sec);
	  mascot->clk_pm=FALSE;
	}
	draw_flag=TRUE;
      }
      break;

    case CLOCK_TYPE_24M:
      if((tmpt->tm_min!=pre_min)||(force_flag)){
	sprintf(mascot->digit,"%2d:%02d",
		tmpt->tm_hour,tmpt->tm_min);
	draw_flag=TRUE;
      }
      break;

    case CLOCK_TYPE_12M:
      if((tmpt->tm_min!=pre_min)||(force_flag)){
	if(tmpt->tm_hour>=12){
	  sprintf(mascot->digit,"%2d:%02d",
		  tmpt->tm_hour==12?12:tmpt->tm_hour-12,tmpt->tm_min);
	  mascot->clk_pm=TRUE;
	}
	else{
	  sprintf(mascot->digit,"%2d:%02d",
		  tmpt->tm_hour,tmpt->tm_min);
	  mascot->clk_pm=FALSE;
	}
	draw_flag=TRUE;
      }
    }

    if(draw_flag){
      // ������
      switch(mascot->clkmode){
      case CLOCK_PIXMAP:
	// Mascot���ľ��ʸ����񤯾��ϰ��٥ޥ����åȤ�pixmap���
	// ���褷�ʤ���ɬ�פ�����(����ʸ����ä�����)
	DrawMascotWithDigit(mascot);
	flag_clock=TRUE;
	break;
	
      case CLOCK_PANEL:
	// �ѥͥ��λ��פξ��ϥѥͥ�Τߤ�����ľ��
	// ���δؿ�������Window���������ѹ��ˤ��configure/expose_event��
	// �ƤӽФ���, ������pixmap����window�ؤ������ԤäƤ���
	if(force_flag){
	  DrawPanelClock0(mascot);
	}
	DrawPanelClock0(mascot);
	break;
      }
      pre_sec=tmpt->tm_sec;
      pre_min=tmpt->tm_min;
    }
  }

  }
  else{
    // ���ư��
    pre_hour=tmpt->tm_hour;
  }
}



gchar *ini_week(gint day_of_week){
  gchar * weekk=NULL;

  switch(day_of_week){
  case 0:
    weekk=g_strdup(_("Sun"));
    break;
  case 1:
    weekk=g_strdup(_("Mon"));
    break;
  case 2:
    weekk=g_strdup(_("Tue"));
    break;
  case 3:
    weekk=g_strdup(_("Wed"));
    break;
  case 4:
    weekk=g_strdup(_("Thu"));
    break;
  case 5:
    weekk=g_strdup(_("Fri"));
    break;
  case 6:
    weekk=g_strdup(_("Sat"));
    break;
  }
  
  return(weekk);
}

// Ǥ�ե��ޥ�ɼ¹�
void ext_play(typMascot *mascot, char *exe_command)
{
#ifdef USE_WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  if(exe_command){
    ZeroMemory(&si, sizeof(si));
    si.cb=sizeof(si);

    if(CreateProcess(NULL, (LPTSTR)exe_command, NULL, NULL,
		     FALSE, NORMAL_PRIORITY_CLASS,
		     NULL, NULL, &si, &pi)){
      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);
    }
      
  }
  
#else
  static pid_t pid;
  gchar *cmdline;
  gint ret;
  
  if(exe_command){
    waitpid(pid,0,WNOHANG);
    if(strcmp(exe_command,"\0")!=0){
      cmdline=g_strdup(exe_command);
      if( (pid = fork()) == 0 ){
	ret=system(cmdline);
	_exit(-1);
	signal(SIGCHLD,ChildTerm);
      }
      g_free(cmdline);
    }
  }
#endif // USE_WIN32
}


// �������� (1�Ĥ���)
void sound_play(typMascot *mascot, char *wav_name)
{
#ifdef USE_WIN32
  gchar *filename1;

  if( wav_name ){

    if(!(filename1=FullPathSoundFile(mascot, wav_name, FALSE))) return;

    PlaySound(filename1,NULL, SND_ASYNC);
  }
#else
  static pid_t pid;
  gchar cmdline[1024];
  gchar *filename1;
  gint ret;
  

  if( (mascot->sound_command) && (wav_name) ){

    if(!(filename1=FullPathSoundFile(mascot, wav_name, FALSE))) return;

    sprintf(cmdline,mascot->sound_command,filename1);
    waitpid(pid,0,WNOHANG);
    if( (pid = fork()) == 0 ){
      ret=system(cmdline);
      _exit(-1);
      signal(SIGCHLD,ChildTerm);
    }
  }
#endif // USE_WIN32
}


#ifndef USE_WIN32
// �ҥץ������ν��� 
void ChildTerm(int dummy)
{
  int s;

  wait(&s);
  signal(SIGCHLD,ChildTerm);
}
#endif // USE_WIN32


// Raise all window
void raise_all(typMascot *mascot){
#ifdef USE_WIN32
  HWND   hWnd;

  SetWindowPos(GDK_WINDOW_HWND(gtk_widget_get_window(mascot->win_main)),HWND_TOPMOST,0,0,0,0,
	SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
  SetWindowLong(GDK_WINDOW_HWND(gtk_widget_get_window(mascot->win_main)), GWL_EXSTYLE, 
	GetWindowLong(GDK_WINDOW_HWND(gtk_widget_get_window(mascot->win_main)), GWL_EXSTYLE)|WS_EX_NOACTIVATE);
  gdk_window_raise(gtk_widget_get_window(mascot->win_sdw));
  gdk_window_raise(gtk_widget_get_window(mascot->win_main));
#else
  gdk_window_raise(gtk_widget_get_window(mascot->win_main));
#endif

#ifdef USE_WIN32
  gdk_window_raise(gtk_widget_get_window(mascot->clock_fg));
#endif
  gdk_window_raise(gtk_widget_get_window(mascot->clock_main));

#ifdef USE_WIN32
  gdk_window_raise(gtk_widget_get_window(mascot->balloon_fg));
#endif
  gdk_window_raise(gtk_widget_get_window(mascot->balloon_main));
#ifdef USE_BIFF
  gdk_window_raise(gtk_widget_get_window(mascot->biff_pix));
#endif
}


void callbacks_arg_init(){
  pre_hour=-1;
  pre_min=-1;
  pre_sec=-1;

  start_x=0;
  start_y=0;
#ifdef USE_BIFF
  biff_start_x=0;
  biff_start_y=0;
#endif
  clk_start_x=0;
  clk_start_y=0;

  flag_balloon=FALSE;
  flag_anime=FALSE;
  flag_clock=FALSE;

  old_day=0;
  errflag=-1;
}

#ifdef USE_WIN32
//*********************************************************
//  �������С��ΰ�ζ�����֤���
//*********************************************************
MyXY GetTaskbarHeight( void )
{
  APPBARDATA abd;
  HWND hWndAutoHide;
  MyXY xy;

  xy.flag=TRUE;
  
  memset( &abd, 0, sizeof( abd ) );
  abd.cbSize = sizeof( abd );
  
  if( SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE ){
    SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd);
  }
  else{
    SHAppBarMessage( ABM_GETTASKBARPOS, &abd );
  }
  if(abd.uEdge==ABE_BOTTOM){
    xy.y=(abd.rc.bottom - abd.rc.top);
  }
  else{
    xy.y=0;
  }	

  //SHAppBarMessage( ABM_GETTASKBARPOS, &abd );
  if(abd.uEdge==ABE_BOTTOM){
    xy.x = (abd.rc.right);
  }
  else{
    xy.x =(GetSystemMetrics(SM_CXVIRTUALSCREEN));
  }	
  
  return(xy);
}//GetTaskbarRect

//*********************************************************
// Win32 �롼�Ȥ�Ƚ�̥�����ɥ��������֤���
//*********************************************************
gboolean IsWin32Root( HWND hWnd, gint root_width, gint root_height )
{
  RECT Rect;
  
  GetWindowRect( hWnd, &Rect ); // ������ɥ�����μ���
  if((Rect.right - Rect.left)==root_width){
    if((Rect.bottom - Rect.top)==root_height){
      return(TRUE);
    }
  };
  return(FALSE);
}//GetWindowWidth
#endif

#ifndef USE_WIN32
//*********************************************************
// X Window  ���ߤΤä��äƤ���Window��̾��
//*********************************************************
gchar* Get_Window_Name(dpy, top)
     Display *dpy;
     Window top;
{
  Window *children, dummy;
  unsigned int nchildren;
  int i;
  Window w=0;
  char *window_name;
  
  if (XFetchName(dpy, top, &window_name)){
    return(window_name);
  }
  
  if (!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren)){
    return(NULL);
  }
  
  for (i=0; i<nchildren; i++) {
    if(window_name!=NULL) g_free(window_name);
    window_name = g_strdup(Get_Window_Name(dpy, children[i]));
    if (window_name!=NULL) break;
  }
  if (children) XFree ((char *)children);
  
  return(window_name);
}

//*********************************************************
// X Window  ���ߤΤä��äƤ���Window��Bar Size
//*********************************************************
gint Get_Window_Bar_Size(dpy, top)
     Display *dpy;
     Window top;
{
  Atom type, atom;
  int format;
  union {
    gulong *prop;
    guchar *prop_ch;
  } prop = { NULL };
  gulong nitems;
  gulong bytes_after; 
  int error, result;
  gint bar_size;
  GdkWMDecoration decor=0;

  bar_size=0;

  gdk_error_trap_push ();
  type = None;

  atom=XInternAtom(dpy,"_NET_FRAME_EXTENTS", False);
  result=XGetWindowProperty(dpy, top,
			    atom,
			    0,  G_MAXLONG,  FALSE,
			    XA_CARDINAL,
			    &type, &format, &nitems,
			    &bytes_after, &prop.prop_ch);
  error = gdk_error_trap_pop ();

  if (error || result != Success){
    return(bar_size);
  }

  if(nitems==4){
    bar_size=prop.prop[2];
  }


  if (prop.prop_ch)
    XFree (prop.prop_ch);

  return(bar_size);
}



//*********************************************************
// X Window  �������С��α����ɸ���֤���
//    Home_X  =  root_x - ret.x
//    Home_y  =  root_y - ret.y
//*********************************************************
MyXY GetAutoHomePos(void){
    GdkAtom atom, atom_ret;
    gint format, length;
    guchar *data;
    gboolean ret;
    MyXY xy;

    atom = gdk_atom_intern ("_NET_WORKAREA", TRUE);
    if (atom != GDK_NONE) {
        ret = gdk_property_get (
				gdk_get_default_root_window (),
				atom, GDK_NONE, 0, G_MAXLONG, FALSE,
				&atom_ret, &format, &length, &data);
        if (ret && format == 32 && length / sizeof(glong) >= 4) {
	  //g_print ("format = %d, length = %d\n", format, length);
	  //  g_print ("x = %d, y = %d\n",
	  //	     ((glong*) data)[0], ((glong*) data)[1]);
	  // g_print ("width = %d, height = %d\n",
	  //	     ((glong*) data)[2], ((glong*) data)[3]);
	  xy.flag=TRUE;
	  xy.x=((glong*) data)[0] + ((glong*) data)[2] ;
	  xy.y=((glong*) data)[3] + ((glong*) data)[1] ;
	} else {
	  xy.flag=FALSE;
	  xy.x=0;
	  xy.y=0 ;
	}
	g_free(data);
    }
    else{
      xy.flag=FALSE;
      xy.x=0;
      xy.y=0 ;
    }

    return(xy);
}

#endif


void InitMascot0(typMascot *mascot){
  int i_page;
#ifndef USE_WIN32
  Window rootwin;
#endif

  int x_root, y_root, width_root, height_root, border, depth;
  // Root Window���礭������

#ifdef USE_WIN32
  width_root=GetSystemMetrics(SM_CXVIRTUALSCREEN);
  height_root=GetSystemMetrics(SM_CYVIRTUALSCREEN);

#else
  XGetGeometry(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(mascot->win_main)),
	       GDK_ROOT_WINDOW(),
	       &rootwin,
	       &x_root,
	       &y_root,
	       &width_root,
	       &height_root,
	       &border,
	       &depth);
#endif
  mascot->width_root=width_root;
  mascot->height_root=height_root;

#ifdef USE_SOCKMSG
  mascot->duet_open=FALSE;
  mascot->duet_file=NULL;
#endif  // USE_SOCKMSG


  mascot->x=0;
  mascot->y=0;
  
  mascot->pixmap_page=1;
  mascot->bal_page=1;
  mascot->clk_page=1;
  
  mascot->balwidth=0;
  mascot->balheight=0;
  mascot->clk_drag=FALSE;

  mascot->flag_menu=FALSE;

#ifndef USE_GTK3  
  for(i_page=0;i_page<2;i_page++){
    mascot->gc_main[i_page] = NULL;
    mascot->gc_mainsd[i_page] = NULL;
    mascot->gc_clk[i_page] = NULL;
    mascot->gc_clksd[i_page] = NULL;
    mascot->gc_clkbg[i_page] = NULL;
    mascot->gc_clkbd[i_page] = NULL;
    mascot->gc_clkmask[i_page] = NULL;
    mascot->gc_bal[i_page] = NULL;
    mascot->gc_balbg[i_page] = NULL;
    mascot->gc_balbd[i_page] = NULL;
    mascot->gc_balmask[i_page] = NULL;
  }
#endif
  
  mascot->sprites=sprite_void;

#ifdef USE_BIFF
  mascot->mail.proc_id=-1;
#ifdef USE_GTK3
  mascot->mail.pixbuf=NULL;
#else  
  mascot->mail.pixmap=NULL;
  mascot->mail.mask=NULL;
#endif  
  mascot->mail.drag=FALSE;
  mascot->mail.fs_max=NULL;
  mascot->mail.pop_readed=FALSE;
  mascot->mail.pop_child_fl=FALSE;
#endif  // USE_BIFF

  mascot->clk_pm=FALSE;

  mascot->colclk=g_malloc0(sizeof(GdkColor));
  mascot->colclksd=g_malloc0(sizeof(GdkColor));
  mascot->colclkbg=g_malloc0(sizeof(GdkColor));
  mascot->colclkbd=g_malloc0(sizeof(GdkColor));

  mascot->colbal=g_malloc0(sizeof(GdkColor));
  mascot->colbalbg=g_malloc0(sizeof(GdkColor));
  mascot->colbalbd=g_malloc0(sizeof(GdkColor));


  InitMascot(mascot);

}


void make_mascot(typMascot *mascot){
  GtkWidget *ebox, *ebox1;
  GtkTargetEntry drag_types[1] = {{"text/uri-list", 0, 0}};
  
  mascot->win_main = gtk_window_new(GTK_WINDOW_POPUP);

  gtk_window_set_accept_focus(GTK_WINDOW(mascot->win_main),FALSE);
  gtk_window_set_resizable(GTK_WINDOW(mascot->win_main),TRUE);
  gtk_widget_set_app_paintable(mascot->win_main, TRUE);
  gtk_window_set_title(GTK_WINDOW(mascot->win_main), "MaCoPiX");

#ifndef USE_GTK3
  gtk_window_set_wmclass(GTK_WINDOW(mascot->win_main),
			 "main_window", "MaCoPiX");
#endif
  
  my_signal_connect(mascot->win_main, "destroy", gtk_main_quit, NULL);

  gtk_window_resize(GTK_WINDOW(mascot->win_main),1,1);

  ebox=gtk_event_box_new();
  gtk_container_add (GTK_CONTAINER (mascot->win_main), ebox);
  mascot->dw_main = gtk_drawing_area_new();
  gtk_widget_set_size_request (mascot->dw_main, 1, 1);
  gtk_container_add(GTK_CONTAINER(ebox), mascot->dw_main);
  gtk_widget_set_app_paintable(mascot->dw_main, TRUE);

  gtk_widget_set_events(ebox, 
			GDK_BUTTON_MOTION_MASK | 
			GDK_POINTER_MOTION_MASK | 
			GDK_FOCUS_CHANGE_MASK | 
			GDK_BUTTON_RELEASE_MASK | 
			GDK_BUTTON_PRESS_MASK | 
			GDK_EXPOSURE_MASK);

  // Realizing after set_events
  gtk_widget_realize(mascot->win_main);

  gtk_event_box_set_above_child(GTK_EVENT_BOX(ebox), TRUE);
  
#ifdef USE_WIN32
  mascot->win_sdw = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_accept_focus(GTK_WINDOW(mascot->win_sdw),FALSE);
  gtk_window_set_resizable(GTK_WINDOW(mascot->win_sdw),TRUE);
  gtk_widget_set_app_paintable(mascot->win_sdw, TRUE);
  gtk_window_set_title(GTK_WINDOW(mascot->win_sdw), "MaCoPiX");

#ifndef USE_GTK3
  gtk_window_set_wmclass(GTK_WINDOW(mascot->win_sdw), "sdw_window", "MaCoPiX");
#endif  
 
  my_signal_connect(mascot->win_sdw, "destroy",gtk_main_quit,NULL);

  gtk_widget_set_events(GTK_WIDGET (mascot->win_sdw), 
			GDK_FOCUS_CHANGE_MASK | 
			GDK_BUTTON_MOTION_MASK | 
			GDK_BUTTON_RELEASE_MASK | 
			GDK_BUTTON_PRESS_MASK | 
			GDK_EXPOSURE_MASK);

  gtk_window_resize(GTK_WINDOW(mascot->win_sdw),1,1);

  ebox1=gtk_event_box_new();
  gtk_container_add (GTK_CONTAINER (mascot->win_main), ebox1);
  mascot->dw_sdw = gtk_drawing_area_new();
  gtk_widget_set_size_request (mascot->dw_sdw, 1, 1);
  gtk_container_add(GTK_CONTAINER(ebox1), mascot->dw_sdw);
  gtk_widget_set_app_paintable(mascot->dw_sdw, TRUE);

  gtk_widget_set_events(ebox1, 
			GDK_BUTTON_MOTION_MASK | 
			GDK_POINTER_MOTION_MASK | 
			GDK_FOCUS_CHANGE_MASK | 
			GDK_BUTTON_RELEASE_MASK | 
			GDK_BUTTON_PRESS_MASK | 
			GDK_EXPOSURE_MASK);

  // Realizing after set_events
  gtk_widget_realize(mascot->win_sdw);

  gtk_event_box_set_above_child(GTK_EVENT_BOX(ebox1), TRUE);
#endif


#ifdef __GTK_STATUS_ICON_H__
  trayicon_create(mascot);
  if(mascot->tray_icon_flag){
    trayicon_show(mascot);
  }
  else{
    trayicon_hide(mascot);
  }
#endif

  gdk_window_set_decorations(gtk_widget_get_window(mascot->win_main), 0);
#ifndef USE_WIN32
  //  gdk_window_set_decorations(win_main->window, GDK_DECOR_MENU);
  // Win32 Gtk+> 2.10 -> trayicon 
  gdk_window_set_override_redirect(gtk_widget_get_window(mascot->win_main),
				   TRUE);
#endif

#ifdef USE_WIN32
  gdk_window_set_decorations(gtk_widget_get_window(mascot->win_sdw), 0);
#endif

  /*
  my_signal_connect(mascot->win_main, "focus_in_event",focus_in, NULL);
  my_signal_connect(mascot->win_main, "focus_out_event",focus_out, NULL);
  my_signal_connect(mascot->win_main, "button_press_event",drag_begin,
		    (gpointer)mascot);
  my_signal_connect(mascot->win_main, "button_release_event",drag_end,
		    (gpointer)mascot);
  my_signal_connect(mascot->win_main, "motion_notify_event",window_motion,
		    (gpointer)mascot);
  */

  /* Drag and Drop */
  gtk_drag_dest_set (mascot->win_main, GTK_DEST_DEFAULT_MOTION
                        | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
		     drag_types, 1, GDK_ACTION_COPY);
  my_signal_connect (mascot->win_main, "drag-data-received",
		     signal_drag_data_received, NULL);

  InitMascot0(mascot);
  ReadMascot(mascot,FALSE);

  // ��������
#ifdef USE_GTK3
  mascot->cursor.normal=gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_NORMAL);
  mascot->cursor.push  =gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_PUSH);
  mascot->cursor.drag_h=gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_DRAG_H);
  mascot->cursor.drag_f=gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_DRAG_F);
#ifdef USE_BIFF
  mascot->cursor.biff  =gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_BIFF);
#endif // USE_BIFF
  mascot->cursor.clk   =gdk_cursor_new_for_display(gdk_display_get_default(),
						   CURSOR_CLK);
#else // USE_GTK3
  mascot->cursor.normal=gdk_cursor_new(CURSOR_NORMAL);
  mascot->cursor.push  =gdk_cursor_new(CURSOR_PUSH);
  mascot->cursor.drag_h=gdk_cursor_new(CURSOR_DRAG_H);
  mascot->cursor.drag_f=gdk_cursor_new(CURSOR_DRAG_F);
#ifdef USE_BIFF
  mascot->cursor.biff  =gdk_cursor_new(CURSOR_BIFF);
#endif // USE_BIFF
  mascot->cursor.clk   =gdk_cursor_new(CURSOR_CLK);
#endif  // USE_GTK3

  my_signal_connect(ebox, "focus_in_event",focus_in, NULL);
  my_signal_connect(ebox, "focus_out_event",focus_out, NULL);
  my_signal_connect(ebox, "button_press_event",drag_begin,
		    (gpointer)mascot);
  my_signal_connect(ebox, "button_release_event",drag_end,
		    (gpointer)mascot);
  my_signal_connect(ebox, "motion-notify-event", window_motion,
		    (gpointer)mascot);
  
  // Window�������ѷ�
  my_signal_connect(mascot->dw_main, "configure_event",dw_configure_main,
  		    (gpointer)Mascot);
  // �Ťʤä����κ������Ϣ
#ifdef USE_GTK3
  my_signal_connect(mascot->dw_main, "draw",dw_expose_main,
  		    (gpointer)mascot);
#else
  my_signal_connect(mascot->dw_main, "expose_event",dw_expose_main,
  		    (gpointer)mascot);
  my_signal_connect(mascot->win_main, "expose_event",expose_main,
  		    (gpointer)mascot);
#endif

#ifdef USE_WIN32
  my_signal_connect(mascot->dw_sdw, "configure_event",dw_configure_sdw,
  		    (gpointer)mascot);
  // �Ťʤä����κ������Ϣ
  my_signal_connect(mascot->dw_sdw, "expose_event",dw_expose_sdw,
  		    (gpointer)mascot);
  my_signal_connect(mascot->win_sdw, "expose_event",expose_main,
  		    (gpointer)mascot);
#endif
  
}