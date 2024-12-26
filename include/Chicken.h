#ifndef _CHICKEN_H_
#define _CHICKEN_H_

#include"Animal.h"
class Map;
class Chicken;
using Chi_Set = Animal_Set<Chicken,64>;

class Chicken:public Animal{
  friend Chi_Set;
public:
  Chicken():Animal(0,0){}
  Chicken(INT32 px,INT32 py);
  ~Chicken(){
    assert(m_direction >=0 && m_direction < 4);
  }
  void UpdateOnce(Map& pgame_map);
  void DisplayOnce(gs::Graphics& pg);
  void changedirection(int dire);
  void moveAgrid(Map& game_map);  
  void adjustY();
public:
  static constexpr INT32 chi_i_w = 128,
  chi_i_h = 128, chi_iblock_w = 32, chi_iblock_h = 32,
  chi_frame_cnt = 16;
  static gs::Image* chi_img;
  static void chi_Init();
  static void chi_Free();
};

#endif // _CHICKEN_H_