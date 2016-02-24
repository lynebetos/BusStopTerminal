#include "StdAfx.h"
#include "VideoPlayList.h"

CVideoPlayList* CVideoPlayList::m_pInstance = NULL;

CVideoPlayList::CVideoPlayList(void)
{
}

CVideoPlayList::~CVideoPlayList(void)
{
}

CVideoPlayList* CVideoPlayList::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CVideoPlayList;
	}

	return m_pInstance;
}

void CVideoPlayList::ReleaseInstance()
{
	if(m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}
