#ifndef _TREE_H_
#define _TREE_H_
#include"sys_header.h"
#include<fstream>
#include"Dis_BLock.h"
#include"Updatable_Block.h"
#include"set_template.h"

class Map;
class NPC;

class Tree final: public UpdatableBlock{
public:
  enum Tree_State{
    NO_CUT, CUT_SELECTED ,WAIT_TO_CUT, ON_CUTTING, INVALID
  };
protected:
  Tree_State m_state = NO_CUT;
  INT32 cut_timer_num = 64;

  static gs::Image* tree_image;
  static gs::Pen* tree_sel_pen, * tree_wait_pen;

  static constexpr INT32 tree_image_w = 32, tree_image_h = 32,
  tree_cutting_consume_time = 64, tree_cut_interval = 8;

public:
  Tree():UpdatableBlock(0,0){}
  Tree(INT32 p_row, INT32 p_col);
  ~Tree() = default;

  void DisplayOnce(gs::Graphics& pg);
  void UpdateOnce(Map& game_map);
  void Write_To_File(std::ofstream& ofs);
  void Load_From_File(std::ifstream& ifs);

  bool valid(){return m_state != INVALID;}

  void cut_select();
  void begin_cut();
  void end_cut(Map& game_map);
  void abort_cut(Map& game_map);
  void wait_to_cut();

  // get function
  Tree_State get_state(){return m_state;}


  static void Init();
  static void Free();
};

#ifndef TREE_SET
#define TREE_SET
using Tree_Set = T_SET<Tree,512>;
#endif


template <>
void Tree_Set::add_set_to_map(Map& game_map);

#endif // _TREE_H_