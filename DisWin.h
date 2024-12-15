#ifndef _DISWIN_H_
#define _DISWIN_H_
#include<Windows.h>
#include<gdiplus.h>
#include<vector>
#include"macro.h"
#include"Chicken.h"
#include"set_template.h"
#include"Tree.h"
#include"Property.h"
#include"NPC.h"
#include"Wall.h"
#include"Stage.h"
// #define DEF_DIS_W 1200
// #define DEF_DIS_H 800

#include"BaseWin.h"
class Map;


class DisWin:public BaseWin{
public:
  friend DWORD diswin_render_proc(PVOID param);
  enum Diswin_State{
    DisWin_None,
    DisWin_Wait_Select_Cut, DisWin_On_Select_Cut,
    DisWin_Wait_Sel_Stage, DisWin_On_Sel_Stage,
    DisWin_Wait_Sel_Wall, DisWin_On_Sel_Wall
  };
protected:
  Diswin_State m_diswin_state = DisWin_None;
  // window size
  INT32 m_w,m_h,m_x,m_y;
  // INT32 diswin_timer_count =0;
  // gdi element;
  HBITMAP m_bmp= 0;
  HDC mem_dc = 0, bk_dc = 0;
  HMENU m_id;
  HANDLE render_proc_handle = 0;
  gs::Rect sel_rect{0,0,0,0};
  gs::Graphics* p_graphics;
  gs::Pen m_pen;
  gs::SolidBrush m_solidBrush;

  Map* m_pmap = NULL;

  NPC_Set* m_pnpc_set = NULL;
  Chi_Set* m_pchicken_set = NULL;

  Tree_Set* m_ptree_set = NULL;
  Wall_Set* m_pwall_set = NULL;
  Property_Set* m_pprop_set = NULL;
  Stage_Set* m_pstage_set = NULL;

  BOOL render_working;// on_sel = FALSE;
  INT32 time_elapse = 0, frame_cnt = 0,fps=0;

  static WNDCLASS diswin_share_class;
public:

  DisWin(INT32 x =0, INT32 y =0,INT32 w = DEF_DIS_W,INT32 h = DEF_DIS_H,
  HWND parent = 0,HMENU id = 0);
  ~DisWin();
  gs::Graphics& getGraphics(){
    return *p_graphics;
  }
  gs::SolidBrush& get_brush(){return m_solidBrush;}
  gs::Pen& get_pen(){return m_pen;}

  void caculate_fps(){
    fps = frame_cnt;
    time_elapse = frame_cnt = 0;
    SendMessage(GetParent(hwnd),WM_USER_FPS,static_cast<WPARAM>(fps),0);
  }

  void enable_render();
  void end_render();
  void copy_frome_dc(HDC p_mem_dc);
  void present();
  void render_group();
  void render_select();
  void clear();
  void show();

  // user inter function

  void update_sel_rect(INT32 cur_x,INT32 cur_y);
  void end_sel_rect(INT32 dest_x,INT32 dest_y);
  // cut sel
  void wait_sel_cut(){m_diswin_state = DisWin_Wait_Select_Cut;}
  void begin_sel_cut(INT32 b_x,INT32 b_y);
  // bool on_selecting_cut(){return m_diswin_state == DisWin_On_Select_Cut;} 
  // bool on_wait_selecting_cut(){return m_diswin_state == DisWin_Wait_Select_Cut;}

  // stage sel
  void wait_sel_stage(){m_diswin_state = DisWin_Wait_Sel_Stage;}
  void begin_sel_stage(INT32 b_x,INT32 b_y);
  // void end_sel_stage(INT32 dest_x,INT32 dest_y);



  // wall sel
  void sel_wall();
  void update_sel_block(INT32 mouse_x,INT32 mouse_y);
  void end_sel_wall();

  Diswin_State get_state(){return m_diswin_state;}


  // get
  HDC get_memdc(){
    return mem_dc;
  }
  HANDLE get_render_proc_handle(){return render_proc_handle;}
  // add
  void add_map(Map& pmap){  m_pmap = &pmap;}
  void add_bk_dc(HDC pbk_dc){bk_dc = pbk_dc;}
  void add_chicken(Chi_Set& pchicken){m_pchicken_set = &pchicken;}
  void add_npc_set(NPC_Set& pnpc){m_pnpc_set = &pnpc;}

  void add_tree_set(Tree_Set& tree_set){m_ptree_set = &tree_set;}
  void add_wall_set(Wall_Set& wall_set){m_pwall_set = &wall_set;}
  void add_prop_set(Property_Set& prop_set){m_pprop_set = & prop_set;}
  void add_stage_set(Stage_Set& stage_set){m_pstage_set = & stage_set;}


  static ULONG_PTR gdi_token;
  static gs::GdiplusStartupInput startup_input;
  static gs::Font* global_font;
  static gs::SolidBrush* global_font_brush;

  static void Init_diswin();
  static void Quit_diswin();
  static LRESULT def_diswin_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
};
DWORD diswin_render_proc(LPVOID param);
#endif // _DISWIN_H_