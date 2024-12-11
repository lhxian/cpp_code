#include"BaseWin.h"
#include<cassert>
#include"macro.h"
// static data
INT32 BaseWin::WinCnt = 0;
HINSTANCE BaseWin::process_inst = 0;
HANDLE BaseWin::b_mutex = 0;
int BaseWin::cmdShow = 0;
WNDCLASS BaseWin::bare_winclass = {0};
// constructor and deconstructor
BaseWin::BaseWin():WinName(NULL){}
BaseWin::BaseWin(LPCSTR p_classname):WinName(p_classname){}

BaseWin::~BaseWin(){}

void BaseWin::Init(HINSTANCE P_inst,int cmdshow,HANDLE g_mutex){
  BaseWin::process_inst = P_inst;
  BaseWin::cmdShow = cmdshow;
  BaseWin::b_mutex = g_mutex;
  bare_winclass.hCursor = static_cast<HCURSOR>(LoadCursor(NULL,IDC_ARROW));
  bare_winclass.lpfnWndProc = bare_win_proc;
  bare_winclass.lpszClassName = "BASE_WIN";
  bare_winclass.hInstance = BaseWin::process_inst;
  auto st = RegisterClass(&bare_winclass);
  OutputDebugStringA("register the base class");
  assert(st);
}
void BaseWin::CloseBaseWin(){
  UnregisterClass("BASE_WIN",BaseWin::process_inst);
}

LRESULT CALLBACK BaseWin::bare_win_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
  // switch(msg){
    // case WM_DESTROY:{
    //   PostQuitMessage(0);
    // }break;
    // default:
    return DefWindowProc(hwnd,msg,wparam,lparam);
  // }
  // return 0;
}