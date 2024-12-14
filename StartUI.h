#ifndef _STARTUI_H_
#define _STARTUI_H_

#include"BaseWin.h"
#define ID_UI_EXIT 0xff
#define ID_UI_NEW  (ID_UI_EXIT+ 0x1)
#define ID_UI_CONTINUE (ID_UI_NEW + 0x1)

// ui window layout
#define UI_W 400
#define UI_H 400
#define UI_BUTTON_W 100
#define UI_BUTTON_H 32
#define UI_BUTTON_X (UI_W/2 - UI_BUTTON_W)

#define UI_BUTTON_EXIT_Y 32
#define UI_BUTTON_NEW_Y (UI_BUTTON_EXIT_Y + UI_BUTTON_H)
#define UI_BUTTON_CONTINUE_Y (UI_BUTTON_NEW_Y + UI_BUTTON_H)


#define DIS_ERCT_W (UI_W - UI_BUTTON_W)
#define DIS_RECT_H (UI_H - UI_BUTTON_H)

class Game_Win;
class SelectWin;

class UI:public BaseWin{
protected:
  Game_Win* m_game_win = NULL;
  SelectWin* m_sel_win = NULL;
public:
  enum USER_SEL{
    EXIT,NEW,CONTINUE
  };
  BOOL user_play = FALSE;
protected:
public:
  // UI(USER_SEL* return_res);
  UI();
  ~UI();
  void new_game(INT32 m_tree,INT32 m_npc,INT32 m_chicken);
  void continue_last_game();
  void new_sel_win();

  // static constexpr INT32 UI_w = 400,UI_h = 400;
  static WNDCLASS ui_win_class ;
  static void Init_ui();
  static void Close_ui();
  static LRESULT UI_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
};

#endif // _WELCOMUI_H_