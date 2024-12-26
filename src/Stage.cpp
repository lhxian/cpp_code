#include"Stage.h"
#include"Map.h"

const WCHAR* stage_image_path = {
  RES_PATH L"stage.png"
};
gs::SolidBrush* Stage::stage_font_brush = NULL;
gs::Pen* Stage::stage_rect_pen = NULL;

void Stage::Init(){
  stage_rect_pen = new gs::Pen(gs::Color::Purple);
  assert(stage_rect_pen->GetLastStatus() == gs::Ok);

  stage_font_brush = new gs::SolidBrush(gs::Color::Black);
  assert(stage_font_brush->GetLastStatus() == gs::Ok);

}

void Stage::Free(){
  // delete stage_image; stage_image = NULL;
  delete stage_rect_pen; stage_rect_pen = NULL;
  delete stage_font_brush; stage_font_brush = NULL;
}


Stage::Stage(INT32 prow,INT32 pcol):
UpdatableBlock(prow, pcol),used(0)
{
  num_display_point.X = static_cast<gs::REAL>(disrect.X);
  num_display_point.Y = static_cast<gs::REAL>(disrect.Y);
  no_collect_dis_point.X = num_display_point.X;
  no_collect_dis_point.Y = num_display_point.Y + stage_num_deviate;
  for(int i=0;i<4;++i)  dis_used[i]= L'0';
}

// discard
void Stage::UpdateOnce(Map& game_map){
  // INT32 sleep_timers = 0;
  // while(m_block_state != DisBlock::Block_Drawn){
  //   Sleep(2);
  //   // ++sleep_timers;
  //   // if(sleep_timers >= UpdatableBlock::MAX_SLEEP_TIMES) return;
  //   LOG("stage update sleep");
  // }
  // TODO
  // LOG("stage row : %d col : %d used : %d",cur_row,cur_col,used);

  // m_block_state = DisBlock::Block_updated;
}

void Stage::DisplayOnce(gs::Graphics& pg){
  // INT32 sleep_times = 0;
  // while(m_block_state != DisBlock::Block_updated) {
  //   Sleep(2);
  //   // ++sleep_times;
  //   // if(sleep_times >= UpdatableBlock::MAX_SLEEP_TIMES) return;
  //   LOG("stage display sleep");
  // }

  // auto it = pg.DrawImage(Stage::stage_image,disrect,0,0,Stage::stage_image_w,Stage::stage_image_h,
  // gs::UnitPixel);
  // assert(it == gs::Ok);
  auto it = pg.DrawRectangle(stage_rect_pen,disrect);
  assert(it == gs::Ok);
  it = pg.DrawString(dis_used,NUM_LEN,UpdatableBlock::global_font_8,num_display_point,
  Stage::stage_font_brush);
  assert(it == gs::Ok);
  if(no_collect> 0){
    auto it = pg.DrawString(dis_no_collect,NUM_LEN,UpdatableBlock::global_font_8,
    no_collect_dis_point,Stage::stage_font_brush);
    assert(it == gs::Ok);
  }

  // m_block_state = DisBlock::Block_Drawn;
}


void Stage::add_stock_include_no_collect(INT32 augmentation){
  if(used + augmentation > DEF_STAGE_CAPACITY){
    no_collect += (augmentation - (DEF_STAGE_CAPACITY - used));
    used  = DEF_STAGE_CAPACITY;
  }else{
    used += augmentation;
  }
  update_wchar();
}
// void Stage::remove_stock(INT32 dec){
//   assert(dec <= used);
//   used -= dec;
//   UpdatableBlock::write_num_to_wchar(dis_used,used);
// }

// void Stage::clear(){
//   used = 0;
// }

void Stage::collect(){
  assert(used < DEF_STAGE_CAPACITY);
  if(used + no_collect <= DEF_STAGE_CAPACITY){
    used += no_collect;
    no_collect = 0;
  }else{
    no_collect -= (DEF_STAGE_CAPACITY - used);
    used = DEF_STAGE_CAPACITY;
  }
  update_wchar();
  
}

INT32 Stage::moveto_used(){
  assert(used > 0);
  INT32 ret = used;
  used = 0;
  LOG("move to used");
  // assert(0);
  update_wchar();
  return ret;
}
void Stage::update_wchar(){
  LOG("stage used: %d no collect: %d",used,no_collect);
  UpdatableBlock::write_num_to_wchar(dis_used,used);
  UpdatableBlock::write_num_to_wchar(dis_no_collect,no_collect);
}

template <>
void Stage_Set::add_set_to_map(Map& game_map){
  INT32 prow =0, pcol = 0;
  TiType_Line* tile_type = game_map.get_map_type_table();
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  assert(tile_type && ptr_table);
  LOG("begin add stage to map %lld",set_vector.size());
  for(auto& it: set_vector){
    prow = it.get_cur_row(); pcol = it.get_cur_col();
    assert(tile_type[prow][pcol][1] == TileType::T_grass);
    assert(ptr_table[prow][pcol][1] == NULL);
    LOG("stage x,y %d %d",prow,pcol);

    tile_type[prow][pcol][1] = TileType::T_stage_area;
    ptr_table[prow][pcol][1] = &it;
  }
}
