// ----------------------------------------------------------------------------------------------------------------------
// Build on Feb/28/2012 by Alberl
// Email:   ItsBird@qq.com  
// Website: www.ItsBird.com
// 
// 注意:播放电影时要使播放窗口激活，否则有时会没声音;没声音的时候，点击下窗口就OK了
//
// 流中的包有种叫做DTS（解码时间戳）和PTS（显示时间戳）的机制。
// 三种选择：同步视频到音频，同步音频到视频，或者都同步到外部时钟（例如你的电脑时钟）  
// ----------------------------------------------------------------------------------------------------------------------

#pragma once
#include <string>
#include "CommHeadFFmpeg.h"
#include "MutexQueue.h"
#include "MutexQueue.cpp"         // 解决其他cpp引用模板出现【无法解析的外部符号】的问题

class  CAVPlayer;

// 图片显示相关参数
typedef struct 
{
  SDL_Overlay *psdlBmp;           // 图片的指针
  double       dbPTS;             // 图片帧的时间戮值
}STC_VideoPicture;

// 音频时钟相关参数，用于同步视频到音频 
typedef struct 
{
    AVStream   *pavsteamAudio;    // 音频流指针
    unsigned    uiBufRemainAudio; // 剩余的音频缓存大小
    double      dbClockAudio;     // 当前正在播放的音频时间
    double      dbClockVideo;     // 当前正在播放的视频时间 
    double      dbFrameTimer;     // 统计出电影播放中所有的延时，指示下一帧该在什么时候显示
    double      dbFrameLastDelay; // 上一帧的视频帧延迟，可简单认为是显示间隔时间
    double      dbFrameLastPTS;   // 上一帧的显示时间戳
    double      dbPTS;            // 图片帧的时间戮值
}STC_VideoSync;  

// 视频时钟相关参数，用于同步音频到视频
typedef struct 
{
    double      dbVideoCurrentPTS;          //<current displayed pts (different from video_clock if frame fifos are used)
    int64_t     llVideoCurrentPTSTime;      //<time (av_gettime) at which we updated m_dbVideoCurPTS - used to have running video pts 
    int         iAVSyncType;
}STC_AudioSync; 

// 此结构体传递外部时钟相关参数，用于将音频、视频都同步到外部时钟
typedef struct 
{  
    double       dbExternalClock;           // external clock base 
    int64_t      dbExternalClockTime;
}STC_ExternalSync;

// 用于视频刷新事件的参数传递
typedef struct 
{
    STC_VideoSync  *pstcVideoSync;          // 传递参数,用于同步视频到音频 
    STC_AudioSync  *pstcAudioSync;          // 传递参数,用于同步音频到视频 
    bool           *pbPause;                // 是否暂停
}STC_VideoRefresh;

// 用于音频回调函数的参数传递
typedef struct
{
    bool            *pbPause;               // 是否暂停
    bool            *pbIsQuit;              // 是否退出
    double          *pdbClockAudio;         // 音频时间
    unsigned        *puiBufRemainAudio;     // 剩余的音频缓存大小
    AVCodecContext  *pavCodecCtxAudio;      // 包含了音频流中所使用的关于编解码器的所有信息 
    AVStream        *pavsteamAudio;         // 音频流指针
    SDL_cond        *psdlCondAudio;         // 条件变量,音频数据已入队后会通知psdlCondAudio
    CMutexQueue<AVPacket> *pQuePktAudio;    // 音频数据队列
    CAVPlayer    *vp;
}STC_AudioCallback;

// 用于AV差分平均值的计算
typedef struct 
{
    double      dbAudioDiffCum; 
    double      dbAudioDiffAvgCoef;
    double      dbAudioDiffThreshold;
    int         iAudioDiffAvgCount;
}STC_Diff;

// 用于播放文件的指定位置
typedef struct 
{
    bool        bSeekReq;      // 是否有Seek请求
    int         iSeekFlags;    // 前进还是后退
    int64_t     llSeekPos;     // 指定的位置
}STC_Seek; 


class  CAVPlayer
{
public:
    CAVPlayer(void);
    ~CAVPlayer(void);

    bool    Init(size_t nScreenWidth, size_t nScreenHeight); // 初始化画面长宽及显示窗口(显示窗口默认为Dos窗口)
    bool    Play(const char* pszPath);      // 播放路径为pszPath的文件
    bool    PlayNext();                     // 播放下一个文件
    bool    PlayPre();                      // 播放上一个文件
    void    Stop ();                        // 终止,并复位到文件开头位置
    void    Pause();                        // 暂停

    bool    VolumeSet(size_t nVol);         // 音量设置为nVol
    bool    VolumeIncrease();               // 音量增大
    bool    VolumeReduce();                 // 音量减小 
    bool    SeekTo(size_t nPos);            // 跳到指定位置nPos
    bool    Forward();                      // 快进
    bool    Backward();                     // 快退
    void    Close();                        // 关闭文件及清理

    bool    SetHwnd(unsigned long ulHwndDisplay);                       // 设置显示画面的窗体句柄,默认显示到Dos窗口 // 传入窗体句柄，让SDL将视频显示到窗体上面
    bool    SetScreenSize(size_t nScreenWidth, size_t nScreenHeight);   // 设置显示画面的长宽
    void    SetScale(size_t nWidth, size_t nHeight);                    // 设置显示画面的分辨率
    void    StreamSeek(int64_t llPos, int rel);                         // 从指定位置播放文件

private:
    bool    m_bOpen;                        // 文件是否被打开
    bool    m_bPause;                       // 是否暂停
    bool    m_bPausedLast;                  // 上一次的暂停状态

    size_t  m_nVolPos;                      // 当前音量
    size_t  m_nFilePos;                     // 当前播放位置
    std::string m_strPath;                  // 当前文件的路径
    static  bool     m_s_bIsQuit;           // 退出标识     // 因为要在静态函数中用，所以设为静态成员
    static  uint64_t m_s_ullVideoPktPTS;    // 每次在得到一个包的时候把PTS保存到这个全局变量中    // 因为要在GetBuffer中使用，所以设为静态成员
    static  int      m_s_iFullScreenWidth;  // 全屏的宽度,只初始化一次
    static  int      m_s_iFullScreenHeight; // 全屏的高度,只初始化一次
    SwsContext      *m_pswsImgConvertCtx;   // 用于图像缩放
//    size_t           m_nWidth;              // 分辨率的宽
//    size_t           m_nHeight;             // 分辨率的高
    // 分辨率暂时设置为和窗口大小一样
    int     m_iScreenWidth;                 // 记录非全屏时，显示窗口的宽度
    int     m_iScreenHeight;                // 记录非全屏时，显示窗口的高度
    bool    m_bFullScreen;                  // 是否处在全屏状态

    STC_VideoRefresh m_stcRefreshTmp;       // 用于视频刷新事件的参数传递 
    STC_VideoSync    m_stcVideoSync;        // 音频时钟相关参数，用于同步视频到音频 
    STC_AudioSync    m_stcAudioSync;        // 视频时钟相关参数，用于同步音频到视频 
    STC_Diff         m_stcDiff;             // 用于AV差分平均值的计算
    STC_Seek         m_stcSeek;             // 用于播放文件的指定位置

    AVFormatContext *m_pavFormatCtx;        // 输入文件格式上下文指针，和iformat 配套使用
    AVStream        *m_pavsteamVideo;       // 视频流指针
    AVStream        *m_pavsteamAudio;       // 音频流指针
    AVCodecContext  *m_pavCodecCtxVideo;    // 包含了视频流中所使用的关于编解码器的所有信息
    AVCodecContext  *m_pavCodecCtxAudio;    // 包含了音频流中所使用的关于编解码器的所有信息
    AVCodec         *m_pavCodecVideo;       // 视频编解码器
    AVCodec         *m_pavCodecAudio;       // 音频编解码器
    int              m_iStreamIndexVideo;   // 视频流索引，表示AVFormatContext 中AVStream *streams[]数组索引 
    int              m_iStreamIndexAudio;   // 音频流索引，表示AVFormatContext 中AVStream *streams[]数组索引 

    SDL_Surface     *m_psdlSurface_Screen;  // 显示屏
    SDL_Thread      *m_psdlThread_Push;     // 解码数据入队线程的指针
    SDL_Thread      *m_psdlThread_Video;    // video 解码线程的指针

    // ----------------------------------------------------------------------------------------------------------------------
    // 条件变量，用于解码后通知数据出队，主要使用在队列数据为空时，出队线程等待数据入队    SDL_CondSignal      SDL_CondWait
    // ----------------------------------------------------------------------------------------------------------------------
    SDL_cond    *m_psdlCondAudio;           // 音频数据已入队，此时通知出队线程，使数据出队     
    SDL_cond    *m_psdlCondVideo;           // 视频数据已入队
    SDL_cond    *m_psdlCondPict;            // 图片数据已入队                                  
    CMutexQueue<AVPacket>           m_QuePktAudio;      // 解码前的音频数据帧队列(或包队列)
    CMutexQueue<AVPacket>           m_QuePktVideo;      // 解码前的视频数据帧队列(或包队列)
    CMutexQueue<STC_VideoPicture>   m_QuePicture;       // 解码后的视频图像帧队列

    // ----------------------------------------------------------------------------------------------------------------------
    // 私有函数列表
    // ---------------------------------------------------------------------------------------------------------------------- 
    bool    OpenFile(const char* pszFilePath);          // 打开文件
    bool    OpenCodec();                                // 打开编解码器
    bool    OpenAudioCodec();                           // 打开音频编解码器
    bool    OpenVideoCodec();                           // 打开视频编解码器
    void    OpenAudioDevice();                          // 打开音频设备
    void    AllocPicture(SDL_Overlay  * &psdlOverlay_Bmp);              // 申请图片内存
    void    ReadFrameToBmp(AVFrame *pavFrame, AVCodecContext *pavCodecCtxVideo, SDL_Overlay *psdlBmp);  // 把帧信息读到图片中

    void    Clear();                                                    // 清除所有成员变量的值，使其恢复到初始状态
    void    SDLWaitEvent();                                             // 等待事件被触发,触发之后执行相应操作
    double  GetPTS(AVPacket*  pavpacket, AVFrame  *pavFrame);           // 获得PTS
    double  GetVideoSyncPTS(AVFrame *src_frame, double dbPTS);          // 更新同步的PTS, 原函数名synchronize_video
    int     GetAudioSyncPTS(short *pshSamples, int iSamplesSize, double dbPTS);     // 更新同步的PTS, 原函数名synchronize_audio

    static  int  decode_interrupt_cb(void);                                         // FFMPEG提供了函数url_set_interrupt_cb来进行回调并检查是否需要退出一些被阻塞的函数。
    static  int  GetBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame);    // 自定义申请帧内存的方法, 此函数封装了默认的申请方法，另外加入了PTS
    static  void ReleaseBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame);// 自定义释放帧内存的方法

    // ----------------------------------------------------------------------------------------------------------------------
    // 声明了友元函数后，无需再声明函数原型
    // ----------------------------------------------------------------------------------------------------------------------
    friend int    thread_decode_video(void *argv);                      // AVPacket数据出队线程 这里仅解码视频数据, 所以仅视频队列的数据出队
    friend int    thread_avpacket_push(void *argv);                     // AVPacket数据入队线程
    friend void   video_refresh(void *pUserData) ;                      // 刷新视频
    friend double get_audio_clock(STC_VideoRefresh *pstcRefreshTmp);    // 得到音频时钟
    friend double get_video_clock(STC_VideoRefresh *pstcRefreshTmp);    // 得到视频时钟
    friend double get_master_clock(STC_VideoRefresh *pstcRefreshTmp);   // 检测m_iAVSyncType变量然后决定调用 get_audio_clock还是get_video_clock
    friend void   audio_callback(void *pUserData, Uint8 *stream, int iLenStream); // thread_audio 音频解码函数,为回调函数
};

void    DisplayPicture(int iWidth, int iHeight, SDL_Overlay *psdlBmp);  // iWidth、iHeight为分辨率
Uint32  SDLEventRefresh(Uint32 interval, void *opaque);                 // 发送FF_REFRESH_EVENT事件，当系统收到FF_REFRESH_EVENT事件后会刷新视频
void    EmptyPktQueue(CMutexQueue<AVPacket>* pQuePkt);                  // 清空AVPacket包队列
double  get_external_clock();   // 获取外部时钟,即电脑时钟
