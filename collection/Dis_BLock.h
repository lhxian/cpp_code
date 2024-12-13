#ifndef _DIS_BLOCK_H_
#define _DIS_BLOCK_H_

class DisBlock{
public:
  enum Tile_BLock_State{
    /*Block_Updating,*/ Block_updated,/* Block_Drawing,*/Block_Drawn,// Block_unavailable
    // Block_NPC_Change_Direction
  };
protected:
  // Tile_BLock_State m_block_state = Block_updated;
public:
};

#endif // _DIS_BLOCK_H_