#include"all.h"
#include"constv.h"
#include"Map.h"
#include"NPC.h"
#include"Wall.h"

const WCHAR* wall_img_path = {
  RES_PATH L"wall.png"
};
gs::Image* Wall::wall_image = NULL;
gs::Pen* Wall::wall_select_pen = NULL, * Wall::wall_to_pen = NULL;
gs::SolidBrush* Wall::wall_brush = NULL;

void Wall::Init(){
  wall_select_pen = new gs::Pen(gs::Color::Chocolate,2.f);
  wall_to_pen = new gs::Pen(gs::Color::AliceBlue);

  wall_brush = new gs::SolidBrush(gs::Color::Azure);
  assert(wall_brush->GetLastStatus() == gs::Ok);

  wall_image = new gs::Image(wall_img_path);
  assert(wall_image->GetLastStatus() == gs::Ok);
}
void Wall::Free(){
  delete wall_select_pen; wall_select_pen = NULL;
  delete wall_to_pen; wall_to_pen = NULL;
  delete wall_brush; wall_brush = NULL;
  delete wall_image;wall_image = NULL;
}


Wall::Wall(INT32 prow,INT32 pcol):
UpdatableBlock(prow, pcol)
{
  // TODO
  num_display_point.X = static_cast<gs::REAL>(disrect.X );
  num_display_point.Y = static_cast<gs::REAL>(disrect.Y);
}
void Wall::add_material(INT32 augmentation){
  assert(has_offer_val < WALL_CONSUME);
  has_offer_val += augmentation;
  UpdatableBlock::write_num_to_wchar(num_char,has_offer_val);
  m_state = TO_BUILD;
  // if(has_offer_val >= WALL_CONSUME) begin_build(game_map);
}
void Wall::begin_build(Map& game_map){
  m_state = ON_BUILDING;
  num_display_point.X = static_cast<gs::REAL>(disrect.X + TILE_W / 4);
  num_display_point.Y = static_cast<gs::REAL>(disrect.Y - TILE_H / 2);
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(ptr_table[cur_row][cur_col][1] == this);
  assert(tile_type[cur_row][cur_col][1] == TileType::T_to_build_wall);
  tile_type[cur_row][cur_col][1] = TileType::T_on_building_wall;

}

void Wall::end_build(Map& game_map){
  OutputDebugString("end buid a wall");

  assert(num_timer == 0);
  m_state = BUILT;
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  // assert(link_target_row == cur_row || link_target_col == cur_col);
  NPC* target_npc = static_cast<NPC*>(ptr_table[cur_row][cur_col][0]);
  assert(target_npc);
  target_npc->end_build(game_map);

  assert(tile_type[cur_row][cur_col][1] == TileType::T_on_building_wall);
  tile_type[cur_row][cur_col][1] = TileType::T_wall;
  link_target_row = link_target_col = 0;
}

void Wall::UpdateOnce(Map& game_map){
  // INT32 sleep_times = 0;
  // while(m_block_state != DisBlock::Block_Drawn) {
  //   Sleep(2);
  //   LOG("wall update sleep");
  //   // ++sleep_times;
  //   // if(sleep_times >= UpdatableBlock::MAX_SLEEP_TIMES) return;
  // }
  // OutputDebugString("begin update a wall");
  switch(m_state){
    case BUILD_SEL:
    break;
    case TO_BUILD:
    break;
    case ON_BUILDING:
    --num_timer;
    if(num_timer <= 0) end_build(game_map);
    break;
    case BUILT:
    break;
    default:
    assert(0);
  }
  // OutputDebugString("update the wall");
  // m_block_state = DisBlock::Block_updated;
}

void Wall::DisplayOnce(gs::Graphics& pg){
  // INT32 sleep_times = 0;
  // while(m_block_state != DisBlock::Block_updated) {
  //   Sleep(2);
  //   LOG("wall display sleep");
  //   // ++sleep_times;
  //   // if(sleep_times >= UpdatableBlock::MAX_SLEEP_TIMES) return;
  // }
  // OutputDebugString("begin display a wall");
  assert(Wall::wall_image);
  switch(m_state){
    case TO_BUILD:
    pg.DrawString(num_char,NUM_LEN,UpdatableBlock::global_font_8,num_display_point,
    Wall::wall_brush);
    pg.DrawRectangle(Wall::wall_to_pen,disrect);
    break;

    case BUILD_SEL:
    pg.DrawRectangle(Wall::wall_select_pen,disrect);
    break;

    case ON_BUILDING:
    pg.DrawRectangle(Wall::wall_select_pen,disrect);
    assert(num_timer <= 64);
    // MessageBoxW(NULL,global_num_string,L"info",MB_NOFOCUS);
    // LOG("timer num %d",num_timer);
    assert(pg.DrawString(global_num_string + num_timer / 8,1,UpdatableBlock::global_font_16,
    num_display_point,Wall::wall_brush) == gs::Ok);
    // LOG("on build %f %f",num_display_point.X,num_display_point.Y);
    break;

    case BUILT:
    pg.DrawImage(Wall::wall_image,disrect,0,0,wall_image_w,wall_image_h,gs::UnitPixel);
    break;
    
    default:assert(0);
  }

  // m_block_state = DisBlock::Block_Drawn;
  // OutputDebugString("end display a wall");
}


template <>
void Wall_Set::add_set_to_map(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(ptr_table && tile_type);
  INT32 prow = 0, pcol = 0;
  TileType map_type = TileType::T_wall;
  for(auto& it: set_vector){
    switch(it.get_state()){
      case Wall::BUILD_SEL: case Wall::TO_BUILD:
      map_type = TileType::T_to_build_wall;
      break;
      case Wall::ON_BUILDING:
      map_type = TileType::T_on_building_wall;
      break;
      case Wall::BUILT:map_type = TileType::T_wall;
      break;
      default:
      assert(0);
    }
    prow = it.get_cur_row(); pcol = it.get_cur_col();
    assert(ptr_table[prow][pcol][1] == NULL);
    assert(tile_type[prow][pcol][1] == TileType::T_grass);
    tile_type[prow][pcol][1] = map_type;
    ptr_table[prow][pcol][1] = &it;
  }
}