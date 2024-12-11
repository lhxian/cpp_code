#include"all.h"
#include"Map.h"
#include<queue>
#include<vector>
#include"Chicken.h"
#include"constv.h"
#include"Tree.h"
#include"Game_win.h"


// Init the image resource;
void Init_Image(){
  //TODO
  
  Map::Init();
  Tree::Init();
  NPC::Init();
  Chicken::chi_Init();
  Wall::Init();
  OutputDebugStringA("has init the image");

}
void Free_Image(){
  Wall::Free();
  Chicken::chi_Free();
  NPC::Free();
  Tree::Free();
  Map::Free();
  OutputDebugStringA("free the image");
}
void Init_Game(){
  // BaseWin::Init(p_inst,cmdshow,p_mutex);
  DisWin::Init_diswin();
  UpdatableBlock::Updatable_Block_Init();
  // ButtonWin::InitButtonwin();
  Property::Init();
  Stage::Init();
  Game_Win::Init_Game_win();
  OutputDebugStringA("begin init the image!!!");
  Init_Image();
  OutputDebugStringA("init the game win!!!");
}

void Close_Game(){
  // TODO

  OutputDebugStringA("begin close all");
  Property::Free();
  Stage::Free();
  Free_Image();

  
  Game_Win::Close_Game_win();
  UpdatableBlock::Updatable_Block_Free();
  // ButtonWin::closeButtonwin();
  DisWin::Quit_diswin();
}

