#ifndef _MDI_TEMPO_HEADER_H_
#define _MDI_TEMPO_HEADER_H_


WORD    TempoMdi_InitInstance();
HWND    TempoMdi_Create(HWND);
LRESULT CALLBACK TempoMdi_WndProc(HWND, UINT, WPARAM, LPARAM);
void    TempoMdi_UpdateScrollBars(HWND);

#endif
