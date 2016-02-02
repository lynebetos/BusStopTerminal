#include "StdAfx.h"
#include "MutexQueue.h"

template<typename T>
CMutexQueue<T>::CMutexQueue()
{
    m_sdlMutex = SDL_CreateMutex();
}

template<typename T>
CMutexQueue<T>::~CMutexQueue()
{
    SDL_DestroyMutex(m_sdlMutex);
}

template<typename T>
void CMutexQueue<T>::Push(T tplData) 
{
    SDL_LockMutex(m_sdlMutex);
    push(tplData);
    SDL_UnlockMutex(m_sdlMutex);
}

template<typename T>
bool CMutexQueue<T>::Pop(T& tplData)
{
    bool bRet = false;
    SDL_LockMutex(m_sdlMutex);

    if(! empty())
    {
        tplData = front();
        pop();
        bRet = true;
    }

    SDL_UnlockMutex(m_sdlMutex);
    return bRet;
}

template<typename T>
bool CMutexQueue<T>::Empty()
{
    bool bRet = false;
    SDL_LockMutex(m_sdlMutex);

    if(empty())
    {
        bRet = true;
    }

    SDL_UnlockMutex(m_sdlMutex);
    return bRet;
}

template<typename T>
bool CMutexQueue<T>::Front(T& tplData)
{
    bool bRet = false;
    SDL_LockMutex(m_sdlMutex);

    if(! empty())
    {
        tplData = front();
        bRet = true;
    }

    SDL_UnlockMutex(m_sdlMutex);
    return bRet;
}

template<typename T>
bool CMutexQueue<T>::Back(T& tplData)
{
    bool bRet = false;
    SDL_LockMutex(m_sdlMutex);

    if(! empty())
    {
        tplData = back();
        bRet = true;
    }

    SDL_UnlockMutex(m_sdlMutex);
    return bRet;
}

template<typename T>
void CMutexQueue<T>::Size(size_type& sizeData)
{
    SDL_LockMutex(m_sdlMutex);
    sizeData = size();
    SDL_UnlockMutex(m_sdlMutex);
}

