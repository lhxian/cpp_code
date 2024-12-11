#ifndef _ANIMAL_H_
#define _ANIMAL_H_
#include<Windows.h>
#include<gdiplus.h>
#include<vector>
#include<fstream>
#include<cassert>
#include"Dis_BLock.h"
namespace gs = Gdiplus;
class Map;
class Animal{
protected:
  INT32 row, col,m_direction,src_x,src_y,
  an_frame_count,still = 0;
  gs::Rect disrect;
public:
  Animal() = delete;
  Animal(INT32 prow,INT32 pcol):
  row(prow),col(pcol),an_frame_count(0){}
};

#define AnT_h
template <typename Ani_T,size_t r_size>
class Animal_Set{
protected:
  enum AN_SET_STATE{ANI_UPDATED ,ANI_ON_UPDATING, ANI_DRAWN, ANI_ON_DRAWING,
   ANI_NONE_TASK}ani_set_state = ANI_UPDATED;

  std::vector<Ani_T> ani_vector;
public:
  Animal_Set(){ani_vector.reserve(r_size);}
  Ani_T& operator[](int index){return ani_vector[index];}
  void display_all(gs::Graphics& pg){
    while(ani_set_state != ANI_UPDATED) Sleep(2);
    ani_set_state = ANI_ON_DRAWING;
    for(auto& it: ani_vector) it.DisplayOnce(pg);
    ani_set_state = ANI_DRAWN;
  }
  void update_all(Map& game_map){
    while(ani_set_state != ANI_DRAWN) Sleep(2);
    ani_set_state = ANI_ON_UPDATING;
    for(auto& it: ani_vector){
      it.UpdateOnce(game_map);
    }
    ani_set_state = ANI_UPDATED;
  }
  void push(Ani_T& a){
    ani_vector.push_back(a);
  }
  void push(Ani_T&& a){
    ani_vector.emplace_back(a);
  }
  void write_to_file(const char* file_name){
    std::ofstream ofs(file_name,std::ios::out | std::ios::binary);
    size_t set_size = ani_vector.size();
    ofs.write(reinterpret_cast<const char*>(&set_size),sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(ani_vector.data()),set_size * sizeof(Ani_T));
    ofs.close();
  }
  void load_from_file(const char* file_name){
    std::ifstream ifs(file_name);
    assert(ifs.is_open());
    size_t set_size=0;
    ifs.read(reinterpret_cast<char*>(&set_size),sizeof(size_t));
    OutputDebugString("chicken size");
    char num[16];
    sprintf(num,"%zu",set_size);
    OutputDebugString(num);

    ani_vector.resize(set_size);
    ifs.read(reinterpret_cast<char*>(ani_vector.data()),set_size * sizeof(Ani_T));
    ifs.close();
  }
};
#endif // _ANIMAL_H_