#ifndef _WALL_H_
#define _WALL_H_
#include"Updatable_Block.h"
#include"set_template.h"

class Wall;
using Wall_Set = T_SET<Wall>;
#define WALL_CONSUME (2 * 75)

class Wall final : public UpdatableBlock{
public:
  enum Wall_State {BUILD_SEL,TO_BUILD,
  ON_BUILDING,BUILT};
private:
  INT32 has_offer_val = 0, num_timer = 64;
  BOOL new_add = TRUE;
  Wall_State m_state = BUILD_SEL;
  WCHAR num_char[4];


public:
  Wall():UpdatableBlock(0,0){}
  Wall(INT32 prow,INT32 pcol);
  ~Wall() = default;
  Wall_State get_state(){return m_state;}
  INT32 get_offer(){return has_offer_val;}

  void add_material(INT32 augmentation=0);
  bool buildable(){return has_offer_val >= WALL_CONSUME;}
  void begin_build(Map& game_map);
  void end_build(Map& game_map);

  void UpdateOnce(Map& game_map);
  void DisplayOnce(gs::Graphics& pg);


  static void Init();
  static void Free();

  static gs::Image* wall_image;
  static gs::Pen* wall_select_pen, * wall_to_pen;
  static gs::SolidBrush* wall_brush;
  static constexpr INT32 wall_image_w = 32, wall_image_h = 32;
};


template <>
void Wall_Set::add_set_to_map(Map& game_map);

#endif // _WALL_H_