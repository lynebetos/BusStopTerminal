// ----------------------------------------------------------------------------------------------------------------------
// Build on Feb/28/2012 by Alberl
// Email:   ItsBird@qq.com  
// Website: www.ItsBird.com
// 
// ע��:���ŵ�ӰʱҪʹ���Ŵ��ڼ��������ʱ��û����;û������ʱ�򣬵���´��ھ�OK��
//
// ���еİ����ֽ���DTS������ʱ�������PTS����ʾʱ������Ļ��ơ�
// ����ѡ��ͬ����Ƶ����Ƶ��ͬ����Ƶ����Ƶ�����߶�ͬ�����ⲿʱ�ӣ�������ĵ���ʱ�ӣ�  
// ----------------------------------------------------------------------------------------------------------------------

#pragma once
#include <string>
#include "CommHeadFFmpeg.h"
#include "MutexQueue.h"
#include "MutexQueue.cpp"         // �������cpp����ģ����֡��޷��������ⲿ���š�������

class  CAVPlayer;

// ͼƬ��ʾ��ز���
typedef struct 
{
  SDL_Overlay *psdlBmp;           // ͼƬ��ָ��
  double       dbPTS;             // ͼƬ֡��ʱ��¾ֵ
}STC_VideoPicture;

// ��Ƶʱ����ز���������ͬ����Ƶ����Ƶ 
typedef struct 
{
    AVStream   *pavsteamAudio;    // ��Ƶ��ָ��
    unsigned    uiBufRemainAudio; // ʣ�����Ƶ�����С
    double      dbClockAudio;     // ��ǰ���ڲ��ŵ���Ƶʱ��
    double      dbClockVideo;     // ��ǰ���ڲ��ŵ���Ƶʱ�� 
    double      dbFrameTimer;     // ͳ�Ƴ���Ӱ���������е���ʱ��ָʾ��һ֡����ʲôʱ����ʾ
    double      dbFrameLastDelay; // ��һ֡����Ƶ֡�ӳ٣��ɼ���Ϊ����ʾ���ʱ��
    double      dbFrameLastPTS;   // ��һ֡����ʾʱ���
    double      dbPTS;            // ͼƬ֡��ʱ��¾ֵ
}STC_VideoSync;  

// ��Ƶʱ����ز���������ͬ����Ƶ����Ƶ
typedef struct 
{
    double      dbVideoCurrentPTS;          //<current displayed pts (different from video_clock if frame fifos are used)
    int64_t     llVideoCurrentPTSTime;      //<time (av_gettime) at which we updated m_dbVideoCurPTS - used to have running video pts 
    int         iAVSyncType;
}STC_AudioSync; 

// �˽ṹ�崫���ⲿʱ����ز��������ڽ���Ƶ����Ƶ��ͬ�����ⲿʱ��
typedef struct 
{  
    double       dbExternalClock;           // external clock base 
    int64_t      dbExternalClockTime;
}STC_ExternalSync;

// ������Ƶˢ���¼��Ĳ�������
typedef struct 
{
    STC_VideoSync  *pstcVideoSync;          // ���ݲ���,����ͬ����Ƶ����Ƶ 
    STC_AudioSync  *pstcAudioSync;          // ���ݲ���,����ͬ����Ƶ����Ƶ 
    bool           *pbPause;                // �Ƿ���ͣ
}STC_VideoRefresh;

// ������Ƶ�ص������Ĳ�������
typedef struct
{
    bool            *pbPause;               // �Ƿ���ͣ
    bool            *pbIsQuit;              // �Ƿ��˳�
    double          *pdbClockAudio;         // ��Ƶʱ��
    unsigned        *puiBufRemainAudio;     // ʣ�����Ƶ�����С
    AVCodecContext  *pavCodecCtxAudio;      // ��������Ƶ������ʹ�õĹ��ڱ��������������Ϣ 
    AVStream        *pavsteamAudio;         // ��Ƶ��ָ��
    SDL_cond        *psdlCondAudio;         // ��������,��Ƶ��������Ӻ��֪ͨpsdlCondAudio
    CMutexQueue<AVPacket> *pQuePktAudio;    // ��Ƶ���ݶ���
    CAVPlayer    *vp;
}STC_AudioCallback;

// ����AV���ƽ��ֵ�ļ���
typedef struct 
{
    double      dbAudioDiffCum; 
    double      dbAudioDiffAvgCoef;
    double      dbAudioDiffThreshold;
    int         iAudioDiffAvgCount;
}STC_Diff;

// ���ڲ����ļ���ָ��λ��
typedef struct 
{
    bool        bSeekReq;      // �Ƿ���Seek����
    int         iSeekFlags;    // ǰ�����Ǻ���
    int64_t     llSeekPos;     // ָ����λ��
}STC_Seek; 


class  CAVPlayer
{
public:
    CAVPlayer(void);
    ~CAVPlayer(void);

    bool    Init(size_t nScreenWidth, size_t nScreenHeight); // ��ʼ�����泤����ʾ����(��ʾ����Ĭ��ΪDos����)
    bool    Play(const char* pszPath);      // ����·��ΪpszPath���ļ�
    bool    PlayNext();                     // ������һ���ļ�
    bool    PlayPre();                      // ������һ���ļ�
    void    Stop ();                        // ��ֹ,����λ���ļ���ͷλ��
    void    Pause();                        // ��ͣ

    bool    VolumeSet(size_t nVol);         // ��������ΪnVol
    bool    VolumeIncrease();               // ��������
    bool    VolumeReduce();                 // ������С 
    bool    SeekTo(size_t nPos);            // ����ָ��λ��nPos
    bool    Forward();                      // ���
    bool    Backward();                     // ����
    void    Close();                        // �ر��ļ�������

    bool    SetHwnd(unsigned long ulHwndDisplay);                       // ������ʾ����Ĵ�����,Ĭ����ʾ��Dos���� // ���봰��������SDL����Ƶ��ʾ����������
    bool    SetScreenSize(size_t nScreenWidth, size_t nScreenHeight);   // ������ʾ����ĳ���
    void    SetScale(size_t nWidth, size_t nHeight);                    // ������ʾ����ķֱ���
    void    StreamSeek(int64_t llPos, int rel);                         // ��ָ��λ�ò����ļ�

private:
    bool    m_bOpen;                        // �ļ��Ƿ񱻴�
    bool    m_bPause;                       // �Ƿ���ͣ
    bool    m_bPausedLast;                  // ��һ�ε���ͣ״̬

    size_t  m_nVolPos;                      // ��ǰ����
    size_t  m_nFilePos;                     // ��ǰ����λ��
    std::string m_strPath;                  // ��ǰ�ļ���·��
    static  bool     m_s_bIsQuit;           // �˳���ʶ     // ��ΪҪ�ھ�̬�������ã�������Ϊ��̬��Ա
    static  uint64_t m_s_ullVideoPktPTS;    // ÿ���ڵõ�һ������ʱ���PTS���浽���ȫ�ֱ�����    // ��ΪҪ��GetBuffer��ʹ�ã�������Ϊ��̬��Ա
    static  int      m_s_iFullScreenWidth;  // ȫ���Ŀ��,ֻ��ʼ��һ��
    static  int      m_s_iFullScreenHeight; // ȫ���ĸ߶�,ֻ��ʼ��һ��
    SwsContext      *m_pswsImgConvertCtx;   // ����ͼ������
//    size_t           m_nWidth;              // �ֱ��ʵĿ�
//    size_t           m_nHeight;             // �ֱ��ʵĸ�
    // �ֱ�����ʱ����Ϊ�ʹ��ڴ�Сһ��
    int     m_iScreenWidth;                 // ��¼��ȫ��ʱ����ʾ���ڵĿ��
    int     m_iScreenHeight;                // ��¼��ȫ��ʱ����ʾ���ڵĸ߶�
    bool    m_bFullScreen;                  // �Ƿ���ȫ��״̬

    STC_VideoRefresh m_stcRefreshTmp;       // ������Ƶˢ���¼��Ĳ������� 
    STC_VideoSync    m_stcVideoSync;        // ��Ƶʱ����ز���������ͬ����Ƶ����Ƶ 
    STC_AudioSync    m_stcAudioSync;        // ��Ƶʱ����ز���������ͬ����Ƶ����Ƶ 
    STC_Diff         m_stcDiff;             // ����AV���ƽ��ֵ�ļ���
    STC_Seek         m_stcSeek;             // ���ڲ����ļ���ָ��λ��

    AVFormatContext *m_pavFormatCtx;        // �����ļ���ʽ������ָ�룬��iformat ����ʹ��
    AVStream        *m_pavsteamVideo;       // ��Ƶ��ָ��
    AVStream        *m_pavsteamAudio;       // ��Ƶ��ָ��
    AVCodecContext  *m_pavCodecCtxVideo;    // ��������Ƶ������ʹ�õĹ��ڱ��������������Ϣ
    AVCodecContext  *m_pavCodecCtxAudio;    // ��������Ƶ������ʹ�õĹ��ڱ��������������Ϣ
    AVCodec         *m_pavCodecVideo;       // ��Ƶ�������
    AVCodec         *m_pavCodecAudio;       // ��Ƶ�������
    int              m_iStreamIndexVideo;   // ��Ƶ����������ʾAVFormatContext ��AVStream *streams[]�������� 
    int              m_iStreamIndexAudio;   // ��Ƶ����������ʾAVFormatContext ��AVStream *streams[]�������� 

    SDL_Surface     *m_psdlSurface_Screen;  // ��ʾ��
    SDL_Thread      *m_psdlThread_Push;     // ������������̵߳�ָ��
    SDL_Thread      *m_psdlThread_Video;    // video �����̵߳�ָ��

    // ----------------------------------------------------------------------------------------------------------------------
    // �������������ڽ����֪ͨ���ݳ��ӣ���Ҫʹ���ڶ�������Ϊ��ʱ�������̵߳ȴ��������    SDL_CondSignal      SDL_CondWait
    // ----------------------------------------------------------------------------------------------------------------------
    SDL_cond    *m_psdlCondAudio;           // ��Ƶ��������ӣ���ʱ֪ͨ�����̣߳�ʹ���ݳ���     
    SDL_cond    *m_psdlCondVideo;           // ��Ƶ���������
    SDL_cond    *m_psdlCondPict;            // ͼƬ���������                                  
    CMutexQueue<AVPacket>           m_QuePktAudio;      // ����ǰ����Ƶ����֡����(�������)
    CMutexQueue<AVPacket>           m_QuePktVideo;      // ����ǰ����Ƶ����֡����(�������)
    CMutexQueue<STC_VideoPicture>   m_QuePicture;       // ��������Ƶͼ��֡����

    // ----------------------------------------------------------------------------------------------------------------------
    // ˽�к����б�
    // ---------------------------------------------------------------------------------------------------------------------- 
    bool    OpenFile(const char* pszFilePath);          // ���ļ�
    bool    OpenCodec();                                // �򿪱������
    bool    OpenAudioCodec();                           // ����Ƶ�������
    bool    OpenVideoCodec();                           // ����Ƶ�������
    void    OpenAudioDevice();                          // ����Ƶ�豸
    void    AllocPicture(SDL_Overlay  * &psdlOverlay_Bmp);              // ����ͼƬ�ڴ�
    void    ReadFrameToBmp(AVFrame *pavFrame, AVCodecContext *pavCodecCtxVideo, SDL_Overlay *psdlBmp);  // ��֡��Ϣ����ͼƬ��

    void    Clear();                                                    // ������г�Ա������ֵ��ʹ��ָ�����ʼ״̬
    void    SDLWaitEvent();                                             // �ȴ��¼�������,����֮��ִ����Ӧ����
    double  GetPTS(AVPacket*  pavpacket, AVFrame  *pavFrame);           // ���PTS
    double  GetVideoSyncPTS(AVFrame *src_frame, double dbPTS);          // ����ͬ����PTS, ԭ������synchronize_video
    int     GetAudioSyncPTS(short *pshSamples, int iSamplesSize, double dbPTS);     // ����ͬ����PTS, ԭ������synchronize_audio

    static  int  decode_interrupt_cb(void);                                         // FFMPEG�ṩ�˺���url_set_interrupt_cb�����лص�������Ƿ���Ҫ�˳�һЩ�������ĺ�����
    static  int  GetBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame);    // �Զ�������֡�ڴ�ķ���, �˺�����װ��Ĭ�ϵ����뷽�������������PTS
    static  void ReleaseBuffer(AVCodecContext *pavCodecCtxAudio, AVFrame *pavFrame);// �Զ����ͷ�֡�ڴ�ķ���

    // ----------------------------------------------------------------------------------------------------------------------
    // ��������Ԫ��������������������ԭ��
    // ----------------------------------------------------------------------------------------------------------------------
    friend int    thread_decode_video(void *argv);                      // AVPacket���ݳ����߳� �����������Ƶ����, ���Խ���Ƶ���е����ݳ���
    friend int    thread_avpacket_push(void *argv);                     // AVPacket��������߳�
    friend void   video_refresh(void *pUserData) ;                      // ˢ����Ƶ
    friend double get_audio_clock(STC_VideoRefresh *pstcRefreshTmp);    // �õ���Ƶʱ��
    friend double get_video_clock(STC_VideoRefresh *pstcRefreshTmp);    // �õ���Ƶʱ��
    friend double get_master_clock(STC_VideoRefresh *pstcRefreshTmp);   // ���m_iAVSyncType����Ȼ��������� get_audio_clock����get_video_clock
    friend void   audio_callback(void *pUserData, Uint8 *stream, int iLenStream); // thread_audio ��Ƶ���뺯��,Ϊ�ص�����
};

void    DisplayPicture(int iWidth, int iHeight, SDL_Overlay *psdlBmp);  // iWidth��iHeightΪ�ֱ���
Uint32  SDLEventRefresh(Uint32 interval, void *opaque);                 // ����FF_REFRESH_EVENT�¼�����ϵͳ�յ�FF_REFRESH_EVENT�¼����ˢ����Ƶ
void    EmptyPktQueue(CMutexQueue<AVPacket>* pQuePkt);                  // ���AVPacket������
double  get_external_clock();   // ��ȡ�ⲿʱ��,������ʱ��
