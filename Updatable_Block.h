#ifndef _UPDATABLE_BLOCK_H_
#define _UPDATABLE_BLOCK_H_

#include"sys_header.h"
#include"macro.h"
#include"Dis_BLock.h"
#define MAX_FILE_READ_BUF_LEN 2048
#define NUM_LEN 3

extern const WCHAR* global_num_string;

class UpdatableBlock: public DisBlock{
protected:
  INT32 cur_row, cur_col, link_target_row = 0, link_target_col = 0;
  INT32 src_x = 0, src_y = 0;
  gs::Rect disrect;
  gs::PointF num_display_point;
public:
  // UpdatableBlock() = delete;
  UpdatableBlock() = delete;
  // UpdatableBlock(const UpdatableBlock& copy) = delete;
  UpdatableBlock(INT32 prow,INT32 pcol):
  cur_row(prow),cur_col(pcol),disrect(pcol* TILE_H,prow* TILE_W,TILE_W,TILE_H)
  {}

  ~UpdatableBlock();

  INT32 get_cur_row(){return cur_row;}
  INT32 get_cur_col(){return cur_col;}
  INT32 get_link_target_row(){return link_target_row;}
  INT32 get_link_target_col(){return link_target_col;}
  void add_link_target(INT32 link_row,INT32 link_col){
    assert(link_target_row == 0 && link_target_col == 0);
    link_target_row = link_row, link_target_col = link_col;
  }

  static gs::Font* global_font_16, * global_font_8;
  static gs::SolidBrush* global_num_brush;
  static gs::Pen * global_pen;

  // share for every element in the game;
  static UINT8* read_file_buf[MAX_FILE_READ_BUF_LEN];
  static void write_num_to_wchar(WCHAR* ch,INT32 num);

  static void Updatable_Block_Init();
  static void Updatable_Block_Free();

  static INT32 check_index;
  
  static constexpr INT32 MAX_SLEEP_TIMES = 8;

};

#endif // _UPDATABLE_BLOCK_H_