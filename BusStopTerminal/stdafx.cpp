
// stdafx.cpp : 只包括标准包含文件的源文件
// BusStopTerminal.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


HANDLE g_hWord;		//站点切换事件
HANDLE g_hPage;			//线路切换信号
int g_nPlaylist = 0;				//播放列表状态
HANDLE g_hExit = INVALID_HANDLE_VALUE;			//退出信号

