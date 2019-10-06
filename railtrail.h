#ifndef ___RAILTRAIL_H_INCLUDED_02837163271463271983
#define ___RAILTRAIL_H_INCLUDED_02837163271463271983




typedef struct _ELEMINFO
{
	int     width;
	int     height;
	int     x;
	int     y;

	int     dx;
	int     dy;
	int     dt;

}ELEMINFO;

typedef struct _DELTAX
{
    int     x0;
    int     x1;
    bool    bSet;

    public:
        _DELTAX()
        {
            bSet = TRUE;
        }

}DELTAX;

DELTAX deltX;

HINSTANCE   hInst;                  // I don`t know why \\\||||\||||

const int   ID_TIMER = 131;         // fine timer for fine adjusting of the position of the element
const int   ID_TIMES = 132;         // raw timer, for operations like detecting the speed set for the element

const int   MAX_ELEMS = 0xff;        // the maximal number of elements on the entire program

const int   V_GUTTER = 24;
const int   V_RASTER = 64;

int     iSpeed = 20;
int     iDecel = 0;
int     iAlpha = 80;
FLOAT       xSpeed = 0;
FLOAT       xDecel = 1.07;

bool        booSwitch = FALSE;      // if the element is ready to be moved vertically (over its current track) it will be true
bool        booDrag = FALSE;        // if the element is ready to be moved, it will be true
bool        g_bDragged = FALSE;     // if the element was moved with the mouse, will be true

LPRECT      g_rc_Elem;           // get together with the element...
POINT       g_pMaus;
POINT       g_pDelta;

ELEMINFO g_elemInfo;
HBITMAP g_hbmElem = NULL;
HBITMAP g_hbmMask = NULL;


//////////////////// Function prototypes //////////////////////////
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
void InitElem(HWND hwnd);
void DrawElem(HDC hdc, RECT* prc);
void UpdateChar(RECT *prc);
void DefElemRect();
















































#endif // ___RAILTRAIL_H_INCLUDED_02837163271463271983
