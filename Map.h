#ifndef _MAP_H_
#define _MAP_H_
#include"sys_header.h"
#include<vector>
#include"macro.h"
#include"Chicken.h"
#include"NPC.h"
#include"Property.h"
#include"Tree.h"
#include"Wall.h"
#include"Stage.h"

#define BLOCK_SIZE 16
#define MAX_CUT_TREE_SEL_NUM 128
#define RESERVE_MAX 256
class DisWin;


// typedef bool Tree_cut_Tag[AA_MAP_WC];
typedef bool Ele_Tag[AA_MAP_WC];
typedef struct{
  INT32 row, col;
}Grid_Point;
using VI32 = std::vector<INT32>;
using VGP = std::vector<Grid_Point>;

struct Tile_Node{
  INT32 x,y,step,d;
  bool operator < (const Tile_Node& a) const{
    return step + d < a.step + a.d;
  }
};
class Ti_Heap{
  int len;
  Tile_Node heap[AA_MAP_HC * AA_MAP_WC];
public:
  Ti_Heap():len(0){}
  Tile_Node& top(){return heap[1];}
  void clear(){len = 0;}
  void push(Tile_Node node);
  void pop();
  bool empty(){return len <= 0;}
};



struct P{
  int x,y,step;
};
class TiQ{
  P q[AA_MAP_HC * AA_MAP_WC];
  int front = 0,back= -1;
public:
  TiQ() = default;
  inline bool empty(){ return front > back;}
  void push(P p){q[++back] = p;}
  void pop(){ ++front;}
  P& top_back(){return q[back];}
  P& top_front(){return q[front];}
  void clear(){front = 0,back = -1;}
};


enum Map_itype{
  i_water, i_vertical_water,
  i_grass_1 ,i_grass_2,
  i_grass_edge_top, i_grass_edge_bottom, i_grass_edge_left, i_grass_edge_right,
  i_grass_angle_1, i_grass_angle_2, i_grass_angle_3, i_grass_angle_4,

  i_map_cnt
};

void Analyse(gs::Status a);







class Map{
public:
  enum Idle_NPC_Find_Work_Type{
    NO_WORK,TO_BUILD_WALL, TO_COLLECT_STAGE, TO_PICK_PROPERTY,
    TO_STOCK_PROPERTY, TO_CUT_TREE, TO_STAGE_FOR_BUILD,
    WORK_TYPE_CNT
  };
  struct Work{
    INT32 dx, dy, step;
    // Idle_NPC_Find_Work_Type work_type;
  };
protected:
  INT32 m_w, m_h;
  gs::Graphics* p_graphics;
  HDC m_mem_dc = 0;
  HBITMAP m_bmp = 0;

  HANDLE map_update_thread_handle = 0;

  TiType_Line* m_tile_type = NULL;
  TiInt* m_tile_fa = NULL;
  TiVis* m_vis = NULL;
  TI_Ptr* m_tile_ptr_table = NULL;

  NPC_Set* m_pnpc_set = NULL;
  Chi_Set* m_pchicken_set = NULL;

  Property_Set* m_pproperty_set = NULL;
  Wall_Set* m_pwall_set = NULL;
  Tree_Set* m_ptree_set = NULL;
  Stage_Set* m_pstage_set = NULL;




  BOOL map_update_working = FALSE;
  TiQ bfs_q;

  BOOL hit_work[WORK_TYPE_CNT] = {0};
  BOOL npc_in_empty_stage = FALSE;
  Work find_work[WORK_TYPE_CNT] = {0};

  INT32 npc_src_row = 0, npc_src_col = 0;
  // used in the function has_work_to_do(), judge when the npc in block type is TileType::T-stage

  // vector of the add wall or stage, add them in the function update_all
  VGP to_add_wall, to_add_stage;

public:
  Map(INT32 w = DEF_DIS_W, INT32 h = DEF_DIS_H,HDC diswin_memdc =0);
  ~Map();

  void write_to_file(const char* file_name);
  void add_npc_set(NPC_Set* npc);
  void add_chi_set(Chi_Set* chickens);
  void add_tree_set(Tree_Set* tree);
  void add_prop_set(Property_Set* prop){
    prop->add_set_to_map(*this);
    m_pproperty_set = prop;
  }
  
  void add_wall_set(Wall_Set* wall);
  void add_stage_set(Stage_Set* stage);

  HDC get_dc(){return m_mem_dc;}
  HANDLE get_update_proc_handle(){return map_update_thread_handle;}
  TiType_Line* get_map_type_table(){return m_tile_type;}
  TI_Ptr* get_ptr_table(){return m_tile_ptr_table;}

  void show_map();
  
  bool add_trees_to_cut(INT32 top,INT32 bottom,INT32 left,INT32 right);
  bool add_stage(INT32 top,INT32 bottom,INT32 left,INT32 right);
  void update_to_add_stage_list();
  bool cancel_trees(INT32 top,INT32 bottom,INT32 left,INT32 right);
  bool add_wall_to_build(INT32 prow,INT32 pcol);
  void update_to_build_wall_list();
  // gen the process after a tree was completely cut!!!
  void gen_property(INT32 prow,INT32 pcol);

  // void cancel_cut();
  bool carry_to_stage(Property& target_prop);
  bool property_find_npc(Property& target_prop);
  bool npc_find_stage_for_property(NPC& pnpc);
  bool idle_npc_find_work(NPC& target_npc);
  bool idle_npc_move_to_find_work(NPC& target_npc);
  bool idle_npc_recollect_property_for_stage(NPC& target_npc);
  bool idle_npc_recut_tree(NPC& target_npc);
  Idle_NPC_Find_Work_Type has_work_to_do(INT32 prow,INT32 pcol,INT32 step_cnt,
  TileType npc_in_block_type);
  void recurrent_path_from_src_to_dest(
    INT32 sx,INT32 sy,INT32 dx,INT32 dy,INT32 step_cnt,NPC& target_npc
  );
  bool assign_npc_a_move_work(NPC& target_npc);
  // discard;
  void recurrent_path_from_dest_to_src(
    INT32 sx,INT32 sy,INT32 dx,INT32 dy,INT32 step_cnt,NPC& target_npc
  );
  // bool cancel_carry();

  bool accessible(INT32 x,INT32 y,INT32 step_cnt);

  void enable_update();
  void end_update();
  void update_all();


  // adjust the npc int current pos
  bool Adjust_NPC(INT32 prow,INT32 pcol);

    



  static gs::Image* map_images[Map_itype::i_map_cnt];
  static constexpr INT32 grass_w = 32,grass_h = 32,
  grass_i_x= 112, grass_i_y = 144,
  water_w = 64, water_h = 32,
  vertical_water_w = 32, vertical_water_h = 32,
  grass_edge_horizontal_w = 32, grass_edge_horizontal_h = 16,
  grass_edge_vertical_w = 16 ,grass_edge_vertical_h = 32,
  grass_angle_w = 16, grass_angle_h = 16;
  // static constexpr DWORD map_update_velocity = 96;

  //static function
  static void Init();
  static void Free();
  static DWORD map_update_function(LPVOID param);
};

// void show_map();



#endif // _MAP_H_