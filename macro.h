#ifndef _MACRO_H_
#define _MACRO_H_

// npc default vector reserve size
#define DEF_NPC_R_SIZE 64
#define update_velocity 36
#define MOVING_VELOCITY 2

//resource parent path
#define RES_PATH L"./res/"
#define AR_PATH  "./archive/"
#define FONT_CONSOLA RES_PATH L"CONSOLAB.TTF"

#define DIS_FONT_SIZE 16
// const numbers
// max thread count;
#define MAXN_THREAD 8

#define ABSTRACT_MAP_W_C  36
#define ABSTRACT_MAP_H_C  22

#define TILE_W            32 
#define TILE_H            TILE_W
#define ABSTRACT_MAP_W    (TILE_W * (ABSTRACT_MAP_W_C + 2))
#define ABSTRACT_MAP_H    (TILE_W * (ABSTRACT_MAP_H_C + 2))

#define AA_MAP_WC         (ABSTRACT_MAP_W_C + 2)
#define AA_MAP_HC         (ABSTRACT_MAP_H_C + 2)
// diswin
#define BAR               64
#define DEF_DIS_W         (ABSTRACT_MAP_W)
#define DEF_DIS_H         (ABSTRACT_MAP_H)
#define DEF_GAME_W        (ABSTRACT_MAP_W + BAR)
#define DEF_GAME_H        (ABSTRACT_MAP_H)

#define Iabs(a) ((a) < 0 ? (-(a)) : (a))
#define Isquare(a) ((a) * (a))
#define Imanha_d(x1,y1,x2,y2) (Iabs(x1-x2) + Iabs(y1 - y2))
#define Euclid_d(x1,y1,x2,y2) (Isquare(x1 - x2) + Isquare(y1 - y2))


//NPC deviation
#define DEVI_X(row_x,i_w) ((row_x)*(TILE_W)-((i_w)-(TILE_W))/2)
#define DEVI_Y(row_y,i_h) ((row_y)*(TILE_H)-((i_h)-(TILE_H))/2)

// debug macro;
// #define NDEBUG
// #define DEBUG
#ifdef DEBUG
#define Init_debug do{\
  AllocConsole(); \
  freopen("CONOUT$","r",stdin); \
  freopen("CONOUT$","w",stdout);\
  freopen("CONOUT$","w",stderr);\
}while(0)
#define Close_debug do{\
  FreeConsole();\
}while(0)

#define LOG(a,...) printf(a "\n",__VA_ARGS__)
#define LOGCHAR(a) putchar(a)
#define LOGSTR(a) printf(a)
#define COLOR_(color) printf("\033[" ## #color ##  "m")
#define RESET printf("\033[0m")
#define COLORCHAR(a,color) do{\
COLOR_(color);LOGCHAR(a);RESET;\
}while(0)

#define CHECK_EXIT_STATE(a) do{\
a == UpdatableBlock::Block_updated}while(0)

#else
#define LOG(a,...) ((void)0)
#define LOGSTR(a,...) ((void)0)
#define LOGCHAR(a) ((void)0)
#define COLOR_(color) ((void)0)
#define RESET ((void)0)
#define COLORCHAR(color,...) ((void)0)
#define Init_debug ((void)0)
#define Close_debug ((void)0)
#define CHECK_EXIT_STATE(a) ((void)0)
#ifdef  OutputDebugString
#undef  OutputDebugString
#endif
#define OutputDebugString(a) ((void)0)
#define OutputDebugStringA(a) ((void)0)
#endif



enum TileType{
  T_grass, T_stone, T_bone, T_tree, T_water,
  T_animal,

  T_wall, T_to_build_wall, T_on_building_wall,
  T_gate, T_to_build_gate,
  
  T_stage_area,
  T_property,

  T_npc_idle,
  T_npc_work,
  T_npc_walking,
  T_npc_adjust,

  T_cnt
};

typedef TileType TiType_Line[AA_MAP_WC][2];
typedef bool TiVis[AA_MAP_WC];
typedef int TiInt[AA_MAP_WC];
typedef void* TI_Ptr[AA_MAP_WC][2];


#define NPC_LOG_TAG 0x0
#define NPC_SP_TAG 0x1
#define NPC_TAG_MASK 0xf
#define NPC_GROUP_ID(a) ( a & NPC_TAG_MASK)
#define NPC_ID_IN(a) (a >> 4)
#define LOG_ID(index) ((index << 4) | NPC_LOG_TAG)
#define SP_ID(index) ((index << 4) | NPC_SP_TAG)

#define ELE_MASK 0xf
#define ELE_TREE_TAG 0x0
#define ELE_WALL_TAG 0x1
#define ELE_GATE_TAG 0x2
#define TREE_ID(index) ((index << 4) | ELE_TREE_TAG)
#define WALL_ID(index) ((index << 4) | ELE_WALL_TAG)
#define GATE_ID(index) ((index << 4) | ELE_GATE_TAG)


// custom window message;
#define WM_USER_CUT_TREE  (WM_USER + 0x2)
// #define WM_USER_CUT_CANSEL (WM_USER + 0x3)
#define WM_USER_CARRY     (WM_USER + 0x4)
#define WM_USER_STAGE   (WM_USER + 0x5)
#define WM_USER_WALL    (WM_USER + 0x6)
// #define WM_USER_CARRY_CANSEL (WM_USER + 0x5)
// add the diswin pointer to the winproc function to modify the static variable
// #define WM_USER_ADD_DISWIN (WM_USER + 0x6)

// paint the button after user click it!!!
#define WM_USER_PAINT_BUTTON (WM_USER + 0x7)
// reset the button's status
#define WM_USER_RESET_BUTTON (WM_USER + 0x8)


// subwindow's ids
#define ID_DIS_WIN 0xf0
#define ID_CUT_SEL 0xf1
// #define ID_CUT_CANL 0xf3
// #define ID_CARRY 0xf4
#define ID_STAGE_SEL 0xf5
#define ID_WALL_SEL 0xf6


// button position
#define BUTTON_W 64
#define BUTTON_H (BUTTON_W)

#define B_POS_X (DEF_DIS_W)
#define B_POS_CUT_SEL_Y 0
#define B_POS_STAGE_Y (B_POS_CUT_SEL_Y + BUTTON_H)
#define B_POS_WALL_Y (B_POS_STAGE_Y + BUTTON_H)
#define B_POS_CUT_CANC_Y (B_POS_WALL_Y + BUTTON_H)
// #define B_POS_CARRY_Y (B_POS_CUT_CANC_Y + BUTTON_H)


// npc animation frame count
#define NPC_picture_frame_cnt 4
#define CONJUCT(row,col) ((row << 16 ) | (col & 0xffff'0000))
#endif // _MACRO_H