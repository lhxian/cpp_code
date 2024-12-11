#ifndef _NPC_H_
#define _NPC_H_
#include"sys_header.h"
#include"macro.h"
#include<vector>
#include"Dis_BLock.h"
#include"Updatable_Block.h"
#include"set_template.h"

class NPC;
#ifndef NPC_SET
#define NPC_SET
using NPC_Set = T_SET<NPC>;
#endif

enum NPC_IMG_TYPE{
  I_WALKING, I_CARRY, I_CUTTING, I_GOLD, 
  I_NPC_CNT
};

class Map;
class Tree;

using NPC_PATH_VECTOR = INT32 [AA_MAP_HC * AA_MAP_WC];

class NPC final:public UpdatableBlock{
public:
  enum Direction{
    Right,Left,Down,Up
  };
  enum NPC_State{
    /*
    to is in walking !!!
    on is in stop for doing something !!!
    */

    NPC_Idle,
    // cut tree
    NPC_To_Cut, // walking
    NPC_On_Cutting,
    // move the property
    NPC_To_Pick_Property, // walking
    NPC_On_Pick_Property,
    NPC_To_Stage_For_Stock_Property, // walking
    // NPC_On_Release_Property_To_Stock,
    // collect the stage
    NPC_To_Collect_Stage, // walking
    NPC_On_Collecting_Stage,
    // build
    NPC_To_Stage_Prepare_To_Build_Wall,
    NPC_To_Build, // walking
    NPC_On_Building,

    NPC_Adjust_Stop
  };
protected:

  Direction m_direction = Up;
  NPC_State m_state = NPC_Idle;
  INT32 walking_frame_count = 0,step_count =0,
  m_slow_count = 0; // record the display fresh count while the npc is in idle state;
  // this variable is also used in the condition where the npc is traversing the gate
  // to slow the animation of the npc walking;
  BOOL in_gate = FALSE;
  INT32 idle_time_elapse = 0;
  // frame_count, the frame(usually 4 photos) a NPC walking to another grid spend 
  // step_count, the step the NPC has walked, must less than the size of the path vector
  NPC_PATH_VECTOR to_dest_path;// back_src_path;
  INT32 path_length = 0,
  carry_property = 0;

public:
  NPC():UpdatableBlock(0,0){}
  NPC(Direction init_dire,
  INT32 px,INT32 py );

  NPC_State get_state(){return m_state;}
  INT32* get_to_dest_vector(){return to_dest_path;}
  // INT32* get_back_src_vector(){return back_src_path;}
  INT32 get_path_len(){return path_length;}

  void ChangeDirection(Map& game_map);
  void UpdateOnce(Map& game_map) ; // universal
  void DisplayOnce(gs::Graphics& pg);

  void Adjust_normal_walking_Y();
  void Adjust_carry_walking_Y();
  void Adjust_walking_Y(){Adjust_normal_walking_Y();}
  void Adjust_cut_Y();
  void Adjust_idle_Y();

  void move_a_bit(Map& game_map); // universal;
  void Walking_Display(gs::Graphics& pg);
  void Display_Carry_Meterial(gs::Graphics& pg);
  void Idle_Display(gs::Graphics& pg);


  void set_path_len(INT32 p_len){path_length = p_len;}
  // void begin_working(NPC::NPC_State working_state);
  void begin_move_to_work(NPC::NPC_State move_to_working_state,Map& game_map);

  void begin_cut();
  void end_cut(Map& game_map);
  void begin_build();
  void end_build(Map& game_map);

  void begin_pick_prop(Map& game_map);


  void traverse_door(Map& game_map);
  void prepare_move(Map& game_map);

  void reach_dest(Map& game_map);

  bool idle_find_work(Map& game_map);

  void adjust_after_working();
  bool Leave_Cur_Pos_To_Adjust(Map& game_map);
  void analyse_direction();


  static void Init();
  static void Free();

  // static gs::Image* npc_cutting_image, * npc_walking_image,
  // * npc_carry_image, * npc_carry_material_image;
protected:
  static gs::Image* npc_images[I_NPC_CNT];
  static TI_Ptr* map_ptr_table;
  static TiType_Line* map_tile_type;
  
public:

  static constexpr INT32 npc_walking_frame_cnt = 16,
  npc_slow_interval = 2,
  npc_traverse_gate_frame_cnt = 32,

  npc_cut_iblock_w = 32, npc_cut_iblock_h= 32,
  npc_walking_iblock_w = 16, npc_walking_iblock_h = 32,

  npc_walking_image_w = 64, npc_walking_image_h = 128,
  npc_cutting_image_w = 128, npc_cutting_image_h = 128,

  npc_gold_image_w = 16, npc_gold_image_h = 16,

  npc_gold_deviate_x = 8,

  npc_max_idle_time = 32;

  // static TiVis* npc_vis;
};


template<>
void NPC_Set::add_set_to_map(Map& game_map);

#endif // _NPC_H_