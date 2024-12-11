#ifndef _SET_TEMPLATE_H_
#define _SET_TEMPLATE_H_


#include<vector>
#include<fstream>
#include"sys_header.h"
#include"Dis_BLock.h"

#define DEF_SET_CAP 256

class Map;

#define TEM_H template <typename T, size_t r_size>

template <typename T, size_t r_size = DEF_SET_CAP>
class T_SET{
public:
  enum SET_STATE{UPDATED ,ON_UPDATING, DRAWN, ON_DRAWING, NONE_TASK};
protected:
  SET_STATE set_state = UPDATED;
  std::vector<T> set_vector;
public:

  T_SET(){set_vector.reserve(r_size);set_state = UPDATED;}
  // ~T_SET();

  void update_all(Map& game_map);
  void display_all(gs::Graphics& pg);

  // TODO
  void add_set_to_map(Map& game_map);
  // void write_set_to_file(const char* file_name);
  void write_to_file(const char* file_name);
  void load_from_file(const char* file_name);

  T& operator[](int index){return set_vector[index];}
  T& get_back(){return set_vector.back();}
  
  void push(T& val){set_vector.push_back(val);}
  void push(T&& val){set_vector.emplace_back(val);}
  void show();
  bool begin_draw();
  bool begin_update();

};

TEM_H
inline void T_SET<T,r_size>::update_all(Map& game_map){
  assert(set_vector.capacity() <= r_size);
  while(set_state != DRAWN) {
    Sleep(2);
    // LOG("set update sleep");
  }
  // LOG("begin update all");
  set_state = ON_UPDATING;
  for(auto& it: set_vector) it.UpdateOnce(game_map);
  // LOG("update finish");
  set_state = UPDATED;
}
TEM_H
inline void T_SET<T,r_size>::display_all(gs::Graphics& pg){
  while(set_state != UPDATED) Sleep(2);
  set_state = ON_DRAWING;
  for(auto& it: set_vector) it.DisplayOnce(pg);
  set_state = DRAWN;
}
TEM_H
inline bool T_SET<T,r_size>::begin_draw(){
  if(set_vector.empty())  return true;
  auto& it = set_vector.begin();
  return it.m_block_state == DisBlock::Block_Drawn;
}

TEM_H
inline bool T_SET<T,r_size>::begin_update(){
  if(set_vector.empty())  return true;
  auto& it = set_vector.begin();
  return it.m_block_state == DisBlock::Block_updated;
}

TEM_H
void T_SET<T,r_size>::write_to_file(const char* file_name){
  size_t set_size = set_vector.size();
  // char num[16];
  // sprintf(num,"%zu",set_size);
  // OutputDebugString(num);
  assert(set_size <= r_size);
  std::ofstream ofs(file_name,std::ios::out | std::ios::binary);
  ofs.write(reinterpret_cast<const char*>(&set_size),sizeof(size_t));
  ofs.write(reinterpret_cast<const char*>(set_vector.data()),set_size * sizeof(T));
  ofs.close();
}

TEM_H
void T_SET<T,r_size>::load_from_file(const char* file_name){
  std::ifstream ifs(file_name,std::ios::in | std::ios::binary);
  assert(ifs.is_open());
  size_t set_size = 0;
  ifs.read(reinterpret_cast<char*>(&set_size),sizeof(size_t));
  char num[16];
  sprintf(num,"%zu",set_size);
  OutputDebugString(num);
  set_vector.resize(set_size);
  ifs.read(reinterpret_cast<char*>(set_vector.data()),set_size * sizeof(T));
  ifs.close();
}


// TEM_H
// inline void T_SET<T,r_size>::write_set_to_file(const char* file_name){
//   std::ofstream ofs(file_name,std::ios::out | std::ios::binary);
//   size_t vector_size = set_vector.size();

//   // write the  count of the elements;
//   ofs.write((CCHAR*)(&vector_size),sizeof(size_t));

//   for(auto& it: set_vector) it.Write_to_File(ofs);
//   ofs.close();
// }



#endif // _SET_TEMPLATE_H_