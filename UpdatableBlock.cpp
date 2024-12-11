#include<windows.h>
#include"macro.h"
#ifdef DEBUG
#include<cstdio>
#endif

#include"Updatable_Block.h"
// #include"macro.h"
constexpr INT32 move_velocity = 2;
extern const WCHAR* global_num_string = L"0123456789";
extern const INT32 dires[4][2]={{0,1},{0,-1},{1,0},{-1,0}};
extern const INT32 disrect_move_dires[4][2] = {
  {move_velocity,0},{-move_velocity},{0,move_velocity},{0,-move_velocity}
};

INT32 UpdatableBlock::check_index = -1;

gs::Font* UpdatableBlock::global_font_16 = NULL, * UpdatableBlock::global_font_8 = NULL;

gs::SolidBrush* UpdatableBlock::global_num_brush = NULL;
gs::Pen* UpdatableBlock::global_pen = NULL;

void UpdatableBlock::Updatable_Block_Init(){
  global_pen = new gs::Pen(gs::Color::AliceBlue);
  assert(global_pen->GetLastStatus() == gs::Ok);
  global_font_16 = new gs::Font(FONT_CONSOLA,16);
  assert(global_font_16->GetLastStatus() == gs::Ok);
  global_font_8 = new gs::Font(FONT_CONSOLA,8);
  assert(global_font_8->GetLastStatus() == gs::Ok);
  global_num_brush = new gs::SolidBrush(gs::Color::SandyBrown);
  assert(global_num_brush->GetLastStatus() == gs::Ok);
}

void UpdatableBlock::Updatable_Block_Free(){
  delete global_pen; global_pen = NULL;
  delete global_font_16; global_font_16= NULL;
  delete global_font_8; global_font_8 = NULL;
  delete global_num_brush ; global_num_brush = NULL;
}

void UpdatableBlock::write_num_to_wchar(WCHAR* ch,INT32 num){
  LOG("begin num: %d",num);
  for(int i=2;i>=0;--i){
    ch[i] = L'0' + num %10;
    num /= 10;
  }
  LOG("error in updateblock num: %d",num);
  assert(num == 0);
}

UpdatableBlock::~UpdatableBlock(){
  // if(check_index == -1){
  //   check_index = m_block_state;
  // }else{
  //   if(check_index != m_block_state){
  //     LOG("error in exit %d check: %d",m_block_state ,check_index);
  //     MessageBox(NULL,"error in exit","error",MB_OK);
  //   }
  //   switch(m_block_state){
  //     case DisBlock::Block_updated:
  //     OutputDebugString("exit updated");
  //     break;
  //     case DisBlock::Block_Drawn:
  //     OutputDebugString("exit drawn");
  //     break;
  //     default:
  //     OutputDebugString("exit miss");
  //   }
  //   assert(check_index == m_block_state);
  // }
}