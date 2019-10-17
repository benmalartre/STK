#ifndef _MDI_ARRANGE_H_
#define _MDI_ARRANGE_H_
#include "ColumnTrack.h"

WORD    Arrange_InitInstance();
HWND    Arrange_Create(HWND);
LRESULT CALLBACK Arrange_WndProc(HWND, UINT, WPARAM, LPARAM);
void    Arrange_UpdateScrollBars(HWND);

#endif
