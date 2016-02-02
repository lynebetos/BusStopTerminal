// 项目 -- 属性 -- 配置属性 -- C/C++ -- 常规 -- 附加包含目录，添加_src;_include/SDL;_include/ffmpeg;
#pragma once

#ifndef snprintf
#define snprintf _snprintf
#endif

#ifndef inline
#define inline _inline
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#ifdef CONFIG_WIN32
#undef main // We don't want SDL to override our main()
#endif

#ifdef WIN32
#pragma comment(lib, "../lib/SDL/x86/SDL.lib")
#pragma comment(lib, "../lib/SDL/x86/SDL_ttf.lib")
#endif

#ifdef WIN64
#pragma comment(lib, "../lib/SDL/x86/SDL.lib")
#endif

#ifdef __cplusplus
}
#endif


#pragma comment(lib,"../lib/ffmpeg/avcodec.lib")
#pragma comment(lib,"../lib/ffmpeg/avdevice.lib")
#pragma comment(lib,"../lib/ffmpeg/avfilter.lib")
#pragma comment(lib,"../lib/ffmpeg/avformat.lib")
#pragma comment(lib,"../lib/ffmpeg/avutil.lib")
#pragma comment(lib,"../lib/ffmpeg/postproc.lib")
#pragma comment(lib,"../lib/ffmpeg/swresample.lib")
#pragma comment(lib,"../lib/ffmpeg/swscale.lib")
