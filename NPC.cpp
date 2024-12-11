// #include"all.h"
// using NPC_CONTAINER = std::vector<NPC_Set*>;
#include"NPC.h"
#include"Map.h"
#include"Tree.h"
#include"constv.h"
#include"Wall.h"
#include"Stage.h"

// static variables
constexpr int walking_velocity = 2;
// TiVis* NPC::npc_vis = NULL;
// gs::Image* NPC::npc_walking_image = NULL, *NPC::npc_carry_image= NULL,
// * NPC::npc_cutting_image = NULL, * NPC::npc_carry_material_image = NULL;
gs::Image* NPC::npc_images[I_NPC_CNT] = {0};
TI_Ptr* NPC::map_ptr_table = NULL;
TiType_Line* NPC::map_tile_type = NULL;

const WCHAR* npc_image_path[]={
  RES_PATH L"npc_walking.png",
  RES_PATH L"npc_carry.png",
  RES_PATH L"npc_cutting.png",
  RES_PATH L"gold.png"
};


const INT32 traverse_gate_velocity[4][2]={
  {1,0},{-1,0},{0,1},{0,-1}
};

void NPC::Init(){

  for(int i =0;i< I_NPC_CNT;++i){
    npc_images[i] = new gs::Image(npc_image_path[i]);
    assert(npc_images[i]->GetLastStatus() == gs::Ok);
  }

}

void NPC::Free(){
  for(int i =0;i< I_NPC_CNT;++i){
    delete npc_images[i]; npc_images[i] = NULL;
  }
  OutputDebugString("free the npc image!!!");
}



// constructor;
NPC::NPC(NPC::Direction dire,INT32 prow,INT32 pcol):
UpdatableBlock(prow,pcol),
m_direction(dire),m_state(NPC::NPC_State::NPC_Idle)
{
  memset(to_dest_path,0,sizeof(to_dest_path));
  // memset(back_src_path,0,sizeof(back_src_path));
  disrect.X = DEVI_X(cur_col,npc_walking_iblock_w);
  disrect.Width = npc_walking_iblock_w;
  OutputDebugString("create a new npc!!!");
  // m_block_state = DisBlock::Block_updated;
}
void NPC::analyse_direction(){
  switch(m_direction){
    case NPC::Up:OutputDebugStringA("up");break;
    case NPC::Down:OutputDebugStringA("down");break;
    case NPC::Right:OutputDebugStringA("right");break;
    case NPC::Left:OutputDebugStringA("left");break;
    default:assert(0);
  }
}

void NPC::UpdateOnce(Map& game_map){
  // while(m_block_state != DisBlock::Block_Drawn) {
  //   // assert(m_block_state == DisBlock::Block_updated);
  //   Sleep(2);
  //   LOG("npc update sleep");
  // }
  // TODO
  // LOG("update npc");

  switch(m_state){
    case NPC_Idle:{
      ++idle_time_elapse;
      if(idle_time_elapse >= npc_max_idle_time){
        idle_time_elapse = 0;
        // LOG("begin find work");
        idle_find_work(game_map);
        LOG("find work end");
      }
    }break;

    case NPC_On_Cutting:
    break;

    case NPC_To_Stage_Prepare_To_Build_Wall:
    LOG("to stage for build step: %d",step_count);
    case NPC_To_Cut: case NPC_To_Build:
    case NPC_To_Pick_Property: case NPC_To_Stage_For_Stock_Property:
    case NPC_To_Collect_Stage:
    move_a_bit(game_map);
    break;

    case NPC_On_Collecting_Stage: case NPC_On_Pick_Property:
    m_state = NPC_Idle;
    break;

    case NPC_On_Building:
    break;



    default:
    LOG("npc update once exception : %d",m_state);
    assert(0);
  }
  // OutputDebugStringA("set the state updated!!!");
  // m_block_state = DisBlock::Block_updated;
  // LOG("npc updated!!!");
}
void NPC::prepare_move(Map& game_map){
  m_direction = static_cast<Direction>(to_dest_path[0]);
  walking_frame_count = npc_walking_frame_cnt;
  step_count = -1;

  TiType_Line* tile_type = game_map.get_map_type_table();
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  assert(tile_type && ptr_table);

  
  switch(m_state){
    case NPC_To_Cut:
    case NPC_To_Collect_Stage:
    break;
    case NPC_To_Build:{
      switch(tile_type[cur_row][cur_col][1]){
        case TileType::T_property:{
          Property* in_prop = static_cast<Property*>(ptr_table[cur_row][cur_col][1]);
          assert(in_prop);
          carry_property = in_prop->picked(game_map);
        }break;
        case TileType::T_stage_area:{
          Stage* in_stage = static_cast<Stage*>(ptr_table[cur_row][cur_col][1]);
          assert(in_stage);
          assert(carry_property == 0);
          carry_property = in_stage->moveto_used();
          // TODO
        }break;
        case TileType::T_wall:
        break;
        
        default:
        LOG("prepare move exception in to build %d",tile_type[cur_row][cur_col][1]);
        assert(0);
      }
    }break;
    case NPC_To_Stage_For_Stock_Property:{
      assert(ptr_table[cur_row][cur_col][1]);
      assert(tile_type[cur_row][cur_col][1] == TileType::T_property);
      Property* target_prop = static_cast<Property*>(ptr_table[cur_row][cur_col][1]);
      carry_property = target_prop->picked(game_map);
      assert(carry_property > 0);
    }
    break;
    case NPC_To_Pick_Property:
    // TODO
    break;

    case NPC_To_Stage_Prepare_To_Build_Wall:{
      assert(tile_type[cur_row][cur_col][1] == TileType::T_grass ||
      tile_type[cur_row][cur_col][1] == TileType::T_wall ||
      tile_type[cur_row][cur_col][1] == TileType::T_to_build_wall || 
      tile_type[cur_row][cur_col][1] == TileType::T_stage_area);
      assert(carry_property == 0);
      OutputDebugString("prepare move : to stage for build!!!");


    }break;

    default:
    LOG("npc prepare_move exception : %d",m_state);
    assert(0);
  }
}
void NPC::move_a_bit(Map& game_map){
  if(walking_frame_count == NPC::npc_walking_frame_cnt){
    // LOG("curx: %d cury: %d",cur_row,cur_col);
    walking_frame_count=0;
    ++step_count;
    if(step_count == path_length){
      reach_dest(game_map);
      return ;
    }
    ChangeDirection(game_map);
  }
  ++walking_frame_count;
  disrect.X += disrect_move_dires[m_direction][0],
  disrect.Y += disrect_move_dires[m_direction][1];
}

inline void NPC::ChangeDirection(Map& game_map){

  m_direction = static_cast<Direction>(to_dest_path[step_count]);
  cur_row += dires[m_direction][0], cur_col += dires[m_direction][1];
  Adjust_walking_Y();
}
void NPC::begin_cut(){
  m_state = NPC_On_Cutting;
  Adjust_cut_Y();
  LOG("end begin cut: %d %d %d %d",disrect.X,disrect.Y,disrect.Width,disrect.Height);
}

void NPC::Adjust_cut_Y(){
  LOG("adjust npc cut y");
  src_x = 0;
  disrect.X = DEVI_X(cur_col,npc_cut_iblock_w);
  disrect.Width = npc_cut_iblock_w;
  switch(m_direction){
    case Down:
    src_y = 0;
    break;
    case Up:
    src_y = npc_cut_iblock_h;
    break;
    case Right:
    src_y = 2 * npc_cut_iblock_h;
    break;
    case Left:
    src_y = 3 * npc_cut_iblock_h;
    break;

    default:assert(0);
  }
  assert(disrect.Width == 32);

}
inline void NPC::Adjust_idle_Y(){
  Adjust_normal_walking_Y();
}
void NPC::Adjust_normal_walking_Y(){
  src_x = 0;
  assert(disrect.Width == 16);
  switch(m_direction){
    case Down:
    src_y = 0;
    break;
    case Right:
    src_y = npc_walking_iblock_h;
    break;
    case Up:
    src_y = 2 * npc_walking_iblock_h;
    break;
    case Left:
    src_y = 3 * npc_walking_iblock_h;
    break;

    default:
    assert(0);
  }
}

inline void NPC::Adjust_carry_walking_Y(){
  Adjust_normal_walking_Y();
}

void NPC::DisplayOnce(gs::Graphics& pg){
  // while(m_block_state != DisBlock::Block_updated) {
  //   // assert(m_block_state == DisBlock::Block_Drawn);
  //   Sleep(2);
  //   // LOG("npc display sleep");
  // }
  switch(m_state){

    case NPC_Idle:
    case NPC_On_Building:
    case NPC_On_Pick_Property:
    // case NPC_On_Release_Property_To_Stock:
    Idle_Display(pg);
    break;
    case NPC_On_Cutting:
    // TODO
    // LOG("cut %d %d %d %d",disrect.X,disrect.Y, disrect.Width,disrect.Height);
    src_x += npc_cut_iblock_w;
    if(src_x >= npc_cutting_image_w) src_x = 0;
    pg.DrawImage(npc_images[I_CUTTING],disrect,src_x,src_y,npc_cut_iblock_w,
    npc_cut_iblock_w,gs::UnitPixel);

    break;


    case NPC_To_Cut:
    case NPC_To_Pick_Property:
    case NPC_To_Collect_Stage:
    case NPC_To_Stage_Prepare_To_Build_Wall:

    pg.DrawImage(npc_images[I_WALKING],disrect,src_x,src_y,npc_walking_iblock_w,
    npc_walking_iblock_h,gs::UnitPixel);
    pg.DrawLine(UpdatableBlock::global_pen,disrect.X + npc_walking_iblock_w/2,
    disrect.Y + npc_walking_iblock_h/2,
    link_target_col* TILE_W + TILE_W/2,link_target_row * TILE_H + TILE_H /2);
    src_x += npc_walking_iblock_w;
    if(src_x >= npc_walking_image_w) src_x = 0;
    break;

    case NPC_To_Build:
    case NPC_To_Stage_For_Stock_Property:

    pg.DrawImage(npc_images[I_CARRY],disrect, src_x, src_y,npc_walking_iblock_w,
    npc_walking_iblock_h,gs::UnitPixel);
    pg.DrawLine(UpdatableBlock::global_pen,disrect.X + npc_walking_iblock_w/2,
    disrect.Y + npc_walking_iblock_h/2,
    link_target_col* TILE_W + TILE_W/2,link_target_row * TILE_H + TILE_H /2);
    src_x += npc_walking_iblock_w;
    if(src_x >= npc_walking_image_w) src_x = 0;
    Display_Carry_Meterial(pg);
    break;

    default:
    LOG("error in display once %d",m_state);
    assert(0);
  }
  // OutputDebugStringA("set the state draw !!!");
  // m_block_state = DisBlock::Block_Drawn;
}
inline void NPC::Idle_Display(gs::Graphics& pg){
  // TODO
  src_x += npc_walking_iblock_w;
  if(src_x >= npc_walking_image_w) src_x = 0;
  assert(npc_images[I_WALKING]);
  pg.DrawImage(npc_images[I_WALKING],disrect,src_x, src_y, NPC::npc_walking_iblock_w,
  NPC::npc_walking_iblock_h,gs::UnitPixel);
}
inline void NPC::Display_Carry_Meterial(gs::Graphics& pg){
  // TODO
  // pg.DrawImage(NPC::npc_carry_material_image,disrect.X ,disrect.Y,10,10);
  pg.DrawImage(NPC::npc_images[I_GOLD],disrect.X ,disrect.Y,
  npc_gold_image_w,npc_gold_image_h);
}

void NPC::reach_dest(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();

  assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_walking);
  assert(ptr_table[cur_row][cur_col][0] == this);

  // tile_type[cur_row][cur_col][0] = TileType::T_npc_work;
    tile_type[cur_row][cur_col][0] = TileType::T_npc_idle;

  switch(m_state){
    /*
    to is in walking !!!
    */
    // cut tree;
    case NPC_To_Cut:{
      // LOG("cur: %d %d link: %d %d",cur_row,cur_col,link_target_row,link_target_col);

      assert(cur_col == link_target_col && cur_row == link_target_row);
      Tree* target_tree= static_cast<Tree*>(ptr_table[cur_row][cur_col][1]);
      assert(target_tree);
      LOG("Begin cut");
      target_tree->begin_cut();
      begin_cut();
      tile_type[cur_row][cur_col][0] = TileType::T_npc_work;
    }break;

    case NPC_To_Build:{
      LOG("npc to build exception row, col: %d %d %d",cur_row,cur_col,
      tile_type[cur_row][cur_col][1]);
      assert(tile_type[cur_row][cur_col][1] == TileType::T_to_build_wall);


      Wall* target_wall = static_cast<Wall*>(ptr_table[cur_row][cur_col][1]);
      target_wall->add_material(carry_property);
      carry_property = 0;
      if(target_wall->buildable()){
        target_wall->begin_build(game_map);
        m_state = NPC_On_Building;
        tile_type[cur_row][cur_col][0] = TileType::T_npc_work;
        LOG("buildable");
        OutputDebugString("wall can build!!!");
      }else{
        // tile_type[cur_row][cur_col][0] = TileType::T_npc_idle;
        m_state = NPC_Idle;
      }
      LOG("npc to build reach dest!!! offer %d",target_wall->get_offer());
    }break;

    case NPC_To_Stage_For_Stock_Property:{
      // m_state = NPC_On_Release_Property_To_Stock;
      Stage* target_stage = static_cast<Stage*>(ptr_table[cur_row][cur_col][1]);
      assert(carry_property > 0);
      m_state = NPC_Idle;
      // Adjust_idle_Y();
      assert(target_stage);
      assert(cur_col == link_target_col && cur_row == link_target_row);
      target_stage->add_stock_include_no_collect(carry_property);
      carry_property = 0;
      LOG("add property to stage");
    }break;

    case NPC_To_Collect_Stage:{
      assert(cur_col == link_target_col && cur_row == link_target_row);
      Stage* target_stage = static_cast<Stage*>(ptr_table[cur_row][cur_col][1]);
      assert(target_stage);
      target_stage->collect();
      m_state = NPC_Idle;
    }break;

    case NPC_To_Pick_Property:{
      assert(cur_col == link_target_col && cur_row == link_target_row);
      m_state = NPC_Idle;
      TiType_Line* tile_type = game_map.get_map_type_table();
      // assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_walking);
      // Property* target_prop = static_cast<Property*>(ptr_table[cur_row][cur_col][1]);
      // assert(target_prop);
      // m_state = NPC_On_Pick_Property;
      // carry_property = target_prop->get_val();
      // target_prop->picked(game_map);

    }break;

    case NPC_To_Stage_Prepare_To_Build_Wall:{
      assert(tile_type[cur_row][cur_col][1] == TileType::T_stage_area);
      Stage* target_stage = static_cast<Stage*>(ptr_table[cur_row][cur_col][1]);
      assert(target_stage);
      // TODO
      m_state = NPC_Idle;
    }break;

    default:
    LOG("error in reach dest %d",m_state);
    assert(0);
  }
  link_target_row = link_target_col =0;
}

bool NPC::idle_find_work(Map& game_map){
  LOG("idle npc begin find work");
  assert(m_state == NPC_Idle);
  return game_map.idle_npc_find_work(*this); 
}

void NPC::end_cut(Map& game_map){
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_work);
  tile_type[cur_row][cur_col][0] = TileType::T_npc_idle;
  assert(game_map.get_ptr_table()[cur_row][cur_col][0] == this);
  assert(disrect.Width == 32);
  disrect.X = DEVI_X(cur_col,npc_walking_iblock_w);
  disrect.Width = npc_walking_iblock_w;
  Adjust_idle_Y();
  link_target_row = link_target_col = 0;
  m_state = NPC_Idle;
  LOG("npc end cut !!!");
}

void NPC::begin_move_to_work(NPC::NPC_State move_to_work_state,Map& game_map){
  m_state = move_to_work_state;
  prepare_move(game_map);
}

void NPC::end_build(Map& game_map){
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(tile_type[cur_row][cur_col][0] == TileType::T_npc_work);
  tile_type[cur_row][cur_col][0] = TileType::T_npc_idle;
  m_state = NPC_Idle;
}







template<>
void NPC_Set::add_set_to_map(Map& game_map){
  TI_Ptr* ptr_table = game_map.get_ptr_table();
  TiType_Line* tile_type = game_map.get_map_type_table();
  assert(ptr_table && tile_type);

  INT32 prow =0, pcol = 0, link_row= 0, link_col = 0;
  for(auto& it: set_vector){
    prow = it.get_cur_row(); pcol = it.get_cur_col();
    link_row = it.get_link_target_row(), link_col = it.get_link_target_col();
    // assert(tile_type[prow][pcol][0] == TileType::T_grass || tile_type[link_row][link_o]);
    assert(ptr_table[prow][pcol][0] == NULL || ptr_table[link_row][link_col][0] == NULL);
    // TileType map_type = TileType::T_npc_idle;
    switch(it.get_state()){
      case NPC::NPC_Idle:
      ptr_table[prow][pcol][0] = &it;
      tile_type[prow][pcol][0] = TileType::T_npc_idle;
      break;
      case NPC::NPC_On_Building:
      case NPC::NPC_On_Cutting:
      // case NPC::NPC_On_Collecting_Stage:
      // case NPC::NPC_On_Pick_Property:
      // case NPC::NPC_On_Release_Property_To_Stock:
      // map_type = TileType::T_npc_work;
      assert(ptr_table[prow][pcol][0] == NULL);
      ptr_table[prow][pcol][0] = &it;
      tile_type[prow][pcol][0] = TileType::T_npc_work;
      break;

      case NPC::NPC_To_Build:
      case NPC::NPC_To_Cut:
      case NPC::NPC_To_Pick_Property:
      case NPC::NPC_To_Stage_For_Stock_Property:
      case NPC::NPC_To_Collect_Stage:
      case NPC::NPC_To_Stage_Prepare_To_Build_Wall:
      tile_type[link_row][link_col][0] = TileType::T_npc_walking;
      assert(ptr_table[link_row][link_col][0] == NULL);
      ptr_table[link_row][link_col][0] = &it;
      break;

      default:
      LOG("add npc exception state: %d",it.get_state());
      assert(0);
      break;

    }
    // tile_type[prow][pcol][0] =map_type;
    // ptr_table[prow][pcol][0] = &it;
  }
}

template <>
void NPC_Set::show(){
  for(auto& it:set_vector){
    switch(it.get_state()){
      case NPC::NPC_Idle:
      LOG("idle ");
      break;

      case NPC::NPC_To_Build:
      case NPC::NPC_To_Stage_Prepare_To_Build_Wall:
      case NPC::NPC_To_Collect_Stage:
      case NPC::NPC_To_Pick_Property:
      case NPC::NPC_To_Cut:
      case NPC::NPC_To_Stage_For_Stock_Property:
      LOG("walking ");
      break;
      case NPC::NPC_On_Building:
      case NPC::NPC_On_Cutting:
      LOG("work ");
      break;
      default:
      LOG("error in show npc state %d",it.get_state());
      assert(0);

    }
  }
  LOGCHAR('\n');
}