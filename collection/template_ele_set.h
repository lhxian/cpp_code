#ifndef _TEMPLATE_ELE_SET_H_
#define _TEMPLATE_ELE_SET_H_
#include<cstring>
#include<cassert>

#define DEF_ELE_SET_R_SIZE 256
template <typename T, size_t r_size = DEF_ELE_SET_R_SIZE>

class T_ELE_SET{
  struct List{
    T val;
    struct List* next, * pre;
  };
  List* used_head = nullptr,* free_head = nullptr, *pool = nullptr;

public:
  T_ELE_SET();
  ~T_ELE_SET();
  void remove_one(void* ptr);
  bool add(T&& val);
  void Write_Ele_Set_To_File();
  void Load_Ele_Set_From_File();

};



template <typename T, size_t r_size>
T_ELE_SET<T,r_size>::T_ELE_SET(){
  pool = new List[r_size];
  memset(pool,0,r_size * sizeof(T));
  for(int i =1;i< r_size-1;++i){
    pool[i].next = &pool[i+1];
  }
  for(int i =2;i< r_size;++i){
    pool[i].pre = &pool[i-1];
  }

  free_head = pool + 1;
  used_head = pool;
  assert(used_head->pre == nullptr && used_head->next == nullptr);
}

template <typename T, size_t r_size>
T_ELE_SET<T,r_size>::~T_ELE_SET(){
  delete[] pool; pool = nullptr;
  used_head = nullptr, free_head = nullptr;
}

template <typename T, size_t r_size>
bool T_ELE_SET<T,r_size>::add(T&& val){
  if(free_head == nullptr)  return false;
  assert(free_head->pre == nullptr);

  List* tem = free_head->next;
  free_head->next = used_head;
  used_head->pre = free_head;

  free_head->val = val;

  used_head = free_head;
  free_head = tem;
  used_head->pre = nullptr;

  return true;
}
template <typename T, size_t r_size>
void T_ELE_SET<T,r_size>::remove_one(void* ptr){
  assert(ptr);
  List* remove_ptr = static_cast<List*>(ptr);
  List* p_pre = remove_ptr->pre, * cur = remove_ptr->next;
  assert(p_pre);
  p_pre->next = cur;
  cur->pre = p_pre;

  remove_ptr->next = free_head;
  free_head = remove_ptr;
}



#endif // _TEMPLATE_ELE_SET_H_