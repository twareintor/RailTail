#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"
#include "railtrail.h"


BOOL CALLBACK DlgMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            UINT iTimMov;       // returns the timer value
            UINT iTimSpd;       // set speed value

            iTimMov = SetTimer(hwnd, ID_TIMER, 30, NULL);
            iTimSpd = SetTimer(hwnd, ID_TIMES, 20, NULL);
            if(iTimMov == 0)
                MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
            InitElem(hwnd);

        }
        return TRUE;

        case WM_CLOSE:
        {
            EndDialog(hwnd, 0);
        }
        return TRUE;
        case WM_PAINT:
        {
            RECT rcClient;
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rcClient);
            DrawElem(hdc, &rcClient);
            DefElemRect();
            EndPaint(hwnd, &ps);

        }
        break;
        case WM_TIMER:
        {
            switch(wParam)
            {
                case ID_TIMER:
                {

                    if(abs(xSpeed)&&(!booDrag)) // if the train has speed and is was released from the mouse...
                    {
                        // move the train, starting with the top speed, then slowing
                        xSpeed/=xDecel;     // squarely decrease speed
                        RECT rcClient;
                        HDC hdc = GetDC(hwnd);
                        GetClientRect(hwnd, &rcClient);
                        UpdateChar(&rcClient);
                        DrawElem(hdc, &rcClient);
                        ReleaseDC(hwnd, hdc);

                    }
                    break;
                }
                case ID_TIMES:
                {
                    if(booDrag)
                        deltX.bSet = !(deltX.bSet);
                    break;
                }

            }


        }
        break;
        case WM_LBUTTONDOWN:
        {
            // code to start drag....
            RECT rcClient;
            g_pMaus.x = (LONG)LOWORD(lParam);
            g_pMaus.y = (LONG)HIWORD(lParam);

            // Store the coordinates x and y (once with WM_LBUTTONDOWN comes the x and y coordinates of the mouse)
            if(
               (g_pMaus.x > g_elemInfo.x) &&
               (g_pMaus.x < g_elemInfo.x + g_elemInfo.width) &&
               (g_pMaus.y > g_elemInfo.y) &&
               (g_pMaus.y < g_elemInfo.y + g_elemInfo.height) &&
               TRUE
               )
            {
                // set here mouse...
                g_pDelta.x = g_pMaus.x - g_elemInfo.x;
                g_pDelta.y = g_pMaus.y - g_elemInfo.y;
                booDrag = TRUE;     // confirms that ist the case to move - the mouse was down, moved then up...
            }
            deltX.x1=0;     // to reset it, in the case it will be no WM_MOUSEMOVE after it... IT WORKS!!!
            break;
        }
        case WM_MOUSEMOVE:
        {
            if(booDrag)     // the mouse is down inside the element area...
            {
                GetCursorPos(&g_pMaus);
                RECT rcClient;
                HDC hdc = GetDC(hwnd);
                GetClientRect(hwnd, &rcClient);
                g_pMaus.x = (LONG)LOWORD(lParam);
                g_pMaus.y = (LONG)HIWORD(lParam);
                g_elemInfo.x = g_pMaus.x - g_pDelta.x; // the movement on the x-axis will be realized by "delt"-Structure
                g_elemInfo.y = g_pMaus.y - g_pDelta.y; // and of the y-axis with the next statement...
                // next statement, restrict the y-position of the element - comment it out if you want to free move the element...
                g_elemInfo.y = V_GUTTER - g_elemInfo.height + ((int)(g_elemInfo.y/V_RASTER)*V_RASTER + V_RASTER*(g_elemInfo.y%V_RASTER>(V_RASTER/2)));
                UpdateChar(&rcClient);
                DrawElem(hdc, &rcClient);
                ReleaseDC(hwnd, hdc);
                // now setting the speed...
                deltX.x1 = g_pMaus.x - deltX.x0;
                if(deltX.bSet)  deltX.x0 = g_pMaus.x;

            }
            break;
        }
        case WM_LBUTTONUP:
        {
            RECT rcClient;
            // code to stop drag and to align the element to vertical raster...
            if(booDrag == TRUE)
            {
                xSpeed = 1.8 * deltX.x1;    // WARNING!!! this time, "x1" member of DELTAX is the actually "delta-x"
                if(xSpeed>40) xSpeed=40;    // km/h ???
                booDrag = FALSE;
            }
            UpdateChar(&rcClient);
            break;

        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            }
        }
        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER);
            DeleteObject(g_hbmElem);
            DeleteObject(g_hbmMask);
            PostQuitMessage(0);
        return TRUE;

    }
    return FALSE;

}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}


HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);
	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);
	SetBkColor(hdcMem, crTransparent);
	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);
	return hbmMask;
}

void DrawElem(HDC hdc, RECT* prc)
{
    int         iTrack = 1;
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, prc->right, prc->bottom);
	HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmMask);
	FillRect(hdcBuffer, prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	BitBlt(hdcBuffer, g_elemInfo.x, g_elemInfo.y, g_elemInfo.width, g_elemInfo.height, hdcMem, 0, 0, SRCAND);
	SelectObject(hdcMem, g_hbmElem);
	BitBlt(hdcBuffer, g_elemInfo.x, g_elemInfo.y, g_elemInfo.width, g_elemInfo.height, hdcMem, 0, 0, SRCPAINT);
	SelectObject(hdc, CreatePen(PS_DOT, 2, RGB(0xff, 0xff, 0xff)));
    while(iTrack * V_RASTER < (prc->bottom + V_GUTTER))
    {   // now drawing the grid of tracks...
        MoveToEx(hdcBuffer, 0, iTrack*V_RASTER + V_GUTTER, NULL);
        LineTo(hdcBuffer, prc->right, iTrack*V_RASTER + V_GUTTER);
        iTrack++;

    }
	BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBuffer, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
	SelectObject(hdcBuffer, hbmOldBuffer);
	DeleteDC(hdcBuffer);
	DeleteObject(hbmBuffer);

}

void DefElemRect()
{
    // it defines a rectangle that will enclose our element...
    if(SetRect(g_rc_Elem, g_elemInfo.x, g_elemInfo.y, g_elemInfo.x+g_elemInfo.width, g_elemInfo.y+g_elemInfo.height))
    {
        return;
    }
    return;

}

void InitElem(HWND hwnd)
{
    // put this function in case WM_INITDIALOG
    BITMAP bm;

    g_hbmElem = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_CHAR));
    if(g_hbmElem == NULL)
        MessageBox(hwnd, "Could not load IDB_CHAR!", "Error", MB_OK | MB_ICONEXCLAMATION);
    g_hbmMask = CreateBitmapMask(g_hbmElem, RGB(0, 0, 0));
    if(g_hbmMask == NULL)
        MessageBox(hwnd, "Could not create mask!", "Error", MB_OK | MB_ICONEXCLAMATION);
    GetObject(g_hbmElem, sizeof(bm), &bm);
    ZeroMemory(&g_elemInfo, sizeof(g_elemInfo));
    g_elemInfo.width = bm.bmWidth;
    g_elemInfo.height = bm.bmHeight;
    g_elemInfo.dt = 1;
    g_elemInfo.dx = xSpeed * g_elemInfo.dt;
    g_elemInfo.y = V_GUTTER + V_RASTER - g_elemInfo.height;
    g_elemInfo.dy = V_RASTER;

}

void UpdateChar(RECT *prc)
{
	g_elemInfo.dx = xSpeed * g_elemInfo.dt;     // from left to right
	g_elemInfo.x += g_elemInfo.dx;
    g_elemInfo.y += booSwitch * g_elemInfo.dy;
    booSwitch=FALSE;
	if(g_elemInfo.x < 0)
	{
	    xSpeed = abs(xSpeed);   // REMARK: "xSpeed=-xSpeed;" does not work because the inertial facts of the fine timer...
	}
	if(g_elemInfo.x + g_elemInfo.width > prc->right)
	{
	    xSpeed = -1*abs(xSpeed);
	}
	if(g_elemInfo.y < 0)
	{
		g_elemInfo.y = V_GUTTER + V_RASTER - g_elemInfo.height;
	}
	else if(g_elemInfo.y + g_elemInfo.height > prc->bottom)
	{
		g_elemInfo.y = V_GUTTER + V_RASTER - g_elemInfo.height;

	}

}

