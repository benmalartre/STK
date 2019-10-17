#ifndef _POSBARBOX_HEADER_H_
#define _POSBARBOX_HEADER_H_


// Notification Code

#define POBXN_SETFOCUS   1
#define POBXN_KILLFOCUS  2

// Message 

#define POBXM_SETHSCROLL        (WM_USER+1)
#define POBXM_SETPRIME          (WM_USER+2)
#define POBXM_SETBKCOLOR        (WM_USER+3)

// Control Class

#define POSBARBOX_CLASS  "PosBarBoxClass32"

// Fonctions

WORD WINAPI PosBarBox_InitControl(HINSTANCE);

#endif
