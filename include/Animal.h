#ifndef _ANIMAL_H_
#define _ANIMAL_H_
#include<Windows.h>
#include<gdiplus.h>
#include<vector>
#include<fstream>
#include<cassert>
#include<cstring>
// #include"Dis_BLock.h"
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
  INT32 get_row(){return row;}
  INT32 get_col(){return col;}
};

#define AnT_h
template <typename Ani_T,size_t r_size>
class Animal_Set{
protected:
  enum AN_SET_STATE{ANI_UPDATED ,ANI_ON_UPDATING, ANI_DRAWN, ANI_ON_DRAWING,
   ANI_NONE_TASK}ani_set_state = ANI_UPDATED;
  BOOL still_display = TRUE;

  std::vector<Ani_T> ani_vector;
public:
  Animal_Set(){ani_vector.reserve(r_size);}
  ~Animal_Set(){
    OutputDebugStringW(L"destruct the animal set");
  }
  void end_display(){still_display = FALSE;}

  Ani_T& operator[](int index){return ani_vector[index];}
  void display_all(gs::Graphics& pg){
    while(ani_set_state != ANI_UPDATED && still_display) Sleep(2);
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
    assert(still_display == FALSE);
    std::ofstream ofs(file_name,std::ios::out | std::ios::binary);
    size_t set_size = ani_vector.size();
    // WCHAR num[32];
    // wsprintfW(num,L"buf write size %d",set_size * sizeof(Ani_T));
    // OutputDebugStringW(num);
    ofs.write(reinterpret_cast<const char*>(&set_size),sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(ani_vector.data()),set_size * sizeof(Ani_T));
    OutputDebugStringW(L"write the chicken data to file");
    // for(auto& it: ani_vector){
    //   assert(it.m_direction >=0 && it.m_direction < 4);
    //   ofs.write(reinterpret_cast<const char*>(&it),sizeof(Ani_T));
    //   assert(it.m_direction >=0 && it.m_direction < 4);


    // }
    // ofs.write(reinterpret_cast<const char*>(ani_vector.data()),set_size * sizeof(Ani_T));
    ofs.close();
  }
  void load_from_file(const char* file_name){
    std::ifstream ifs(file_name,std::ios::in | std::ios::binary);
    assert(ifs.is_open());
    size_t set_size=0;
    ifs.read(reinterpret_cast<char*>(&set_size),sizeof(size_t));
    OutputDebugString("chicken size");
    // WCHAR num[32];
    // wsprintfW(num,L"size:%d",set_size);
    // OutputDebugStringW(num);

    ani_vector.resize(set_size);
    auto read_count = ifs.read(reinterpret_cast<char*>(ani_vector.data()),set_size * sizeof(Ani_T)).gcount();
    // wsprintfW(num,L"read count:%d sizeof T %d",read_count,sizeof(Ani_T));
    // OutputDebugStringW(num);
    assert(read_count == set_size * sizeof(Ani_T));

    // int i =0;
    for(auto& it: ani_vector) {
      // BUG
      it.m_direction &= 0x3;
      // wsprintfW(num,L"%d",it.get_row());
      // OutputDebugStringW(num);
      // wsprintfW(num,L"%d",it.get_col());
      // OutputDebugStringW(num);
      // assert(it.m_direction >=0 && it.m_direction < 4);
      // if(it.m_direction < 0 || it.m_direction >=4){
      //   wsprintfW(num,L"%d",it.m_direction);
      //   OutputDebugStringW(num);
      //   wsprintfW(num,L"%d",i);
      //   OutputDebugStringW(num);
      //   assert(0);
      // }
      // ++i;
    }
    ifs.close();
  }
};
#endif // _ANIMAL_H_