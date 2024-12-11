#ifndef _AR_H_
#define _AR_H_
#include"Dis_BLock.h"
#include"NPC.h"

struct Ar_NPC{
  DisBlock::Tile_BLock_State block_state;
  NPC::Direction last_directon;
  NPC::NPC_State m_npc_state;
  int m_target_obj_index;
  INT32 src_x,src_y,row,col;
  INT32 disrect_x ,disrect_y;
  INT32 walking_frame_count, step_count;
  INT32 walking_vector_size;
  

};
#endif // _AR_H_