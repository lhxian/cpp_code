#include<cassert>
#include "sys_header.h"
#include<cstdlib>
#include<ctime>
#include<cstring>
#include"constv.h"
#include"DisWin.h"
#include"Property.h"
#include "Map.h"
#include"NPC.h"
#include"Tree.h"
#include"Stage.h"
#include"Wall.h"


// #include"Tree.h"
const INT32 c_grass_angle_pos[4][2] = {
  {TILE_W - Map::grass_angle_w, TILE_H - Map::grass_angle_h}, 
  {ABSTRACT_MAP_W - TILE_W, TILE_H - Map::grass_angle_h},
  {TILE_W - Map::grass_angle_w, ABSTRACT_MAP_H -TILE_H},
  {ABSTRACT_MAP_W - TILE_W, ABSTRACT_MAP_H - TILE_H}
};
// HANDLE g_heap_handle = 0;
void Analyse(gs::Status a){
  switch(a){
    case gs::GenericError:OutputDebugStringA("generic");break;
    case gs::InvalidParameter:OutputDebugStringA("invalid p");break;
    case gs::OutOfMemory:OutputDebugStringA("out of mem");break;
    case gs::Win32Error:OutputDebugStringA("win32 err");break;
    case gs::FileNotFound:OutputDebugStringA("file no found");break;
    case gs::UnknownImageFormat:OutputDebugStringA("unkown img");break;
    default:
    return;
  }
  assert(0);
}

//static function



// extern const int dires[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

gs::Image* Map::map_images[Map_itype::i_map_cnt] = {0};
static const WCHAR* map_image_path[]={
  RES_PATH L"water.png",
  RES_PATH L"vertical_water.png",

  RES_PATH L"grass_1.png",
  RES_PATH L"grass_2.png",

  RES_PATH L"grass_edge_top.png",
  RES_PATH L"grass_edge_bottom.png",
  RES_PATH L"grass_edge_left.png",
  RES_PATH L"grass_edge_right.png",

  RES_PATH L"grass_angle_1.png",
  RES_PATH L"grass_angle_2.png",
  RES_PATH L"grass_angle_3.png",
  RES_PATH L"grass_angle_4.png"
};



void Ti_Heap::push(Tile_Node node){
  heap[len++] = node;
  int tail = len;
  while(tail > 1){
    if(heap[tail] < heap[tail/2]){
      std::swap(heap[tail],heap[tail/2]);
      tail /=2;
    }else break;
  }
}
void Ti_Heap::pop(){
  int beg = 1,index;
  heap[1] = heap[len--];
  while(beg < len){
    index = beg;
    if(beg *2 <= len && heap[beg*2] < heap[index]){
      index = beg * 2;
    }
    if(beg *2+1 <= len && heap[beg*2+1] < heap[index]){
      index = beg * 2+1;
    }
    if(index == beg)  break;
    std::swap(heap[index],heap[beg]);
    beg = index;
  }
}
//staticv
// static Ti_Heap g_heap;







// initiate function
void Map::Init(){
  LOG("begin init the map");
  for(int i=0;i<Map_itype::i_map_cnt;++i){
    Map::map_images[i] = new gs::Image(map_image_path[i]);
    LOG("error in %d",i);
    assert(
      gs::Ok == map_images[i]->GetLastStatus()
    );
  }
}
void Map::Free(){
  // delete_static();
  for(int i =0;i<Map_itype::i_map_cnt;++i){
    assert(map_images[i]);
    delete Map::map_images[i];
    Map::map_images[i] = NULL;
    OutputDebugStringA("free map image");
  }
}



Map::Map(INT32 w,INT32 h,HDC diswin_memdc):
m_w(w),m_h(h)
{
  m_mem_dc = CreateCompatibleDC(diswin_memdc);
  assert(m_mem_dc);
  m_bmp = CreateCompatibleBitmap(diswin_memdc,m_w,m_w);
  assert(m_bmp);
  SelectObject(m_mem_dc,m_bmp);

  p_graphics = new gs::Graphics(m_mem_dc);
  m_tile_fa = new TiInt[AA_MAP_HC];
  m_tile_type = new TiType_Line[AA_MAP_HC];
  m_vis = new TiVis[AA_MAP_HC];
  m_tile_ptr_table = new TI_Ptr[AA_MAP_HC];
  memset(m_tile_type,0,AA_MAP_HC * sizeof(TiType_Line));
  memset(m_tile_ptr_table,0,AA_MAP_HC * sizeof(TI_Ptr));

  to_add_wall.reserve(RESERVE_MAX);
  to_add_stage.reserve(RESERVE_MAX);

  // assert(m_image);
  gs::Rect disrect{TILE_W,TILE_H,Map::grass_w,Map::grass_h};

  // draw the water
  disrect = { 0,0,Map::water_w,Map::water_h};
  while(disrect.X < ABSTRACT_MAP_W){
    p_graphics->DrawImage(Map::map_images[i_water],disrect,0,0,water_w,water_h,gs::UnitPixel);
    disrect.X += water_w;
  }
  disrect.X = 0; disrect.Y =  ABSTRACT_MAP_H - TILE_H;
  while(disrect.X < ABSTRACT_MAP_W){
    p_graphics->DrawImage(Map::map_images[i_water],disrect,0,0,water_w,water_h,gs::UnitPixel);
    disrect.X += water_w;
  }

  disrect = {0,vertical_water_w,Map::vertical_water_w,Map::vertical_water_h};
  while(disrect.Y < ABSTRACT_MAP_W - TILE_H){
    disrect.X = 0;
    p_graphics->DrawImage(map_images[i_vertical_water],disrect,0,0,vertical_water_w,
    vertical_water_h,gs::UnitPixel);
    disrect.X = ABSTRACT_MAP_W - vertical_water_w;
    p_graphics->DrawImage(map_images[i_vertical_water],disrect,0,0,vertical_water_w,
    vertical_water_h,gs::UnitPixel);

    disrect.Y += Map::vertical_water_h;

  }
  // draw the grass plain
  disrect.X = TILE_W, disrect.Y = TILE_H;
  srand(static_cast<UINT32>(time(0)));
  while(disrect.Y < ABSTRACT_MAP_H - TILE_H){
    disrect.X = TILE_W;
    while(disrect.X < ABSTRACT_MAP_W - TILE_W){
      if(rand()& 3){
        p_graphics->DrawImage(Map::map_images[Map_itype::i_grass_1],
        disrect,0,0,Map::grass_w,Map::grass_h,gs::UnitPixel);     
      }else{
        p_graphics->DrawImage(Map::map_images[Map_itype::i_grass_2],
        disrect,0,0,Map::grass_w,Map::grass_h,gs::UnitPixel);     
      }
      disrect.X += Map::grass_w;
    }
    disrect.Y += Map::grass_h;
  }

  // draw the grass edge horizontal
  disrect = {TILE_W, TILE_H - grass_edge_horizontal_h, grass_edge_horizontal_w,
  grass_edge_horizontal_h};

  while(disrect.X < ABSTRACT_MAP_W - TILE_W){
    p_graphics->DrawImage(map_images[i_grass_edge_top],disrect,0,0,grass_edge_horizontal_w,
    grass_edge_horizontal_h,gs::UnitPixel);
    disrect.X += grass_edge_horizontal_w;
  }
  disrect.X = TILE_W; disrect.Y = ABSTRACT_MAP_H -TILE_H;
  while(disrect.X < ABSTRACT_MAP_W - TILE_W){
    p_graphics->DrawImage(map_images[i_grass_edge_bottom],disrect,0,0,grass_edge_horizontal_w,
    grass_edge_horizontal_h,gs::UnitPixel);
    disrect.X += grass_edge_horizontal_w;
  }

  // draw the grass edge vertical
  disrect = {TILE_W - grass_edge_vertical_w, TILE_H,grass_edge_vertical_w,grass_edge_vertical_h};
  while(disrect.Y < ABSTRACT_MAP_H - TILE_H){
    p_graphics->DrawImage(map_images[i_grass_edge_left],disrect,0,0,grass_edge_vertical_w,
    grass_edge_vertical_h,gs::UnitPixel);
    disrect.Y += grass_edge_vertical_h;
  }
  disrect.X = ABSTRACT_MAP_W - TILE_W; disrect.Y = TILE_H;
  while(disrect.Y < ABSTRACT_MAP_H - TILE_H){
    p_graphics->DrawImage(map_images[i_grass_edge_right],disrect,0,0,grass_edge_vertical_w,
    grass_edge_vertical_h,gs::UnitPixel);
    disrect.Y += grass_edge_vertical_h;
  }

  // draw the grass angle
  for(int i = i_grass_angle_1; i <= i_grass_angle_4;++i){
    p_graphics->DrawImage(map_images[i],c_grass_angle_pos[i - i_grass_angle_1][0],
    c_grass_angle_pos[i - i_grass_angle_1][1],grass_angle_w,grass_angle_h);
  }

  OutputDebugStringA("create map");
  delete p_graphics;
  p_graphics = NULL;
  // enable_update();
}
Map::~Map(){
  // end_update();
  delete[] m_tile_fa ; m_tile_fa = NULL;
  delete[] m_tile_type; m_tile_type = NULL;
  delete[] m_vis; m_vis = NULL;
  delete[] m_tile_ptr_table; m_tile_ptr_table = NULL;


  DeleteObject(m_bmp);
  DeleteDC(m_mem_dc);
  m_bmp = 0,m_mem_dc = 0;
}
void Map::write_to_file(const char* file_name){
  std::ofstream ofs(file_name,std::ios::out | std::ios::binary);
  ofs.write(reinterpret_cast<char*>(m_tile_type),AA_MAP_HC * sizeof(TiType_Line));
  ofs.close();
}



bool Map::add_trees_to_cut(INT32 top,INT32 bottom,INT32 left,INT32 right){
  // assert(to_cut_tree_table);
  Tree* sel_tree = NULL;
  for(auto i = top;i<=bottom;++i){
    for(auto j = left;j<= right;++j){
      if(m_tile_type[i][j][1] == TileType::T_tree){
        sel_tree = static_cast<Tree*>(m_tile_ptr_table[i][j][1]);
        if(m_tile_type[i][j][0]){
          assert(sel_tree->get_state() == Tree::ON_CUTTING ||
           sel_tree->get_state() == Tree::WAIT_TO_CUT);
          assert(m_tile_type[i][j][0] == TileType::T_npc_work || 
          m_tile_type[i][j][0] == TileType::T_npc_walking);
        }else{
          assert(m_tile_type[i][j][0] == TileType::T_grass);
          // to_cut_tree_table[i][j] =true;
          sel_tree->cut_select();
        }
      }
    }
  }
  return true;
}
void Map::update_to_build_wall_list(){
  if(to_add_wall.empty()) return;
  WaitForSingleObject(BaseWin::b_mutex,INFINITE);
  INT32 prow,pcol;
  for(auto& it: to_add_wall){
    prow = it.row, pcol = it.col;
    assert(m_tile_type[prow][pcol][1] == TileType::T_to_build_wall);
    assert(m_tile_ptr_table[prow][pcol][1] == NULL);
    m_pwall_set->push(Wall{prow,pcol});
    m_tile_ptr_table[prow][pcol][1] = & m_pwall_set->get_back();
  }
  to_add_wall.clear();
  ReleaseMutex(BaseWin::b_mutex);
}
bool Map::add_wall_to_build(INT32 prow,INT32 pcol){
  // assert(prow > 0 && prow <= ABSTRACT_MAP_H && pcol > 0 && pcol <= ABSTRACT_MAP_W_C);
  if(prow < 1 || prow > ABSTRACT_MAP_H_C || pcol < 1 || pcol > ABSTRACT_MAP_W_C){
    MessageBox(NULL,"out the the boundary of the map!","info",MB_NOFOCUS);
    return false;
  }
  assert(BaseWin::b_mutex);
  WaitForSingleObject(BaseWin::b_mutex,INFINITE);


  if(m_tile_type[prow][pcol][0] != TileType::T_grass
  || m_tile_type[prow][pcol][1] != TileType::T_grass){
    MessageBox(NULL,"can\'t build a wall here!","info",MB_NOFOCUS);
    return false;
  }
  m_tile_type[prow][pcol][1] = TileType::T_to_build_wall;
  m_pwall_set->push(Wall{prow,pcol});

  assert(m_tile_ptr_table[prow][pcol][1] == NULL);
  to_add_wall.push_back({prow,pcol});
  m_tile_ptr_table[prow][pcol][1] = &m_pwall_set->get_back();
  OutputDebugStringA("add the wall to build");
  ReleaseMutex(BaseWin::b_mutex);
  return true;
}

void Map::update_to_add_stage_list(){
  if(to_add_stage.empty())  return;
  WaitForSingleObject(BaseWin::b_mutex,INFINITE);
  INT32 prow ,pcol;
  for(auto& it: to_add_stage){
    prow = it.row, pcol = it.col;
    assert(m_tile_type[prow][pcol][1] == TileType::T_stage_area);
    assert(m_tile_ptr_table[prow][pcol][1] == NULL);
    m_pstage_set->push(Stage{prow,pcol});
    m_tile_ptr_table[prow][pcol][1] = &m_pstage_set->get_back();
  }
  to_add_stage.clear();
  ReleaseMutex(BaseWin::b_mutex);
}

bool Map::add_stage(INT32 top,INT32 bottom,INT32 left,INT32 right){
  LOG("begin add stage: %d %d %d %d",top,bottom,left,right);
  WaitForSingleObject(BaseWin::b_mutex,INFINITE);
  to_add_stage.clear();
  for(auto i = top;i<= bottom;++i){
    for(auto j = left;j <= right;++j){

      if(m_tile_type[i][j][1] == TileType::T_grass){
        LOG("add stage area %d %d",i,j);
        to_add_stage.push_back({i,j});
        m_tile_type[i][j][1] = TileType::T_stage_area;
        m_pstage_set->push(Stage{i,j});
        assert(m_tile_ptr_table[i][j][1] == NULL);
        m_tile_ptr_table[i][j][1] =  &m_pstage_set->get_back();
      }
    }
  }
  ReleaseMutex(BaseWin::b_mutex);
  LOG("add stage!!!");
  return true;
}
bool Map::cancel_trees(INT32 top,INT32 bottom,INT32 left,INT32 right){
  assert(top>0 && top <= bottom && bottom <= ABSTRACT_MAP_H_C);
  assert(left>0 && left<= right && right<= ABSTRACT_MAP_W_C);
  for(auto i = top;i <= bottom;++i){
    for(auto j = left;j <= right;++j){
      if(m_tile_type[i][j][1] == TileType::T_tree){
        Tree* p_cancel_tree = static_cast<Tree*>(m_tile_ptr_table[i][j][1]);
        assert(p_cancel_tree);
        if(p_cancel_tree->get_state() != Tree::INVALID){
          p_cancel_tree->abort_cut(*this);
        }
      }
    }
  }
  return true;
}

void Map::update_all(){
  // TODO
  assert(m_ptree_set);
  m_ptree_set->update_all(*this);
  OutputDebugString("update tree");
  assert(m_pwall_set);
  OutputDebugString("begin update wall");
  m_pwall_set->update_all(*this);
  // update_to_build_wall_list();
  OutputDebugString("update wall");
  // assert(m_pproperty_set);
  // m_pproperty_set->update_all(*this);
  assert(m_pstage_set);
  m_pstage_set->update_all(*this);
  // update_to_add_stage_list();
  OutputDebugString("update stage");
  assert(m_pproperty_set);
  m_pproperty_set->update_all(*this);

  assert(m_pnpc_set);
  // LOG("begin update all npc");
  m_pnpc_set->update_all(*this);
  // LOG("update all npc");
  OutputDebugString("update npc");
  assert(m_pchicken_set);
  m_pchicken_set->update_all(*this);
  OutputDebugString("update chicken");


}

void Map::enable_update(){
  assert(map_update_working == FALSE);
  map_update_thread_handle = CreateThread(NULL,0,Map::map_update_function,this,
  0,NULL);

  assert(map_update_thread_handle);
  map_update_working = TRUE;
  OutputDebugStringA("create a thread for update!!!!");
}

void Map::end_update(){
  assert(map_update_working);
  map_update_working = FALSE;
  // WaitForSingleObject(map_update_thread_handle,INFINITE);
  OutputDebugStringA("close the update thread");
}

void Map::show_map(){
  static INT32 show_cnt = 0;
  ++show_cnt;
  if(show_cnt > 8){
    show_cnt = 0;
    #ifdef DEBUG
    system("cls");
    #endif
  }
  LOG("SHOW MAP:");
  for(int i =1;i<=ABSTRACT_MAP_H_C;++i){
    for(int j =1;j<=ABSTRACT_MAP_W_C;++j){
      switch(m_tile_type[i][j][0]){
        case TileType::T_grass:
        COLORCHAR('g',33);
        break;
        case TileType::T_npc_idle:
        COLORCHAR('z',31);
        break;

        case TileType::T_npc_work:
        COLORCHAR('w',31);
        break;
        
        case TileType::T_npc_walking:
        COLORCHAR('i',31);
        break;

        default:
        LOG("show map : exception in 0: %d",m_tile_type[i][j][0]);
        assert(0);
      }
      // putchar('/');
      switch(m_tile_type[i][j][1]){
        case TileType::T_grass:
        COLORCHAR('g',33);
        break;
        case TileType::T_tree:
        COLORCHAR('t',34);
        break;
        case TileType::T_property:
        COLORCHAR('p',35);
        break;

        case TileType::T_stage_area:
        COLORCHAR('s',36);
        break;
        case TileType::T_to_build_wall:
        COLORCHAR('#',31);
        break;

        case TileType::T_wall:
        COLORCHAR('*',31);
        break;
        case TileType::T_on_building_wall:
        COLORCHAR('^',31);
        break;

        default:
        LOG("show map: exception in 1: %d",m_tile_type[i][j][1]);
        assert(0);
      }
      LOGCHAR(' ');

    }
    LOGCHAR('\n');
  }
  LOG("show npc state");
  m_pnpc_set->show();
}

DWORD WINAPI Map::map_update_function(LPVOID param){
  assert(param);
  Map* p_map= static_cast<Map*>(param);
  DWORD timer_count = 0;
  INT32 show_count = 0;
  while(p_map->map_update_working){
    // TODO
    timer_count = GetTickCount();
    // OutputDebugString("update loop !!!");
    p_map->update_all();
    // OutputDebugString("update all!!!");
    timer_count = GetTickCount() - timer_count;
    if(timer_count < update_velocity) Sleep(update_velocity - timer_count);
    ++show_count;
    if(show_count > 80) {
      p_map->show_map();
      show_count = 0;
    }
    if(p_map->map_update_working == FALSE){

    }
  }
  return 0;
}


bool Map::idle_npc_find_work(NPC& target_npc){
  LOG("idle npc begin find work!");
  assert(target_npc.get_state() == NPC::NPC_Idle);
  INT32 sx = target_npc.get_cur_row(), sy = target_npc.get_cur_col(), dx = 0, dy = 0;
  assert(m_tile_ptr_table[sx][sy][0] == &target_npc);
  assert(m_tile_type[sx][sy][0] == TileType::T_npc_idle);

  switch(m_tile_type[sx][sy][1]){
    case TileType::T_grass: case TileType::T_property:
    case TileType::T_to_build_wall: case TileType::T_wall:

    return idle_npc_move_to_find_work(target_npc);

    case TileType::T_stage_area:
    if(idle_npc_recollect_property_for_stage(target_npc)){
      return true;
    }else return idle_npc_move_to_find_work(target_npc);

    case TileType::T_tree:{
      Tree* target_tree = static_cast<Tree*>(m_tile_ptr_table[sx][sy][1]);
      assert(target_tree);
      if(target_tree->get_state() == Tree::CUT_SELECTED){
        return idle_npc_recut_tree(target_npc);
      }
    }break;

    default:
    LOG("idle npc find work exception %d",m_tile_type[sx][sy][1]);
    assert(0);
  }
  return false;
}

bool Map::idle_npc_move_to_find_work(NPC& target_npc){
  LOG("idle npc begin move to find work");
  INT32 sx = target_npc.get_cur_row(), sy = target_npc.get_cur_col(),
  dx = 0, dy = 0;
  INT32 cx, cy, step_cnt = 0;
  // FIX BUG
  npc_src_row = sx, npc_src_col = sy;
  if(m_tile_type[sx][sy][1] == TileType::T_stage_area){
    Stage* target_stage = static_cast<Stage*>(m_tile_ptr_table[sx][sy][1]);
    assert(target_stage);
    if(target_stage->empty()) npc_in_empty_stage = TRUE;
    else npc_in_empty_stage = FALSE;
  }

  LOG("error in move to find work %d",m_tile_type[sx][sy][1]);
  assert(m_tile_type[sx][sy][1] == TileType::T_grass || 
  m_tile_type[sx][sy][1] == TileType::T_property ||
  m_tile_type[sx][sy][1] == TileType::T_stage_area ||
  m_tile_type[sx][sy][1] == TileType::T_to_build_wall ||
  m_tile_type[sx][sy][1] == TileType::T_wall
  );

  assert(target_npc.get_link_target_col() == 0
  && target_npc.get_link_target_row() == 0);

  memset(m_vis,0,AA_MAP_HC * sizeof(TiVis));
  memset(hit_work,0,sizeof(hit_work));
  memset(find_work,0,sizeof(find_work));

  bfs_q.clear();
  bfs_q.push({sx,sy,0});
  m_vis[sx][sy] = true;
  TileType npc_in_block_type = m_tile_type[sx][sy][1];
  while(!bfs_q.empty()){
    auto& tem_pos = bfs_q.top_front();
    bfs_q.pop();
    cx = tem_pos.x , cy = tem_pos.y, step_cnt = tem_pos.step + 1;
    for(int i =0;i<4;++i){
      dx = cx + dires[i][0], dy = cy + dires[i][1];
      if(!accessible(dx,dy,step_cnt) || m_vis[dx][dy])   continue;

      m_tile_fa[dx][dy] = i;
      has_work_to_do(dx,dy,step_cnt,npc_in_block_type);
      m_vis[dx][dy] = true;
      
      bfs_q.push({dx,dy,step_cnt});
    }
  }
  // for(int i =0;i<WORK_TYPE_CNT;++i){
  //   LOG("%d %d",i,hit_work[i]);
  // }
  for(int i =0;i< WORK_TYPE_CNT;++i){
    if(hit_work[i]){
      assign_npc_a_move_work(target_npc);
      LOG("assign npc a work %d",i);
      return true;
    }
  }
  LOG("no work can move to do ^-^");
  return false;
}
bool Map::idle_npc_recollect_property_for_stage(NPC& target_npc){
  INT32 prow = target_npc.get_cur_row(), pcol= target_npc.get_cur_col();
  assert(m_tile_type[prow][pcol][1] == TileType::T_stage_area);
  Stage* target_stage = static_cast<Stage*>(m_tile_ptr_table[prow][pcol][1]);
  assert(target_stage);
  // assert(target_stage->can_collect());
  if(target_stage->can_collect()){
    // assert(0);
    return true;
    target_stage->collect();
  }
  return false;
}

// this function is efficient while the bfs traversing is ended!
bool Map::assign_npc_a_move_work(NPC& target_npc){
  INT32 dx, dy, sx = target_npc.get_cur_row(), sy = target_npc.get_cur_col();
  LOG("begin assign npc a work");
  OutputDebugString("begin assign npc a work");
  for(int i =1;i< WORK_TYPE_CNT;++i){
    if(hit_work[i]){
      LOG("assign npc work : work type: %d",i);
      dx = find_work[i].dx, dy = find_work[i].dy;
      OutputDebugString("begin recurrent path");
      LOG("before recurrent path %d %d %d",dx,dy,
      find_work[i].step);
      // assert(0);
      recurrent_path_from_src_to_dest(sx,sy,
      dx,dy,find_work[i].step,target_npc);

      OutputDebugString("recurrent path");
      // if(i == TO_BUILD_WALL){
      //   assert(target_npc.get_path_len() > 0);
        
      //   target_npc.set_path_len(target_npc.get_path_len() - 1);
      // }
      assert(target_npc.get_link_target_row() == 0
      && target_npc.get_link_target_col() == 0);


      target_npc.add_link_target(find_work[i].dx, find_work[i].dy);
      OutputDebugString("assign work add link target!!!");
      m_tile_ptr_table[sx][sy][0] = NULL;
      assert(m_tile_ptr_table[dx][dy][0] == NULL);
      m_tile_ptr_table[dx][dy][0] = &target_npc;
      assert(m_tile_type[dx][dy][0] == TileType::T_grass);
      m_tile_type[dx][dy][0] = TileType::T_npc_walking;
      LOG("assign dx %d dy %d",dx,dy);
      assert(m_tile_type[sx][sy][0] == TileType::T_npc_idle); 
      m_tile_type[sx][sy][0] = TileType::T_grass;
      switch(i){
        case TO_BUILD_WALL:{
          target_npc.begin_move_to_work(NPC::NPC_To_Build,*this);
        }break;
        case TO_CUT_TREE:{
          target_npc.begin_move_to_work(NPC::NPC_To_Cut,*this);
          Tree* target_tree = static_cast<Tree*>(m_tile_ptr_table[dx][dy][1]);
          assert(target_tree);
          target_tree->wait_to_cut();
          LOG("npc to cut tree");
        }break;
        case TO_COLLECT_STAGE:{
          target_npc.begin_move_to_work(NPC::NPC_To_Collect_Stage,*this);
        }break;
        case TO_PICK_PROPERTY:{
          target_npc.begin_move_to_work(NPC::NPC_To_Pick_Property,*this);
          
        }break;

        case TO_STOCK_PROPERTY:{
          assert(m_tile_type[sx][sy][1] == TileType::T_property);
          target_npc.begin_move_to_work(NPC::NPC_To_Stage_For_Stock_Property,*this);
        }break;

        case TO_STAGE_FOR_BUILD:{
          assert(m_tile_type[sx][sy][1] == TileType::T_grass ||
          m_tile_type[sx][sy][1] == TileType::T_stage_area ||
          m_tile_type[sx][sy][1] == TileType::T_wall ||
          m_tile_type[sx][sy][1] == TileType::T_to_build_wall);
          target_npc.begin_move_to_work(NPC::NPC_To_Stage_Prepare_To_Build_Wall,*this);
          LOG("assign to stage for build!!!");
          OutputDebugString("assgin to stage for build!!!");
          LOG("to stage for build :%d %d step: %d",find_work[TO_STAGE_FOR_BUILD].dx,
          find_work[TO_STAGE_FOR_BUILD].dy,
          find_work[TO_STAGE_FOR_BUILD].dx);
          // assert(0);
        }break;

        default:
        LOG("assign npc move work exception %d",i);
        assert(0);
      }
      return true;
    }
  }
  LOG("no work to ssign");
  return false;
}

Map::Idle_NPC_Find_Work_Type Map::has_work_to_do(INT32 prow,INT32 pcol,INT32 step_cnt,
TileType npc_in_block_type){
  assert(npc_in_block_type == TileType::T_grass || npc_in_block_type == TileType::T_stage_area
  || npc_in_block_type == TileType::T_property || npc_in_block_type == TileType::T_wall
  || npc_in_block_type == TileType::T_to_build_wall);

  if(m_tile_type[prow][pcol][0] != TileType::T_grass) return NO_WORK;

  if(m_tile_ptr_table[prow][pcol][0]){
    LOG("hit 430: row: %d col : %d \n%d %d", prow, pcol,
    m_tile_type[prow][pcol][0],m_tile_type[prow][pcol][0]);

  }
  assert(m_tile_ptr_table[prow][pcol][0] == NULL);
  // LOG("check the no person block");
  switch(m_tile_type[prow][pcol][1]){
    case TileType::T_grass:return NO_WORK;

    case TileType::T_property:{
      if(npc_in_block_type == TileType::T_property) return NO_WORK;
      if(hit_work[TO_PICK_PROPERTY] == FALSE){
        hit_work[TO_PICK_PROPERTY] = TRUE;
        find_work[TO_PICK_PROPERTY] = {prow,pcol,step_cnt};
      }
      return TO_PICK_PROPERTY;
    }break;

    case TileType::T_tree:{
      LOG("meet a tree");
      Tree* target_tree = static_cast<Tree*>(m_tile_ptr_table[prow][pcol][1]);
      if(hit_work[TO_CUT_TREE] == FALSE && target_tree->get_state() == Tree::CUT_SELECTED){
        hit_work[TO_CUT_TREE] = TRUE;
        find_work[TO_CUT_TREE] = {prow,pcol,step_cnt};
      }
      return TO_CUT_TREE;
    }break;

    case TileType::T_stage_area:{
      Stage* target_stage = static_cast<Stage*>(m_tile_ptr_table[prow][pcol][1]);
      assert(target_stage);
      if(npc_in_block_type == TileType::T_property
      && target_stage->full() == false){
        if(hit_work[TO_STOCK_PROPERTY] == FALSE){
          hit_work[TO_STOCK_PROPERTY] = TRUE;
          find_work[TO_STOCK_PROPERTY]=    {prow,pcol,step_cnt};
          LOG("add pick property to the work list");
        }
        // LOG("to pick prop to stage");
        return TO_STOCK_PROPERTY;
      }
      if(target_stage->can_collect()){
        // if(hit_work[TO_COLLECT_STAGE] == FALSE){
          hit_work[TO_COLLECT_STAGE] = TRUE;
          find_work[TO_COLLECT_STAGE] = {prow,pcol,step_cnt};
        // }
        return TO_COLLECT_STAGE;
      }else if(target_stage->empty() == false &&
        (npc_in_block_type == TileType::T_grass || npc_in_block_type == TileType::T_wall
        || (npc_in_block_type == TileType::T_stage_area && npc_in_empty_stage))
        || npc_in_block_type == TileType::T_to_build_wall){
        
        if(hit_work[TO_STAGE_FOR_BUILD] == FALSE){
          hit_work[TO_STAGE_FOR_BUILD] = TRUE;
          find_work[TO_STAGE_FOR_BUILD] = {prow,pcol,step_cnt};
        }
        return TO_STAGE_FOR_BUILD;
      }
    }break;
    
    case TileType::T_to_build_wall:{
      // TODO
      LOG("to build wall!!!");
      switch(npc_in_block_type){
        case TileType::T_stage_area:{
          Stage* target_stage = static_cast<Stage*>(m_tile_ptr_table[npc_src_row][npc_src_col][1]);
          assert(target_stage);
          if(target_stage->empty()) {
            LOG("in stage area no work used %d",target_stage->get_used());
            // assert(0);
            return NO_WORK;
          }
        }
        case TileType::T_property:{
          Wall* target_wall = static_cast<Wall*>(m_tile_ptr_table[prow][pcol][1]);
          assert(target_wall);
          assert(target_wall->get_state() == Wall::BUILD_SEL || target_wall->get_state()
          == Wall::TO_BUILD);

          // INT32 dx = find_work[TO_BUILD_WALL].dx, dy = find_work[TO_BUILD_WALL].dy;
          // Wall* pre_wall = static_cast<Wall*>(m_tile_ptr_table[dx][dy][1]);
          // assert(pre_wall);
          // replace;
          if(hit_work[TO_BUILD_WALL] == FALSE){
            hit_work[TO_BUILD_WALL] = TRUE;
            find_work[TO_BUILD_WALL]= {prow,pcol,step_cnt};

          }

          return TO_BUILD_WALL;

        }
        default:
        break;
      }
    }break;



    default:
    LOG("has work to do error: %d",m_tile_type[prow][pcol][1]);
    assert(0);
  }
  return NO_WORK;
}

void Map::recurrent_path_from_src_to_dest(INT32 sx,INT32 sy,INT32 dx,INT32 dy,INT32 step_cnt,
NPC& target_npc){
  assert(sx == target_npc.get_cur_row() 
  && sy == target_npc.get_cur_col());
  // from dx ,dy , recover to sx, sy;
  assert(step_cnt > 0);
  target_npc.set_path_len(step_cnt);
  INT32* path_vector = target_npc.get_to_dest_vector(), fa=0;
  assert(path_vector);
  OutputDebugString("recurrent begin loop");
  assert(m_tile_fa);
  while(sx != dx || sy != dy){
    fa = m_tile_fa[dx][dy];
    assert(step_cnt > 0);
    path_vector[--step_cnt] = m_tile_fa[dx][dy];
    dx -= dires[fa][0], dy -= dires[fa][1];
  }
  assert(step_cnt == 0);

}


void Map::gen_property(INT32 prow,INT32 pcol){
  assert(m_tile_type[prow][pcol][1] == TileType::T_tree);
  assert(m_pproperty_set);
  m_pproperty_set->push(Property{prow,pcol});
  LOG("push property");
  m_tile_type[prow][pcol][1] = TileType::T_property;
  m_tile_ptr_table[prow][pcol][1] = &m_pproperty_set->get_back();
  LOG("in map : end gen property");

}

bool Map::idle_npc_recut_tree(NPC& target_npc){
  INT32 prow = target_npc.get_cur_row(), pcol = target_npc.get_cur_col();
  assert(m_tile_type[prow][pcol][1] == TileType::T_tree);
  Tree* target_tree = static_cast<Tree*>(m_tile_ptr_table[prow][pcol][1]);
  assert(target_tree);
  target_tree->begin_cut();
  target_npc.begin_cut();
  return true;
}

bool Map::accessible(INT32 x,INT32 y,INT32 step_cnt){
  if(x < 1 || x > ABSTRACT_MAP_H_C || y < 1 || y > ABSTRACT_MAP_W_C)  return false;
  switch(m_tile_type[x][y][0]){
    case TileType::T_grass:
    break;
    default:return false;
  }
  switch(m_tile_type[x][y][1]){
    case TileType::T_grass:
    case TileType::T_property:
    case TileType::T_stage_area:
    case TileType::T_to_build_wall:
    return true;

    case TileType::T_tree:{
      Tree* target_tree= static_cast<Tree*>(m_tile_ptr_table[x][y][1]);
      assert(target_tree);
      if(hit_work[TO_CUT_TREE] == FALSE && target_tree->get_state() == Tree::CUT_SELECTED){
        INT32 fa_x,fa_y;
        for(int i=0;i< 4;++i){
          fa_x = x - dires[i][0], fa_y = y - dires[i][1];
          if(fa_x > 0 && fa_x <= ABSTRACT_MAP_H_C && fa_y > 0 && fa_y <= ABSTRACT_MAP_W_C
          && m_vis[fa_x][fa_y]){
            m_tile_fa[x][y] = i;
            break;
          }
        }
        hit_work[TO_CUT_TREE] = TRUE;
        find_work[TO_CUT_TREE] = {x,y,step_cnt};
        OutputDebugString("add a tree to cut");
      }
    }break;
    default:
    return false;
  }
  return false;
}

void Map::add_npc_set(NPC_Set* npc){
  npc->add_set_to_map(*this);
  m_pnpc_set = npc;
}
void Map::add_chi_set(Chi_Set* chickens){
  m_pchicken_set = chickens;
}
void Map::add_tree_set(Tree_Set* tree){
  tree->add_set_to_map(*this);
  m_ptree_set= tree;
}
// void Map::add_prop_set(Property_Set* prop){
//   prop->add_set_to_map(*this);
//   m_pproperty_set = prop;
// }
void Map::add_wall_set(Wall_Set* wall){
  wall->add_set_to_map(*this);
  m_pwall_set= wall;
}
void Map::add_stage_set(Stage_Set* stage){
  stage->add_set_to_map(*this);
  m_pstage_set = stage;
}