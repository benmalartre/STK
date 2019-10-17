#ifndef _MDI_MIXER_HEADER_H_
#define _MDI_MIXER_HEADER_H_


WORD    MixerMdi_InitInstance();
HWND    MixerMdi_Create(HWND);
LRESULT CALLBACK MixerMdi_WndProc(HWND, UINT, WPARAM, LPARAM);

#endif
