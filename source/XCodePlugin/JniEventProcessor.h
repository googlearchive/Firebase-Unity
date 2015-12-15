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
//  JniEventProcessor.h
//  XCodePlugin
//
//  Created by benwu on 9/1/15.
//
//

#ifndef __XCodePlugin__JniEventProcessor__
#define __XCodePlugin__JniEventProcessor__

#include <stdio.h>
#include "JniEventProcessor.h"
#if _WIN64
#include "pthread.h"
#include <Windows.h>
#else
#include <pthread.h>
#include <dispatch/dispatch.h>
#endif
#include <stdlib.h>
#include <mutex>
#include "JniDataSnapshot.h"
#include "jnistub_ChildEventListenerStub.h"
#include "jnistub_ValueEventListenerStub.h"
#include "JniHelper.h"
#include "JniEvent.h"

#define BUFFER_SIZE 500


class JniEventProcessor {
public:
    JniEventProcessor();
    
    void EnqueueEvent(JniEvent* item);
    void ProcessLoop();
    void TerminateThread();
    
    static JniEventProcessor* GetInstance();
    

private:
    void enQ(JniEvent* item);
    void EnsureThread();
    JniEvent* deQ();
    static void* ThreadStart(void* );

    JniEvent* m_buffer[BUFFER_SIZE];
    int m_currentStart;
    int m_currentEnd;
    bool m_isTerminated;
    
#if _WIN64
    HANDLE m_cEmpty;
    HANDLE m_cFull;
#else
    dispatch_semaphore_t m_cEmpty;
    dispatch_semaphore_t m_cFull;
#endif
    std::mutex m_mutex;
    std::mutex m_threadHold;

    pthread_t m_tid;
    
    static JniEventProcessor* s_this;
    static std::mutex s_lock;
};

#endif /* defined(__XCodePlugin__JniEventProcessor__) */
