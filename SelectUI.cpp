// #include"all.h"
#include"SelectUI.h"
#include"Game_win.h"
#include"StartUI.h"

const CHAR sel_winclass_name[] = "SEL_WIN";
WNDCLASS SelectWin::sel_winclass = {0};


LRESULT CALLBACK SelectWin::Select_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
  switch(msg){


    case WM_SYSCOMMAND:{
      switch(wparam){
        case SC_CLOSE:{
          PostQuitMessage(0);
        }break;
        default:return DefWindowProc(hwnd,msg,wparam,lparam);
      }
    }break;

    case WM_COMMAND:{
      WORD id = LOWORD(wparam);
      SelectWin* psel_win = reinterpret_cast<SelectWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(psel_win);
      INT32 sel_num = static_cast<INT32>(id & 0xfff);
      switch(id & 0xf000){
        case S_NPC_ID:{
          psel_win->m_npc = sel_num;
        }break;
        case S_TREE_ID:{
          psel_win->m_tree = sel_num;
        }break;
        case S_CHICKEN_ID:{
          psel_win->m_chicken = sel_num;
        }break;

        case S_ID_OK:{
          psel_win->begin_new_game();
          OutputDebugStringW(L"begin new game sel");
        }break;
        default:
        break;
      }
    }break;

    case WM_CREATE:{
      CreateWindow("STATIC","select the number of npc",WS_CHILD | WS_VISIBLE | SS_SIMPLE,
      P_CLUE_X,P_CLUE_NPC_Y,CLUE_W,CLUE_H,hwnd,0,BaseWin::process_inst,NULL);

      CreateWindow("BUTTON","2",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT | WS_GROUP,
      SEL_X1,P_SEL_NPC_Y,SEL_W,SEL_H,hwnd,(HMENU)S_NPC_ID_1,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","4",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X2,P_SEL_NPC_Y,SEL_W,SEL_H,hwnd,(HMENU)S_NPC_ID_2,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","8",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X3,P_SEL_NPC_Y,SEL_W,SEL_H,hwnd,(HMENU)S_NPC_ID_3,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","16",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X4,P_SEL_NPC_Y,SEL_W,SEL_H,hwnd,(HMENU)S_NPC_ID_4,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","32",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X5,P_SEL_NPC_Y,SEL_W,SEL_H,hwnd,(HMENU)S_NPC_ID_5,BaseWin::process_inst,NULL);


      CreateWindow("STATIC","select the number of tree",WS_CHILD | WS_VISIBLE | SS_SIMPLE,
      P_CLUE_X,P_CLUE_TREE_Y,CLUE_W,CLUE_H,hwnd,0,BaseWin::process_inst,NULL);

      CreateWindow("BUTTON","128",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT | WS_GROUP,
      SEL_X1,P_SEL_TREE_Y,SEL_W,SEL_H,hwnd,(HMENU)S_TREE_ID_7,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","256",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X2,P_SEL_TREE_Y,SEL_W,SEL_H,hwnd,(HMENU)S_TREE_ID_8,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","350",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X3,P_SEL_TREE_Y,SEL_W,SEL_H,hwnd,(HMENU)S_TREE_ID_350,BaseWin::process_inst,NULL);

      CreateWindow("STATIC","select the number of chicken",WS_CHILD | WS_VISIBLE | SS_SIMPLE,
      P_CLUE_X,P_CLUE_CHICKEN_Y,CLUE_W,CLUE_H,hwnd,0,BaseWin::process_inst,NULL);

      CreateWindow("BUTTON","4",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT | WS_GROUP,
      SEL_X1,P_SEL_CHICKEN_Y,SEL_W,SEL_H,hwnd,(HMENU)S_CHI_ID_2,BaseWin::process_inst,NULL);
      CreateWindow("BUTTON","8",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | SS_LEFT,
      SEL_X2,P_SEL_CHICKEN_Y,SEL_W,SEL_H,hwnd,(HMENU)S_CHI_ID_3,BaseWin::process_inst,NULL);

      CreateWindow("BUTTON","OK",WS_CHILD | WS_VISIBLE,P_CLUE_X,P_SEL_CHICKEN_Y + 40,80,30,hwnd,
      (HMENU)S_ID_OK,BaseWin::process_inst,NULL);

      CREATESTRUCT* p_struc = reinterpret_cast<CREATESTRUCT*>(lparam);
      assert(p_struc->lpCreateParams);
      SelectWin* pselwin = reinterpret_cast<SelectWin*>(p_struc->lpCreateParams);
      SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>((pselwin)));
    }break;

    default:
    return DefWindowProc(hwnd,msg,wparam,lparam);
  }
  return 0;
}

void SelectWin::Init_SelWin(){
  sel_winclass.lpfnWndProc = Select_proc;
  sel_winclass.lpszClassName = sel_winclass_name;
  sel_winclass.hCursor = static_cast<HCURSOR>(LoadCursor(NULL,IDC_ARROW));
  sel_winclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
  sel_winclass.hInstance = BaseWin::process_inst;

  RegisterClass(&sel_winclass);
}

void SelectWin::Close_SelWin(){
  UnregisterClass(sel_winclass_name,BaseWin::process_inst);
}


SelectWin::SelectWin(UI* p_ui): 
parent_ui(p_ui),m_npc(DEF_NPC_NUM),m_tree(DEF_TREE_NUM),m_chicken(DEF_CHICKEN_NUM){

  hwnd = CreateWindow(sel_winclass_name,"select",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,
  CW_USEDEFAULT,500,400,NULL,0,BaseWin::process_inst,this);
  assert(hwnd);
  ShowWindow(hwnd,BaseWin::cmdShow);
}

SelectWin::~SelectWin(){
  // DestroyWindow(hwnd);hwnd = 0;
  // parent_ui->new_game(m_npc,m_tree,m_chicken);
}

void SelectWin::begin_new_game(){
  assert(parent_ui);
  DestroyWindow(hwnd);hwnd = 0;
  parent_ui->new_game(m_tree,m_npc,m_chicken);
}