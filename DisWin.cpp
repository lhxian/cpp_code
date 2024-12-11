#include<Windows.h>
#include<gdiplus.h>
#include"DisWin.h"
#include<cassert>
#include<windowsx.h>
#include"macro.h"
#include"Map.h"
#include"NPC.h"
#include"Tree.h"
#include"Wall.h"
#include"Stage.h"
#include"Property.h"
#include"Chicken.h"

// static member;


WNDCLASS DisWin::diswin_share_class = {0};
ULONG_PTR DisWin::gdi_token = 0;
gs::GdiplusStartupInput DisWin::startup_input = {0};
gs::Font* DisWin::global_font = NULL;
gs::SolidBrush* DisWin::global_font_brush = NULL;

// const INT32 update_velocity = 96;
const WCHAR* font_path = {
  RES_PATH L"CONSOLAB.TTF"
};

void DisWin::Init_diswin(){

  diswin_share_class.lpszClassName = "DISWIN";
  diswin_share_class.lpfnWndProc = def_diswin_proc;
  diswin_share_class.hCursor = (HCURSOR)LoadCursorA(NULL,IDC_ARROW);
  diswin_share_class.hInstance = BaseWin::process_inst;
  auto ret = RegisterClassA(&diswin_share_class);
  assert(ret);

  auto status =   gs::GdiplusStartup(&gdi_token,&startup_input,NULL);
  assert( gs::Ok == status);

  global_font = new gs::Font(font_path,DIS_FONT_SIZE); 
  global_font->GetLastStatus();
  assert( 1);
  global_font_brush = new gs::SolidBrush(gs::Color::DarkRed);
}
void DisWin::Quit_diswin(){
  delete global_font;global_font = NULL;
  delete global_font_brush; global_font_brush = NULL;
  gs::GdiplusShutdown(DisWin::gdi_token);
}

// constructor
DisWin::DisWin(INT32 x,INT32 y,INT32 w,INT32 h,HWND parent,HMENU id):
m_x(x),m_y(y),m_w(w),m_h(h),m_id(id),
m_pen(gs::Color::DarkBlue),m_solidBrush(0xffff'ffff),
m_pnpc_set(NULL),render_working(FALSE)
{
  DWORD style = (parent)? WS_CHILD | WS_VISIBLE | WS_BORDER : WS_OVERLAPPEDWINDOW;
  if(!parent)  ++BaseWin::WinCnt;
  assert(parent);
  assert(style == (WS_CHILD | WS_VISIBLE | WS_BORDER));
  hwnd = CreateWindowExA(0,"DISWIN",NULL,style,m_x,m_y,m_w,m_h,
  parent,m_id,BaseWin::process_inst,this);

  assert(
    hwnd
  );

  HDC hdc = GetDC(hwnd);
  mem_dc = CreateCompatibleDC(hdc);
  assert(mem_dc);
  m_bmp = CreateCompatibleBitmap(hdc,m_w,m_h);
  assert(m_bmp);
  SelectObject(mem_dc,m_bmp);
  p_graphics = new gs::Graphics(mem_dc);
  // assert(p_graphics);
  p_graphics->SetSmoothingMode(gs::SmoothingModeAntiAlias);

}
DisWin::~DisWin(){
  assert(p_graphics);
  delete p_graphics;p_graphics = NULL;
  end_render();

  m_pnpc_set = NULL;
  m_pchicken_set = NULL;

  m_pwall_set= NULL; m_pstage_set = NULL;
  m_ptree_set = NULL; m_pprop_set = NULL;

  DeleteObject(m_bmp);
  DeleteDC(mem_dc);
  DestroyWindow(hwnd);
}
void DisWin::show(){
  OutputDebugStringA("begin show");
  assert(hwnd);
  ShowWindow(hwnd,BaseWin::cmdShow);
  OutputDebugStringA("end show");
}
void DisWin::copy_frome_dc(HDC p_mem_dc){
  BitBlt(mem_dc,0,0,m_w,m_h,p_mem_dc,0,0,SRCCOPY);
}
void DisWin::present(){
  HDC hdc= GetDC(hwnd);
  BitBlt(hdc,0,0,m_w,m_h,mem_dc,0,0,SRCCOPY);
  ValidateRect(hwnd,NULL);
  ReleaseDC(hwnd,hdc);
}

void DisWin::render_group(){
  // diswin_timer_count = (diswin_timer_count + 1) & 0x7;
  // assert(m_pele);
  // OutputDebugString("in render group: begin render ele!!!");
  // OutputDebugString("in render group: begin render npc!!!");
  OutputDebugString("begin display tree");
  if(m_ptree_set) m_ptree_set->display_all(*p_graphics);
  // LOG("display tree");
  OutputDebugString("Begin display wall");
  if(m_pwall_set) m_pwall_set->display_all(*p_graphics);
  // LOG("display wall");
  // LOG("begin display stage");
  OutputDebugString("begin display stage");
  if(m_pstage_set)  m_pstage_set->display_all(*p_graphics);
  // LOG("display stage");
  if(m_pprop_set) m_pprop_set->display_all(*p_graphics);

  // OutputDebugStringA("begin display npc");
  if(m_pnpc_set) {
    m_pnpc_set->display_all(*p_graphics);
    // LOG("display npc");
  }
  // OutputDebugString("display all npc!!!");

  if(m_pchicken_set)  m_pchicken_set->display_all(*p_graphics);

  
}
void DisWin::clear(){
  // m_pmap->todiswin(*this);
  BitBlt(mem_dc,0,0,m_w,m_h,bk_dc,0,0,SRCCOPY);
  // p_graphics->FillRectangle(&m_solidBrush,0,0,m_w,m_h);
}
void DisWin::render_select(){
  switch(m_diswin_state){
    case DisWin_None:break;

    case DisWin_On_Select_Cut:
    case DisWin_On_Sel_Stage:
    case DisWin_On_Sel_Wall:

    p_graphics->DrawRectangle(&m_pen,sel_rect);
    break;

    default:
    break;
  }
}

void DisWin::enable_render(){
  render_working = TRUE;
  render_proc_handle = CreateThread(NULL,0,diswin_render_proc,
  this,0,NULL
  );
  assert(render_proc_handle);
}
void DisWin::end_render(){
  OutputDebugStringA("begin end render proc");
  if(render_working == FALSE)  return;
  render_working = FALSE;
  // WaitForSingleObject(render_proc_handle,INFINITE);
  // render_proc_handle = 0;
  OutputDebugStringA("end the render proc thread !!!");
}

void DisWin::begin_sel_cut(INT32 bx,INT32 by){
  if(m_diswin_state != DisWin_Wait_Select_Cut) return ;
  sel_rect = {bx,by,0,0};
  m_diswin_state = DisWin_On_Select_Cut;
}
void DisWin::begin_sel_stage(INT32 bx,INT32 by){
  if(m_diswin_state != DisWin_Wait_Sel_Stage) return ;
  sel_rect = {bx,by,0,0};
  m_diswin_state = DisWin_On_Sel_Stage;
}
void DisWin::update_sel_rect(INT32 cur_x,INT32 cur_y){
  assert(m_diswin_state == DisWin_On_Select_Cut || 
  m_diswin_state == DisWin_On_Sel_Stage);
  sel_rect.Width = cur_x  - sel_rect.X;
  sel_rect.Height = cur_y - sel_rect.Y;

}

void DisWin::end_sel_rect(INT32 end_x,INT32 end_y){
  // m_diswin_state = DisWin_None;
  LOG("%d %d %d %d",sel_rect.X, sel_rect.Y ,end_x,end_y);
  // caculate the valid rectangle
  INT32 left = sel_rect.X, right= end_x,
  top = sel_rect.Y , bottom = end_y;

  if(left % TILE_W) left = (left + TILE_W) / TILE_W;
  if(top % TILE_H) top = (top + TILE_H) / TILE_H;
  right = (right - TILE_W) / TILE_W;
  bottom = (bottom - TILE_H) / TILE_H;
  
  assert(m_pmap);
  switch(m_diswin_state){
    case DisWin_On_Select_Cut:
    m_pmap->add_trees_to_cut(top,bottom,left,right);
    break;

    case DisWin_On_Sel_Stage:
    m_pmap->add_stage(top,bottom,left,right);
    break;

    default:
    LOG("execption in end sel rect %d",m_diswin_state);
  }
  m_diswin_state = DisWin_None;

}

void DisWin::sel_wall(){
  // if(m_diswin_state == DisWin_On_Sel_Stage || m_diswin_state == DisWin_On_Select_Cut) return;
  m_diswin_state = DisWin_On_Sel_Wall;
  sel_rect.Width = TILE_W, sel_rect.Height = TILE_H;
}

void DisWin::update_sel_block(INT32 mouse_x,INT32 mouse_y){
  assert(sel_rect.Width == TILE_W && sel_rect.Height == TILE_H);
  sel_rect.X = mouse_x - mouse_x % TILE_W;
  sel_rect.Y = mouse_y - mouse_y % TILE_H;
}

void DisWin::end_sel_wall(){
  assert(m_diswin_state == DisWin_On_Sel_Wall);
  m_diswin_state = DisWin_None;
  m_pmap->add_wall_to_build(sel_rect.Y / TILE_W,sel_rect.X / TILE_H);
}



// rende proc, multi thread

DWORD WINAPI diswin_render_proc(LPVOID param){
  // DisWin::RenderProcParam* pp = static_cast<DisWin::RenderProcParam*>(param);
  assert(param);
  DisWin& diswin = *static_cast<DisWin*>(param);

  INT32 time_rec(0);
  OutputDebugStringA("render proc begin");
  while(diswin.render_working){
    time_rec = GetTickCount();
    assert(BaseWin::b_mutex);
    // WaitForSingleObject(BaseWin::b_mutex,INFINITE);
    // OutputDebugStringA("loop");
    diswin.clear();
    // OutputDebugStringA("begin render group");
    diswin.render_group();
    diswin.render_select();
    // OutputDebugStringA("render group");
    diswin.present();
    // OutputDebugStringA("present!!!");
    // ReleaseMutex(BaseWin::b_mutex);
    // OutputDebugStringA("release mutex");
    // LOG("render");
    time_rec = GetTickCount() - time_rec;
    if(time_rec +8 < update_velocity)  Sleep(update_velocity -8 - time_rec);
    // Sleep(8);
    // OutputDebugStringA("after sleep");
  }
  OutputDebugStringA("render end");
  return 0;
}




LRESULT CALLBACK DisWin::def_diswin_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
  switch (msg) {
    case WM_USER_CUT_TREE:{
      // SetCursor(LoadCursor(NULL,IDC_HAND));
      LOG("diswin receive the cut message");
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      p_diswin->wait_sel_cut();
    }break;

    case WM_USER_STAGE:{
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      p_diswin->wait_sel_stage();
    }break;

    case WM_USER_WALL:{
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      p_diswin->sel_wall();
    }break;


    case WM_LBUTTONDOWN:{
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      INT32 mouse_x = GET_X_LPARAM(lparam), mouse_y = GET_Y_LPARAM(lparam);
      switch(p_diswin->m_diswin_state){
        case DisWin_Wait_Select_Cut:
        p_diswin->begin_sel_cut(mouse_x,mouse_y);
        break;
        case DisWin_Wait_Sel_Stage:
        p_diswin->begin_sel_stage(mouse_x,mouse_y);
        break;

        case DisWin_On_Sel_Wall:
        p_diswin->end_sel_wall();
        break;

        default:
        break;
      }
      // if(p_diswin->on_wait_selecting_cut()){
      //   LOG("begin sel");
      //   p_diswin->begin_sel_cut(GET_X_LPARAM(lparam),GET_Y_LPARAM(lparam));
      // }

    }break;
    case WM_LBUTTONUP:{
      INT32 mouse_x = GET_X_LPARAM(lparam), mouse_y = GET_Y_LPARAM(lparam);
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      switch(p_diswin->m_diswin_state){
        case DisWin_On_Select_Cut:
        case DisWin_On_Sel_Stage:
        p_diswin->end_sel_rect(mouse_x,mouse_y);
        LOG("end sel rect");
        break;
        // case DisWin_On_Sel_Wall:

        default:
        break;

      }
      // if(p_diswin->on_selecting_cut()){
      //   p_diswin->end_sel_cut(GET_X_LPARAM(lparam),GET_Y_LPARAM(lparam));
      // }
    }break;
    case WM_MOUSEMOVE:{
      DisWin* p_diswin = reinterpret_cast<DisWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_diswin);
      INT32 x = GET_X_LPARAM(lparam), y = GET_Y_LPARAM(lparam);
      switch(p_diswin->m_diswin_state){
        case DisWin_On_Select_Cut:
        case DisWin_On_Sel_Stage:
        p_diswin->update_sel_rect(x,y);
        break;

        case DisWin_On_Sel_Wall:
        p_diswin->update_sel_block(x,y);
        break;

        default:
        break;
      }
      // if(p_diswin)
      // if(p_diswin->on_selecting_cut()){
      //   p_diswin->update_sel_cut(GET_X_LPARAM(lparam),GET_Y_LPARAM(lparam));
      // }
    }break;

    // case WM_DESTROY:{
    //   --BaseWin::WinCnt;
    //   if(BaseWin::WinCnt <= 1)  PostQuitMessage(0);
    // }break;
    case WM_CREATE:{
      OutputDebugStringA("receive the create message!!!");
      CREATESTRUCT* p_struc = reinterpret_cast<CREATESTRUCT*>(lparam);
      SetWindowLongPtr(hwnd,GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_struc->lpCreateParams));
    }break;
    default:
    return DefWindowProcA(hwnd,msg,wparam,lparam);
  }
  return 0;
}
