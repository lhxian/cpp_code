#ifndef _BUTTONWIN_H_
#define _BUTTONWIN_H_
#include"all.h"

#define BUTTON_FOCUS_ELAPSE 1500
#define ID_TIMER 0xf2
class ButtonWin:public BaseWin{
  UINT button_type = 0;
  HDC mem_dc =0;
  HBITMAP bmp = 0;
  gs::Image* m_image = NULL;
  gs::Graphics* p_graphics = NULL;
protected:
  static gs::Pen* button_pen;
  static gs::Rect button_disrect;
  static WNDCLASS button_class;
public:
  ButtonWin(HWND parent,INT32 px,INT32 py,UINT button_type,const WCHAR* img_path);
  ~ButtonWin();
  UINT get_type(){return button_type;}
  void reset();
  void focus();

  static void InitButtonwin();
  static void closeButtonwin();
  static LRESULT ButtonWin_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);

};

#endif // _BUTTONWIN_H_