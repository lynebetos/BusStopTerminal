#pragma once
#include <queue>
#include "CommHeadFFmpeg.h"

template<typename T>
class CMutexQueue: public std::queue<T>
{
public:
    CMutexQueue(void);
    ~CMutexQueue(void);

    void Push(T tplData); 
    bool Pop(T& tplData);
    bool Front(T& tplData);
    bool Back(T& tplData);
    void Size(size_type& sizeData);
    bool Empty();

private:       
    SDL_mutex *m_sdlMutex;  // ª•≥‚ÃÂ
};
