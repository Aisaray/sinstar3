﻿/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SCriticalSection.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/09/27
* 
* Describe    CriticalSection 包装
*/

#ifndef __SCRITICALSECTION__H__
#define __SCRITICALSECTION__H__
#include <Windows.h>

namespace TASKLOOP
{
    class  SCriticalSection
    {
    public:
		SCriticalSection();
		virtual ~SCriticalSection();

		void Enter();
		void Leave();
    protected:
        CRITICAL_SECTION    m_cs;
    };

    class SAutoLock
    {
    public:
        SAutoLock(SCriticalSection & cs):m_cs(cs)
        {
            m_cs.Enter();
        }

        ~SAutoLock()
        {
            m_cs.Leave();
        }
    protected:
        SCriticalSection & m_cs;
    };
}

#endif // __SCRITICALSECTION__H__