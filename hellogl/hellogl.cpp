// hellogl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "quadrenderer.h"
#include "unitquad.h"
#include "hellogl.h"
#include <GL/gl3w.h>

#define MAX_LOADSTRING 100
#define IDC_MAIN_BUTTON 101

static const wchar_t* c_glwin_class = L"glwin";
int g_pattern;

// Global Variables:
HWND glwnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HGLRC hGLRC;									// gl rendering context

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
ATOM				RegisterGLWindowClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	GlWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void setupPixelFormat(HDC hDC);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HELLOGL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	RegisterGLWindowClass(hInstance);

	// Perform application initialization:
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd) {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HELLOGL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HELLOGL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HELLOGL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

ATOM RegisterGLWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= GlWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HELLOGL);
	wcex.lpszClassName	= c_glwin_class;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// Request a pixel format.
// Used from GlWndProc when the OpenGL rendering context is set up.
// based on http://www.cs.rit.edu/~ncs/Courses/570/UserGuide/OpenGLonWin-11.html
void setupPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  /* size */
		1,                              /* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,               /* support double-buffering */
		PFD_TYPE_RGBA,                  /* color type */
		24,                             /* prefered color depth */
		0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
		0,                              /* no alpha buffer */
		0,                              /* alpha bits (ignored) */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits (ignored) */
		24,                             /* depth buffer */
		8,                              /* no stencil buffer */
		0,                              /* no auxiliary buffers */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0,                        /* no layer, visible, damage masks */
	};
	int pixelFormat;

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat == 0) {
		MessageBox(WindowFromDC(hDC), L"ChoosePixelFormat failed.", L"Error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}

	if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) {
		MessageBox(WindowFromDC(hDC), L"SetPixelFormat failed.", L"Error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}
	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.dwFlags & PFD_NEED_PALETTE) {
		MessageBox(WindowFromDC(hDC), L"Palette pixel format not supported..", L"Error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}
}

//
// in WM_CREATE, set up OpenGL 3.1+ rendering context.
// Get access to modern OpenGL functions using gl3w.
// Draw textured quad in WM_PAINT.
//
LRESULT CALLBACK GlWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CREATE:
	{
		/* initialize OpenGL rendering  */
		HDC hDC = GetDC(hWnd);
		setupPixelFormat(hDC);
		hGLRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hGLRC);

		if (gl3wInit()) {
		    MessageBoxA(0, "Failed to initialize gl3w", "OPENGL VERSION", 0);
			return false;
		}
		if (!gl3wIsSupported(3, 1)) {
		    MessageBoxA(0, "OpenGL 3.1 not supported", "OPENGL VERSION", 0);
			return false;
		}

		MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
		break;
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		if (hGLRC) {
			int winWidth = (int)LOWORD(lParam);
			int winHeight = (int)HIWORD(lParam);
			glViewport(0, 0, winWidth, winHeight);
			break;
		}
	case WM_PAINT:
	{
	    PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		if (hGLRC) {
			// render a textured quad.
			quadrenderer qr;
			unsigned char tex[] = { 255, 0, 0,
				0, 255, 0,
				0, 0, 255,
				0, 0, 0,
				255, 255, 0,
				255, 0, 255,
				0, 255, 255,
				255, 255, 255
			};
			// cycle through quad pattern on button press
			int npattern = sizeof(tex) / 3;
			unsigned char swizzled[sizeof(tex)];
			for (int i = 0; i < sizeof(tex); ++i) {
				swizzled[i] = tex[(i + 3 * g_pattern) % sizeof(tex)];
			}
			qr.upload_texture(swizzled, 4, 2, GL_RGB, GL_UNSIGNED_BYTE);
			qr.draw();

			SwapBuffers(hDC);
			GLenum err = glGetError();
			if (err) {
				OutputDebugString(L"ERROR");
			}
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		if (hGLRC) {
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hGLRC);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//
// Message handler for parent window.
// In WM_CREATE, create OpenGL window.
// This window proc does not paint;
// painting is done in GlWndProc
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
	{
		glwnd = CreateWindow(c_glwin_class, szTitle, WS_CHILD,
			0, 0, 640, 480, hWnd, NULL, GetModuleHandle(NULL), NULL);
		ShowWindow(glwnd, SW_SHOW);
		UpdateWindow(glwnd);

		HWND hWndButton = CreateWindowEx(NULL,
			L"BUTTON",
			L"OK",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			0,
			480,
			100,
			24,
			hWnd,
			(HMENU)IDC_MAIN_BUTTON,
			GetModuleHandle(NULL),
			NULL);
		break;
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_MAIN_BUTTON:
			g_pattern++;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
