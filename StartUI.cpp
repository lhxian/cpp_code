// #include"all.h"
#include"StartUI.h"
#include"Init.h"
#include"Game_win.h"
#include"SelectUI.h"


const CHAR* ui_type[]={
  "exit","new","continue"
};
constexpr CHAR UI_name[]="UI";
const WCHAR* last_record_path = {
  AR_PATH L"map_type_table"
};

WNDCLASS UI::ui_win_class = {0};

void UI::Init_ui(){
  ui_win_class.lpszClassName = UI_name;
  ui_win_class.hCursor = (HCURSOR)LoadCursor(NULL,IDC_ARROW);
  ui_win_class.hInstance = BaseWin::process_inst;
  ui_win_class.lpfnWndProc = UI_proc;
  ui_win_class.hbrBackground = static_cast<HBRUSH>(::GetStockObject(GRAY_BRUSH));
  RegisterClass(&ui_win_class);
}
void UI::Close_ui(){
  UnregisterClass(UI_name,BaseWin::process_inst);
}
LRESULT CALLBACK UI::UI_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
  switch(msg){

    case WM_COMMAND:{
      switch(LOWORD(wparam)){
        case ID_UI_EXIT: PostQuitMessage(0);break;

        case ID_UI_NEW:{
          if(MessageBox(NULL,"this operation will purge you data !!!","info",
          MB_OKCANCEL) == IDCANCEL) break;
          
        }
        case ID_UI_CONTINUE:{
          UI* app_ui = reinterpret_cast<UI*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
          assert(app_ui);
          if(wparam == ID_UI_NEW) app_ui->new_sel_win();
          else app_ui->continue_last_game();
          app_ui->user_play = TRUE;
        }break;

        default:assert(0);
      }
    }break;

    case WM_CREATE:{
      CREATESTRUCT* p_struc = reinterpret_cast<CREATESTRUCT*>(lparam);
      SetWindowLongPtr(hwnd,GWLP_USERDATA,
      reinterpret_cast<LONG_PTR>(p_struc->lpCreateParams));
    }break;

    case WM_SYSCOMMAND:{
      switch(wparam){
        case SC_CLOSE:{
          PostQuitMessage(0);
        }break;
        default:
        return DefWindowProc(hwnd,msg,wparam,lparam);
      }
    }break;
    default:
    return DefWindowProc(hwnd,msg,wparam,lparam);
  }
  return 0;
}






UI::UI()
// res(return_res)
{
  hwnd = CreateWindow(UI_name,NULL,WS_OVERLAPPEDWINDOW,
  CW_USEDEFAULT,CW_USEDEFAULT,UI_W,UI_H,NULL,0,BaseWin::process_inst,this);
  assert(hwnd);

  CreateWindow("Button",ui_type[0],WS_CHILD | WS_VISIBLE,UI_BUTTON_X,UI_BUTTON_EXIT_Y,
  UI_BUTTON_W,UI_BUTTON_H,hwnd,(HMENU)ID_UI_EXIT,BaseWin::process_inst,0);

  CreateWindow("Button",ui_type[1],WS_CHILD | WS_VISIBLE,UI_BUTTON_X,UI_BUTTON_NEW_Y,
  UI_BUTTON_W,UI_BUTTON_H,hwnd,(HMENU)ID_UI_NEW,BaseWin::process_inst,0);

  CreateWindow("Button",ui_type[2],WS_CHILD | WS_VISIBLE,UI_BUTTON_X,UI_BUTTON_CONTINUE_Y,
  UI_BUTTON_W,UI_BUTTON_H,hwnd,(HMENU)ID_UI_CONTINUE,BaseWin::process_inst,0);

  ShowWindow(hwnd,BaseWin::cmdShow);
}

UI::~UI(){
  OutputDebugString("begin deconstruc ui");
  if(hwnd)  {
    DestroyWindow(hwnd);
    hwnd = 0;
  }
  if(user_play == FALSE) {
    return;
    OutputDebugString("user no play!!!");
  }
  if(m_game_win){
    OutputDebugStringA("delete the game win");
    delete m_game_win; m_game_win = NULL;
    Close_Game();
  }
}

void UI::new_game(INT32 m_npc,INT32 m_tree,INT32 m_chicken){
  // TODO
  
  if(hwnd) DestroyWindow(hwnd);hwnd = 0;
  OutputDebugString("new a game win!!!");
  Init_Game();
  // m_sel_win = new SelectWin(this);

  m_game_win = new Game_Win(false,m_npc,m_tree,m_chicken);
}
void UI::continue_last_game(){
  // TODO
  Init_Game();
  if(GetFileAttributesW(last_record_path) == INVALID_FILE_ATTRIBUTES){
    MessageBox(NULL,"no game record exists!!!","info",MB_OK);
    return ;
  }
  DestroyWindow(hwnd); hwnd =0;
  m_game_win = new Game_Win(true);
}

void UI::new_sel_win(){
  SelectWin::Init_SelWin();
  DestroyWindow(hwnd); hwnd = 0;
  m_sel_win = new SelectWin(this);
}