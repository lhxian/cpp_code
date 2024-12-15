// #include<iostream>
#include"Map.h"
#include"Tree.h"
#include"Game_win.h"
#include"NPC.h"
#include"Wall.h"
#include"Stage.h"
#include"Property.h"

WNDCLASS Game_Win::game_win_class = {0};
const char* game_win_name = "GAME_WIN";


const WCHAR* button_image_pathes[]={
  RES_PATH L"cut_sel.png",
  RES_PATH L"cut_canc.png",
  RES_PATH L"carry.png"
};

const char* ar_file_pathes[]={
  AR_PATH "npc",
  AR_PATH "chickens",
  AR_PATH "tree",
  AR_PATH "wall",
  AR_PATH "stage",
  AR_PATH "property",
  AR_PATH "map_type_table"
};
enum ar_file_type{
  ar_npc, ar_chi, ar_tree, ar_wall, ar_stage, ar_property, ar_map_type_table,
  ar_cnt
};
enum Button_img_type{
  bi_cut_sel, bi_cut_cancel,
  bi_carry,
  bi_cnt
};

LRESULT CALLBACK Game_Win::game_win_proc(HWND hwnd ,UINT msg,WPARAM wparam,LPARAM lparam){
  switch(msg){
    case WM_COMMAND:{
      Game_Win * p_game_win = reinterpret_cast<Game_Win*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_game_win);
      HWND diswin = p_game_win->p_diswin->GetHwnd();
      assert(diswin);
      switch(LOWORD(wparam)){
        case ID_CUT_SEL:OutputDebugString("click the cut sel!!!");
        LOG("click the cut button");
        // SendMessage(p_game_win->p_diswin->GetHwnd(),WM_USER_CUT_TREE,0,0);
        SendMessage(diswin,WM_USER_CUT_TREE,0,0);
        break;
        // case ID_CUT_CANL:OutputDebugString("click the cut cancel!!!");
        // LOG("click the cut cancel button");
        // SendMessage(diswin,WM_USER_CUT_CANSEL,0,0);
        // break;

        case ID_STAGE_SEL:
        SendMessage(diswin,WM_USER_STAGE,0,0);
        break;
        case ID_WALL_SEL:
        SendMessage(diswin,WM_USER_WALL,0,0);
        break;

        
        default:
        LOG("game win proc command exception: %lld",wparam);
        assert(0);
      }
    }break;

    case WM_USER_FPS:{
      HWND fps_win = GetDlgItem(hwnd,ID_WINDOW_FPS);
      assert(fps_win);
      Game_Win* p_game_win =reinterpret_cast<Game_Win*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_game_win);
      p_game_win->translate_fps_to_char(wparam);
      SetWindowText(fps_win,p_game_win->fps_ch);
    }break;

    case WM_CREATE:{
      assert(lparam);
      CREATESTRUCT* p_struc = reinterpret_cast<CREATESTRUCT*>(lparam);
      SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(p_struc->lpCreateParams));
      OutputDebugStringA("set the userdata");
    }break;
    case WM_DESTROY:{
      // Game_Win* pgame = reinterpret_cast<Game_Win*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      // assert(pgame);
      // LOG("%s",pgame->ch);
      OutputDebugStringA("game win destroy!!!");
      PostQuitMessage(0);
    }break;

    default:
    return DefWindowProcA(hwnd,msg,wparam,lparam);
  }
  return 0;
}

void Game_Win::Init_Game_win(){
  OutputDebugStringA("begin init game_win");
  game_win_class.hCursor = static_cast<HCURSOR>(LoadCursorA(NULL,IDC_ARROW));
  game_win_class.lpszClassName = game_win_name;
  game_win_class.lpfnWndProc= game_win_proc;
  game_win_class.hInstance = BaseWin::process_inst;
  game_win_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));

  auto it = RegisterClassA(&game_win_class);
  assert(it);
  OutputDebugStringA("has init the game win!!!");
}
void Game_Win::Close_Game_win(){
  UnregisterClassA("GAME_WIN",BaseWin::process_inst);
}

// constructor

Game_Win::Game_Win(bool load_from_file,INT32 tree_cnt , INT32 npc_cnt ,INT32 chi_cnt){

  create_interface();
  ShowWindow(hwnd,BaseWin::cmdShow);
  OutputDebugStringA("show the game win!!!");


  // elemen and npc
  game_map = new Map(DEF_DIS_W,DEF_DIS_H,p_diswin->get_memdc());

  m_pnpc_set = new NPC_Set;
  m_pchi_set = new Chi_Set;
  
  m_ptree_set = new Tree_Set;
  m_pstage_set = new Stage_Set;
  m_pwall_set = new Wall_Set;
  m_pprop_set = new Property_Set;

  if(load_from_file){
    m_pnpc_set->load_from_file(ar_file_pathes[ar_npc]);
    m_pchi_set->load_from_file(ar_file_pathes[ar_chi]);

    m_ptree_set->load_from_file(ar_file_pathes[ar_tree]);
    m_pprop_set->load_from_file(ar_file_pathes[ar_property]);
    m_pwall_set->load_from_file(ar_file_pathes[ar_wall]);
    m_pstage_set->load_from_file(ar_file_pathes[ar_stage]);
  }else{
    OutputDebugString("begin gen new map");
    gen_new_map(tree_cnt,npc_cnt,chi_cnt);
  }

  prepare_resource();

}

void Game_Win::gen_new_map(INT32 tree_cnt,INT32 npc_cnt,INT32 chicken_cnt){
  constexpr INT32 len =ABSTRACT_MAP_H_C * ABSTRACT_MAP_W_C; 
  assert(tree_cnt + npc_cnt + chicken_cnt<= len);
  INT32 * buf_rand = new INT32[len];
  for(int i =0;i< len;++i) buf_rand[i] = i;
  srand(static_cast<int>(time(0)));
  for(int i = len-1;i>0;--i){
    int index = rand() % i;
    std::swap(buf_rand[index],buf_rand[i]);
  }
  INT32 prow =0, pcol =0;
  int i =0;
  for(;i< tree_cnt;++i){
    prow = buf_rand[i] / ABSTRACT_MAP_W_C + 1;
    pcol = buf_rand[i] % ABSTRACT_MAP_W_C + 1;
    m_ptree_set->push(Tree{prow,pcol});
  }
  for(;i< tree_cnt + npc_cnt;++i){
    prow = buf_rand[i] / ABSTRACT_MAP_W_C + 1;
    pcol = buf_rand[i] % ABSTRACT_MAP_W_C + 1;
    m_pnpc_set->push(NPC{NPC::Up,prow,pcol});
  }
  for(;i < tree_cnt + npc_cnt + chicken_cnt;++i){
    prow = buf_rand[i] / ABSTRACT_MAP_W_C + 1;
    pcol = buf_rand[i] % ABSTRACT_MAP_W_C + 1;
    m_pchi_set->push(Chicken{prow,pcol});
  }
  delete[] buf_rand;
}

Game_Win::~Game_Win(){
  OutputDebugStringA("begin deconstruc game win");
  assert(BaseWin::b_mutex);
  WaitForSingleObject(BaseWin::b_mutex,INFINITE);
  p_diswin->end_render();
  game_map->end_update();
  ReleaseMutex(BaseWin::b_mutex);
  assert(thread_handle_cnt == 2);
  WaitForMultipleObjects(thread_handle_cnt,proc_handles,TRUE,INFINITE);
  // HANDLE wait_handle = p_diswin->get_render_proc_handle();
  // if(wait_handle){
  //   WaitForSingleObject(wait_handle, INFINITE);
  // }
  // wait_handle = game_map->get_update_proc_handle();
  // if(wait_handle){
  //   WaitForSingleObject(wait_handle, INFINITE);
  // }
  // buffer
  Sleep(128);
  OutputDebugStringA("all sub thread exit!!!");

  // begin save the data of the game

  m_pnpc_set->write_to_file(ar_file_pathes[ar_npc]);

  m_ptree_set->write_to_file(ar_file_pathes[ar_tree]);
  m_pwall_set->write_to_file(ar_file_pathes[ar_wall]);
  m_pprop_set->write_to_file(ar_file_pathes[ar_property]);
  m_pstage_set->write_to_file(ar_file_pathes[ar_stage]);
  OutputDebugStringW(L"begin write the chicken data");
  m_pchi_set->write_to_file(ar_file_pathes[ar_chi]);

  game_map->write_to_file(ar_file_pathes[ar_map_type_table]);
  
  OutputDebugStringA("begin delete ptr");
  delete game_map; game_map = NULL;

  delete p_diswin; p_diswin = NULL;
  delete m_pchi_set; m_pchi_set = NULL;
  delete m_pnpc_set; m_pnpc_set = NULL;

  delete m_pwall_set; m_pwall_set= NULL;
  delete m_pstage_set; m_pstage_set = NULL;
  delete m_pprop_set ; m_pprop_set = NULL;
  delete m_ptree_set; m_ptree_set = NULL;

  DestroyWindow(hwnd);hwnd = 0;
}

bool Game_Win::Load_from_file(){
  // std::ifstream ifs(arch_path[0]);
  // assert(ifs.is_open());

  return true;
}

void Game_Win::prepare_resource(){
  p_diswin->add_bk_dc(game_map->get_dc());
  p_diswin->add_npc_set(*m_pnpc_set);
  p_diswin->add_chicken(*m_pchi_set);
  p_diswin->add_map(*game_map);

  p_diswin->add_tree_set(*m_ptree_set);
  p_diswin->add_stage_set(*m_pstage_set);
  p_diswin->add_wall_set(*m_pwall_set);
  p_diswin->add_prop_set(*m_pprop_set);


  game_map->add_npc_set(m_pnpc_set);
  game_map->add_chi_set(m_pchi_set);

  game_map->add_stage_set(m_pstage_set);
  OutputDebugString("add tree set to map");
  game_map->add_tree_set(m_ptree_set);
  game_map->add_wall_set(m_pwall_set);
  game_map->add_prop_set(m_pprop_set);


  game_map->show_map();

  game_map->enable_update();
  p_diswin->enable_render();

  add_thread_rec(game_map->get_update_proc_handle());
  add_thread_rec(p_diswin->get_render_proc_handle());


}
void Game_Win::create_interface(){
  OutputDebugStringA("begin game construtor!!");
  hwnd = CreateWindow(game_win_name,"game",WS_OVERLAPPEDWINDOW,
  0,0,DEF_GAME_W + 48,DEF_GAME_H + 64,0,0,BaseWin::process_inst,this);
  OutputDebugStringA("create game win");
  assert(hwnd);
  OutputDebugStringA("create the main window!!!");
  // create the fps display
  CreateWindow("STATIC","fps: ",WS_CHILD | WS_VISIBLE | SS_LEFT,B_POS_X,B_POS_WALL_Y + 128,BUTTON_W,
  BUTTON_H,hwnd,(HMENU)ID_WINDOW_FPS,BaseWin::process_inst,NULL);
  // create butttons
  //
  CreateWindow("Button","cut",WS_CHILD | WS_VISIBLE,B_POS_X,B_POS_CUT_SEL_Y,
  BUTTON_W,BUTTON_H,hwnd,(HMENU)ID_CUT_SEL,BaseWin::process_inst,0);

  // CreateWindow("Button","cancel",WS_CHILD | WS_VISIBLE,B_POS_X,B_POS_CUT_CANC_Y,
  // BUTTON_W,BUTTON_H,hwnd,(HMENU)ID_CUT_CANL,BaseWin::process_inst,0);

  CreateWindow("Button","stage",WS_CHILD | WS_VISIBLE,B_POS_X,B_POS_STAGE_Y,
  BUTTON_W,BUTTON_H,hwnd,(HMENU)ID_STAGE_SEL,BaseWin::process_inst,0);
  CreateWindow("Button","wall",WS_CHILD | WS_VISIBLE,B_POS_X,B_POS_WALL_Y,
  BUTTON_W,BUTTON_H,hwnd,(HMENU)ID_WALL_SEL,BaseWin::process_inst,0);
  p_diswin = new DisWin(0,0,DEF_DIS_W,DEF_DIS_H,this->hwnd,(HMENU)ID_DIS_WIN);

}

void Game_Win::test(){
  OutputDebugStringA("begin test");
  LOG("TEST");
  for(int i =1;i<=ABSTRACT_MAP_H_C;i+=2){
    for(int j =1;j<=ABSTRACT_MAP_W_C;j+=2){
      m_ptree_set->push(Tree{i,j});
    }
  }
  m_pnpc_set->push(NPC{NPC::Up,1,4});
  m_pnpc_set->push(NPC{NPC::Up,2,4});
  m_pnpc_set->push(NPC{NPC::Up,2,5});
  m_pnpc_set->push(NPC{NPC::Up,2,6});
  m_pnpc_set->push(NPC{NPC::Up,2,7});

  m_pchi_set->push(Chicken{2,7});
  m_pchi_set->push(Chicken{2,7});
  m_pchi_set->push(Chicken{2,7});


  OutputDebugStringA("test end");

}

void Game_Win::add_thread_rec(HANDLE proc_handle){
  proc_handles[thread_handle_cnt++] = proc_handle;
}

void Game_Win::translate_fps_to_char(WPARAM pfps){
  int cnt = 0;
  while(pfps){
    sta[++cnt] = pfps % 10 + '0';
    pfps /= 10;
  }
  char* fill_ptr = fps_ch + 5;
  while(cnt){
    *fill_ptr++ = sta[cnt--];
  }
  *fill_ptr = '\0';
}