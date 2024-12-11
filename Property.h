#ifndef _PROPERTY_H_
#define _PROPERTY_H_
#include"Updatable_Block.h"
#include"set_template.h"

#define DEF_PRO_VAL 55

#ifndef PROPERTY_SET
#define PROPERTY_SET
class Property;
using Property_Set = T_SET<Property,512>;
#endif

class Property final : public UpdatableBlock{
public:
  INT32 val = DEF_PRO_VAL;
  BOOL exists = TRUE;
  WCHAR val_num_char[4];

public:
  Property():UpdatableBlock(0,0){} 
  Property(INT32 prow,INT32 pcol);
  ~Property() = default;

  INT32 get_val(){return val;}
  INT32 picked(Map& game_map);
  void UpdateOnce(Map& game_map){}
  void DisplayOnce(gs::Graphics& pg);


  static gs::SolidBrush* prop_font_brush;
  static gs::Image* prop_image;
  static void Init();
  static void Free();
};


template <>
void Property_Set::add_set_to_map(Map& game_map);

#endif // _PROPERTY_H_