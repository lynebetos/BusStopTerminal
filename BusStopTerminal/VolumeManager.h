#pragma once

#include <mmsystem.h>
#define HMIXER_NUM_MAX 10

#pragma comment(lib, "winmm.lib")

class CVolumeManager
{
public:
	CVolumeManager(void);
	~CVolumeManager(void);

	static CVolumeManager* GetInstance();
	static void ReleaseInstance();

private:
	HMIXER m_hMixer[HMIXER_NUM_MAX];
	BOOL m_barrOpened[HMIXER_NUM_MAX];
	HMIXER m_hMixerMic;
	HMIXER m_hMixerSpk;
	int m_iMixerNums;
	MIXERLINE m_MixerLineMicrophone;

	static CVolumeManager* m_pInstance;

public:
	BOOL MixerOpen();
	void MixerClose();
	BOOL m_bIsOpen;
	BOOL GetMicrophoneID();
	BOOL GetMicrophoneLevel(DWORD* dwLevel, DWORD* dwLevelMax);
	BOOL SetMicrophoneLevel(DWORD dwLevel);
	BOOL GetSpeakerLevel(DWORD* dwLevel, DWORD* dwLevelMax);
	BOOL SetSpeakerLevel(DWORD dwLevel);

};
