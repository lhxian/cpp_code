#ifndef _STAGE_H_
#define _STAGE_H_
#include"Updatable_Block.h"
#include"set_template.h"

#define DEF_STAGE_CAPACITY 75

#include<fstream>

#ifndef STAGE_SET
#define STAGE_SET
class Stage;
using Stage_Set = T_SET<Stage>;
#endif

class Stage final: public UpdatableBlock{
  friend Stage_Set;
protected:
  INT32 used = 0, no_collect = 0;
  gs::PointF no_collect_dis_point;
  WCHAR dis_used[4], dis_no_collect[4];

public:
  Stage():UpdatableBlock(0,0){}
  Stage(INT32 prow,INT32 pcol);
  ~Stage() =default;
  void UpdateOnce(Map& game_map);
  void DisplayOnce(gs::Graphics& pg);

  void add_stock_include_no_collect(INT32 augmentation = 0);
  void collect();
  
  void write_to_file(std::ofstream& ofs);
  void load_from_file(std::ifstream& ifs);
  bool full(){return DEF_STAGE_CAPACITY == used;}
  bool empty(){return used == 0;}
  void update_wchar();
  bool can_collect(){
    return used < DEF_STAGE_CAPACITY && no_collect > 0;
  }
  INT32 get_used(){return used;}
  INT32 get_rest(){return DEF_STAGE_CAPACITY - used;}
  INT32 get_no_collection(){return no_collect;}

  INT32 moveto_used();

  static constexpr INT32 stage_capacity = 100;
  static gs::SolidBrush* stage_font_brush;
  static gs::Pen* stage_rect_pen ;
  static constexpr INT32 stage_image_w = 32, stage_image_h = 32;
  static constexpr  gs::REAL stage_num_deviate = 16;
  static void Init();
  static void Free();
};

template <>
void Stage_Set::add_set_to_map(Map& game_map);

#endif // _STAGE_H_