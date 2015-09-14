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
#include <pthread.h>
#include <stdlib.h>
#include <mutex>
#include <dispatch/dispatch.h>
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
    
    dispatch_semaphore_t m_cEmpty;
    dispatch_semaphore_t m_cFull;
    std::mutex m_mutex;
    std::mutex m_threadHold;

    pthread_t m_tid;
    
    static JniEventProcessor* s_this;
    static std::mutex s_lock;
};

#endif /* defined(__XCodePlugin__JniEventProcessor__) */
