/*
 * File: arcade.cpp
 * Version: 1.1
 */

#include <iostream>
#include "arcade.h"

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x500
#include <windows.h>
#include <olectl.h>

using namespace std;

// link with -lgdi32 -luuid -lole32 -lolepro32 -lmsimg32

/*
 *     InitArcade(320,240,0,0,0,100);
 *     while (IsArcadeActive()) {
 *         if (IsKeyPressed(ESCAPE))
 *             break;
 *         ...
 *         DrawSprite("ball.gif,x,y);
 *     }
 */


struct Image {
    string      file;
    IPicture *  picture;
};

const int MAX_IMAGES = 100;
static Image    images[MAX_IMAGES];
static int      imageCount = 0;

struct Sprite {
    int         x,y,index;
};

const int MAX_SPRITES = 2000;
static Sprite   sprites[MAX_SPRITES];
static int      spriteCount = 0;
static Sprite   draw[MAX_SPRITES];
static int      drawCount = 0;

static HWND		hWnd = 0;
static HDC      backDC;
static HBITMAP  backBitmap;

static bool     initialized = false;
static char     title[251];
static COLORREF background;
static HBRUSH   bgBrush;
static int      interval;
static RECT     bounds;


static DWORD __stdcall ThreadMain(void *);
extern int main();

class Exit {
public:
	~Exit() {
        if (hWnd)
            SendNotifyMessage(hWnd,WM_CLOSE,0,0);
        while (hWnd)
            Sleep(500);
        for (int i=0; i<imageCount; i++)
            images[i].picture->Release();
    }
};

static Exit myExit;

static void Error(const char *msg)
{
    cerr << msg << endl;
    exit(0);
}


void InitArcade(const string &iTitle,int width,int height, double r,double g,double b,int delay)
{
	if (iTitle.length()<=0)   	Error("InitArcade: title string must have length > 0");
	if (iTitle.length()>=250) 	Error("InitArcade: title string must have length < 250");
	if (initialized) 			Error("InitArcade: must not call InitArcade() more than once");

    // initialize our variables
    strcpy(title,iTitle.c_str());
    background = RGB((int)(255*r),(int)255*g,(int)255*b);
    bounds.left = bounds.top = 0;
    bounds.right = width;
    bounds.bottom = height;
    interval = delay;

    // get the drawing thread going
	CreateThread(NULL,0,&ThreadMain,title,0,NULL);
	while (!initialized)
	    Sleep(10);
    SetConsoleCtrlHandler(NULL,TRUE);
}

void DrawSprite(const string &name, int x, int y)
{
	if (!initialized) Error("DrawSprite: must call InitArcade() before making other calls");
	if (spriteCount >= MAX_SPRITES) Error("DrawSprite: too many drawing operations");

    int index;
    for (index=0; index<imageCount; index++)
        if (images[index].file == name)
            break;
            
    if (index >= imageCount) {
        if (imageCount >= MAX_IMAGES) Error("DrawSprite: too many different images");

        HANDLE hFile = CreateFile(name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (!hFile) Error("open file failed"); // ,name.c_str(),GetLastError());
        DWORD dwFileSize = GetFileSize(hFile, NULL);
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
        if (!hGlobal) Error("alloc failed"); //,GetLastError());
        LPVOID pvData = GlobalLock(hGlobal);
        if (!pvData) Error("lock failed"); //,GetLastError());
        DWORD dwBytesRead = 0;
        BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
        if (!bRead) Error("read failed"); //,GetLastError());
        GlobalUnlock(hGlobal);
        CloseHandle(hFile);

        LPSTREAM pstm = NULL;
        HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
        if (hr) Error("CreateStreamOnHGlobal failed"); //,GetLastError());
        IPicture *picture;
        hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID*)&picture);
        if (hr) Error("OleLoadPicture(%s) failed"); //,name.c_str(),GetLastError());
        pstm->Release();

        images[imageCount].file = name;
        images[imageCount].picture = picture;
        index = imageCount;
        imageCount++;
    }
    
    sprites[spriteCount].index = index;
    sprites[spriteCount].x = x;
    sprites[spriteCount].y = y;
	spriteCount++;

    if (hWnd)
        InvalidateRect(hWnd, NULL, TRUE);
}

bool IsKeyPressed(int k)
{
    return GetAsyncKeyState(k) < 0;
}

bool IsArcadeActive()
{
    if (!hWnd) return false;

    UpdateArcade();
    return true;
}

void UpdateArcade()
{
    for (int i=0; i<spriteCount; i++)
        draw[i] = sprites[i];
    drawCount = spriteCount;
    spriteCount = 0;

    Sleep(interval);
}


/* Private functions */

static LRESULT CALLBACK WndProc(HWND iHWnd,UINT iMessage,WPARAM iWParam,LPARAM iLParam)
{
	HDC hDC; 
	PAINTSTRUCT ps;
	int result = 0;
    HDC tempDC;
    const COLORREF transparent = RGB(255,255,255);

	switch (iMessage) {
        case WM_PAINT:
            hDC = BeginPaint(iHWnd,&ps);
            tempDC = CreateCompatibleDC(hDC);
            FillRect(backDC,&bounds,bgBrush);
            for (int i=0; i<drawCount; i++) {
                Image *s = images + draw[i].index;
                OLE_HANDLE hbm;
                OLE_XSIZE_HIMETRIC w;
                OLE_YSIZE_HIMETRIC h;
                s->picture->get_Handle(&hbm);
                s->picture->get_Width(&w);
                s->picture->get_Height(&h);
                w = MulDiv(w,GetDeviceCaps(tempDC,LOGPIXELSX),2540);
                h = MulDiv(h,GetDeviceCaps(tempDC,LOGPIXELSY),2540);
                HANDLE oldBM = SelectObject(tempDC,(HANDLE)hbm);
                TransparentBlt(backDC,draw[i].x,draw[i].y,w,h,tempDC,0,0,w,h,transparent);
                SelectObject(tempDC,oldBM);
            }
            DeleteDC(tempDC);
    		BitBlt(hDC,0,0,bounds.right,bounds.bottom,backDC,0,0,SRCCOPY);
            EndPaint(hWnd,&ps);
			break;
			
		case WM_CLOSE:
			DestroyWindow(iHWnd);
			hWnd = NULL;
			break;

		default:
			result = DefWindowProc(iHWnd,iMessage,iWParam,iLParam);
			break;
	}

	return result;
}

static DWORD __stdcall ThreadMain(void *pointer)
{
	const char *name = reinterpret_cast<const char *>(pointer);
	
	// register window class
	static WNDCLASS windowClass;
	memset(&windowClass,0,sizeof(windowClass));
	windowClass.style			= CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	windowClass.lpfnWndProc		= WndProc;
	windowClass.hCursor			= LoadCursor(0,IDC_ARROW);
	windowClass.lpszClassName	= name;
	RegisterClass(&windowClass);
	
    // calculate window size
	RECT rect = bounds;
	AdjustWindowRect(&rect,WS_POPUP|WS_SYSMENU|WS_CAPTION,0);
	rect.right		-= rect.left;
	rect.bottom		-= rect.top;

	hWnd = CreateWindowEx(0,name,name,WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,CW_USEDEFAULT,CW_USEDEFAULT,rect.right,rect.bottom,0,0,0,0);
	if (!hWnd)
		return false;
		
	// create bitmap
	HDC hdc = GetDC(hWnd);
	backDC = CreateCompatibleDC(hdc);
	backBitmap = CreateCompatibleBitmap(hdc,bounds.right,bounds.bottom);
    SelectObject(backDC,backBitmap);
    
    bgBrush = CreateSolidBrush(background);
	ShowWindow(hWnd,SW_NORMAL);
	initialized = true;
	
	MSG msg;
	bool quit = false;
	while(!quit && hWnd) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
 			if(msg.message == WM_QUIT)
				quit = true;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		    Sleep(0);
	}
	
	return 0;
}
