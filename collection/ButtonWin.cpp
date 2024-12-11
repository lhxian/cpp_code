#include"all.h"
#include"ButtonWin.h"

gs::Rect ButtonWin::button_disrect = {0,0,BUTTON_W,BUTTON_H};
gs::Pen* ButtonWin::button_pen = NULL;
WNDCLASS ButtonWin::button_class = {0};
const CHAR* buttonwin_name = "BUTTON_WIN";
void ButtonWin::InitButtonwin(){
  button_class.lpszClassName = buttonwin_name;
  button_class.hCursor = static_cast<HCURSOR>(LoadCursor(NULL,IDC_ARROW));
  button_class.hInstance = BaseWin::process_inst;
  button_class.lpfnWndProc = ButtonWin_proc;
  assert(RegisterClass(&button_class));
  button_pen = new gs::Pen(gs::Color::DarkGreen,2.f);
  assert(button_pen->GetLastStatus() == gs::Ok);
  OutputDebugStringA("register the button win class");
}

void ButtonWin::closeButtonwin(){
  delete button_pen; button_pen = NULL;
  UnregisterClass(buttonwin_name,BaseWin::process_inst);
}
LRESULT CALLBACK ButtonWin::ButtonWin_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam){
  switch(msg){
    case WM_CREATE:{
      assert(lparam);
      SetWindowLongPtr(hwnd,GWLP_USERDATA,lparam);
      OutputDebugStringA("set the userdata");
    }break;
    case WM_TIMER:{
      assert(KillTimer(hwnd,wparam));
      ButtonWin* p_button = reinterpret_cast<ButtonWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_button);
      OutputDebugString("Button begin reset");
      LOG("button type : %x",p_button->get_type());
      assert(p_button->get_type() == WM_USER_CUT_TREE);
      p_button->reset();
      OutputDebugStringA("button reset");
    }break;
    case WM_LBUTTONUP:{
      SetTimer(hwnd,ID_TIMER,BUTTON_FOCUS_ELAPSE,NULL);
      LOG("set timer!!!");
      ButtonWin* p_button = reinterpret_cast<ButtonWin*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
      assert(p_button);
      p_button->focus();
    }break;
    default:
    return DefWindowProc(hwnd,msg,wparam,lparam);
  }
  return 0;
}

ButtonWin::ButtonWin(HWND parent,INT32 px,INT32 py,UINT button_type,const WCHAR* img_path):
button_type(button_type)
{
  assert(parent);
  LOG("button type : %x",button_type);
  hwnd = CreateWindow(buttonwin_name,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,px,py,BUTTON_W,
  BUTTON_H,parent,0,BaseWin::process_inst,this);
  assert(hwnd);
  m_image = new gs::Image(img_path);
  assert(m_image->GetLastStatus() == gs::Ok);
  HDC hdc = GetDC(hwnd);
  assert(hdc);
  mem_dc = CreateCompatibleDC(hdc);
  assert(mem_dc);
  bmp = CreateCompatibleBitmap(hdc,BUTTON_W,BUTTON_H);
  assert(bmp);
  SelectObject(mem_dc,bmp);
  p_graphics = new gs::Graphics(mem_dc);

  auto it =p_graphics->DrawImage(m_image,button_disrect,0,0,BUTTON_W,BUTTON_H,gs::UnitPixel);
  assert(it == gs::Ok);
  assert(::Rectangle(hdc,4,4,16,16));
  BitBlt(hdc,0,0,BUTTON_W,BUTTON_H,mem_dc,0,0,SRCCOPY);
  ValidateRect(hwnd,NULL);
  ReleaseDC(hwnd,hdc);

}
ButtonWin::~ButtonWin(){
  OutputDebugStringA("begin delete a button...");
  delete m_image; m_image = NULL;
  delete p_graphics ; p_graphics = NULL;
  DeleteObject(bmp);
  DeleteDC(mem_dc);mem_dc = 0;

  OutputDebugStringA("delete a button !!!");
}

void ButtonWin::reset(){
  OutputDebugString("in rest funct begin");
  HDC hdc = GetDC(hwnd);
  assert(hdc);
  assert(::Rectangle(hdc,0,0,BUTTON_W,BUTTON_H));
  auto it =p_graphics->DrawImage(m_image,button_disrect,0,0,BUTTON_W,BUTTON_H,gs::UnitPixel);
  assert(it == gs::Ok);
  BitBlt(hdc,0,0,BUTTON_W,BUTTON_H,mem_dc,0,0,SRCCOPY);
  ValidateRect(hwnd,NULL);

  OutputDebugStringA("validate");
  ReleaseDC(hwnd,hdc);
  OutputDebugStringA("rset");
}
void ButtonWin::focus(){
  OutputDebugString("button focus");
  HDC hdc = GetDC(hwnd);
  assert(hdc);
  assert(button_pen);
  auto it = p_graphics->DrawRectangle(button_pen,button_disrect);
  assert(it == gs::Ok);
  ReleaseDC(hwnd,hdc);
}