#include"all.h"
#include"constv.h"
#include"Sp.h"
#include"Chicken.h"
// #include<cassert>
HANDLE g_mutex;
extern HANDLE g_threads[MAXN_THREAD];
extern DWORD g_thread_cnt;
// extern Tree_Set* g_tree_set;
// extern Log_Set*  g_log_set;
// extern TiPtr* tile_ptr_table;
// extern TiLine* tile_type;
void* p_log_test = NULL;
int WINAPI WinMain(
  _In_ HINSTANCE inst,_In_opt_ HINSTANCE pre,
  _In_ LPSTR cmdLine, _In_ int cmdshow
){
  Init_debug;
  g_mutex = CreateMutexA(NULL,FALSE,NULL);
  assert(
    g_mutex
  );
  InitAll(inst,cmdshow,g_mutex);
  DisWin* pwin = new DisWin;
  // Init_Image();
  global_map = new Map(DEF_DIS_W,DEF_DIS_H,pwin->get_memdc());

  global_tree_set = new Tree_Set;
  for(int i =1;i<6;i+= 2){
    for(int j = 1;j< 20;j+= 2){
      global_tree_set->push(Tree{i,j});
    }
  }
  // for(int i = 2;i< 20;i+=2){
  //   global_tree_set->push(Tree{5,i});
  // }
  OutputDebugStringA("push the tree");
  std::vector<Ele_Set*> g_ele_group;

  g_ele_group.push_back(global_tree_set);

  // Log
  Log_Set* log_set = new Log_Set;
  Sp_Set* sp_set = new Sp_Set;
  Chi_Set* chi_set = new Chi_Set;

  chi_set->push(Chicken{10,26});
  chi_set->push(Chicken{6,3});
  chi_set->push(Chicken{6,5});
  chi_set->push(Chicken{11,20});

  const int x = 8,y = 16,sx = 10,sy = 1,x2= 4,y2 = 19;
  log_set->push(Log{NPC::Up,sx,sy});
  sp_set->push(Sp{NPC::Up,x,y});
  sp_set->push(Sp{NPC::Left,x2,y2});
  p_log_test = &log_set[0];
  assert(tile_type[x][y] == TileType::T_npc);
  show_map();

  OutputDebugStringA("push the npc");
  std::vector<NPC_Set*> g_npc_group;
  g_npc_group.push_back(log_set);
  g_npc_group.push_back(sp_set);

  pwin->show();

  pwin->add_map(*global_map);
  pwin->add_Ele_group(g_ele_group);
  pwin->add_NPC_group(g_npc_group);
  pwin->add_chicken(*chi_set);

  OutputDebugStringA("added");
  OutputDebugStringA("show");
  pwin->enable_render();
  OutputDebugStringA("enble render");

  assert((*global_tree_set)[0].cut_tree());
  assert((*global_tree_set)[2].cut_tree());
  assert((*global_tree_set)[6].cut_tree());
  show_map();
  
  MSG msg;
  while(GetMessageA(&msg,NULL,0,0)){
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
  pwin->end_render();
  // WaitForMultipleObjects(g_thread_cnt,g_threads,TRUE,INFINITE);
  CloseAll();
  OutputDebugStringA("close all");
  CloseHandle(g_mutex);
  OutputDebugStringA("quit");
  // Sleep(8000);
  delete global_map;global_map = NULL;
  delete global_tree_set;global_tree_set = NULL;
  delete log_set; log_set = NULL;
  delete sp_set;sp_set = NULL;
  delete pwin;
  pwin = NULL;
  // DisWin::Quit_diswin();
  // OutputDebugStringA("quit diswin");
  Close_debug;
  OutputDebugStringA("close the consola");
  assert(msg.wParam == 0);
  return static_cast<int>(msg.wParam);
}