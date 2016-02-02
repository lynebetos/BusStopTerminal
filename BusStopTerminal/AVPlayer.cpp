#include "StdAfx.h"
#include "AVPlayer.h"
// ���ڵ��ڴ���30M���£�CPU��20--30֮��

// ffplay.c�������´�С
//#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
// #define MAX_VIDEOQ_SIZE (5 * 256 * 1024) 

// ����3��const��ֵ���ڴ�ռ��������أ�̫С���沥�Ų�����
// ������ڵ��ڴ���35M���£�CPU��20--40֮��
const size_t  MAX_AUDIOQ_SIZE  = 1024 * 100;  // һ֡���ݵ�ָ���Լ64B
const size_t  MAX_VIDEOQ_SIZE  = 1024;  // һ֡���ݵ�ָ���Լ64B
const size_t  MAX_PICTURE_SIZE = 100;     // һ֡���ݵ�ָ���Լ16B(�ֽ�)    ��Ϊ128ʱ,���ڴ�Ϊ200��M; ��Ϊ3ʱ,���ڴ�Ϊ60M;
const double  AV_SYNC_THRESHOLD     = 0.01;
const double  AV_NOSYNC_THRESHOLD   = 10.0;
const int     AUDIO_DIFF_AVG_NB     = 20;
const int     SDL_AUDIO_BUFFER_SIZE = 1024;
const int     SAMPLE_CORRECTION_PERCENT_MAX = 10;

#define DEFAULT_AV_SYNC_TYPE    AV_SYNC_AUDIO_MASTER
#define FF_REFRESH_EVENT        (SDL_USEREVENT)

enum 
{
    AV_SYNC_AUDIO_MASTER,
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_MASTER
};

AVPacket  g_avpktFlush;

bool      CAVPlayer::m_s_bIsQuit = true;
uint64_t  CAVPlayer::m_s_ullVideoPktPTS      = AV_NOPTS_VALUE;
int       CAVPlayer::m_s_iFullScreenWidth    = 0;  
int       CAVPlayer::m_s_iFullScreenHeight   = 0;  
double CAVPlayer::m_dbVideoPTS = 0.0;

CAVPlayer::CAVPlayer(void)    
    //:
    //m_bOpen(false),
    //m_bPause(false),
    //m_bPausedLast(false),
    //m_bFullScreen(false),
    //m_iScreenWidth(0),
    //m_iScreenHeight(0),
    //m_nVolPos(50),
    //m_nFilePos(0),
    //m_psdlThread_Push(NULL),
    //m_psdlThread_Video(NULL),
    //m_pavCodecCtxVideo(NULL),
    //m_pavFormatCtx(NULL),
    //m_pavsteamVideo(NULL),
    //m_pavsteamAudio(NULL),
    //m_pswsImgConvertCtx(NULL),
    //m_psdlCondPict(NULL),
    //m_psdlCondAudio(NULL),
    //m_psdlCondVideo(NULL),
    //m_nWidth(0),
    //m_nHeight(0)
{
    Clear();
}

CAVPlayer::~CAVPlayer(void)
{
    Close();
}

void    CAVPlayer::Clear()
{
    m_s_ullVideoPktPTS      = AV_NOPTS_VALUE;
    m_s_iFullScreenWidth    = 0;  
    m_s_iFullScreenHeight   = 0;  
	m_dbVideoPTS				 = 0.0;

    m_bOpen		        = false;
    m_bPause		    = false;
    m_bPausedLast		= false;
    m_bFullScreen		= false;
	m_bOver				= false;
	m_bEof					= false;
    m_iScreenWidth		= 0;
    m_iScreenHeight		= 0;
    m_nVolPos		    = 50;
    m_nFilePos		    = 0;
    m_psdlThread_Push	= NULL;
    m_psdlThread_Video	= NULL;
    m_pavCodecCtxVideo	= NULL;
    m_pavFormatCtx		= NULL;
    m_pavsteamVideo		= NULL;
    m_pavsteamAudio		= NULL;
    m_pswsImgConvertCtx	= NULL;
    m_psdlCondPict		= NULL;
    m_psdlCondAudio		= NULL;
    m_psdlCondVideo		= NULL;
//    m_nWidth		    = 0;
//    m_nHeight		    = 0;

    av_init_packet(&g_avpktFlush);
    g_avpktFlush.data = (uint8_t *)"FLUSH";

    m_stcVideoSync.dbFrameLastPTS     = 0;
    m_stcVideoSync.dbFrameLastDelay   = 40e-3;
    m_stcVideoSync.dbFrameTimer       = (double)av_gettime() / 1000000.0;
    m_stcVideoSync.dbClockVideo       = 0;
    m_stcVideoSync.dbClockAudio       = 0;
    m_stcVideoSync.uiBufRemainAudio   = 0;
    m_stcVideoSync.pavsteamAudio      = NULL;
	m_stcVideoSync.dbPTS						= 0;

    m_stcAudioSync.iAVSyncType        = DEFAULT_AV_SYNC_TYPE;   // DEFAULT_AV_SYNC_TYPE;��ֻ����Ƶʱ�������ظ�����Ƶʱ��
    m_stcAudioSync.dbVideoCurrentPTS  = 0;
    m_stcAudioSync.llVideoCurrentPTSTime = av_gettime();

    m_stcDiff.dbAudioDiffAvgCoef = exp(log(0.01 / AUDIO_DIFF_AVG_NB));
    m_stcDiff.iAudioDiffAvgCount = 0;
	m_stcDiff.dbAudioDiffCum = 0.0;
	m_stcDiff.dbAudioDiffThreshold = 0.0;

    m_stcSeek.bSeekReq  = false;
    m_stcSeek.llSeekPos = 0;
}

void    CAVPlayer::Close()
{ 
	LogObject;
    if(m_s_bIsQuit)
    {
        return;
    }
	m_s_bIsQuit = true;


    // ---------------------------------------
    // ֪ͨ������������,��ֹ�����������߳���
    // ---------------------------------------
    SDL_CondSignal(m_psdlCondPict); 
    SDL_CondSignal(m_psdlCondVideo); 
    SDL_CondSignal(m_psdlCondAudio); 
    //SDL_Delay(100);     // �ȴ������˳�
	Sleep(100);

	LogOutInfo("׼���ر���Ƶ�豸");
	SDL_CloseAudio();   // �ر���Ƶ�豸
	LogOutInfo("�ر���Ƶ�豸");

	if(m_pstcAudioCallback)
	{
		//delete m_pstcAudioCallback;
		m_pstcAudioCallback = NULL;
	}

    if(m_psdlThread_Push)
    {
        SDL_WaitThread(m_psdlThread_Push, NULL);
    }

    if(m_psdlThread_Video)
    {
        SDL_WaitThread(m_psdlThread_Video, NULL);
    }

//     if(m_psdlCondPict)
//     {
//         SDL_DestroyCond(m_psdlCondPict);
//     }
// 
//     if(m_psdlCondAudio)
//     {
//         SDL_DestroyCond(m_psdlCondAudio);
//     }
// 
//     if(m_psdlCondVideo)
//     {
//         SDL_DestroyCond(m_psdlCondVideo);
//     }

	LogOutInfo("�رս�����");
    if(m_pavCodecCtxVideo)
    {
        avcodec_close(m_pavCodecCtxVideo);      // Close the codec
    }

	LogOutInfo("�ر���Ƶ�ļ�");
	if(m_pavFormatCtx)
    {
        av_close_input_file(m_pavFormatCtx);    // Close the video file
    }

    EmptyPktQueue(&m_QuePktAudio);
    EmptyPktQueue(&m_QuePktVideo);

    STC_VideoPicture stcVP;
	LogOutInfo("����ͼƬ����");
    while(m_QuePicture.Pop(stcVP))
    {
        SDL_FreeYUVOverlay(stcVP.psdlBmp);
    }

	if(m_pswsImgConvertCtx != NULL)
		sws_freeContext(m_pswsImgConvertCtx);

	SDL_Quit();         // �˳�SDL

	LogOutInfo("���������ڴ�");
	Clear();
    //   exit(0);            // SDL��Ҫexit�����˳�
}

bool    CAVPlayer::Init(size_t nScreenWidth, size_t nScreenHeight)
{
    // ע�����е��ļ���ʽ�ͱ�����
	LogObject;
    av_register_all();

	char szDriverName[20] = {0};

	int nRes = SDL_putenv("SDL_VIDEODRIVER=directx");
    
    // ��ʼ��SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        // fprintf(stderr, "Could not initialize SDL - %s\iChannels", SDL_GetError());
		string strSDLError = "SDL��ʼ��ʧ��, Error = ";
		strSDLError += SDL_GetError();
		LogOutError(strSDLError.c_str());
        return false;
    }

	/* ��ʼ������� */
// 	if (TTF_Init() == -1 )
// 		return 0;

    const SDL_VideoInfo *vi = SDL_GetVideoInfo();
    m_s_iFullScreenWidth  = vi->current_w;
    m_s_iFullScreenHeight = vi->current_h;

    // ������ʾ����ĳ���
    m_iScreenWidth  = nScreenWidth;
    m_iScreenHeight = nScreenHeight;
    SetScreenSize(m_iScreenWidth, m_iScreenHeight);

	char szName[20] = {0};
	char* pName = SDL_VideoDriverName(szName, 20);

    return true;
}

bool    CAVPlayer::SetHwnd(unsigned long ulHwndDisplay)
{
    if(ulHwndDisplay)
    {
        // ���봰��������SDL����Ƶ��ʾ��������
        char szVar[MAX_PATH];

        // %lx��unsinged long     �������Σ�
        // %x�� unsinged int      �����Σ�          
        // ���� SDL_WINDOWID=0x0001
        sprintf(szVar, "SDL_WINDOWID=0x%lx", ulHwndDisplay);
        SDL_putenv(szVar);

		m_hWndParent = (HWND)ulHwndDisplay;

        return  true;
    }

    return false;
}

bool    CAVPlayer::SetScreenSize(size_t nScreenWidth, size_t nScreenHeight)
{ 
	LogObject;
    int iFlags = SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_DOUBLEBUF|SDL_NOFRAME; // |SDL_NOFRAME;   // SDL_NOFRAME �ޱ߿�ͱ�����

    if(! m_bFullScreen)
    {
        iFlags |= SDL_RESIZABLE;
    }
    else        
    {
        iFlags |= SDL_FULLSCREEN;
        nScreenWidth  = m_s_iFullScreenWidth;
        nScreenHeight = m_s_iFullScreenHeight;
    }

#ifndef __DARWIN__
	m_psdlSurface_Screen = SDL_SetVideoMode(nScreenWidth, nScreenHeight, 0, iFlags); 
#else
    m_psdlSurface_Screen = SDL_SetVideoMode(nScreenWidth, nScreenHeight, 24, iFlags);
#endif

    if(!m_psdlSurface_Screen) 
    {
        // fprintf(stderr, "SDL: could not set video mode - exiting\iChannels");
		LogOutError("����SDL��ʾ����ʧ��")
        return  false;
    }

 //   SDL_WM_SetCaption("ItsBirdPlayer", NULL);
 //       SetScale(m_psdlSurface_Screen->w, m_psdlSurface_Screen->h);
    SetScale(nScreenWidth, nScreenHeight);

	SDL_Color cl = {0, 0, 0};
// 	if(SDL_SetColors(m_psdlSurface_Screen, &cl, 0, 0) != 1)
// 	{
// 		return false;
// 	}
    return true;
}

void    CAVPlayer::AllocPicture(SDL_Overlay  * &psdlOverlay_Bmp)
{
    // ����һ����ʾ�㣬��ָ���ֱ��ʣ�width��heightָ��ʾ�ķֱ���
    psdlOverlay_Bmp = SDL_CreateYUVOverlay(m_pavCodecCtxVideo->width, m_pavCodecCtxVideo->height,  
        SDL_YV12_OVERLAY, m_psdlSurface_Screen);
}

bool    CAVPlayer::OpenFile(const char* pszFilePath)
{     
	LogObject;
    // ���ļ�����ȡ�ļ���ͷ��,����Ƿ�����Ƶ�ļ� avformat_open_input
    if(avformat_open_input(&m_pavFormatCtx, pszFilePath, NULL, NULL)!=0)
    {
		LogOutError("����Ƶ�ļ�ʧ��");
        return false; // Couldn't open file
    }

    // ����ļ��е�����Ϣ
    if(av_find_stream_info(m_pavFormatCtx)<0)
    {
		LogOutError("����ļ��е�����Ϣʧ��");
       return false; // Couldn't find stream information
    }

	//avformat_find_stream_info(m_pavFormatCtx, NULL);

    // Dump information about file onto standard error
    av_dump_format(m_pavFormatCtx, 0, pszFilePath, 0);

    m_iStreamIndexVideo = -1;
    m_iStreamIndexAudio = -1;
    for(unsigned i=0; i < m_pavFormatCtx->nb_streams; i++)
    {
        if((-1 == m_iStreamIndexVideo) && (AVMEDIA_TYPE_VIDEO == m_pavFormatCtx->streams[i]->codec->codec_type))
        {
            // Find the first video stream
            m_iStreamIndexVideo = i;
            m_pavsteamVideo     = m_pavFormatCtx->streams[m_iStreamIndexVideo];
            m_stcAudioSync.llVideoCurrentPTSTime = av_gettime();
        }

        if((-1 == m_iStreamIndexAudio) && (AVMEDIA_TYPE_AUDIO == m_pavFormatCtx->streams[i]->codec->codec_type))
        {
            // Find the first audio stream
            m_iStreamIndexAudio = i;
            m_pavsteamAudio     = m_pavFormatCtx->streams[m_iStreamIndexAudio];
            m_stcDiff.dbAudioDiffThreshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / m_pavsteamAudio->codec->sample_rate;
        }
    }

    if (m_iStreamIndexVideo == -1 && m_iStreamIndexAudio == -1)
    {
        return false; // Didn't find a audio stream
    }

    return true;
}

bool    CAVPlayer::OpenAudioCodec()
{
	LogObject;
    if(! m_pavsteamAudio)
    {
        return false;
    }

    // Get a pointer to the codec context for the audio stream
    m_pavCodecCtxAudio = m_pavsteamAudio->codec;

    // ������Ƶ��������
    m_pavCodecAudio = avcodec_find_decoder(m_pavCodecCtxAudio->codec_id);
    if(! m_pavCodecAudio) 
    {
		LogOutError("������Ƶ��������ʧ��");
        // fprintf(stderr, "Unsupported codec!\iChannels");
        return false; // Codec not found
    }

    // �򿪽�����
    if(avcodec_open(m_pavCodecCtxAudio, m_pavCodecAudio) < 0)
    {
        return false; // Could not open codec
    }

    return true;
}

bool    CAVPlayer::OpenVideoCodec()
{
    if(! m_pavsteamVideo)
    {
        return false;
    }

    // Get a pointer to the codec context for the video stream
    m_pavCodecCtxVideo = m_pavsteamVideo->codec;

    // ������Ƶ��������
    m_pavCodecVideo = avcodec_find_decoder(m_pavCodecCtxVideo->codec_id);

    if(! m_pavCodecVideo) 
    {
        // fprintf(stderr, "Unsupported codec!\iChannels");
        return false; // Codec not found
    }

	m_pavCodecCtxVideo->codec = m_pavCodecVideo;
	m_pavCodecCtxVideo->thread_count = 4;

	
	AVDictionaryEntry entry;
	entry.key = "threads";
	AVDictionaryEntry* optEntry = av_dict_get(NULL, "threads", &entry, 0);	


	int frame_threading_supported = (m_pavCodecCtxVideo->codec->capabilities & CODEC_CAP_FRAME_THREADS)
		&& !(m_pavCodecCtxVideo->flags & CODEC_FLAG_TRUNCATED)
		&& !(m_pavCodecCtxVideo->flags & CODEC_FLAG_LOW_DELAY)
		&& !(m_pavCodecCtxVideo->flags2 & CODEC_FLAG2_CHUNKS);

	if(m_pavCodecCtxVideo->thread_count == 1)
	{
		m_pavCodecCtxVideo->active_thread_type = 0;
	}
	else if (frame_threading_supported && (m_pavCodecCtxVideo->thread_type & FF_THREAD_FRAME)) 
	{//  ��codec��ʼ����ʱ������avctx->thread_type |=FF_THREAD_FRAME
		m_pavCodecCtxVideo->active_thread_type = FF_THREAD_FRAME;   //�̵߳���������Ϊ֡����
	} 
	else if (m_pavCodecCtxVideo->codec->capabilities & CODEC_CAP_SLICE_THREADS &&
		m_pavCodecCtxVideo->thread_type & FF_THREAD_SLICE)
	{   //ͬ����Ҫʵ��Ƭ����������Ҫ��codec��ʼ����ʱ����������
		m_pavCodecCtxVideo->active_thread_type = FF_THREAD_SLICE;
	} 
	else if (!(m_pavCodecCtxVideo->codec->capabilities & CODEC_CAP_AUTO_THREADS)) 
	{
		m_pavCodecCtxVideo->thread_count       = 1;
		m_pavCodecCtxVideo->active_thread_type = 0;
	}

	m_pavCodecCtxVideo->get_buffer     = GetBuffer;
	m_pavCodecCtxVideo->release_buffer = ReleaseBuffer;

	if (m_pavCodecCtxVideo->thread_count > 4)
		av_log(m_pavCodecCtxVideo, AV_LOG_WARNING,
		"Application has requested %d threads. Using a thread count greater than %d is not recommended.\n",
		m_pavCodecCtxVideo->thread_count, 4);
		
	// �򿪽�����
	if(avcodec_open(m_pavCodecCtxVideo, m_pavCodecVideo) < 0)
	{
		return false; // Could not open codec
	}


    return true;
}

// ����ֻ����Ƶ������Ƶ��
bool    CAVPlayer::OpenCodec()
{   
    // return OpenVideoCodec() || OpenAudioCodec(); // ��ǰһ��Ϊ��ʱ���Ͳ���ִ�к�һ��
    bool bRet = OpenVideoCodec();
    bRet = (OpenAudioCodec() || bRet);
    return bRet;
}

void    CAVPlayer::OpenAudioDevice()
{
    SDL_AudioSpec      sdlAudioSpecWanted;//, sdlAudioSpecReally;       // sdlAudioSpecWanted����ϣ�������õĲ���ֵ��sdlAudioSpecReally����ʵ�ʱ����õĲ���ֵ
    STC_AudioCallback* pstcAudioCallback = new STC_AudioCallback;

    pstcAudioCallback->pbPause          = &m_bPause;
    pstcAudioCallback->pbIsQuit         = &m_s_bIsQuit;
    pstcAudioCallback->pavCodecCtxAudio = m_pavCodecCtxAudio; 
    pstcAudioCallback->pavsteamAudio    = m_pavsteamAudio;
    pstcAudioCallback->pQuePktAudio     = &m_QuePktAudio;
    pstcAudioCallback->pdbClockAudio    = &m_stcVideoSync.dbClockAudio;
    pstcAudioCallback->puiBufRemainAudio= &m_stcVideoSync.uiBufRemainAudio;
    pstcAudioCallback->psdlCondAudio    = m_psdlCondAudio;
    pstcAudioCallback->vp               = this;

    // ������Ƶ�������Ϣ
    sdlAudioSpecWanted.freq       = m_pavCodecCtxAudio->sample_rate;  // ������
    sdlAudioSpecWanted.format     = AUDIO_S16SYS;                     // ���ݸ�ʽ
    sdlAudioSpecWanted.channels   = m_pavCodecCtxAudio->channels;     // ������ͨ����
    sdlAudioSpecWanted.silence    = 0;                                // ��ʾ������ֵ
    sdlAudioSpecWanted.samples    = SDL_AUDIO_BUFFER_SIZE;            // �����������ĳߴ�
    sdlAudioSpecWanted.callback   = audio_callback;                   // �ص��������������Ƶ���ݣ���������Ƶ��
    sdlAudioSpecWanted.userdata   = pstcAudioCallback;                // SDL�����ص��������еĲ���

    // ����Ƶ�豸
    //if(SDL_OpenAudio(&sdlAudioSpecWanted, &sdlAudioSpecReally) < 0) // ��ʱ����ҪsdlAudioSpecReally
    if(SDL_OpenAudio(&sdlAudioSpecWanted, NULL) < 0) 
    {
        // fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
		delete pstcAudioCallback;
        return ;
    }   

    // ��ͣ��ָ�audio_callback������ִ�У�0�ǻָ�������������ͣ��  
    // ��audio_callback������ʼ�����ı�����
	SDL_PauseAudio(0);
	m_pstcAudioCallback = pstcAudioCallback;

}

bool    CAVPlayer::Play(const char* pszPath)
{
	LogObject;
// 	string strInfo = "��Ƶ���ſ�ʼ �ļ���";
// 	strInfo+= pszPath;
// 
// 	LogOutInfo(strInfo);

    if(! pszPath || ! strlen(pszPath))
    {
        return false;
    }

    if(! m_s_bIsQuit)
    {
        Pause();
        return  true;
    }
    
    m_s_bIsQuit = false;
//    url_set_interrupt_cb(decode_interrupt_cb);

    if( OpenFile(pszPath) )
    {
        if( OpenCodec() )
        {
            m_stcVideoSync.pavsteamAudio   = m_pavsteamAudio;

            m_stcRefreshTmp.pstcAudioSync  = &m_stcAudioSync;
            m_stcRefreshTmp.pstcVideoSync  = &m_stcVideoSync;
            m_stcRefreshTmp.pbPause        = &m_bPause;

            m_psdlCondPict  = SDL_CreateCond();
            m_psdlCondAudio = SDL_CreateCond();
            m_psdlCondVideo = SDL_CreateCond();

			//�����ļ���Ϣ������Ƶ��С��λ��
			int nWidth = m_pavCodecCtxVideo->width;
			int nHeight = m_pavCodecCtxVideo->height;

			double dbSreen = (double)nWidth / (double)nHeight;

			if(dbSreen >= (16 / 9))
			{
				double dbMin = 1080 / (double)nWidth ;
				nHeight = nHeight * dbMin;
				nWidth = 1080;
			}
			else
			{
				double dbMin = 607 / (double)nHeight ;
				nWidth = nWidth * dbMin;
				nHeight = 607;
			}

			double dbper = (double)nWidth / (double)nHeight;
			int nHeight2 = 607;
			int nWidth2 = (int)(607 * dbper);

// 			m_psdlSurface_Screen->w = nWidth2;
// 			m_psdlSurface_Screen->h = nHeight2;

			m_sdlrcPictrue.x = (1080 - nWidth2) / 2;
			m_sdlrcPictrue.y = 0;
			m_sdlrcPictrue.w = nWidth2;
			m_sdlrcPictrue.h = nHeight2;
			
			/*
			double dbIncrement = 100;
			double dbPos = get_master_clock(&m_stcRefreshTmp);
			dbPos += dbIncrement;
			StreamSeek((int64_t)(dbPos * AV_TIME_BASE), (int)dbIncrement);
			*/

			LogOutInfo("��������������߳�");
            m_psdlThread_Push = SDL_CreateThread(thread_avpacket_push, this);       // ��������������߳�

            if(m_pavsteamAudio) 
            {
				LogOutInfo("����Ƶ�豸����������Ƶ�߳�");
	             OpenAudioDevice();  // ����Ƶ�豸����������Ƶ�߳�
            }

            if(m_pavsteamVideo) 
            {                  
				LogOutInfo("������Ƶ�߳�");
	             m_psdlThread_Video = SDL_CreateThread(thread_decode_video, this);   // ������Ƶ�߳�

				 if(!m_psdlThread_Video)
				 {
					 LogOutError("������Ƶ�߳�ʧ��");
				 }

                SetScale(m_psdlSurface_Screen->w , m_psdlSurface_Screen->h );         // ���÷ֱ���Ϊw*h
                SDL_TimerID sdlTimerID =  SDL_AddTimer(1, SDLEventRefresh, &m_stcRefreshTmp);                 // ֻ�败��һ��FF_REFRESH_EVENT�¼���֮���ݹ鴥��
            }   

            SDLWaitEvent();
//			LogOutInfo("��Ƶ�������");
			return  true;
        }
    }
	

    return false;
}

bool    CAVPlayer::PlayNext()
{

    return  true;
}

bool    CAVPlayer::PlayPre()
{

    return  true;
}

void    CAVPlayer::Stop()
{
	LogObject;
	if(m_s_bIsQuit)
	{
		return;
	}
	m_s_bIsQuit = true;
	LogOutInfo("�ر���Ƶ");

	// ---------------------------------------
	// ֪ͨ������������,��ֹ�����������߳���
	// ---------------------------------------
	SDL_CondSignal(m_psdlCondPict); 
	SDL_CondSignal(m_psdlCondVideo); 
	SDL_CondSignal(m_psdlCondAudio); 
	//SDL_Delay(100);     // �ȴ������˳�
	Sleep(100);

	LogOutInfo("�ر���Ƶ�豸");
	SDL_CloseAudio();   // �ر���Ƶ�豸

	LogOutInfo("�ȴ������߳̽���");
	if(m_psdlThread_Push)
	{
		SDL_WaitThread(m_psdlThread_Push, NULL);
	}

	LogOutInfo("�ȴ���Ƶ�����߳̽���");
	if(m_psdlThread_Video)
	{
		SDL_WaitThread(m_psdlThread_Video, NULL);
	}

	LogOutInfo("m_psdlCondPict");
	if(m_psdlCondPict)
	{
		SDL_DestroyCond(m_psdlCondPict);
	}

	LogOutInfo("m_psdlCondAudio");
	if(m_psdlCondAudio)
	{
		SDL_DestroyCond(m_psdlCondAudio);
	}

	LogOutInfo("m_psdlCondVideo");
	if(m_psdlCondVideo)
	{
		SDL_DestroyCond(m_psdlCondVideo);
	}

	LogOutInfo("�رս�����");
	if(m_pavCodecCtxVideo)
	{
		avcodec_close(m_pavCodecCtxVideo);      // Close the codec
	}

	LogOutInfo("�ر���Ƶ�ļ�");
	if(m_pavFormatCtx)
	{
		av_close_input_file(m_pavFormatCtx);    // Close the video file
	}

	EmptyPktQueue(&m_QuePktAudio);
	EmptyPktQueue(&m_QuePktVideo);

	STC_VideoPicture stcVP;
	LogOutInfo("����ͼƬ����");
	while(m_QuePicture.Pop(stcVP))
	{
		SDL_FreeYUVOverlay(stcVP.psdlBmp);
	}

	//LogOutInfo("׼���ر���Ƶ�豸");

	LogOutInfo("���������ڴ�");
	Clear();
}

void    CAVPlayer::Pause()
{
    m_bPause = !m_bPause;
    if (! m_bPause) // ���ȡ����ͣ�����ȡ���ڵ�ʱ��
    {
        m_stcAudioSync.dbVideoCurrentPTS = get_video_clock(&m_stcRefreshTmp);
        m_stcVideoSync.dbFrameTimer      += (av_gettime() - m_stcAudioSync.llVideoCurrentPTSTime) / 1000000.0;
    }
}

bool    CAVPlayer::VolumeSet(size_t nVol)
{

    return  true;
}

bool    CAVPlayer::VolumeIncrease()
{

    return  true;
}

bool    CAVPlayer::VolumeReduce()
{

    return  true;
}

bool    CAVPlayer::SeekTo(size_t nPos)
{

    return  true;
}

bool    CAVPlayer::Forward()
{

    return  true;
}

bool    CAVPlayer::Backward()
{

    return  true;
}

void    CAVPlayer::SetScale(size_t nWidth, size_t nHeight)
{
    //m_nWidth  = nWidth;
    //m_nHeight = nHeight;
        
    m_iScreenWidth  = nWidth;
    m_iScreenHeight = nHeight;
}

void    CAVPlayer::SDLWaitEvent()
{
    SDL_Event         sdlEvent;           
    STC_VideoPicture  stcVPTmp;
    STC_VideoRefresh* pstcRefreshTmp;
    double dbIncrement, dbPos;
	LogObject;

    while(! m_s_bIsQuit)
    {
        SDL_WaitEvent(&sdlEvent);               

		if(FF_REFRESH_EVENT != sdlEvent.type)
		{
			TRACE("%d\n", sdlEvent.type);
		}
        switch(sdlEvent.type)
        {
        case FF_REFRESH_EVENT:      // ˢ�²���ʾͼ��  
            if(m_pavsteamVideo)     // ���������Ƶ
            {
                pstcRefreshTmp = (STC_VideoRefresh*)(sdlEvent.user.data1);
                if( m_QuePicture.Pop(stcVPTmp) )
                {                        
//                    DisplayPicture(m_nWidth, m_nHeight, stcVPTmp.psdlBmp);  // ��ʾͼƬ����ֱ���Ϊm_nWidth*m_nHeight
                    DisplayPicture(m_sdlrcPictrue, stcVPTmp.psdlBmp);  // ��ʾͼƬ����ֱ���Ϊm_nWidth*m_nHeight
					//log
					stringstream sstr;
					sstr<<"��ǰ��Ƶ֡��PTS = "<<stcVPTmp.dbPTS;
					//LogOutDebug(sstr.str().c_str());
					sstr.str() = "";

                    SDL_FreeYUVOverlay(stcVPTmp.psdlBmp);
                    pstcRefreshTmp->pstcVideoSync->dbPTS = stcVPTmp.dbPTS;

                    // SDL_AddTimer(30, SDLEventRefresh, NULL);             // ֮ǰû��ͬ����Ƶ����30ms���ٶ�ˢ��ͼƬ��Ҳ���Ի�ýϺõ�Ч��(���Դ˵�Ӱ���Թ�)
                    video_refresh(pstcRefreshTmp);

					

					double dbCurPTS = m_s_ullVideoPktPTS * av_q2d(m_pavsteamVideo->time_base);
					if(abs(dbCurPTS - 121.609) <= 1e-3)
					{
						int a = 0;
					}

					//log
					//stringstream sstr;
					sstr<<"��ǰ��Ƶ֡��PTS = "<<stcVPTmp.dbPTS;
					//LogOutDebug(sstr.str().c_str());
					sstr.str().clear();

// 					if(dbCurPTS >= m_dbVideoPTS)
// 					{
// 						m_bOver = true;
// 						return;
// 					}
                }
                else 
                {
                    SDL_AddTimer(1, SDLEventRefresh, pstcRefreshTmp);
					LogOutInfo("��Ƶ����������һ֡��׼���˳���Ƶ�����߳�");

					if(m_bEof == true)
					{
						LogOutInfo("�˳���Ƶ�����߳�");
						return;
					}
                }
            }
            break;
        case SDL_QUIT:
			LogOutInfo("���յ�SDL���˳���Ϣ");
            Close();
            break;

        case SDL_KEYDOWN:
            switch(sdlEvent.key.keysym.sym) 
            {
            case SDLK_LEFT:
                dbIncrement = -10.0;
                goto GOTO_SEEK;
            case SDLK_RIGHT:
                dbIncrement = 10.0;
                goto GOTO_SEEK;
            case SDLK_UP:
                dbIncrement = 60.0;
                goto GOTO_SEEK;
            case SDLK_DOWN:
                dbIncrement = -60.0;
GOTO_SEEK:
                dbPos = get_master_clock(&m_stcRefreshTmp);
                dbPos += dbIncrement;
                StreamSeek((int64_t)(dbPos * AV_TIME_BASE), (int)dbIncrement);
                break;            

            case SDLK_p:
            case SDLK_SPACE:
                Pause();
                break;

            case SDLK_ESCAPE:
            case SDLK_q:
                Close();
                break;

            case SDLK_f:
                m_bFullScreen = !m_bFullScreen;
                SetScreenSize(0,0); // ���������������
                break;

                //case SDLK_s: //S: Step to next frame
                //    step_to_next_frame();
                //    break;
                //case SDLK_a:
                //    if (cur_stream)
                //        stream_cycle_channel(cur_stream, AVMEDIA_TYPE_AUDIO);
                //    break;
                //case SDLK_v:
                //    if (cur_stream)
                //        stream_cycle_channel(cur_stream, AVMEDIA_TYPE_VIDEO);
                //    break;
                //case SDLK_t:
                //    if (cur_stream)
                //        stream_cycle_channel(cur_stream, AVMEDIA_TYPE_SUBTITLE);
                //    break;
                //case SDLK_w:
                //    toggle_audio_display();
                //    break;

            default:
                break;
            }  

        case SDL_VIDEORESIZE:
            if (m_pavsteamVideo)
            {
                //if(! m_bFullScreen && sdlEvent.resize.w <= m_s_iFullScreenWidth && sdlEvent.resize.h <= m_s_iFullScreenHeight)
                //{
                //    m_iScreenWidth = sdlEvent.resize.w;
                //    m_iScreenHeight = sdlEvent.resize.h;
                //}

                //SetScreenSize(m_iScreenWidth, m_iScreenHeight);   // ��MFC�ĶԻ��������д�������SDL_VIDEORESIZE��Ϣ���Բ���Ҫ
            }
            break;
            //case SDL_MOUSEBUTTONDOWN: // ��굥��
            //    break;
            //case SDL_MOUSEMOTION:     // ����ƶ�
            //    break;

        default:
            break;
        }
    }
}

void    CAVPlayer::ReadFrameToBmp(AVFrame *pavFrame, AVCodecContext  *pavCodecCtxVideo, SDL_Overlay *psdlBmp)
{
    if(! pavFrame || ! psdlBmp)
    {
        return;
    }

    AVPicture avpicture;

    SDL_LockYUVOverlay(psdlBmp);   
    avpicture.data[0] = psdlBmp->pixels[0];
    avpicture.data[1] = psdlBmp->pixels[2];
    avpicture.data[2] = psdlBmp->pixels[1];
    avpicture.linesize[0] = psdlBmp->pitches[0];
    avpicture.linesize[1] = psdlBmp->pitches[2];
    avpicture.linesize[2] = psdlBmp->pitches[1];

    // ���԰�һ�� AVFrame�ṹ���ָ��ת��ΪAVPicture�ṹ���ָ��    
    // (AVPicture *)pavFrame �������һ֡ͼ���������Ϣ ��Ⱥ͸߶�Ϊ��ΪpavCodecCtxVideo->width, pavCodecCtxVideo->height
    // 
    // ��֡��ԭʼ��ʽ(pavCodecCtxVideo->pix_fmt) ת����ΪRGB��ʽ
    // Convert the image into YUV format that SDL uses
    // dst_pix_fmt = PIX_FMT_YUV420P;
    // �ϰ溯��Ϊimg_convert(&avpicture, dst_pix_fmt,(AVPicture *)pFrame, pVideoPlayer->video_st->codec->pix_fmt, pVideoPlayer->video_st->codec->width, pVideoPlayer->video_st->codec->height);
    // �°溯��Ϊavpicture_deinterlace(&avpicture, (AVPicture *)pavFrame, pavCodecCtxVideo->pix_fmt, pavCodecCtxVideo->width, pavCodecCtxVideo->height);
    // ���ܸ�ǿ�ĺ���Ϊsws_scale
    if(m_pswsImgConvertCtx == NULL) // ֻ����һ��
    {
        int w = pavCodecCtxVideo->width;
        int h = pavCodecCtxVideo->height;

        m_pswsImgConvertCtx = sws_getContext(w, h, pavCodecCtxVideo->pix_fmt, w, h, 
            PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
        if(m_pswsImgConvertCtx == NULL)
        {
            fprintf(stderr, "Cannot initialize the conversion context!\n");
            exit(1);
        }
    }
    sws_scale(m_pswsImgConvertCtx, pavFrame->data, pavFrame->linesize,
        0, pavCodecCtxVideo->height, avpicture.data, avpicture.linesize);

   SDL_UnlockYUVOverlay(psdlBmp);
}

int thread_avpacket_push(void *argv)
{
    CAVPlayer* pVideoPlayer = (CAVPlayer*)argv;
    AVPacket avpacket;
    size_t sizeVideo = 0, sizeAudio = 0;
	LogObject;

	//LogOutInfo("�����ȡ���ݰ�ѭ��");

    while(! pVideoPlayer->m_s_bIsQuit)
    {
        //if (is->abort_request)
        //    break;

        if (pVideoPlayer->m_bPause != pVideoPlayer->m_bPausedLast)  // �����ͣ״̬�ı��ˣ������ִ�У���ִֻ֤��һ������Ĵ���
        {
            pVideoPlayer->m_bPausedLast = pVideoPlayer->m_bPause;

            if (pVideoPlayer->m_bPause)
            {
                av_read_pause(pVideoPlayer->m_pavFormatCtx);
            }
            else
            {
                av_read_play(pVideoPlayer->m_pavFormatCtx);
            }
        }

#if defined(CONFIG_RTSP_DEMUXER) || defined(CONFIG_MMSH_PROTOCOL)
        if (pVideoPlayer->m_bPause &&
            (!strcmp(pVideoPlayer->m_pavFormatCtx->iformat->name, "rtsp") ||
            (pVideoPlayer->m_pavFormatCtx->pb && !strcmp(url_fileno(pVideoPlayer->m_pavFormatCtx->pb)->prot->name, "mmsh")))) {
                /* wait 10 ms to avoid trying to get another packet */
                /* XXX: horrible */
                SDL_Delay(10);
                continue;
        }
#endif

        // seek stuff goes here
        if(pVideoPlayer->m_stcSeek.bSeekReq)    // ����п�����˲���
        {
            int64_t llSeekTarget = pVideoPlayer->m_stcSeek.llSeekPos;
            int     iSeekRel     = pVideoPlayer->m_stcSeek.iSeekFlags;
            int     iStreamIndex = -1;
            int     iRet         = -1;

            if     (pVideoPlayer->m_iStreamIndexVideo >= 0) 
				iStreamIndex = pVideoPlayer->m_iStreamIndexVideo;
            else if(pVideoPlayer->m_iStreamIndexAudio >= 0) 
				iStreamIndex = pVideoPlayer->m_iStreamIndexAudio;

            if(iStreamIndex>=0)
            {
                //	llSeekTarget= av_rescale_q(llSeekTarget, AV_TIME_BASE_Q, m_pavFormatCtx->streams[iStreamIndex]->time_base);
                AVRational cq = {1, AV_TIME_BASE};
                llSeekTarget= av_rescale_q(llSeekTarget, cq, pVideoPlayer->m_pavFormatCtx->streams[iStreamIndex]->time_base);
            }

            //int64_t llSeekMin= iSeekRel > 0 ? llSeekTarget - iSeekRel + 2: INT64_MIN;
            //int64_t llSeekMax= iSeekRel < 0 ? llSeekTarget - iSeekRel - 2: INT64_MAX;
            //iRet = avformat_seek_file(pVideoPlayer->m_pavFormatCtx, -1, llSeekMin, llSeekTarget, llSeekMax, iSeekRel);

            iRet = av_seek_frame(pVideoPlayer->m_pavFormatCtx, iStreamIndex, llSeekTarget, iSeekRel);
            if(iRet < 0)
            {
                fprintf(stderr, "%s: error while seeking\n", pVideoPlayer->m_pavFormatCtx->filename);
            }
            else
            {
                if(pVideoPlayer->m_iStreamIndexAudio >= 0)
                {
                    EmptyPktQueue(&pVideoPlayer->m_QuePktAudio);
                    pVideoPlayer->m_QuePktAudio.Push(g_avpktFlush);     // ���һ���������Ϣ�İ�
                    SDL_CondSignal(pVideoPlayer->m_psdlCondAudio);      // ֪ͨ����������֪ͨ��Ƶ���ݳ���
                }
                if(pVideoPlayer->m_iStreamIndexVideo >= 0) 
                {
                    EmptyPktQueue(&pVideoPlayer->m_QuePktVideo);
                    pVideoPlayer->m_QuePktVideo.Push(g_avpktFlush);     // ���һ���������Ϣ�İ�
                    SDL_CondSignal(pVideoPlayer->m_psdlCondVideo);      // ֪ͨ����������֪ͨ��Ƶ���ݳ���
                }
            }

            pVideoPlayer->m_stcSeek.bSeekReq = false;
        }

        // ����ȡ��һ����С�����ݺ�,������ͣ��ȡ4
        pVideoPlayer->m_QuePktVideo.Size(sizeVideo);
        pVideoPlayer->m_QuePktAudio.Size(sizeAudio);

        if(sizeVideo > MAX_VIDEOQ_SIZE || sizeAudio > MAX_AUDIOQ_SIZE) 
        {
            // ---------------------------------------------------------------------------------  
            // ����ӳٻ�����Ӱ�첥��Ч��,��Ϊ10��100��1000��Ч������һ��
            // ---------------------------------------------------------------------------------  
			//LogOutInfo(" SDL_Delay��100��");
            //SDL_Delay(100);                             // ����1sֻ���ż�ʮ֡ͼ��,���Կ����ӳ���ô��
            //SDL_Delay(1000 * MAX_VIDEOQ_SIZE / 256);  // ����1s����256֡
            //SDL_Delay(20); 
			Sleep(100);
            continue;
        }


		int nRes = av_read_frame(pVideoPlayer->m_pavFormatCtx, &avpacket);
        if(nRes < 0)  // av_read_frame()��ȡһ�������������浽AVPacket�ṹ����
        {
            // ���ṹ���Ƿ��ȡ�ļ�����
//            if(url_ferror(pVideoPlayer->m_pavFormatCtx->pb) == 0)
			pVideoPlayer->m_bEof = true;
			stringstream strOut;
			strOut<<pVideoPlayer->m_pavFormatCtx->flags<<" error="<<nRes ;
			//LogOutError(strOut.str().c_str());
			//AVERROR(ENOMEM);  
			//nRes = AVERROR(nRes);
			if(url_feof(pVideoPlayer->m_pavFormatCtx->pb) == 0)
            {
                //SDL_Delay(100); /* no error; wait for user input */
				Sleep(100);
                continue;
            }
            else 
            {
               break;
            }
        }

		stringstream sstr;
		sstr<<"��ȡһ�����ݣ�����="<<avpacket.size;
		LogOutDebug(sstr.str().c_str());
		sstr.str("");

		stringstream strOut;
		strOut<<pVideoPlayer->m_pavFormatCtx->flags<<" error="<<nRes ;
		//LogOutError(strOut.str().c_str());

        // ��ȡ����
        if(avpacket.stream_index == pVideoPlayer->m_iStreamIndexAudio) 
        {           
            pVideoPlayer->m_QuePktAudio.Push(avpacket);     // ��Ƶ���������
            SDL_CondSignal(pVideoPlayer->m_psdlCondAudio);  // ֪ͨ����������֪ͨ��Ƶ���ݳ���
        } 
		else if(avpacket.stream_index == pVideoPlayer->m_iStreamIndexVideo) 
		{           
			pVideoPlayer->m_QuePktVideo.Push(avpacket);     // ��Ƶ���������
			SDL_CondSignal(pVideoPlayer->m_psdlCondVideo);  // ֪ͨ����������֪ͨ��Ƶ���ݳ���
		}
        else 
        {            
            av_free_packet(&avpacket);  // Free the avpacket that was allocated by av_read_frame
        }
    }
	LogOutInfo("�˳���ȡ���ݰ�ѭ��");

    return 0;
}

int thread_decode_video(void *argv)  
{
    CAVPlayer *pVideoPlayer    = (CAVPlayer*)argv;
    SDL_Overlay  *psdlOverlay_Bmp = NULL; 
    AVFrame  *pavFrame  = avcodec_alloc_frame(); // ����һ����Ƶ֡���ڴ� 
    AVPacket  avpacket;
    double    dbPTS; 
    size_t    nPictSize = 0;
	size_t    nPkSize = 0;
    int       iFrameFinished;
    STC_VideoPicture stcVPTmp;
	int nAllFrameCount = 0;
	int nFrameI = 0;
	LogObject;

    while(! pVideoPlayer->m_s_bIsQuit) 
    {
        pVideoPlayer->m_QuePicture.Size(nPictSize);

        if(nPictSize >= MAX_PICTURE_SIZE || pVideoPlayer->m_bPause) // ���ͼƬ�������ˣ�������ͣ�ˣ���ȴ�
        {
            //SDL_Delay(100); 
			//LogOutInfo("�ȴ�ͼƬ����");
           //SDL_Delay(1);  // �ȴ�ͼƬ����
			Sleep(1);
            //SDL_CondWait(pVideoPlayer->m_psdlCondPict, NULL); // ������SDL_CondWait�������������ѭ�������ֵ���ȴ�����ֻ�ó����̵߳ȴ�����̵߳�֪ͨ
            continue;
        }           

        if(! (pVideoPlayer->m_QuePktVideo.Pop(avpacket)))       
        {
            // ������������ݣ��ȴ��������
			//LogOutInfo("�ȴ��������");
            //SDL_CondWait(pVideoPlayer->m_psdlCondVideo, NULL);
			Sleep(100);
			//LogOutInfo("���������");
            continue;
        }  

        if(avpacket.data == g_avpktFlush.data) 
        {
            // ����������Ϣ�İ�����ˢ��������Ϣ
            avcodec_flush_buffers(pVideoPlayer->m_pavsteamVideo->codec);
            continue;
        }
		nAllFrameCount ++;
		TRACE("All Frame Count = %d\n", nAllFrameCount);

		/*{//����֮ǰ���ҹؼ�֡
			uint8_t* pData = avpacket.data;
			if(avpacket.flags & AV_PKT_FLAG_KEY)
			{
				nFrameI++;
				TRACE("I Frame Count = %d\n", nFrameI);
			}
			else if(nAllFrameCount % 5 == 0)
			{
				av_free_packet(&avpacket);               
				continue;
			}

		}*/

        pVideoPlayer->m_s_ullVideoPktPTS = avpacket.pts;
        avcodec_decode_video2(pVideoPlayer->m_pavCodecCtxVideo, pavFrame, &iFrameFinished, &avpacket);      // �Ѱ����ݽ���Ϊ֡����

        dbPTS = pVideoPlayer->GetPTS(&avpacket, pavFrame);  // avcodec_decode_video2�����еõ�֡��ʱ���,���ϴδ���İ��еõ�DTS

		//log
		stringstream sstr;
		sstr<<"avcodec_decode_video2�����еõ�֡��ʱ��� = "<<dbPTS;
		//LogOutDebug(sstr.str().c_str());
		sstr.str().clear();

        if(iFrameFinished)              
        {
            // ����ɹ���ȡ����Ƶ֡��Ϣ
            dbPTS = pVideoPlayer->GetVideoSyncPTS(pavFrame, dbPTS);
			
            pVideoPlayer->AllocPicture(psdlOverlay_Bmp);    // ����ͼƬ�ڴ�
            pVideoPlayer->ReadFrameToBmp(pavFrame, pVideoPlayer->m_pavCodecCtxVideo, psdlOverlay_Bmp);      // ��֡���ݽ���ΪbmpͼƬ

            stcVPTmp.dbPTS   = dbPTS;   // ��¼��psdlBmp��dbPTS,����ȷ��ʱ���ֵ
            stcVPTmp.psdlBmp = psdlOverlay_Bmp;

			//DisplayPicture(pVideoPlayer->m_sdlrcPictrue, stcVPTmp.psdlBmp);  // ��ʾͼƬ����ֱ���Ϊm_nWidth*m_nHeight
			//video_refresh(&pVideoPlayer->m_stcRefreshTmp);

            pVideoPlayer->m_QuePicture.Push(stcVPTmp);      // ͼƬ���,���ڴ����ָ��,����ÿ�ζ���ҪAllocPicture           
            SDL_CondSignal(pVideoPlayer->m_psdlCondPict);
        }
        av_free_packet(&avpacket);               
    }


    av_free(pavFrame);
    return 0;       
}

//ԭ��:int avcodec_decode_audio2(AVCodecContext *avctx, int16_t *samples,
//                         int *frame_size_ptr,
//                         const uint8_t *buf, int buf_size);
//    avctx ��       ������������
//    samples��      �������  ������ݵĻ����׵�ַ.
//    frame_size_ptr:�������������������֡�ɽⷵ��0������ʧ�ܷ��ظ���������ɹ����ؽ����һ֡��Ƶ��ռ�ռ䣬���ֽ�Ϊ��λ 
//    buf��          ����������������ݵĻ���
//    buf_size��     ���������buf�Ĵ�С
//    ����ֵ��       ��֡�ɽⷵ��0������ʧ�ܷ��ظ���������ɹ����ؽ���ǰһ֡��Ƶ��ռ�ռ�
//
// avcodec_decode_audio2()�����Ĺ��ܾ������Ľ��ú��� avcodec_decode_video()һ����
//    Ψһ������������һ����������в�ֹһ������֡�����������Ҫ���úܶ����������������е����ݡ�   
//    ͬ ʱҲҪ��ס����ָ��audio_buf��ǿ��ת������ΪSDL��������8λ���ͻ���ָ���ffmpeg������������16λ������ָ�롣
//    ���һ�ε���û�д�����һ���������ݣ���¼�´��������Ǹ�λ���ˣ��´ν��Ŵ������������������Ϊһ����Ƶ�������������Ƶ֡����������
// ��Ӧ��Ҳ��ע�⵽ len1��data_size�Ĳ�ͬ��len1��ʾ����ʹ�õ����ݵ��ڰ��еĴ�С��data_size��ʾʵ�ʷ��ص�ԭʼ�������ݵĴ�С��
//    �����ǵõ�һЩ���ݵ�ʱ���������̷�������һ���Ƿ���Ȼ��Ҫ�Ӷ����еõ����Ӷ�����ݻ��������Ѿ�����ˡ�
//    ���������Ȼ�и��Ӷ������Ҫ�������ǰ������浽��һ�Ρ�������������һ�����Ĵ����������Ҫ�ͷ�����
// audio_callback�е�audio_decode_frame�����Ӷ����ж�ȡ���������ݣ����������͸�SDL������SDL���൱�����ǵ�������
// STC_AudioCallback����������������ã����޶�������
// pavCodecCtx  : ���룬����������
// uszAudioBuf  : ���������ɹ���������Ļ�����׵�ַ
// iAudioBufSize������, �����Ǳ��uszAudioBuf�Ĵ�С
// ����ֵ��������һ֡��Ƶ������󣬻���ռ�õ�ʵ�ʴ�С�����ֽ�Ϊ��λ��Ϊ������ʾʧ��
int audio_decode_frame(STC_AudioCallback *pstcAudioCallback, uint8_t *uszAudioBuf,const  int iAudioBufSize, double *pdbPTS) 
{
	LogObject;
    AVPacket avpacket;  // ���һ����Ƶ�����ж��֡�����ڱ����м�״̬
    int iAudioPktSize;  // ��Ƶ�����ݴ�С�����audio_pkt �����м�״̬
    int iLen, iDataSize , iChannels;
    double dbPTS;

    do
    {   
        if(*pstcAudioCallback->pbPause)
        {
            return -1;
        }

        // ��ȡ��Ƶ����, ��ȡ����ʱ�ȴ�10ms                  
        if(! pstcAudioCallback->pQuePktAudio->Pop(avpacket)) 
        {            
            Sleep(10);    // ��SDL_CondWait������,��Ϊ��10ms��һ��������,���ҿ���10ms̫������SDL_CondWait���ܵȴ�һ����ȷ��ʱ�䣬���Ҳ����ظ�ѯ���Ƿ�������
            //SDL_CondWait(pstcAudioCallback->psdlCondAudio, NULL);
            continue;
        }

        if(avpacket.data == g_avpktFlush.data) 
        {
            // ����������Ϣ�İ�����ˢ��������Ϣ
            avcodec_flush_buffers(pstcAudioCallback->pavsteamAudio->codec);
            continue;
        }

        // ������Ƶ����
        iAudioPktSize = avpacket.size;

        if(avpacket.pts != AV_NOPTS_VALUE) 
        {
            // ��ȡ����ʱ���������ʱ��
            (*(pstcAudioCallback->pdbClockAudio)) = av_q2d(pstcAudioCallback->pavsteamAudio->time_base) * avpacket.pts;
        }

        while(iAudioPktSize > 0) 
        {
            iDataSize = iAudioBufSize;
			//LogOutInfo("������Ƶ֡");
            iLen = avcodec_decode_audio3(pstcAudioCallback->pavCodecCtxAudio, (int16_t *)uszAudioBuf, &iDataSize, &avpacket) ;
			Sleep(1);
            if(iLen < 0) 
            {
                // ����ʧ��,�������һ֡����
				//LogOutError("������Ƶ֡ʧ��");
               iAudioPktSize = 0;
                break;
            }
			//LogOutInfo("������Ƶ֡�ɹ�");
           iAudioPktSize -= iLen;

            if(iDataSize <= 0) 
            {
                /* No data yet, get more frames */
                continue;
            }

            dbPTS = *(pstcAudioCallback->pdbClockAudio);
            *pdbPTS = dbPTS; // ����ͬ��ʱ��
			//log
			stringstream sstr;
			sstr<<"avcodec_decode_audio3��Ƶ֡��PTS = "<<dbPTS;
			//LogOutDebug(sstr.str().c_str());
			sstr.str().clear();

            iChannels = 2 * pstcAudioCallback->pavsteamAudio->codec->channels;

            // �������ʱ��ҲҪ��������ʱ��
            (*(pstcAudioCallback->pdbClockAudio)) += (double)iDataSize / (double)(iChannels * pstcAudioCallback->pavsteamAudio->codec->sample_rate);

            /* We have data, return it and come back for more later */
            return iDataSize;   // iDataSize��ʾʵ�ʷ��ص�ԭʼ�������ݵĴ�С
        }

        if(avpacket.data)
        {
            av_free_packet(&avpacket);
        }

    }while(! (*pstcAudioCallback->pbIsQuit));

	LogOutInfo("�˳���Ƶ�߳�");

    return -1;  // ����˳���ѭ������˵�������߳̽�Ҫ�˳�����ʱӦ����-1֪ͨ�ϲ㺯��
}

// ���ѭ���ѽ��д�뵽�м仺������������������д��iLenStream�ֽ�,
// ����������û���㹻���ݵ�ʱ����ȡ���������,���ߵ������ж������ݵ�ʱ�򱣴�����Ϊ����ʹ�á�
// ���uszAudioBuf�Ĵ�СΪ 1.5��������֡�Ĵ�С�Ա�����һ���ȽϺõĻ��壬�������֡�Ĵ�С��ffmpeg�����ġ�
//
// pUserdata�����룬puszStream�������iLenStream(һ��Ϊ4096)������
// audio_callback�����Ĺ����ǵ���audio_decode_frame�������ѽ�������ݿ�uszAudioBuf׷����puszStream�ĺ��棬
// ͨ��SDL���audio_callback�Ĳ��ϵ��ã����Ͻ������ݣ�Ȼ��ŵ�puszStream��ĩβ��SDL����ΪpuszStream�����ݹ�����һ֡��Ƶ�ˣ��Ͳ�������
//
// ����������len����stream��д���ݵ��ڴ����߶ȣ��Ƿ����audio_callback����д�뻺���С��
// pUserdata ��ṹ��STC_AudioCallback����������������ã����޶�������
// puszStream������Ҫ����������д��Ļ�����ָ��
// iLenStream�ǻ������Ĵ�С
void audio_callback(void *pUserdata, Uint8 *puszStream, int iLenStream) 
{
	LogObject;
    STC_AudioCallback *pstcAudioCallback = (STC_AudioCallback *)pUserdata; 
    CAVPlayer      *pVideoPlayer      = pstcAudioCallback->vp;
    AVCodecContext    *aCodecCtx         = pstcAudioCallback->pavCodecCtxAudio;

    //  uint8_t uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];    // �����Ƶ�Ļ���   
    //  uszAudioBuf����16�ֽڶ��룬������avcodec_decode_audio3���ᷢ���ڴ�����쳣��
	static DECLARE_ALIGNED(16, uint8_t, uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2]);  
	//static DECLARE_ALIGNED(16, uint8_t, uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2)]);  

    // Ҫ���ɾ�̬�����������������������
    static unsigned int iAudioBufSize  = 0;    // �������Ƶ���ݴ�С
    static unsigned int iAudioBufIndex = 0;    // �������Ƶ���ݴ�С
    int iLenBuf, iAudioSize;
    double dbPTS;

    for( ; iLenStream > 0; iLenStream -= iLenBuf) 
    {
        if(iAudioBufIndex >= iAudioBufSize) 
        {
            /* We have already sent all our data; get more */
            iAudioSize = audio_decode_frame(pstcAudioCallback, uszAudioBuf, sizeof(uszAudioBuf), &dbPTS);

            if(iAudioSize < 0) 
            {
                /* If error, output silence */
                iAudioBufSize = 1024; // arbitrary?
                memset(uszAudioBuf, 0, iAudioBufSize);
				LogOutInfo("quit audio");
				return;
            } 
            else 
            {
                iAudioSize = pVideoPlayer->GetAudioSyncPTS((int16_t *)uszAudioBuf, iAudioSize, dbPTS);  // ͬ����Ƶʱ��
                iAudioBufSize = iAudioSize;
            }
            iAudioBufIndex = 0;
        }

        iLenBuf = iAudioBufSize - iAudioBufIndex;
        iLenBuf = iLenBuf > iLenStream ? iLenStream : iLenBuf;
        memcpy(puszStream, (uint8_t *)uszAudioBuf + iAudioBufIndex, iLenBuf);

        puszStream += iLenBuf;
        iAudioBufIndex += iLenBuf;     
        (*(pstcAudioCallback->puiBufRemainAudio)) = iAudioBufSize - iAudioBufIndex;
    }
}

void   DisplayPicture(SDL_Rect& rcDisplay, SDL_Overlay *psdlBmp)
{
    SDL_Rect sdlRect = rcDisplay;

//     sdlRect.x = 0;
//     sdlRect.y = 0;
//     sdlRect.w = iWidth;
//     sdlRect.h = iHeight;
    SDL_DisplayYUVOverlay(psdlBmp, &sdlRect);
	
}

Uint32 SDLEventRefresh(Uint32 interval, void *opaque)
{
    SDL_Event sdlEvent;

    sdlEvent.type = FF_REFRESH_EVENT;
    sdlEvent.user.data1 = opaque;   // ���ݵ�����

    SDL_PushEvent(&sdlEvent);       // ����FF_REFRESH_EVENT�¼�
    return 0; 
}

double  CAVPlayer::GetPTS(AVPacket*  pavpacket, AVFrame  *pavFrame)
{
    double dbPTS = 0;

    if(pavpacket->dts == AV_NOPTS_VALUE 
        && pavFrame->opaque && *(uint64_t*)pavFrame->opaque != AV_NOPTS_VALUE) 
    {
        dbPTS = (double)*(uint64_t *)pavFrame->opaque;
    } 
    else if(pavpacket->dts != AV_NOPTS_VALUE) 
    {
        dbPTS = (double)pavpacket->dts;
    } 

	double dbAllPTS = m_pavsteamVideo->duration * av_q2d(m_pavsteamVideo->time_base);
	CAVPlayer::m_dbVideoPTS = dbAllPTS;

    dbPTS *= av_q2d(m_pavsteamVideo->time_base);
    return dbPTS;
}

int     CAVPlayer::GetBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame)
{
    int iRet = avcodec_default_get_buffer(pavCodecCtxAudio, pavFrame);
    uint64_t *dbPTS = (uint64_t *)av_malloc(sizeof(uint64_t));

    *dbPTS = m_s_ullVideoPktPTS;
    pavFrame->opaque = dbPTS;
    return iRet;
}

void     CAVPlayer::ReleaseBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame)
{
    if(pavFrame) 
    {
        av_freep(&pavFrame->opaque);
    }
    avcodec_default_release_buffer(pavCodecCtxAudio, pavFrame);
}

double CAVPlayer::GetVideoSyncPTS(AVFrame *pavframeSrc, double dbPTS)
{
    double dbFrameDelay;

    if(dbPTS != 0)
    {
        /* if we have dbPTS, set video clock to it */
        m_stcVideoSync.dbClockVideo = dbPTS;
    }
    else
    {
        /* if we aren't given a dbPTS, set it to the clock */
        dbPTS = m_stcVideoSync.dbClockVideo;
    }

    /* update the video clock */
    dbFrameDelay   = av_q2d(m_pavsteamVideo->codec->time_base);
    /* if we are repeating a frame, adjust clock accordingly */
    dbFrameDelay   += pavframeSrc->repeat_pict * (dbFrameDelay * 0.5);
    m_stcVideoSync.dbClockVideo += dbFrameDelay;
    return dbPTS;
}

/* Add or subtract samples to get a better sync, return new
audio buffer size */
int CAVPlayer::GetAudioSyncPTS(short *pshSamples, int iSamplesSize, double dbPTS) 
{
    int n;
    double dbRefClock;

    n = 2 * m_pavsteamAudio->codec->channels;

    if(m_stcAudioSync.iAVSyncType != AV_SYNC_AUDIO_MASTER)
    {
        double dbDiff, dbDiffAvg;
        int iWantedSize, iMinSize, iMaxSize; 

        dbRefClock = get_master_clock(&m_stcRefreshTmp);
        dbDiff = get_audio_clock(&m_stcRefreshTmp) - dbRefClock;

        if(dbDiff < AV_NOSYNC_THRESHOLD)
        {
            // accumulate the diffs
            m_stcDiff.dbAudioDiffCum = dbDiff + m_stcDiff.dbAudioDiffAvgCoef * m_stcDiff.dbAudioDiffCum;

            if(m_stcDiff.iAudioDiffAvgCount < AUDIO_DIFF_AVG_NB) 
            {
                m_stcDiff.iAudioDiffAvgCount++;
            } 
            else
            {
                dbDiffAvg = m_stcDiff.dbAudioDiffCum * (1.0 - m_stcDiff.dbAudioDiffAvgCoef);
                if(fabs(dbDiffAvg) >= m_stcDiff.dbAudioDiffThreshold) 
                {
                    iWantedSize = iSamplesSize + ((int)(dbDiff * m_pavsteamAudio->codec->sample_rate) * n);
                    iMinSize    = iSamplesSize * ((100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100);
                    iMaxSize    = iSamplesSize * ((100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100);

                    if(iWantedSize < iMinSize)
                    {
                        iWantedSize = iMinSize;
                    } 
                    else if (iWantedSize > iMaxSize)
                    {
                        iWantedSize = iMaxSize;
                    }

                    if(iWantedSize < iSamplesSize) 
                    {
                        /* remove shSamples */
                        iSamplesSize = iWantedSize;
                    } 
                    else if(iWantedSize > iSamplesSize) 
                    {
                        uint8_t *puszSamplesEnd, *puszTmp;
                        int i_nb_samples = iSamplesSize - iWantedSize;

                        /* add samples by copying final sample*/
                        puszSamplesEnd = (uint8_t *)pshSamples + iSamplesSize - n;
                        puszTmp  = puszSamplesEnd + n;

                        while(i_nb_samples > 0) 
                        {
                            memcpy(puszTmp, puszSamplesEnd, n);
                            puszTmp      += n;
                            i_nb_samples -= n;
                        }

                        iSamplesSize = iWantedSize;
                    }
                }
            }
        } 
        else
        {
            /* difference is TOO big; reset dbDiff stuff */
            m_stcDiff.iAudioDiffAvgCount = 0;
            m_stcDiff.dbAudioDiffCum     = 0;
        }
    }
    return iSamplesSize;
}

// �������������˺ܶ��飺���ȣ����Ǳ�֤���ڵ�ʱ�������һ��ʱ���֮��Ĵ���dbDelay��������ġ�
// ������ǵĻ������ǾͲ²������ϴε��ӳ١����ţ��� ����һ��ͬ����ֵ����Ϊ��ͬ����ʱ�����鲢��������ô�����ġ�
// ��ffplay��ʹ��0.01��Ϊ����ֵ������Ҳ��֤��ֵ�����ʱ���֮��ļ���̡���� ���ǰ���С��ˢ��ֵ����Ϊ10���롣
// ���Ǽ򵥵�����µ�֡��ʱ����ʱ�������͵��Ե�ϵͳʱ����бȽϣ�Ȼ��ʹ���Ǹ�ֵ��������һ��ˢ��
// ʹ��Ƶͬ������Ƶ   
void video_refresh(void *pUserData) 
{
    STC_VideoRefresh  *pstcRefreshTmp  = (STC_VideoRefresh*)pUserData;

    double dbActualDelay;
    double dbSyncThreshold, dbRefClock, dbDiff;
    double dbDelay = pstcRefreshTmp->pstcVideoSync->dbPTS - pstcRefreshTmp->pstcVideoSync->dbFrameLastPTS; /* the pts from last time */

    pstcRefreshTmp->pstcAudioSync->dbVideoCurrentPTS      = pstcRefreshTmp->pstcVideoSync->dbPTS;
    pstcRefreshTmp->pstcAudioSync->llVideoCurrentPTSTime  = av_gettime();

    if(dbDelay <= 0 /*|| dbDelay >= 1.0*/) 
    {
        /* if incorrect dbDelay, use previous one */
        dbDelay = pstcRefreshTmp->pstcVideoSync->dbFrameLastDelay;
    }
    /* save for next time */
    pstcRefreshTmp->pstcVideoSync->dbFrameLastDelay = dbDelay;
    pstcRefreshTmp->pstcVideoSync->dbFrameLastPTS   = pstcRefreshTmp->pstcVideoSync->dbPTS;

    /* update delay to sync to audio if not master source */
    if(pstcRefreshTmp->pstcAudioSync->iAVSyncType != AV_SYNC_VIDEO_MASTER)  // ����ƵΪ��ʱ�ӵ�ʱ��ͬ����Ƶ
    {
        /* update dbDelay to sync to audio */
        //        dbRefClock = get_audio_clock(pstcRefreshTmp);
        dbRefClock = get_master_clock(pstcRefreshTmp);
        dbDiff     = pstcRefreshTmp->pstcVideoSync->dbPTS - dbRefClock;

        /* Skip or repeat the frame. Take dbDelay into account
        FFPlay still doesn't "know if this pVideoPlayer the best guess." */
        dbSyncThreshold = (dbDelay > AV_SYNC_THRESHOLD) ? dbDelay : AV_SYNC_THRESHOLD;

        if(fabs(dbDiff) < AV_NOSYNC_THRESHOLD) 
        {
            if(dbDiff <= -dbSyncThreshold)
            {
                dbDelay = 0;
            }
            else if(dbDiff >= dbSyncThreshold) 
            {
                dbDelay = 2 * dbDelay;
            }
        }

    }

    pstcRefreshTmp->pstcVideoSync->dbFrameTimer += dbDelay;
    /* computer the REAL dbDelay */
    dbActualDelay = pstcRefreshTmp->pstcVideoSync->dbFrameTimer - (av_gettime() / 1000000.0);

    if(dbActualDelay < 0.010) 
    {
        /* Really it should skip the picture instead */
        dbActualDelay = 0.010;
    }

    SDL_AddTimer((int)(dbActualDelay * 1000 ), SDLEventRefresh, pUserData);   
}

// ��ȡ��ƵĿǰ���ŵ�ʱ��
// audio_callback����������ʱ���������ݴ����������Ƶ����ǵ�����������С�
// ����ζ����������ʱ���м�¼��ʱ���ʵ�ʵ�Ҫ��̫�ࡣ
// �������Ǳ���Ҫ���һ�����ǻ��ж���û��д��
double get_audio_clock(STC_VideoRefresh *pstcRefreshTmp)
{
    double dbPTS        = pstcRefreshTmp->pstcVideoSync->dbClockAudio;                       // ����Ϊ��һ�β��ŵ���Ƶλ�ã�Ȼ��������λ�������һ�β��ŵ���Ƶλ��
    int iBufSizeRemain  = pstcRefreshTmp->pstcVideoSync->uiBufRemainAudio;                   // ʣ�����Ƶ�����С   
    int iChannels       = pstcRefreshTmp->pstcVideoSync->pavsteamAudio->codec->channels * 2; // ������ͨ����
    int iBytesPerSec    = 0;

    if(pstcRefreshTmp->pstcVideoSync->pavsteamAudio)
    {
        iBytesPerSec = pstcRefreshTmp->pstcVideoSync->pavsteamAudio->codec->sample_rate * iChannels;
    }

    if(iBytesPerSec) 
    {
        dbPTS -= (double)iBufSizeRemain / iBytesPerSec;
    }
    return dbPTS;
}

double get_video_clock(STC_VideoRefresh *pstcRefreshTmp) 
{
    double dbDelta = 0;

    if (! pstcRefreshTmp->pbPause) // ���������ͣ���Ÿ��£�����dbDelta = 0;
    {
        dbDelta = (av_gettime() - pstcRefreshTmp->pstcAudioSync->llVideoCurrentPTSTime) / 1000000.0;
    }
    return pstcRefreshTmp->pstcAudioSync->dbVideoCurrentPTS + dbDelta;
}

double get_external_clock()
{
    return av_gettime() / 1000000.0;
}

double get_master_clock(STC_VideoRefresh *pstcRefreshTmp)
{
    if(pstcRefreshTmp->pstcAudioSync->iAVSyncType == AV_SYNC_VIDEO_MASTER) 
    {
        return get_video_clock(pstcRefreshTmp);
    } 
    else if(pstcRefreshTmp->pstcAudioSync->iAVSyncType == AV_SYNC_AUDIO_MASTER)
    {
        return get_audio_clock(pstcRefreshTmp);
    } 
    else
    {
        return get_external_clock();
    }
}

int CAVPlayer::decode_interrupt_cb(void) 
{
    return (int)m_s_bIsQuit;
}

void    EmptyPktQueue(CMutexQueue<AVPacket>* pQuePkt)
{
    AVPacket avpacket;

    while(pQuePkt->Pop(avpacket))
    {
        av_free_packet(&avpacket);  
    }
}

void CAVPlayer::StreamSeek(int64_t llPos, int rel)
{
    if(! m_stcSeek.bSeekReq) // ͬһʱ��ֻ����һ�ο�����˲���
    {
        m_stcSeek.llSeekPos  = llPos;
        m_stcSeek.iSeekFlags = rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;  // ������ǿ���
        m_stcSeek.bSeekReq   = true;
    }
}