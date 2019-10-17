#ifndef _MIXER_HEADER_H_
#define _MIXER_HEADER_H_

#include <mmsystem.h>

class CMixer
{
public:
	CMixer();
	~CMixer();
	
	BOOL  Init();
	void  Close();
	
	BOOL  GetStereoMixControl();
	BOOL  SetStereoMixValue(int);

private:
	HMIXER m_hMixer;
    DWORD  m_dwControlType;
    DWORD  m_dwMultipleItems;
    DWORD  m_dwIndex;
    UINT   m_iMixerControlID;
	BOOL   m_bSuccess;
};

#endif
