#ifndef _BASEWIN_H_
#define _BASEWIN_H_
#include<Windows.h>
typedef LRESULT (* WINPROC)(HWND,UINT,WPARAM,LPARAM);
class BaseWin{
protected:
  HWND hwnd;//, m_parent;
  LPCSTR WinName;
  WINPROC Winproc = 0;
  
public:
  BaseWin();
  BaseWin(LPCSTR p_winname);
  HWND GetHwnd(){
    return hwnd;
  }
  virtual ~BaseWin()=0;
  

  static INT32 WinCnt;
  static HINSTANCE process_inst;
  static HANDLE b_mutex;
  static int cmdShow;
  static WNDCLASS bare_winclass;

  static LRESULT bare_win_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
  static void Init(HINSTANCE p_inst,int cmdShow,HANDLE p_mutex = 0);
  static void CloseBaseWin();
};
#endif // _BASEWIN_H_