#ifndef _GAME_WIN_H_
#define _GAME_WIN_H_

#include"all.h"
#include"Chicken.h"
#define MAX_HANDLE_CNT 4
#define DEF_TREE_NUM 256
#define DEF_NPC_NUM 32
#define DEF_CHICKEN_NUM 4

#define ID_WINDOW_FPS 0xf9

class Game_Win:public BaseWin{
  // char ch[32];
  DisWin* p_diswin = NULL;

  Map* game_map = NULL;
  NPC_Set* m_pnpc_set = NULL;
  Chi_Set* m_pchi_set = NULL;

  Tree_Set* m_ptree_set = NULL;
  Stage_Set* m_pstage_set = NULL;
  Wall_Set* m_pwall_set = NULL;
  Property_Set* m_pprop_set = NULL;

  HANDLE proc_handles[MAX_HANDLE_CNT];
  DWORD thread_handle_cnt = 0;
  
  CHAR fps_ch[32]="fps: ",sta[16];



public:
  Game_Win() = delete;
  Game_Win(bool load_from_file = false,INT32 tree_cnt = DEF_TREE_NUM, INT32 npc_cnt = DEF_NPC_NUM,
  INT32 chi_cnt = DEF_CHICKEN_NUM);
  ~Game_Win();
  void gen_new_map(INT32 tree_cnt , INT32 ncp_cnt ,INT32 chicken_cnt);
  bool Load_from_file();
  void create_interface();
  void prepare_resource();
  void test();
  void add_thread_rec(HANDLE thread_handle = 0);

  void translate_fps_to_char(WPARAM p_fps);


  static void Init_Game_win();
  static void Close_Game_win();

  static WNDCLASS game_win_class;
  static LRESULT game_win_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
  static constexpr INT32 game_button_cnt = 3;
};

#endif // _GAME_WIN_H_