#include <windows.h>
#include "Mixer.h"

CMixer::CMixer()
{
    m_hMixer = 0;
    m_bSuccess = FALSE;
}    

CMixer::~CMixer()
{
    Close();
}

void CMixer::Close()
{
    if(m_hMixer)
    {    
        mixerClose(m_hMixer);
        m_hMixer = NULL;
    }    
}

BOOL CMixer::Init()
{
    m_dwMultipleItems = 0;
    m_dwControlType = 0;
    m_dwIndex = 0;
    
    if(m_hMixer)
        return FALSE;
    if(mixerGetNumDevs()<1)
        return FALSE;
    if(mixerOpen(&m_hMixer, 0, 0, 0, CALLBACK_NULL)!=MMSYSERR_NOERROR)
        return FALSE;
    return TRUE;
}    


BOOL CMixer::GetStereoMixControl()
{
    if(!m_hMixer)
        return FALSE;
    
    m_dwMultipleItems = 0;
    m_dwControlType = 0;
    m_dwIndex = 0;    
    
    MIXERLINE mxl;
    
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
    if(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxl, MIXER_OBJECTF_HMIXER| 
    MIXER_GETLINEINFOF_COMPONENTTYPE)!= MMSYSERR_NOERROR) 
        return FALSE;
    
    MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
    
    m_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = mxl.dwLineID;
    mxlc.dwControlType = m_dwControlType;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;
    if(mixerGetLineControls((HMIXEROBJ)m_hMixer, &mxlc, MIXER_OBJECTF_HMIXER|
    MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
    {
        m_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
        mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = m_dwControlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = &mxc;
        if(mixerGetLineControls((HMIXEROBJ)m_hMixer, &mxlc, MIXER_OBJECTF_HMIXER|
        MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
            return FALSE;
    }    
    
    m_iMixerControlID = mxc.dwControlID;
    m_dwMultipleItems = mxc.cMultipleItems;
    if(m_dwMultipleItems==0)
        return FALSE;
    
    MIXERCONTROLDETAILS_LISTTEXT *pmxcd_list = 
        new MIXERCONTROLDETAILS_LISTTEXT[m_dwMultipleItems];
    
    if(!pmxcd_list)
        return FALSE;
    
    MIXERCONTROLDETAILS mxcd;
	
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_iMixerControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_dwMultipleItems;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
	mxcd.paDetails = pmxcd_list;	
	
	if(mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mxcd, 
    MIXER_GETCONTROLDETAILSF_LISTTEXT)!=MMSYSERR_NOERROR)
    {
        delete[]pmxcd_list;
	    return FALSE;
	}    
    
    MIXERLINE mxlBuf;
    DWORD i;
    m_dwIndex = m_dwMultipleItems;
    
    for(i=0; i<m_dwMultipleItems; i++)
    {
        mxlBuf.cbStruct = sizeof(MIXERLINE);
        mxlBuf.dwLineID = pmxcd_list[i].dwParam1;
        if((mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxlBuf, MIXER_OBJECTF_HMIXER| 
        MIXER_GETLINEINFOF_LINEID)== MMSYSERR_NOERROR)&&
        (mxlBuf.dwComponentType==MIXERLINE_COMPONENTTYPE_SRC_ANALOG)&&
        ((!lstrcmp(pmxcd_list[i].szName, "Mixage stéréo"))|| 
        (!lstrcmp(pmxcd_list[i].szName, "Stereo Mix"))))
        {
            m_dwIndex = i;
            break;
        }
    }
    
    delete[]pmxcd_list;
    
    if(m_dwIndex>=m_dwMultipleItems)
        return FALSE;
    return TRUE;
}    

BOOL CMixer::SetStereoMixValue(int value)
{
    if((!m_hMixer)||(!m_dwMultipleItems)||(m_dwIndex>=m_dwMultipleItems))
        return FALSE;
    
    MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN *pmxcd_b = new MIXERCONTROLDETAILS_BOOLEAN[m_dwMultipleItems];
    
    if(!pmxcd_b)
        return FALSE;
    
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_iMixerControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_dwMultipleItems;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = pmxcd_b;	
	
	if(mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,MIXER_OBJECTF_HMIXER| 
    MIXER_GETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)
    {
        delete[]pmxcd_b;
        return FALSE;
    }    
    
    if((value)&&(m_dwControlType==MIXERCONTROL_CONTROLTYPE_MUX))
        ZeroMemory(pmxcd_b, sizeof(MIXERCONTROLDETAILS_BOOLEAN)*m_dwMultipleItems);
    
    pmxcd_b[m_dwIndex].fValue = value;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_iMixerControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_dwMultipleItems;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = pmxcd_b;
	if(mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,MIXER_OBJECTF_HMIXER| 
    MIXER_SETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)
    {
        delete[]pmxcd_b;
        return FALSE;
    }
	
    delete[]pmxcd_b;
    
    return TRUE;
}    

