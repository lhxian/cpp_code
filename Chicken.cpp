#include"all.h"
#include"constv.h"
#include"Map.h"
#include"Chicken.h"

gs::Image* Chicken::chi_img = NULL;

const WCHAR* chi_img_path = {
  RES_PATH L"chicken_walk.png"
};
// static function
void Chicken::chi_Init(){
  chi_img = new gs::Image(chi_img_path);
  assert(chi_img->GetLastStatus() == gs::Ok);
}

void Chicken::chi_Free(){
  delete chi_img; chi_img = NULL;
}

Chicken::Chicken(INT32 prow,INT32 pcol):
Animal(prow,pcol)
{
  // assert(tile_type[prow][pcol] == TileType::T_grass);
  disrect.X = DEVI_X(pcol,chi_iblock_w);
  disrect.Y = DEVI_Y(prow,chi_iblock_h);
  disrect.Width = chi_iblock_w; disrect.Height = chi_iblock_h;
  m_direction = rand() & 0x3;
  src_x = 0;
  adjustY();
  still = 1;
  // an_frame_count = chi_frame_cnt;
  an_frame_count = 0;
  // m_block_state = DisBlock::Block_Drawn;
}
void Chicken::adjustY(){
  switch(m_direction){
    case 0:// right
    src_y = 3*chi_iblock_h;
    break;
    case 1://left;
    src_y = chi_iblock_h;
    break;
    case 2:// down;
    src_y = 2*chi_iblock_h;
    break;
    case 3://up
    src_y = 0;
    break;
    default:assert(0);
  }
}
void Chicken::UpdateOnce(Map& game_map){
  OutputDebugString("begin update the chicken!!!");
  // LOG("chicken frame cnt: %d",an_frame_count);
  // while(m_block_state != DisBlock::Block_Drawn) Sleep(2);
  // m_block_state = DisBlock::Block_Updating;
  OutputDebugString("chicken in updating");

  if(an_frame_count == chi_frame_cnt){
    an_frame_count = 0;
    OutputDebugString("chicken in update prepare move a grid");
    moveAgrid(game_map);
  }
  OutputDebugString("chicken add an frame");
  ++an_frame_count;
  if(still == 0){
    OutputDebugString("chicken begin change x");
    
    char num[16];
    sprintf(num,"%d",m_direction);
    OutputDebugString(num);
    assert(m_direction < 4 && m_direction >=0);
    disrect.X += disrect_move_dires[m_direction][0],
    OutputDebugString("chicken begin change y");
    disrect.Y += disrect_move_dires[m_direction][1];
    // LOG("chicken direction: %d pos: %d %d",m_direction,row,col);
    // OutputDebugString("chicken move");
  }
  OutputDebugString("update the chicken");


  // m_block_state = DisBlock::Block_updated;
}
void Chicken::moveAgrid(Map& game_map){
  OutputDebugString("chicken move a grid");
  int rand_direction = rand() & 0x3;
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(tile_type);
  still = 1;
  int ax,ay;
  for(int i=0;i<4;++i){
    ax = row + dires[rand_direction][0], ay = col + dires[rand_direction][1];
    if(ax > 0 && ax <= ABSTRACT_MAP_H_C && ay > 0 && ay <= ABSTRACT_MAP_W_C 
    && tile_type[ax][ay][0] == TileType::T_grass && 
    (tile_type[ax][ay][1] == TileType::T_grass ||
    tile_type[ax][ay][1] == TileType::T_to_build_wall
    || tile_type[ax][ay][1] == TileType::T_stage_area)
    ){
      still = 0;
      // tile_type[ax][ay][0]  = TileType::T_animal;
      changedirection(rand_direction);
      OutputDebugString("chicken change direction");
      // LOG("chicken change the directoin!!!");
      return;
    }
    rand_direction = (rand_direction + 1) & 0x3;
  }
}

void Chicken::changedirection(int dire){
  m_direction = dire;
  assert(m_direction >=0 && m_direction < 4);
  row += dires[m_direction][0], col += dires[m_direction][1];
  adjustY();
}

void Chicken::DisplayOnce(gs::Graphics& pg){
  // while(m_block_state != DisBlock::Block_updated) Sleep(2);
  // m_block_state = DisBlock::Block_Drawing;
  src_x += Chicken::chi_iblock_w;
  if(src_x >= Chicken::chi_i_w) src_x = 0;

  pg.DrawImage(Chicken::chi_img,disrect,src_x,src_y,chi_iblock_w,chi_iblock_h,
  gs::UnitPixel);

  // m_block_state = DisBlock::Block_Drawn;
}
