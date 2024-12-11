#include"all.h"
#include"constv.h"
#include"Chicken.h"
#include"Game_win.h"
#include"StartUI.h"

// #include<cassert>
HANDLE g_mutex;
// extern HANDLE g_threads[MAXN_THREAD];
// extern DWORD g_thread_cnt;

void* p_log_test = NULL;
int WINAPI WinMain(
  _In_ HINSTANCE inst,_In_opt_ HINSTANCE pre,
  _In_ LPSTR cmdLine, _In_ int cmdshow
){
  Init_debug;
  g_mutex = CreateMutexA(NULL,FALSE,NULL);  assert(g_mutex);
  BaseWin::Init(inst,cmdshow,g_mutex);
  UI::Init_ui();

  MSG msg = {0};
  UI* ui_win = new UI();

  while(GetMessage(&msg,NULL,0,0)){
    TranslateMessage(&msg);
    DispatchMessageA(&msg);

  }

  delete ui_win; ui_win = NULL;
  UI::Close_ui();
  OutputDebugStringA("close the ui");

  

  BaseWin::CloseBaseWin();
  
  CloseHandle(g_mutex);
  OutputDebugStringA("quit");
  Close_debug;
  OutputDebugStringA("close the consola");
  assert(msg.wParam == 0);
  return static_cast<int>(msg.wParam);
}
