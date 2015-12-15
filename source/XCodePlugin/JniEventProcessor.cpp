/*
Copyright 2015 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
//
//  JniEventProcessor.cpp
//  XCodePlugin
//
//  Created by benwu on 9/1/15.
//
//

#include "JniEventProcessor.h"
#if _WIN64
#include "pthread.h"
#else
#include <pthread.h>
#include <dispatch/dispatch.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "JniDataSnapshot.h"
#include "jnistub_ChildEventListenerStub.h"
#include "jnistub_ValueEventListenerStub.h"
#include "plugin.pch"
#include "JniHelper.h"

JniEventProcessor* JniEventProcessor::s_this = NULL;
std::mutex JniEventProcessor::s_lock;

JniEventProcessor::JniEventProcessor()
{
    m_currentStart = 0;
    m_currentEnd = 0;
#if _WIN64
    m_cFull = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    m_cEmpty = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);
#else
    m_cFull = dispatch_semaphore_create(BUFFER_SIZE);
    m_cEmpty = dispatch_semaphore_create(0);
#endif

    m_isTerminated = true;
}

void JniEventProcessor::EnsureThread() {
    lock<std::mutex> lock(m_threadHold);
    if (m_isTerminated) {
        m_isTerminated = false;
        pthread_create( &m_tid, NULL, JniEventProcessor::ThreadStart, this);
    }
}

void JniEventProcessor::TerminateThread() {
    lock<std::mutex> lock(m_threadHold);
    if (!m_isTerminated) {
        m_isTerminated = true;
#if _WIN64
        ReleaseSemaphore(m_cEmpty, 1, NULL); /* post to empty */
#else
        dispatch_semaphore_signal(m_cEmpty); /* post to empty */
#endif
    }
}

JniEventProcessor* JniEventProcessor::GetInstance() {
    if (s_this == NULL) {
        lock<std::mutex> lock(s_lock);
        if (s_this == NULL) {
            s_this = new JniEventProcessor();
        }
    }
    s_this->EnsureThread();
    return s_this;
}

void* JniEventProcessor::ThreadStart(void* thisPtr) {
    JniEventProcessor* pThis = (JniEventProcessor*)thisPtr;
    pThis->ProcessLoop();
    return NULL;
}

void JniEventProcessor::EnqueueEvent(JniEvent* item)
{
#if _WIN64
    WaitForSingleObject(m_cFull, INFINITE); /* Wait if full */
#else
    dispatch_semaphore_wait(m_cFull, DISPATCH_TIME_FOREVER); /* Wait if full */
#endif
    lock<std::mutex> lock(m_mutex);
    enQ(item);
#if _WIN64
    ReleaseSemaphore(m_cEmpty, 1, NULL); /* post to empty */
#else
    dispatch_semaphore_signal(m_cEmpty); /* post to empty */
#endif
}

void JniEventProcessor::ProcessLoop()
{
    while(true)
    {
#if _WIN64
        WaitForSingleObject(m_cEmpty, INFINITE); /* Wait if full */
#else
        dispatch_semaphore_wait(m_cEmpty, DISPATCH_TIME_FOREVER); /* Wait if full */
#endif
        lock<std::mutex> lock(m_mutex);
        if (m_isTerminated) {
            break;
        }

        //consume
        JniEvent* pEvent = deQ();
        if (pEvent == NULL) {
            break;
        }
        try {
            pEvent->Process();
            delete pEvent;
        }
        catch(...) {
            //TODO log an error thru debuglog
            //we do our best not to let this thread die or else you dont get firebase events.
        }
#if _WIN64
        ReleaseSemaphore(m_cFull, 1, NULL); /* post to full */
#else
        dispatch_semaphore_signal(m_cFull); /* post to full */
#endif
    }
}

void JniEventProcessor::enQ(JniEvent* item)
{
    m_buffer[m_currentEnd] = item;
    m_currentEnd++;
    m_currentEnd = m_currentEnd % BUFFER_SIZE;
}

JniEvent* JniEventProcessor::deQ()
{
    JniEvent* result = m_buffer[m_currentStart];
    m_currentStart++;
    m_currentStart = m_currentStart % BUFFER_SIZE;
    return result;
}
