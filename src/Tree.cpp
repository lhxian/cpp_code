#include"all.h"
#include"Tree.h"
#include"NPC.h"
#include"Map.h"

const WCHAR* tree_image_path={
  RES_PATH L"tree.png"
};

gs::Image* Tree::tree_image = NULL;
gs::Pen* Tree::tree_sel_pen = NULL, * Tree::tree_wait_pen = NULL;

void Tree::Init(){
  tree_image = new gs::Image(tree_image_path);
  assert(tree_image->GetLastStatus() == gs::Ok);
  tree_sel_pen = new gs::Pen(gs::Color::OrangeRed,2.f);
  tree_wait_pen = new gs::Pen(gs::Color::BlueViolet,2.f);
  assert(tree_sel_pen->GetLastStatus() == gs::Ok
  && tree_wait_pen->GetLastStatus() == gs::Ok);

}

void Tree::Free(){
  delete tree_image; tree_image = NULL;
  delete tree_sel_pen; tree_sel_pen = NULL;
  delete tree_wait_pen; tree_wait_pen = NULL;
}




Tree::Tree(INT32 prow,INT32 pcol):
UpdatableBlock(prow,pcol),m_state(NO_CUT)
{
  num_display_point.X = (gs::REAL)(disrect.X + TILE_W /4);
  num_display_point.Y = (gs::REAL)(disrect.Y - TILE_H / 2);
  // LOG("tree %f %f",num_display_point.X,num_display_point.Y);
}



// universal function
void Tree::UpdateOnce(Map& game_map){
  // while(m_block_state != DisBlock::Block_Drawn) Sleep(2);

  switch(m_state){
    case NO_CUT:
    case INVALID: 
    case WAIT_TO_CUT:
    case CUT_SELECTED:
    break;

    case ON_CUTTING:{
      assert(cut_timer_num > 0);
      --cut_timer_num;
      if(cut_timer_num == 0){
        // m_state = INVALID;
        // game_map.gen_property(cur_row,cur_col);
        end_cut(game_map);
        LOG("tree end cut");
      }
    }break;
  }
  // m_block_state = DisBlock::Block_updated;
}

void Tree::DisplayOnce(gs::Graphics& pg){
  // while(m_block_state != DisBlock::Block_updated) Sleep(2);

  switch(m_state){
    case INVALID:break;
    case NO_CUT:
    case WAIT_TO_CUT:
    case ON_CUTTING:
    case CUT_SELECTED:{
      pg.DrawImage(Tree::tree_image,disrect,0,0,Tree::tree_image_w,Tree::tree_image_h,
      gs::UnitPixel);
      switch(m_state){
        case WAIT_TO_CUT:
        pg.DrawEllipse(Tree::tree_wait_pen,disrect);
        break;
        case CUT_SELECTED:
        pg.DrawEllipse(Tree::tree_sel_pen,disrect);
        break;

        case ON_CUTTING:
        // LOG("draw string %f %f",num_display_point.X,num_display_point.Y);
        pg.DrawString(global_num_string+ cut_timer_num / Tree::tree_cut_interval,1,
        UpdatableBlock::global_font_16,num_display_point, UpdatableBlock::global_num_brush);

        default:break;
      }
    }break;

    default:assert(0);
  }
  // m_block_state = DisBlock::Block_Drawn;
}

void Tree::Write_To_File(std::ofstream & ofs){
  if(m_state == INVALID)  return;
  ofs.write((const char*)(this),sizeof(Tree));
}

void Tree::Load_From_File(std::ifstream& ifs){
  ifs.read(reinterpret_cast<char*>(this),sizeof(Tree));
}

/******************************************************************************

split  ^-^

*******************************************************************************/

// unique function;

void Tree::begin_cut(){
  assert(m_state == WAIT_TO_CUT);
  assert(cut_timer_num == 64);
  m_state = ON_CUTTING;
  LOG("tree begin cut");
}

void Tree::end_cut(Map& game_map){
  LOG("tree end cut");
  assert(m_state == ON_CUTTING);
  m_state = INVALID;
  TiType_Line* tile_type = game_map.get_map_type_table();

  assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_work);
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  NPC* target_npc= static_cast<NPC*>(ptr_table[cur_row][cur_col][0]);
  assert(target_npc);
  LOG("tree begin end cut");
  target_npc->end_cut(game_map);
  LOG("tree begin disappeal");
  link_target_row = link_target_col = 0;
  LOG("begin gen property");
  game_map.gen_property(cur_row,cur_col);
  LOG("gen property!!!");

}

// discard
void Tree::abort_cut(Map& game_map){
  assert(m_state != INVALID);
  m_state = NO_CUT;
  cut_timer_num = tree_cutting_consume_time;
  assert(link_target_row && link_target_col);

  if(link_target_row == 0 && link_target_col == 0)  return;
  assert(link_target_row == cur_row && link_target_col == cur_col);

  TI_Ptr* tile_ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_work);
  NPC* target_npc = static_cast<NPC*>(tile_ptr_table[cur_row][cur_col][0]);
  assert(target_npc);

  link_target_row = link_target_col = 0;
}

void Tree::cut_select(){
  if(m_state != NO_CUT) return;
  m_state = CUT_SELECTED;
}

void Tree::wait_to_cut(){
  m_state = WAIT_TO_CUT;
}


template <>
void Tree_Set::add_set_to_map(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(ptr_table && tile_type);
  INT32 prow = 0, pcol = 0;
  LOG("begin add tree to map : %lld",set_vector.size());
  for(auto& it: set_vector){
    if(it.valid()){
      prow = it.get_cur_row(); pcol = it.get_cur_col();
      assert(ptr_table[prow][pcol][1] == NULL);

      LOG("error in tree:%d %d %d %lld",prow,pcol,tile_type[prow][pcol][1],set_vector.size());
      assert(tile_type[prow][pcol][1] == TileType::T_grass);
      tile_type[prow][pcol][1] = TileType::T_tree;
      ptr_table[prow][pcol][1] = &it;
    }
  }
}