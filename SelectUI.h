#ifndef _SELECTUI_H_
#define _SELECTUI_H_
#include"BaseWin.h"

#define S_NPC_ID 0x1000
#define S_NPC_ID_1 (S_NPC_ID + (1<<1))
#define S_NPC_ID_2 (S_NPC_ID + (1<<2))
#define S_NPC_ID_3 (S_NPC_ID + (1<<3))
#define S_NPC_ID_4 (S_NPC_ID + (1<<4))
#define S_NPC_ID_5 (S_NPC_ID + (1<<5))

#define S_TREE_ID 0x2000
#define S_TREE_ID_7 (S_TREE_ID + (1<<7))
#define S_TREE_ID_8 (S_TREE_ID + (1<<8))
#define S_TREE_ID_350 (S_TREE_ID + 350)

#define S_CHICKEN_ID 0x3000
#define S_CHI_ID_2 (S_CHICKEN_ID + (1<<2))
#define S_CHI_ID_3 (S_CHICKEN_ID + (1<<3))

#define S_ID_OK 0x4000
// window layout
#define CLUE_H 30
#define CLUE_W 200
#define SEL_H 30
#define SEL_W 72

#define P_CLUE_X 10

#define P_CLUE_NPC_Y 20
#define P_SEL_NPC_Y 50
#define P_CLUE_TREE_Y 120
#define P_SEL_TREE_Y 150
#define P_CLUE_CHICKEN_Y 220
#define P_SEL_CHICKEN_Y 250

#define SEL_X1 P_CLUE_X
#define SEL_X2 (SEL_X1 + 80)
#define SEL_X3 (SEL_X2 + 80)
#define SEL_X4 (SEL_X3 + 80)
#define SEL_X5 (SEL_X4 + 80)

class UI;
class SelectWin final : public BaseWin{
  INT32 m_tree, m_npc,m_chicken;
  UI* parent_ui = NULL;
public:
  SelectWin(UI* parent_ui);
  ~SelectWin();
  void begin_new_game();
  static LRESULT Select_proc(HWND hwnd,UINT msg,WPARAM wparam, LPARAM lparam);
  static WNDCLASS sel_winclass;
  static void Init_SelWin();
  static void Close_SelWin();
};

#endif // _SELECTUI_H_