/*
 * Live floatbeat player, 48,000 Hz, Windows 10 compatible C.
 * Build with MinGW-w64:
 *   windres floatbeat.rc -O coff -o floatbeat_res.o
 *   gcc -O2 -std=c99 -mwindows floatbeat.c floatbeat_res.o -o floatbeat.exe -lwinmm -lgdi32 -lshell32 -lm
 * Run: floatbeat.exe [seconds]
 */
#ifdef _WIN32
#define COBJMACROS
#define INITGUID
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <oleauto.h>
#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 3
#endif
#endif
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SAMPLE_RATE 48000u
#define DEFAULT_SECONDS 240u
#define APP_ICON 101
static double rem(double x, double y) {
    return x - trunc(x / y) * y;
}

static double sample(uint32_t s) {
 static const int M[32]={1,5,12,1,5,12,1,5,0,4,10,0,4,10,13,12,5,8,15,5,8,15,13,12,3,7,13,3,13,12,10,12};
 static const int B[32]={13,13,13,13,13,13,13,13,12,12,12,12,12,12,13,12,5,5,5,5,5,5,5,5,8,8,8,8,8,8,10,12};
 static const char P[]="00055522227744440005552222774422";
 static const char A[]="1111100000011111111111111111111111111111111111111111111000001111111001111111110001000000000000001111110000000000000000000111111111111001111110000000000000000000000000000000000";
 static double a = 0, b = 0, d = 0, e = 0;
 const double n = 440.0 * pow(2.0, 1.0 / 12.0 - 9.0) / 48000.0 * 39000.0;
 const double t = (double)s / 48000.0 / 60.0 * 70.0 * 32000.0;
 const int ti = (int)floor(t);
 const double cn = n * 2 * pow(2, M[(ti >> 13) & 31] / 12.0);
 const double gn = n * 2 * pow(2, B[(ti >> 13) & 31] / 12.0) / 2;
 const double c = (a += (b += (cn - b) / 800));
 const double g = (d += (e += (gn - e) / 400));
 const int ci = (int)rem(floor(rem(c, 1048576) * 175 / 256), sizeof(A) - 1);
 const int gi = (int)rem(floor(rem(g, 1048576) * 175 / 256), sizeof(A) - 1);
 const int hi = (int)rem(floor(rem(g / 2, 1048576) * 175 / 256), sizeof(A) - 1);
 const double lead = (A[ci] - '0' - .5) / 1.3;
 const double chord = (A[gi] - '0' - .5) / 2 + (A[hi] - '0' - .5) / 3;
 const int gate = ((-ti + P[(ti >> 12) & 31] * 4096) >> 7) & 255;
 const double hit = (((-ti >> 13) & 1) && !((-ti >> 20) & 1));
 double out = lead + chord * gate / 200 + sin(t * pow(2, 1.0 / 12) * 3.141592653589793 / 512 * pow(2, .25)) * hit * 3;
 if (out > 1) return 1;
 if (out < -1) return -1;
 return out;
}
#ifdef _WIN32
static const char *errors[] = {"ERROR: Reality stopped responding", "WARNING: Too much floatbeat", "Fatal exception: rhythm overflow", "System message: please remain calm", "ERROR: Reinstall floatbeat.sys"};
static const char *jokes[] = {"That button was decorative.", "Nice try. The error remains.", "ERROR: You clicked optimism.", "The computer has considered your request.", "Fix postponed indefinitely."};
#define POPUPS 4
static HWND popups[POPUPS];
static double popup_progress[POPUPS];
#define BACK_POPUPS 4
static HWND back_popups[BACK_POPUPS];
static double back_progress[BACK_POPUPS];
#define DVD_WINDOWS 1
static int screen_shift_direction;
static int screen_shift_phase;
static HWND explorer_window;
static int explorer_x, explorer_y, explorer_dx=9, explorer_dy=7;
static LRESULT CALLBACK popup_proc(HWND w, UINT msg, WPARAM wp, LPARAM lp) {
 if (msg == WM_CREATE) { CreateWindowExA(0,"BUTTON","OK",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,310,118,80,28,w,(HMENU)1,GetModuleHandleA(0),0); return 0; }
 if (msg == WM_PAINT) { PAINTSTRUCT ps; RECT r={85,25,390,110}; HDC dc=BeginPaint(w,&ps); DrawIconEx(dc,22,30,LoadIconA(0,IDI_ERROR),48,48,0,0,DI_NORMAL); SetBkColor(dc,GetSysColor(COLOR_WINDOW)); SetTextColor(dc,GetSysColor(COLOR_WINDOWTEXT)); DrawTextA(dc,(LPCSTR)GetWindowLongPtrA(w,GWLP_USERDATA),-1,&r,DT_LEFT|DT_VCENTER|DT_WORDBREAK); EndPaint(w,&ps); return 0; }
 if (msg == WM_COMMAND && LOWORD(wp)==1) { SetWindowLongPtrA(w,GWLP_USERDATA,(LONG_PTR)jokes[GetTickCount()%5]); SetWindowTextA(w,"Still an Error"); InvalidateRect(w,0,TRUE); return 0; }
 return DefWindowProcA(w,msg,wp,lp);
}
static void show_popups(void) {
 WNDCLASSA wc={0}; wc.lpfnWndProc=popup_proc; wc.hInstance=GetModuleHandleA(0); wc.hIcon=LoadIconA(wc.hInstance,MAKEINTRESOURCEA(APP_ICON)); wc.lpszClassName="FloatbeatError"; wc.hCursor=LoadCursor(0,IDC_ARROW); RegisterClassA(&wc);
 RECT desk; SystemParametersInfoA(SPI_GETWORKAREA,0,&desk,0); srand((unsigned)GetTickCount());
 for(int i=0;i<BACK_POPUPS;i++){back_progress[i]=((double)rand()/RAND_MAX)*6.283185307; int x=desk.left+rand()%((desk.right-desk.left)-500),y=desk.top+rand()%((desk.bottom-desk.top)-240); back_popups[i]=CreateWindowExA(WS_EX_APPWINDOW|WS_EX_TOPMOST,"FloatbeatError","Background Error",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE,x,y,500,240,0,0,wc.hInstance,0); SetWindowLongPtrA(back_popups[i],GWLP_USERDATA,(LONG_PTR)errors[rand()%4]); InvalidateRect(back_popups[i],0,TRUE);}
 for(int i=0;i<POPUPS;i++){popup_progress[i]=((double)rand()/RAND_MAX)*6.283185307; int x=desk.left+rand()%((desk.right-desk.left)-420),y=desk.top+rand()%((desk.bottom-desk.top)-180); popups[i]=CreateWindowExA(WS_EX_APPWINDOW|WS_EX_TOPMOST,"FloatbeatError","Error",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE,x,y,420,180,0,0,wc.hInstance,0); SetWindowLongPtrA(popups[i],GWLP_USERDATA,(LONG_PTR)errors[rand()%4]); InvalidateRect(popups[i],0,TRUE);}
}
static void move_popups(void) {
 RECT desk; SystemParametersInfoA(SPI_GETWORKAREA,0,&desk,0);
 const double center_x=(desk.left+desk.right)/2.0;
 const double center_y=(desk.top+desk.bottom)/2.0;
 const double radius_x=(desk.right-desk.left-500)*0.46;
 const double radius_y=(desk.bottom-desk.top-240)*0.46;
 for(int i=0;i<BACK_POPUPS;i++){
  back_progress[i]=fmod(back_progress[i]-0.12,6.283185307);
  if(back_progress[i]<0) back_progress[i]+=6.283185307;
  int x=(int)(center_x+cos(back_progress[i])*radius_x-250);
  int y=(int)(center_y+sin(back_progress[i])*radius_y-120);
  SetWindowPos(back_popups[i],HWND_TOPMOST,x,y,500,240,SWP_SHOWWINDOW|SWP_NOACTIVATE);
 }
 const double front_radius_x=(desk.right-desk.left-420)*0.28;
 const double front_radius_y=(desk.bottom-desk.top-180)*0.28;
 for(int i=0;i<POPUPS;i++){
  popup_progress[i]=fmod(popup_progress[i]+0.18,6.283185307);
  int x=(int)(center_x+cos(popup_progress[i])*front_radius_x-210);
  int y=(int)(center_y+sin(popup_progress[i])*front_radius_y-90);
  SetWindowPos(popups[i],HWND_TOPMOST,x,y,420,180,SWP_SHOWWINDOW|SWP_NOACTIVATE);
 }
}
static void close_popups(void){for(int i=0;i<BACK_POPUPS;i++)if(back_popups[i])DestroyWindow(back_popups[i]);for(int i=0;i<POPUPS;i++)if(popups[i])DestroyWindow(popups[i]);}
static void shift_screen(void){
 int width=GetSystemMetrics(SM_CXSCREEN),height=GetSystemMetrics(SM_CYSCREEN),step=14;
 HDC dc=GetDC(0);if(!dc)return;
 if(screen_shift_phase==2){
  int margin=24+(rand()%2)*16; int inner_width=width-2*margin,inner_height=height-2*margin;
  if(rand()%2==0) StretchBlt(dc,margin,margin,inner_width,inner_height,dc,0,0,width,height,SRCCOPY);
  else StretchBlt(dc,0,0,width,height,dc,margin,margin,inner_width,inner_height,SRCCOPY);
  ReleaseDC(0,dc); return;
 }
 if((rand()%4)==0)screen_shift_direction=rand()%4;
 int dx=0,dy=0;if(screen_shift_direction==0)dx=-step;if(screen_shift_direction==1)dx=step;if(screen_shift_direction==2)dy=-step;if(screen_shift_direction==3)dy=step;
 int copy_width=width-abs(dx),copy_height=height-abs(dy);int dst_x=dx<0?0:dx,dst_y=dy<0?0:dy;int src_x=dx<0?-dx:0,src_y=dy<0?-dy:0;
 BitBlt(dc,dst_x,dst_y,copy_width,copy_height,dc,src_x,src_y,SRCCOPY);
 if (dx < 0) PatBlt(dc, width - step, 0, step, height, BLACKNESS);
 if (dx > 0) PatBlt(dc, 0, 0, step, height, BLACKNESS);
 if (dy < 0) PatBlt(dc, 0, height - step, width, step, BLACKNESS);
 if (dy > 0) PatBlt(dc, 0, 0, width, step, BLACKNESS);
 ReleaseDC(0,dc);
}
static void invert_screen(void){
 int width=GetSystemMetrics(SM_CXSCREEN),height=GetSystemMetrics(SM_CYSCREEN);
 HDC dc=GetDC(0);if(!dc)return;BitBlt(dc,0,0,width,height,dc,0,0,NOTSRCCOPY);ReleaseDC(0,dc);
}
static void open_random_directory(void){
 char paths[6][MAX_PATH],temp[MAX_PATH];WCHAR wide_path[MAX_PATH];DWORD n=0;
 if(GetWindowsDirectoryA(paths[n],MAX_PATH))n++;
 if(GetSystemDirectoryA(paths[n],MAX_PATH))n++;
 if(GetTempPathA(MAX_PATH,temp)){lstrcpynA(paths[n],temp,MAX_PATH);n++;}
 if(GetCurrentDirectoryA(MAX_PATH,paths[n]))n++;
 if(SHGetFolderPathA(0,CSIDL_COMMON_DOCUMENTS,0,SHGFP_TYPE_CURRENT,paths[n])==S_OK)n++;
 if(SHGetFolderPathA(0,CSIDL_DESKTOPDIRECTORY,0,SHGFP_TYPE_CURRENT,paths[n])==S_OK)n++;
 if(!n)return;
 const char *path=paths[rand()%n];
 if(!explorer_window){
  ShellExecuteA(0,"open",path,0,0,SW_SHOWNORMAL);Sleep(500);
  explorer_window=FindWindowA("CabinetWClass",0);
  if(explorer_window){RECT r;GetWindowRect(explorer_window,&r);explorer_x=r.left;explorer_y=r.top;}
  return;
 }
 int wide_len=MultiByteToWideChar(CP_ACP,0,path,-1,wide_path,MAX_PATH);
 if(!wide_len)return;
 IShellWindows *windows=0;
 if(FAILED(CoCreateInstance(&CLSID_ShellWindows,0,CLSCTX_LOCAL_SERVER,&IID_IShellWindows,(void **)&windows)))return;
 LONG count=0;IShellWindows_get_Count(windows,&count);
 for(LONG i=0;i<count;i++){
  VARIANT index;VariantInit(&index);V_VT(&index)=VT_I4;V_I4(&index)=i;
  IDispatch *dispatch=0;
  if(SUCCEEDED(IShellWindows_Item(windows,index,&dispatch))&&dispatch){
   IWebBrowser2 *browser=0;SHANDLE_PTR hwnd=0;
   if(SUCCEEDED(IDispatch_QueryInterface(dispatch,&IID_IWebBrowser2,(void **)&browser))&&SUCCEEDED(IWebBrowser2_get_HWND(browser,&hwnd))&&(HWND)hwnd==explorer_window){
    VARIANT url,empty;VariantInit(&url);VariantInit(&empty);V_VT(&url)=VT_BSTR;V_BSTR(&url)=SysAllocString(wide_path);IWebBrowser2_Navigate2(browser,&url,&empty,&empty,&empty,&empty);VariantClear(&url);browser->lpVtbl->Release(browser);dispatch->lpVtbl->Release(dispatch);break;
   }
   if(browser)browser->lpVtbl->Release(browser);
   dispatch->lpVtbl->Release(dispatch);
  }
  VariantClear(&index);
 }
 windows->lpVtbl->Release(windows);
}
static void move_explorer(void){if(!explorer_window||!IsWindow(explorer_window)){explorer_window=0;return;}RECT desk,win;SystemParametersInfoA(SPI_GETWORKAREA,0,&desk,0);GetWindowRect(explorer_window,&win);explorer_x+=explorer_dx;explorer_y+=explorer_dy;if(explorer_x<=desk.left||explorer_x+win.right-win.left>=desk.right)explorer_dx=-explorer_dx;if(explorer_y<=desk.top||explorer_y+win.bottom-win.top>=desk.bottom)explorer_dy=-explorer_dy;SetWindowPos(explorer_window,0,explorer_x,explorer_y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);}
static void close_explorer(void){if(explorer_window&&IsWindow(explorer_window))PostMessageA(explorer_window,WM_CLOSE,0,0);explorer_window=0;}
int main(int argc,char**argv){
 uint32_t sec=DEFAULT_SECONDS;if(argc>1)sec=(uint32_t)strtoul(argv[1],0,10);if(!sec)return 1;
 MessageBoxA(0,"WARNING: This program contains flashing lights, rapid screen effects, and audio. It also opens and moves an Explorer window. Close it immediately if you feel uncomfortable or are sensitive to flashing lights.","AndroidTrollware warning",MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
 CoInitialize(0);
 WAVEFORMATEX fmt={WAVE_FORMAT_IEEE_FLOAT,1,SAMPLE_RATE,SAMPLE_RATE*4,4,32,0};HWAVEOUT dev;
 if(waveOutOpen(&dev,WAVE_MAPPER,&fmt,0,0,CALLBACK_NULL)!=MMSYSERR_NOERROR){fprintf(stderr,"Could not open audio device\\n");return 1;}
 show_popups();
 enum{N=8,FRAMES=4096};float buf[N][FRAMES];WAVEHDR hdr[N];memset(hdr,0,sizeof hdr);
 for(int b=0;b<N;b++){for(int i=0;i<FRAMES;i++)buf[b][i]=(float)sample((uint32_t)b*FRAMES+i);hdr[b].lpData=(LPSTR)buf[b];hdr[b].dwBufferLength=sizeof buf[b];waveOutPrepareHeader(dev,&hdr[b],sizeof hdr[b]);waveOutWrite(dev,&hdr[b],sizeof hdr[b]);}
 uint64_t total=(uint64_t)sec*SAMPLE_RATE;uint64_t sent=(uint64_t)N*FRAMES;
 DWORD started=GetTickCount(),next_shift=started+27000,next_invert=started+29000,next_explorer=started+27000;int explorer_phase=0; MSG msg;
 while(sent<total){for(int b=0;b<N;b++)if(hdr[b].dwFlags&WHDR_DONE){uint32_t count=(uint32_t)((total-sent)>FRAMES?FRAMES:total-sent);for(uint32_t i=0;i<count;i++)buf[b][i]=(float)sample((uint32_t)sent+i);hdr[b].dwBufferLength=count*sizeof(float);hdr[b].dwFlags&=~WHDR_DONE;waveOutWrite(dev,&hdr[b],sizeof hdr[b]);sent+=count;} while(PeekMessageA(&msg,0,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageA(&msg);} DWORD now=GetTickCount();DWORD elapsed=now-started;if(elapsed<27000){screen_shift_phase=0;}else if(elapsed<56000){screen_shift_phase=1;}else if(elapsed<84000){screen_shift_phase=0;}else if(elapsed<112000){screen_shift_phase=2;}else if(elapsed<141000){screen_shift_phase=0;}else{screen_shift_phase=3;}int active_explorer_phase=(screen_shift_phase==1)?1:(screen_shift_phase==2)?2:(screen_shift_phase==3)?3:0;if(active_explorer_phase!=explorer_phase){if(active_explorer_phase==0)close_explorer();else next_explorer=now;explorer_phase=active_explorer_phase;}if(active_explorer_phase&&now>=next_explorer){open_random_directory();next_explorer=now+5000;}move_explorer();move_popups();if(screen_shift_phase&&now>=next_shift){if(screen_shift_phase==3){screen_shift_phase=1;shift_screen();screen_shift_phase=2;shift_screen();screen_shift_phase=3;}else{shift_screen();}next_shift=now+45;}if(screen_shift_phase==3&&now>=next_invert){invert_screen();next_invert=now+3000;}else if((screen_shift_phase==1||screen_shift_phase==2)&&now>=next_invert){if((rand()%2)==0)invert_screen();next_invert=now+2000;}Sleep(8);}
 Sleep(500);close_popups();for(int b=0;b<N;b++)waveOutUnprepareHeader(dev,&hdr[b],sizeof hdr[b]);waveOutClose(dev);CoUninitialize();return 0;
}
#else
int main(void){fprintf(stderr,"This player requires Windows 10.\\n");return 1;}
#endif


