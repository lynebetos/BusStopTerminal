#include "StdAfx.h"
#include "AVPlayer.h"
// 调节到内存在30M上下，CPU在20--30之间

// ffplay.c中用以下大小
//#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
// #define MAX_VIDEOQ_SIZE (5 * 256 * 1024) 

// 下面3个const的值和内存占用密切相关，太小画面播放不流畅
// 这里调节到内存在35M上下，CPU在20--40之间
const size_t  MAX_AUDIOQ_SIZE  = 1024 * 100;  // 一帧数据的指针大约64B
const size_t  MAX_VIDEOQ_SIZE  = 1024;  // 一帧数据的指针大约64B
const size_t  MAX_PICTURE_SIZE = 100;     // 一帧数据的指针大约16B(字节)    设为128时,总内存为200多M; 设为3时,总内存为60M;
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

    m_stcAudioSync.iAVSyncType        = DEFAULT_AV_SYNC_TYPE;   // DEFAULT_AV_SYNC_TYPE;当只有音频时，不能呢个用视频时钟
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
    // 通知所有条件成立,防止他们阻塞在线程里
    // ---------------------------------------
    SDL_CondSignal(m_psdlCondPict); 
    SDL_CondSignal(m_psdlCondVideo); 
    SDL_CondSignal(m_psdlCondAudio); 
    //SDL_Delay(100);     // 等待他们退出
	Sleep(100);

	LogOutInfo("准备关闭音频设备");
	SDL_CloseAudio();   // 关闭音频设备
	LogOutInfo("关闭音频设备");

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

	LogOutInfo("关闭解码器");
    if(m_pavCodecCtxVideo)
    {
        avcodec_close(m_pavCodecCtxVideo);      // Close the codec
    }

	LogOutInfo("关闭视频文件");
	if(m_pavFormatCtx)
    {
        av_close_input_file(m_pavFormatCtx);    // Close the video file
    }

    EmptyPktQueue(&m_QuePktAudio);
    EmptyPktQueue(&m_QuePktVideo);

    STC_VideoPicture stcVP;
	LogOutInfo("清理图片队列");
    while(m_QuePicture.Pop(stcVP))
    {
        SDL_FreeYUVOverlay(stcVP.psdlBmp);
    }

	if(m_pswsImgConvertCtx != NULL)
		sws_freeContext(m_pswsImgConvertCtx);

	SDL_Quit();         // 退出SDL

	LogOutInfo("清理播放器内存");
	Clear();
    //   exit(0);            // SDL需要exit才能退出
}

bool    CAVPlayer::Init(size_t nScreenWidth, size_t nScreenHeight)
{
    // 注册所有的文件格式和编解码库
	LogObject;
    av_register_all();

	char szDriverName[20] = {0};

	int nRes = SDL_putenv("SDL_VIDEODRIVER=directx");
    
    // 初始化SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        // fprintf(stderr, "Could not initialize SDL - %s\iChannels", SDL_GetError());
		string strSDLError = "SDL初始化失败, Error = ";
		strSDLError += SDL_GetError();
		LogOutError(strSDLError.c_str());
        return false;
    }

	/* 初始化字体库 */
// 	if (TTF_Init() == -1 )
// 		return 0;

    const SDL_VideoInfo *vi = SDL_GetVideoInfo();
    m_s_iFullScreenWidth  = vi->current_w;
    m_s_iFullScreenHeight = vi->current_h;

    // 设置显示画面的长宽
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
        // 传入窗体句柄，让SDL将视频显示到窗体上
        char szVar[MAX_PATH];

        // %lx：unsinged long     （长整形）
        // %x： unsinged int      （整形）          
        // 例如 SDL_WINDOWID=0x0001
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
    int iFlags = SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_DOUBLEBUF|SDL_NOFRAME; // |SDL_NOFRAME;   // SDL_NOFRAME 无边框和标题栏

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
		LogOutError("创建SDL显示层面失败")
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
    // 创建一个显示层，并指定分辨率，width和height指显示的分辨率
    psdlOverlay_Bmp = SDL_CreateYUVOverlay(m_pavCodecCtxVideo->width, m_pavCodecCtxVideo->height,  
        SDL_YV12_OVERLAY, m_psdlSurface_Screen);
}

bool    CAVPlayer::OpenFile(const char* pszFilePath)
{     
	LogObject;
    // 打开文件并读取文件的头部,检测是否音视频文件 avformat_open_input
    if(avformat_open_input(&m_pavFormatCtx, pszFilePath, NULL, NULL)!=0)
    {
		LogOutError("打开视频文件失败");
        return false; // Couldn't open file
    }

    // 检查文件中的流信息
    if(av_find_stream_info(m_pavFormatCtx)<0)
    {
		LogOutError("检查文件中的流信息失败");
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

    // 查找音频流解码器
    m_pavCodecAudio = avcodec_find_decoder(m_pavCodecCtxAudio->codec_id);
    if(! m_pavCodecAudio) 
    {
		LogOutError("查找音频流解码器失败");
        // fprintf(stderr, "Unsupported codec!\iChannels");
        return false; // Codec not found
    }

    // 打开解码器
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

    // 查找视频流解码器
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
	{//  在codec初始化的时候设置avctx->thread_type |=FF_THREAD_FRAME
		m_pavCodecCtxVideo->active_thread_type = FF_THREAD_FRAME;   //线程的类型设置为帧并行
	} 
	else if (m_pavCodecCtxVideo->codec->capabilities & CODEC_CAP_SLICE_THREADS &&
		m_pavCodecCtxVideo->thread_type & FF_THREAD_SLICE)
	{   //同样的要实现片级并行则需要在codec初始化的时候设置条件
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
		
	// 打开解码器
	if(avcodec_open(m_pavCodecCtxVideo, m_pavCodecVideo) < 0)
	{
		return false; // Could not open codec
	}


    return true;
}

// 允许只有视频流或音频流
bool    CAVPlayer::OpenCodec()
{   
    // return OpenVideoCodec() || OpenAudioCodec(); // 当前一个为真时，就不会执行后一个
    bool bRet = OpenVideoCodec();
    bRet = (OpenAudioCodec() || bRet);
    return bRet;
}

void    CAVPlayer::OpenAudioDevice()
{
    SDL_AudioSpec      sdlAudioSpecWanted;//, sdlAudioSpecReally;       // sdlAudioSpecWanted代表希望被设置的参数值，sdlAudioSpecReally代表实际被设置的参数值
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

    // 设置音频编解码信息
    sdlAudioSpecWanted.freq       = m_pavCodecCtxAudio->sample_rate;  // 采样率
    sdlAudioSpecWanted.format     = AUDIO_S16SYS;                     // 数据格式
    sdlAudioSpecWanted.channels   = m_pavCodecCtxAudio->channels;     // 声音的通道数
    sdlAudioSpecWanted.silence    = 0;                                // 表示静音的值
    sdlAudioSpecWanted.samples    = SDL_AUDIO_BUFFER_SIZE;            // 声音缓冲区的尺寸
    sdlAudioSpecWanted.callback   = audio_callback;                   // 回调函数用来混合音频数据，并放入音频流
    sdlAudioSpecWanted.userdata   = pstcAudioCallback;                // SDL供给回调函数运行的参数

    // 打开音频设备
    //if(SDL_OpenAudio(&sdlAudioSpecWanted, &sdlAudioSpecReally) < 0) // 暂时不需要sdlAudioSpecReally
    if(SDL_OpenAudio(&sdlAudioSpecWanted, NULL) < 0) 
    {
        // fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
		delete pstcAudioCallback;
        return ;
    }   

    // 暂停或恢复audio_callback函数的执行（0是恢复，其他的是暂停）  
    // 让audio_callback函数开始反复的被调用
	SDL_PauseAudio(0);
	m_pstcAudioCallback = pstcAudioCallback;

}

bool    CAVPlayer::Play(const char* pszPath)
{
	LogObject;
// 	string strInfo = "视频播放开始 文件：";
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

			//根据文件信息设置视频大小和位置
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

			LogOutInfo("启动包数据入队线程");
            m_psdlThread_Push = SDL_CreateThread(thread_avpacket_push, this);       // 启动包数据入队线程

            if(m_pavsteamAudio) 
            {
				LogOutInfo("打开音频设备，并启动音频线程");
	             OpenAudioDevice();  // 打开音频设备，并启动音频线程
            }

            if(m_pavsteamVideo) 
            {                  
				LogOutInfo("启动视频线程");
	             m_psdlThread_Video = SDL_CreateThread(thread_decode_video, this);   // 启动视频线程

				 if(!m_psdlThread_Video)
				 {
					 LogOutError("启动视频线程失败");
				 }

                SetScale(m_psdlSurface_Screen->w , m_psdlSurface_Screen->h );         // 设置分辨率为w*h
                SDL_TimerID sdlTimerID =  SDL_AddTimer(1, SDLEventRefresh, &m_stcRefreshTmp);                 // 只需触发一次FF_REFRESH_EVENT事件，之后会递归触发
            }   

            SDLWaitEvent();
//			LogOutInfo("视频播放完成");
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
	LogOutInfo("关闭视频");

	// ---------------------------------------
	// 通知所有条件成立,防止他们阻塞在线程里
	// ---------------------------------------
	SDL_CondSignal(m_psdlCondPict); 
	SDL_CondSignal(m_psdlCondVideo); 
	SDL_CondSignal(m_psdlCondAudio); 
	//SDL_Delay(100);     // 等待他们退出
	Sleep(100);

	LogOutInfo("关闭音频设备");
	SDL_CloseAudio();   // 关闭音频设备

	LogOutInfo("等待读包线程结束");
	if(m_psdlThread_Push)
	{
		SDL_WaitThread(m_psdlThread_Push, NULL);
	}

	LogOutInfo("等待视频解码线程结束");
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

	LogOutInfo("关闭解码器");
	if(m_pavCodecCtxVideo)
	{
		avcodec_close(m_pavCodecCtxVideo);      // Close the codec
	}

	LogOutInfo("关闭视频文件");
	if(m_pavFormatCtx)
	{
		av_close_input_file(m_pavFormatCtx);    // Close the video file
	}

	EmptyPktQueue(&m_QuePktAudio);
	EmptyPktQueue(&m_QuePktVideo);

	STC_VideoPicture stcVP;
	LogOutInfo("清理图片队列");
	while(m_QuePicture.Pop(stcVP))
	{
		SDL_FreeYUVOverlay(stcVP.psdlBmp);
	}

	//LogOutInfo("准备关闭音频设备");

	LogOutInfo("清理播放器内存");
	Clear();
}

void    CAVPlayer::Pause()
{
    m_bPause = !m_bPause;
    if (! m_bPause) // 如果取消暂停，则获取现在的时钟
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
        case FF_REFRESH_EVENT:      // 刷新并显示图像  
            if(m_pavsteamVideo)     // 如果包含视频
            {
                pstcRefreshTmp = (STC_VideoRefresh*)(sdlEvent.user.data1);
                if( m_QuePicture.Pop(stcVPTmp) )
                {                        
//                    DisplayPicture(m_nWidth, m_nHeight, stcVPTmp.psdlBmp);  // 显示图片，其分辨率为m_nWidth*m_nHeight
                    DisplayPicture(m_sdlrcPictrue, stcVPTmp.psdlBmp);  // 显示图片，其分辨率为m_nWidth*m_nHeight
					//log
					stringstream sstr;
					sstr<<"当前视频帧的PTS = "<<stcVPTmp.dbPTS;
					//LogOutDebug(sstr.str().c_str());
					sstr.str() = "";

                    SDL_FreeYUVOverlay(stcVPTmp.psdlBmp);
                    pstcRefreshTmp->pstcVideoSync->dbPTS = stcVPTmp.dbPTS;

                    // SDL_AddTimer(30, SDLEventRefresh, NULL);             // 之前没有同步视频，以30ms的速度刷新图片，也可以获得较好的效果(仅对此电影测试过)
                    video_refresh(pstcRefreshTmp);

					

					double dbCurPTS = m_s_ullVideoPktPTS * av_q2d(m_pavsteamVideo->time_base);
					if(abs(dbCurPTS - 121.609) <= 1e-3)
					{
						int a = 0;
					}

					//log
					//stringstream sstr;
					sstr<<"当前视频帧的PTS = "<<stcVPTmp.dbPTS;
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
					LogOutInfo("视频播放完成最后一帧，准备退出视频播放线程");

					if(m_bEof == true)
					{
						LogOutInfo("退出视频播放线程");
						return;
					}
                }
            }
            break;
        case SDL_QUIT:
			LogOutInfo("接收到SDL的退出消息");
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
                SetScreenSize(0,0); // 这两个参数随便填
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

                //SetScreenSize(m_iScreenWidth, m_iScreenHeight);   // 在MFC的对话框中已有处理，所以SDL_VIDEORESIZE消息可以不需要
            }
            break;
            //case SDL_MOUSEBUTTONDOWN: // 鼠标单击
            //    break;
            //case SDL_MOUSEMOTION:     // 鼠标移动
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

    // 可以把一个 AVFrame结构体的指针转换为AVPicture结构体的指针    
    // (AVPicture *)pavFrame 这个就是一帧图像的所有信息 宽度和高度为别为pavCodecCtxVideo->width, pavCodecCtxVideo->height
    // 
    // 把帧从原始格式(pavCodecCtxVideo->pix_fmt) 转换成为RGB格式
    // Convert the image into YUV format that SDL uses
    // dst_pix_fmt = PIX_FMT_YUV420P;
    // 老版函数为img_convert(&avpicture, dst_pix_fmt,(AVPicture *)pFrame, pVideoPlayer->video_st->codec->pix_fmt, pVideoPlayer->video_st->codec->width, pVideoPlayer->video_st->codec->height);
    // 新版函数为avpicture_deinterlace(&avpicture, (AVPicture *)pavFrame, pavCodecCtxVideo->pix_fmt, pavCodecCtxVideo->width, pavCodecCtxVideo->height);
    // 功能更强的函数为sws_scale
    if(m_pswsImgConvertCtx == NULL) // 只进入一次
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

	//LogOutInfo("进入读取数据包循环");

    while(! pVideoPlayer->m_s_bIsQuit)
    {
        //if (is->abort_request)
        //    break;

        if (pVideoPlayer->m_bPause != pVideoPlayer->m_bPausedLast)  // 如果暂停状态改变了，则继续执行；保证只执行一次下面的代码
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
        if(pVideoPlayer->m_stcSeek.bSeekReq)    // 如果有快进快退操作
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
                    pVideoPlayer->m_QuePktAudio.Push(g_avpktFlush);     // 入队一个含清除信息的包
                    SDL_CondSignal(pVideoPlayer->m_psdlCondAudio);      // 通知条件成立，通知音频数据出队
                }
                if(pVideoPlayer->m_iStreamIndexVideo >= 0) 
                {
                    EmptyPktQueue(&pVideoPlayer->m_QuePktVideo);
                    pVideoPlayer->m_QuePktVideo.Push(g_avpktFlush);     // 入队一个含清除信息的包
                    SDL_CondSignal(pVideoPlayer->m_psdlCondVideo);      // 通知条件成立，通知视频数据出队
                }
            }

            pVideoPlayer->m_stcSeek.bSeekReq = false;
        }

        // 当读取了一定大小的数据后,就先暂停读取4
        pVideoPlayer->m_QuePktVideo.Size(sizeVideo);
        pVideoPlayer->m_QuePktAudio.Size(sizeAudio);

        if(sizeVideo > MAX_VIDEOQ_SIZE || sizeAudio > MAX_AUDIOQ_SIZE) 
        {
            // ---------------------------------------------------------------------------------  
            // 这个延迟会严重影响播放效果,设为10、100、1000的效果都不一样
            // ---------------------------------------------------------------------------------  
			//LogOutInfo(" SDL_Delay（100）");
            //SDL_Delay(100);                             // 由于1s只播放几十帧图像,所以可以延迟这么久
            //SDL_Delay(1000 * MAX_VIDEOQ_SIZE / 256);  // 假设1s播放256帧
            //SDL_Delay(20); 
			Sleep(100);
            continue;
        }


		int nRes = av_read_frame(pVideoPlayer->m_pavFormatCtx, &avpacket);
        if(nRes < 0)  // av_read_frame()读取一个包并把它保存到AVPacket结构体中
        {
            // 检测结构体是否读取文件错误
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
		sstr<<"读取一包数据，长度="<<avpacket.size;
		LogOutDebug(sstr.str().c_str());
		sstr.str("");

		stringstream strOut;
		strOut<<pVideoPlayer->m_pavFormatCtx->flags<<" error="<<nRes ;
		//LogOutError(strOut.str().c_str());

        // 读取数据
        if(avpacket.stream_index == pVideoPlayer->m_iStreamIndexAudio) 
        {           
            pVideoPlayer->m_QuePktAudio.Push(avpacket);     // 音频流数据入队
            SDL_CondSignal(pVideoPlayer->m_psdlCondAudio);  // 通知条件成立，通知音频数据出队
        } 
		else if(avpacket.stream_index == pVideoPlayer->m_iStreamIndexVideo) 
		{           
			pVideoPlayer->m_QuePktVideo.Push(avpacket);     // 视频流数据入队
			SDL_CondSignal(pVideoPlayer->m_psdlCondVideo);  // 通知条件成立，通知视频数据出队
		}
        else 
        {            
            av_free_packet(&avpacket);  // Free the avpacket that was allocated by av_read_frame
        }
    }
	LogOutInfo("退出读取数据包循环");

    return 0;
}

int thread_decode_video(void *argv)  
{
    CAVPlayer *pVideoPlayer    = (CAVPlayer*)argv;
    SDL_Overlay  *psdlOverlay_Bmp = NULL; 
    AVFrame  *pavFrame  = avcodec_alloc_frame(); // 申请一个视频帧的内存 
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

        if(nPictSize >= MAX_PICTURE_SIZE || pVideoPlayer->m_bPause) // 如果图片队列满了，或者暂停了，则等待
        {
            //SDL_Delay(100); 
			//LogOutInfo("等待图片出队");
           //SDL_Delay(1);  // 等待图片出队
			Sleep(1);
            //SDL_CondWait(pVideoPlayer->m_psdlCondPict, NULL); // 不能用SDL_CondWait，否则很容易死循环，保持单向等待，即只让出队线程等待入队线程的通知
            continue;
        }           

        if(! (pVideoPlayer->m_QuePktVideo.Pop(avpacket)))       
        {
            // 如果队列无数据，等待数据入队
			//LogOutInfo("等待数据入队");
            //SDL_CondWait(pVideoPlayer->m_psdlCondVideo, NULL);
			Sleep(100);
			//LogOutInfo("有数据入队");
            continue;
        }  

        if(avpacket.data == g_avpktFlush.data) 
        {
            // 如果是清空信息的包，则刷新所有信息
            avcodec_flush_buffers(pVideoPlayer->m_pavsteamVideo->codec);
            continue;
        }
		nAllFrameCount ++;
		TRACE("All Frame Count = %d\n", nAllFrameCount);

		/*{//解码之前查找关键帧
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
        avcodec_decode_video2(pVideoPlayer->m_pavCodecCtxVideo, pavFrame, &iFrameFinished, &avpacket);      // 把包数据解码为帧数据

        dbPTS = pVideoPlayer->GetPTS(&avpacket, pavFrame);  // avcodec_decode_video2函数中得到帧的时间戳,从上次处理的包中得到DTS

		//log
		stringstream sstr;
		sstr<<"avcodec_decode_video2函数中得到帧的时间戳 = "<<dbPTS;
		//LogOutDebug(sstr.str().c_str());
		sstr.str().clear();

        if(iFrameFinished)              
        {
            // 如果成功获取了视频帧信息
            dbPTS = pVideoPlayer->GetVideoSyncPTS(pavFrame, dbPTS);
			
            pVideoPlayer->AllocPicture(psdlOverlay_Bmp);    // 申请图片内存
            pVideoPlayer->ReadFrameToBmp(pavFrame, pVideoPlayer->m_pavCodecCtxVideo, psdlOverlay_Bmp);      // 把帧数据解码为bmp图片

            stcVPTmp.dbPTS   = dbPTS;   // 记录此psdlBmp的dbPTS,即正确的时间戳值
            stcVPTmp.psdlBmp = psdlOverlay_Bmp;

			//DisplayPicture(pVideoPlayer->m_sdlrcPictrue, stcVPTmp.psdlBmp);  // 显示图片，其分辨率为m_nWidth*m_nHeight
			//video_refresh(&pVideoPlayer->m_stcRefreshTmp);

            pVideoPlayer->m_QuePicture.Push(stcVPTmp);      // 图片入队,由于存的是指针,所以每次都需要AllocPicture           
            SDL_CondSignal(pVideoPlayer->m_psdlCondPict);
        }
        av_free_packet(&avpacket);               
    }


    av_free(pavFrame);
    return 0;       
}

//原型:int avcodec_decode_audio2(AVCodecContext *avctx, int16_t *samples,
//                         int *frame_size_ptr,
//                         const uint8_t *buf, int buf_size);
//    avctx ：       解码器上下文
//    samples：      输出参数  输出数据的缓存首地址.
//    frame_size_ptr:既是输入又是输出，无帧可解返回0，解码失败返回负数，解码成功返回解码后一帧音频所占空间，以字节为单位 
//    buf：          输入参数，输入数据的缓存
//    buf_size：     输入参数，buf的大小
//    返回值：       无帧可解返回0，解码失败返回负数，解码成功返回解码前一帧音频所占空间
//
// avcodec_decode_audio2()，它的功能就像它的姐妹函数 avcodec_decode_video()一样，
//    唯一的区别是它的一个包里可能有不止一个声音帧，所以你可能要调用很多次来解码出包中所有的数据。   
//    同 时也要记住进行指针audio_buf的强制转换，因为SDL给出的是8位整型缓冲指针而ffmpeg给出的数据是16位的整型指针。
//    如果一次调用没有处理完一个包的数据，记录下处理到包的那个位置了，下次接着处理（这种情况可能是因为一个音频包，包含多个音频帧的数据引起）
// 你应该也会注意到 len1和data_size的不同，len1表示解码使用的数据的在包中的大小，data_size表示实际返回的原始声音数据的大小。
//    当我们得到一些数据的时候，我们立刻返回来看一下是否仍然需要从队列中得到更加多的数据或者我们已经完成了。
//    如果我们仍然有更加多的数据要处理，我们把它保存到下一次。如果我们完成了一个包的处理，我们最后要释放它。
// audio_callback中的audio_decode_frame函数从队列中读取并处理数据，最后把数据送给SDL，于是SDL就相当于我们的声卡。
// STC_AudioCallback仅仅起参数传递作用，并无独立意义
// pavCodecCtx  : 输入，解码上下文
// uszAudioBuf  : 输出，解码成功后，输出到的缓存的首地址
// iAudioBufSize：输入, 仅仅是标记uszAudioBuf的大小
// 返回值：解码完一帧音频到缓存后，缓存占用的实际大小，以字节为单位，为负数表示失败
int audio_decode_frame(STC_AudioCallback *pstcAudioCallback, uint8_t *uszAudioBuf,const  int iAudioBufSize, double *pdbPTS) 
{
	LogObject;
    AVPacket avpacket;  // 如果一个音频包中有多个帧，用于保存中间状态
    int iAudioPktSize;  // 音频包数据大小，配合audio_pkt 保存中间状态
    int iLen, iDataSize , iChannels;
    double dbPTS;

    do
    {   
        if(*pstcAudioCallback->pbPause)
        {
            return -1;
        }

        // 获取音频数据, 获取不到时等待10ms                  
        if(! pstcAudioCallback->pQuePktAudio->Pop(avpacket)) 
        {            
            Sleep(10);    // 用SDL_CondWait代替了,因为等10ms后不一定有数据,而且可能10ms太长，用SDL_CondWait就能等待一个精确的时间，并且不会重复询问是否有数据
            //SDL_CondWait(pstcAudioCallback->psdlCondAudio, NULL);
            continue;
        }

        if(avpacket.data == g_avpktFlush.data) 
        {
            // 如果是清空信息的包，则刷新所有信息
            avcodec_flush_buffers(pstcAudioCallback->pavsteamAudio->codec);
            continue;
        }

        // 解码音频数据
        iAudioPktSize = avpacket.size;

        if(avpacket.pts != AV_NOPTS_VALUE) 
        {
            // 获取包的时候更新声音时钟
            (*(pstcAudioCallback->pdbClockAudio)) = av_q2d(pstcAudioCallback->pavsteamAudio->time_base) * avpacket.pts;
        }

        while(iAudioPktSize > 0) 
        {
            iDataSize = iAudioBufSize;
			//LogOutInfo("解码音频帧");
            iLen = avcodec_decode_audio3(pstcAudioCallback->pavCodecCtxAudio, (int16_t *)uszAudioBuf, &iDataSize, &avpacket) ;
			Sleep(1);
            if(iLen < 0) 
            {
                // 解码失败,则解码下一帧数据
				//LogOutError("解码音频帧失败");
               iAudioPktSize = 0;
                break;
            }
			//LogOutInfo("解码音频帧成功");
           iAudioPktSize -= iLen;

            if(iDataSize <= 0) 
            {
                /* No data yet, get more frames */
                continue;
            }

            dbPTS = *(pstcAudioCallback->pdbClockAudio);
            *pdbPTS = dbPTS; // 用于同步时钟
			//log
			stringstream sstr;
			sstr<<"avcodec_decode_audio3音频帧的PTS = "<<dbPTS;
			//LogOutDebug(sstr.str().c_str());
			sstr.str().clear();

            iChannels = 2 * pstcAudioCallback->pavsteamAudio->codec->channels;

            // 处理包的时候也要更新声音时钟
            (*(pstcAudioCallback->pdbClockAudio)) += (double)iDataSize / (double)(iChannels * pstcAudioCallback->pavsteamAudio->codec->sample_rate);

            /* We have data, return it and come back for more later */
            return iDataSize;   // iDataSize表示实际返回的原始声音数据的大小
        }

        if(avpacket.data)
        {
            av_free_packet(&avpacket);
        }

    }while(! (*pstcAudioCallback->pbIsQuit));

	LogOutInfo("退出音频线程");

    return -1;  // 如果退出了循环，则说明解码线程将要退出，这时应返回-1通知上层函数
}

// 这个循环把结果写入到中间缓冲区，尝试着向流中写入iLenStream字节,
// 并且在我们没有足够数据的时候会获取更多的数据,或者当我们有多余数据的时候保存下来为后面使用。
// 这个uszAudioBuf的大小为 1.5倍的声音帧的大小以便于有一个比较好的缓冲，这个声音帧的大小是ffmpeg给出的。
//
// pUserdata是输入，puszStream是输出，iLenStream(一般为4096)是输入
// audio_callback函数的功能是调用audio_decode_frame函数，把解码后数据块uszAudioBuf追加在puszStream的后面，
// 通过SDL库对audio_callback的不断调用，不断解码数据，然后放到puszStream的末尾，SDL库认为puszStream中数据够播放一帧音频了，就播放它。
//
// 第三个参数len是向stream中写数据的内存分配尺度，是分配给audio_callback函数写入缓存大小。
// pUserdata 其结构体STC_AudioCallback仅仅起参数传递作用，并无独立意义
// puszStream是我们要把声音数据写入的缓冲区指针
// iLenStream是缓冲区的大小
void audio_callback(void *pUserdata, Uint8 *puszStream, int iLenStream) 
{
	LogObject;
    STC_AudioCallback *pstcAudioCallback = (STC_AudioCallback *)pUserdata; 
    CAVPlayer      *pVideoPlayer      = pstcAudioCallback->vp;
    AVCodecContext    *aCodecCtx         = pstcAudioCallback->pavCodecCtxAudio;

    //  uint8_t uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];    // 输出音频的缓存   
    //  uszAudioBuf换成16字节对齐，否则在avcodec_decode_audio3处会发生内存访问异常！
	static DECLARE_ALIGNED(16, uint8_t, uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2]);  
	//static DECLARE_ALIGNED(16, uint8_t, uszAudioBuf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 2)]);  

    // 要换成静态变量，否则声音输出有问题
    static unsigned int iAudioBufSize  = 0;    // 解码后音频数据大小
    static unsigned int iAudioBufIndex = 0;    // 已输出音频数据大小
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
                iAudioSize = pVideoPlayer->GetAudioSyncPTS((int16_t *)uszAudioBuf, iAudioSize, dbPTS);  // 同步音频时钟
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
    sdlEvent.user.data1 = opaque;   // 传递的数据

    SDL_PushEvent(&sdlEvent);       // 发送FF_REFRESH_EVENT事件
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

// 我们在这里做了很多检查：首先，我们保证现在的时间戳和上一个时间戳之间的处以dbDelay是有意义的。
// 如果不是的话，我们就猜测着用上次的延迟。接着，我 们有一个同步阈值，因为在同步的时候事情并不总是那么完美的。
// 在ffplay中使用0.01作为它的值。我们也保证阈值不会比时间戳之间的间隔短。最后， 我们把最小的刷新值设置为10毫秒。
// 我们简单的添加新的帧定时器延时，把它和电脑的系统时间进行比较，然后使用那个值来调度下一次刷新
// 使视频同步到音频   
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
    if(pstcRefreshTmp->pstcAudioSync->iAVSyncType != AV_SYNC_VIDEO_MASTER)  // 在视频为主时钟的时候不同步视频
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

// 获取音频目前播放的时间
// audio_callback函数花费了时间来把数据从声音包中移到我们的输出缓冲区中。
// 这意味着我们声音时钟中记录的时间比实际的要早太多。
// 所以我们必须要检查一下我们还有多少没有写入
double get_audio_clock(STC_VideoRefresh *pstcRefreshTmp)
{
    double dbPTS        = pstcRefreshTmp->pstcVideoSync->dbClockAudio;                       // 设置为上一次播放的音频位置，然后根据这个位置算出这一次播放的音频位置
    int iBufSizeRemain  = pstcRefreshTmp->pstcVideoSync->uiBufRemainAudio;                   // 剩余的音频缓存大小   
    int iChannels       = pstcRefreshTmp->pstcVideoSync->pavsteamAudio->codec->channels * 2; // 声音的通道数
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

    if (! pstcRefreshTmp->pbPause) // 如果不是暂停，才更新；否则dbDelta = 0;
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
    if(! m_stcSeek.bSeekReq) // 同一时刻只允许一次快进快退操作
    {
        m_stcSeek.llSeekPos  = llPos;
        m_stcSeek.iSeekFlags = rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;  // 快进还是快退
        m_stcSeek.bSeekReq   = true;
    }
}