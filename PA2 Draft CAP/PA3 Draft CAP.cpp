// PA3 Draft CAP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "PA3 Draft CAP.h"
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include "global.h"
//#include "global.h"
//extern RGBAColor borderColor[POLYGON_NUM][VERTEX_NUM + 1];
#define MAX_LOADSTRING 100

TCHAR greeting[] = _T("Welcome to My Screen Saver");
int openGLMain();
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PA3DRAFTCAP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PA3DRAFTCAP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW; 
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PA3DRAFTCAP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);
        
    if (!hWnd)
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    borderColor[0] = { 0, 0, 0, 1 };
    borderColor[1] = { -1,-1,-1,-1 };
    convexFillColor[0] = { 0, 0, 0, 1 };
    convexFillColor[1] = { -1,-1,-1,-1 };
    for (int i = 0; i < POLYGON_NUM;i++)
        polygons[i].vertices = new Vertex[VERTEX_NUM];

    return TRUE;
}

void CheckItem(HWND hWnd, int id) {
    CheckMenuItem(GetMenu(hWnd), id, MF_CHECKED);
    int allIDs[] = { ID_FILLPATTERN_SOLID ,ID_FILLPATTERN_HATCHED ,ID_FILLPATTERN_FLY ,ID_FILLPATTERN_HALFTONE};
    for(int i = 0; i < 4;i++)
        if(allIDs[i]!=id)
            CheckMenuItem(GetMenu(hWnd), allIDs[i], MF_UNCHECKED);
}

void CheckRotationItem(HWND hWnd, int id) {
    CheckMenuItem(GetMenu(hWnd), id, MF_CHECKED);
    int allIDs[] = { ID_ROTATION_CLOCKWISE, ID_ROTATION_COUNTER };
    for (int i = 0; i < 2; i++)
        if (allIDs[i] != id)
            CheckMenuItem(GetMenu(hWnd), allIDs[i], MF_UNCHECKED);
}

void CheckMovementItem(HWND hWnd, int id) {
    CheckMenuItem(GetMenu(hWnd), id, MF_CHECKED);
    int allIDs[] = { ID_DIRECTION_RIGHT_UP, ID_DIRECTION_LEFT_UP, ID_DIRECTION_RIGHT_DOWN, ID_DIRECTION_LEFT_DOWN };
    for (int i = 0; i < 4; i++)
        if (allIDs[i] != id)
            CheckMenuItem(GetMenu(hWnd), allIDs[i], MF_UNCHECKED);
}

BOOL parseColor(TCHAR str[], RGBAColor color[]) {
    TCHAR* next;
    TCHAR* rest = _wcsdup(str);//safe 16-bit version of strdup
    TCHAR* token = wcstok_s(rest, L"\n", &next);//safe version of strtok, but it works for wide character
    float temp[4];
    int currentVertex = 0;
    while (token != NULL && currentVertex < VERTEX_NUM - 1) {
        if (swscanf_s(token, L"(%f,%f,%f,%f)", temp, temp + 1, temp + 2, temp + 3) == 4)
            color[currentVertex++] = { temp[0] ,temp[1] ,temp[2] ,temp[3] };
        token = wcstok_s(NULL, L"\n", &next);
    }
    if (currentVertex == 0)//no valid color was input by user...
        return FALSE;
    color[currentVertex] = { -1,-1,-1,-1 };//The # of vertices for a polygon is variable
    return TRUE;
}

INT_PTR CALLBACK PickBorderColor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (!parseColor(buffer, borderColor))
                    break;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickBorderPattern(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    unsigned long y = 0XFFFF;
    std::stringstream ss;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%x", &y) != 1)
                    break;
                borderPattern = (unsigned short) y;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickConvexFillAreaColor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (!parseColor(buffer, convexFillColor))
                    break;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickRotationSpeed(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    float q = ROTATION_SPEED;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));                
                if (swscanf_s(buffer, L"%f", &q) != 1)
                    break;
                rotation_speed = (double)q;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickConstantMovementSpeed(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    float x_s = X_SPEED;
    float y_s = Y_SPEED;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &x_s) != 1)
                    break;
                x_speed_constant = (double)x_s;
                GetDlgItemText(hDlg, IDC_EDIT2, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &y_s) != 1)
                    break;               
               y_speed_constant = (double)y_s;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK AddCircle(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    float x_s = 1;
    float y_s = 1;
    float r = 1;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &x_s) != 1)
                    break;
                center_x = (double)x_s;
                GetDlgItemText(hDlg, IDC_EDIT2, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &y_s) != 1)
                    break;
                center_y = (double)y_s;
                GetDlgItemText(hDlg, IDC_EDIT3, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &r) != 1)
                    break;
                radius = (double)radius;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickShrinkSpeed(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    float q = BOUNCE_BACK_PERIOD;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &q) != 1)
                    break;
                given_shrink_speed = (double)q;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PickShrinkRatio(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    float q = SHRINK_WHEN_BOUNCE;
    {
        TCHAR buffer[MAX_PATH];
        switch (message)
        {
        case WM_COMMAND:
            int x = LOWORD(wParam);
            switch (x)
            {
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof(buffer));
                if (swscanf_s(buffer, L"%f", &q) != 1)
                    break;
                given_shrink_ratio = (double)q;
                // Fall through. 
            case IDXCANCEL:
                EndDialog(hDlg, wParam);
                return TRUE;
            }
        }
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_FILE_OPENAWINDOW:
            openGLMain();
            break;
        case ID_SETTINGS_BORDERCOLOR:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, PickBorderColor);//IDD_DIALOG1
            break;
        case ID_SETTINGS_FILLCOLOR:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, PickConvexFillAreaColor);//IDD_DIALOG2
            break;
        case ID_SETTINGS_BORDERPATTERN:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, PickBorderPattern);//IDD_DIALOG3
            break;
        case ID_FILLPATTERN_HATCHED:
            fillPattern = HATCHED;
            //SetMenuItemInfo(GetMenu(hWnd), ID_FILLPATTERN_HATCHED,FALSE,&m);
            CheckItem(hWnd, ID_FILLPATTERN_HATCHED);
            break;
        case ID_FILLPATTERN_SOLID:
            fillPattern = SOLID;
            CheckItem(hWnd, ID_FILLPATTERN_SOLID);
            break;
        case ID_FILLPATTERN_HALFTONE:
            fillPattern = HALF_TONE;
            CheckItem(hWnd, ID_FILLPATTERN_HALFTONE);
            break;
        case ID_FILLPATTERN_FLY:
            fillPattern = FLY;
            CheckItem(hWnd, ID_FILLPATTERN_FLY);
            break;
        case ID_ROTATION_CLOCKWISE:
            initial_rotation_direction = 32;
            CheckRotationItem(hWnd, ID_ROTATION_CLOCKWISE);
            break;
        case ID_ROTATION_COUNTER:
            initial_rotation_direction = 16;
            CheckRotationItem(hWnd, ID_ROTATION_COUNTER);
            break;
        case ID_MOVEMENT_SPEED:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG4), hWnd, PickRotationSpeed);//IDD_DIALOG4
            break;
        case ID_DIRECTION_RIGHT_UP:
            initial_x_movement = RIGHT;
            initial_y_movement = UP;
            CheckMovementItem(hWnd, ID_DIRECTION_RIGHT_UP);
            break;
        case ID_DIRECTION_LEFT_UP:
            initial_x_movement = LEFT;
            initial_y_movement = UP;
            CheckMovementItem(hWnd, ID_DIRECTION_LEFT_UP);
            break;
        case ID_DIRECTION_RIGHT_DOWN:
            initial_x_movement = RIGHT;
            initial_y_movement = DOWN;
            CheckMovementItem(hWnd, ID_DIRECTION_RIGHT_DOWN);
            break;
        case ID_DIRECTION_LEFT_DOWN:
            initial_x_movement = LEFT;
            initial_y_movement = DOWN;
            CheckMovementItem(hWnd, ID_DIRECTION_LEFT_DOWN);
            break;
        case ID_MOVEMENT_CONSTANT_SPEED:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG5), hWnd, PickConstantMovementSpeed);//IDD_DIALOG5
            break;
        case ID_ADDOTHERSHAPES_ADDACIRCLE:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG6), hWnd, AddCircle);//IDD_DIALOG6
            break;
        case ID_YES_SHRINKSPEED:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG7), hWnd, PickShrinkSpeed);//IDD_DIALOG7
            break;
        case ID_YES_SHRINKRATIO:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG8), hWnd, PickShrinkRatio);//IDD_DIALOG8
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
         TextOut(hdc,
            200, 150,
            greeting, _tcslen(greeting));
        
        EndPaint(hWnd, &ps);
    }
    break;
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