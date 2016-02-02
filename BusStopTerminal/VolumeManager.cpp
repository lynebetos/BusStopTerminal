#include "StdAfx.h"
#include "VolumeManager.h"

CVolumeManager* CVolumeManager::m_pInstance = NULL;

CVolumeManager::CVolumeManager(void)
{
	m_bIsOpen = FALSE;
	m_iMixerNums = 0;
	m_hMixerMic = 0;
	m_hMixerSpk = 0;
	for (int i = 0; i < HMIXER_NUM_MAX; i ++)
		m_barrOpened[i] = FALSE;

	MixerOpen();
}

CVolumeManager::~CVolumeManager(void)
{
	MixerClose();
}

CVolumeManager* CVolumeManager::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CVolumeManager;
	}

	return m_pInstance;
}

void CVolumeManager::ReleaseInstance()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

BOOL CVolumeManager::MixerOpen()
{
	m_iMixerNums = mixerGetNumDevs();
	if (mixerGetNumDevs() < 1)
		return FALSE;
	m_iMixerNums = min(m_iMixerNums, HMIXER_NUM_MAX);
	m_bIsOpen = FALSE;
	for (int i = 0; i < m_iMixerNums; i ++)
	{
		MMRESULT mmresult = mixerOpen(&m_hMixer[i], (UINT)i, 0, 0, MIXER_OBJECTF_MIXER);

		if (mmresult == MMSYSERR_NOERROR)
		{
			m_barrOpened[i] = TRUE;
			m_bIsOpen = TRUE;
		}
	}
	return m_bIsOpen;
}

void CVolumeManager::MixerClose()
{
	if (m_bIsOpen)
	{
		for (int i = 0; i < m_iMixerNums; i ++)
		{
			if (m_barrOpened[i])
			{
				mixerClose(m_hMixer[i]);
				m_barrOpened[i] = FALSE;
			}
		}
		m_bIsOpen = FALSE;
	}
}

BOOL CVolumeManager::GetMicrophoneID()
{
	MMRESULT mmresult;
	if (!m_bIsOpen)
		return FALSE;
	m_MixerLineMicrophone.cbStruct = sizeof(m_MixerLineMicrophone);
	m_MixerLineMicrophone.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	for (int i = 0; i < m_iMixerNums; i ++)
	{
		mmresult = mixerGetLineInfo((HMIXEROBJ)m_hMixer[i], &m_MixerLineMicrophone,
			MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
		if (mmresult == MMSYSERR_NOERROR)
		{
			m_hMixerMic = m_hMixer[i];
			break;
		}
	}
	if (m_hMixerMic == 0)
		return FALSE;
	DWORD dwConnections = m_MixerLineMicrophone.cConnections;
	DWORD dwLineID = 0;
	for (DWORD i = 0; i < dwConnections; i++)
	{
		m_MixerLineMicrophone.dwSource = i;

		MMRESULT mr = mixerGetLineInfo((HMIXEROBJ)m_hMixerMic, &m_MixerLineMicrophone,

			MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);

		if (mr != MMSYSERR_NOERROR)

		{

			break;

		}

		if (m_MixerLineMicrophone.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)

		{

			dwLineID = m_MixerLineMicrophone.dwLineID;

			break;

		}

	}

	if (dwLineID == 0)

	{

		return FALSE;

	} 
	return TRUE;
}

BOOL CVolumeManager::GetMicrophoneLevel(DWORD* dwLevel, DWORD* dwLevelMax)
{
	MMRESULT mmresult;
	MIXERCONTROL mxc;

	MIXERLINECONTROLS mxlc;
	if (!m_bIsOpen)
		return FALSE;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);

	mxlc.dwLineID = m_MixerLineMicrophone.dwLineID;

	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;

	mxlc.cControls = 1;

	mxlc.cbmxctrl = sizeof(MIXERCONTROL);

	mxlc.pamxctrl = &mxc;

	mmresult = mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixerMic), &mxlc, MIXER_OBJECTF_HMIXER |MIXER_GETLINECONTROLSF_ONEBYTYPE);

	if (mmresult != MMSYSERR_NOERROR)

		return FALSE;
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;

	MIXERCONTROLDETAILS mxcd;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);

	mxcd.dwControlID = mxc.dwControlID;

	mxcd.cChannels = 1;

	mxcd.cMultipleItems = 0;

	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);

	mxcd.paDetails = &mxcdVolume;

	mmresult = mixerGetControlDetails(
		reinterpret_cast<HMIXEROBJ>(m_hMixerMic),
		&mxcd,
		MIXER_GETCONTROLDETAILSF_VALUE);

	if (mmresult != MMSYSERR_NOERROR)

		return FALSE;
	*dwLevel = mxcdVolume.dwValue;
	*dwLevelMax = mxc.Bounds.dwMaximum;
	return TRUE;
}

BOOL CVolumeManager::SetMicrophoneLevel(DWORD dwLevel)
{
	MMRESULT mmresult;
	MIXERCONTROL mxc;

	MIXERLINECONTROLS mxlc;
	if (!m_bIsOpen)
		return FALSE;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);

	mxlc.dwLineID = m_MixerLineMicrophone.dwLineID;

	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;

	mxlc.cControls = 1;

	mxlc.cbmxctrl = sizeof(MIXERCONTROL);

	mxlc.pamxctrl = &mxc;

	mmresult = mixerGetLineControls(

		reinterpret_cast<HMIXEROBJ>(m_hMixerMic),

		&mxlc,

		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);

	if (mmresult != MMSYSERR_NOERROR)

		return FALSE;
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set =

	{

		dwLevel

	};

	MIXERCONTROLDETAILS mxcd_Set;

	mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);

	mxcd_Set.dwControlID = mxc.dwControlID;

	mxcd_Set.cChannels = 1;

	mxcd_Set.cMultipleItems = 0;

	mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);

	mxcd_Set.paDetails = &mxcdVolume_Set;

	mmresult = mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixerMic),

		&mxcd_Set,

		MIXER_OBJECTF_HMIXER |

		MIXER_SETCONTROLDETAILSF_VALUE);

	return (mmresult == MMSYSERR_NOERROR);
}

BOOL CVolumeManager::GetSpeakerLevel(DWORD* dwLevel, DWORD* dwLevelMax)
{
	MMRESULT mmr;
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	if (!m_bIsOpen)
		return FALSE;
	for (int i = 0; i < m_iMixerNums; i ++)
	{
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		mmr = mixerGetLineInfo((HMIXEROBJ)m_hMixer[i], &mxl, MIXER_OBJECTF_HMIXER|MIXER_GETLINEINFOF_COMPONENTTYPE);
		if ( mmr == MMSYSERR_NOERROR)
		{
			m_hMixerSpk = m_hMixer[i];
			break;
		}
	}
	if (m_hMixerSpk == 0)
		return FALSE;
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.cControls = mxl.cControls;
	mxlc.dwControlID = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cbmxctrl = sizeof(mxc);
	mxlc.pamxctrl = &mxc;
	mmr = mixerGetLineControls((HMIXEROBJ)m_hMixerSpk, &mxlc, MIXER_OBJECTF_HMIXER|MIXER_GETLINECONTROLSF_ONEBYTYPE);
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mxcdVolume);
	mxcd.paDetails = &mxcdVolume;
	mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixerSpk, &mxcd, 
		MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);

	if (mmr != MMSYSERR_NOERROR)

		return FALSE;
	*dwLevelMax = mxc.Bounds.dwMaximum;
	*dwLevel = (double)mxcdVolume.dwValue / (double)*dwLevelMax * 100;
	return TRUE;
}

BOOL CVolumeManager::SetSpeakerLevel(DWORD dwLevelPercent)
{
	MMRESULT mmr;
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	if (!m_bIsOpen)
		return FALSE;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	mmr = mixerGetLineInfo((HMIXEROBJ)m_hMixerSpk, &mxl, MIXER_OBJECTF_HMIXER|MIXER_GETLINEINFOF_COMPONENTTYPE);
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.cControls = mxl.cControls;
	mxlc.dwControlID = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cbmxctrl = sizeof(mxc);
	mxlc.pamxctrl = &mxc;
	mmr = mixerGetLineControls((HMIXEROBJ)m_hMixerSpk, &mxlc, MIXER_OBJECTF_HMIXER|MIXER_GETLINECONTROLSF_ONEBYTYPE);
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set =
	{
		dwLevelPercent * 65535 / 100
	};

	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mxcdVolume);
	mxcd.paDetails = &mxcdVolume_Set;
	mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixerSpk, &mxcd, 
		MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);

	return (mmr == MMSYSERR_NOERROR);

}
