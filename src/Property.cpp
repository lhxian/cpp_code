#include"all.h"
#include"Map.h"
#include"Property.h"

gs::SolidBrush* Property::prop_font_brush = NULL;
gs::Image* Property::prop_image = NULL;
const WCHAR * prop_image_file_name ={
  RES_PATH L"property.png"
};

void Property::Init(){
  prop_font_brush= new gs::SolidBrush(gs::Color::Gold);
  assert(prop_font_brush->GetLastStatus() == gs::Ok);
  prop_image = new gs::Image(prop_image_file_name);
  assert(prop_image->GetLastStatus() == gs::Ok);
}

void Property::Free(){
  delete prop_font_brush; prop_font_brush = NULL;
  delete prop_image; prop_image = NULL;
}

Property::Property(INT32 prow,INT32 pcol):
UpdatableBlock(prow, pcol), exists(TRUE),val(DEF_PRO_VAL)
{
  // TODO
  // disrect.X = DEVI_X(prow,)
  num_display_point.X = static_cast<gs::REAL>(disrect.X);
  num_display_point.Y = static_cast<gs::REAL>(disrect.Y);
  disrect = {DEVI_X(pcol,prop_iblock_w),DEVI_Y(prow,prop_iblock_h),prop_iblock_w,prop_iblock_h};
  src_x = 0, src_y = 0;
  UpdatableBlock::write_num_to_wchar(val_num_char,val);

}

INT32 Property::picked(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();

  assert(tile_type[cur_row][cur_col][1] == TileType::T_property);
  // LOG("npc state %d row, col: %d %d",tile_type[cur_row][cur_col][0],cur_row,cur_col);
  assert(tile_type[cur_row][cur_col][0] == TileType::T_grass);
  assert(ptr_table[cur_row][cur_col][1] == this);
  assert(ptr_table[cur_row][cur_col][1]);

  exists = FALSE;
  tile_type[cur_row][cur_col][1] = TileType::T_grass;
  ptr_table[cur_row][cur_col][1] = NULL;

  return val;

}

void Property::DisplayOnce(gs::Graphics& pg){
  if(exists == FALSE) return;
  // while(m_block_state != DisBlock::Block_updated) Sleep(1);
  // LOG("disply property");
  pg.DrawImage(prop_image,disrect,src_x,src_y,prop_iblock_w,prop_iblock_h,gs::UnitPixel);
  src_x += prop_iblock_w;
  if(src_x >= prop_image_w) src_x = 0;
  auto it =pg.DrawString(val_num_char,3,UpdatableBlock::global_font_8,
  num_display_point,Property::prop_font_brush);
  assert(it == gs::Ok);
  // m_block_state = DisBlock::Block_Drawn;
}


template<>
void Property_Set::add_set_to_map(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(ptr_table && tile_type);

  INT32 prow =0, pcol = 0;
  for(auto& it: set_vector){
    if(it.exists){
      prow = it.get_cur_row(); pcol = it.get_cur_col();
      assert(tile_type[prow][pcol][1] == TileType::T_grass);
      assert(ptr_table[prow][pcol][1] == NULL);
      tile_type[prow][pcol][1] = TileType::T_property;
      ptr_table[prow][pcol][1] = &it;
    }
  }
}