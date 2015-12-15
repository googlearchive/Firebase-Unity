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
//  JniHelper.h
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#ifndef XCodePlugin_JniHelper_h
#define XCodePlugin_JniHelper_h

#include <stdio.h>
#if _WIN64
#else
#include <dlfcn.h>
#endif
#include <mutex>
#include <map>
#include <sstream>
#include "jni.h"

extern std::mutex g_lock;
extern JavaVM *g_jvm;
extern std::map<jclass, jmethodID> s_toStringMap;
extern std::mutex s_toStringlock;

JNIEnv* getEnv();

inline
int ExceptionCheck(JNIEnv* env) {
    if (env->ExceptionCheck()) {
        // have to clear the exception before JNI will work again.
        env->ExceptionDescribe();
        return -1;
    }
    return 0;
}

template <class Lockable>
class lock{
public:
    lock(Lockable & m) : mtx(m){
        mtx.lock();
    }
    ~lock(){
        mtx.unlock();
    }
private:
    Lockable & mtx;
};

struct JOBJECT {
    jobject local;
    JNIEnv* env;
    
    JOBJECT(JNIEnv* pEnv, jobject object) {
        local = object;
        env = pEnv;
    }
    operator jobject() {
        return local;
    }
    ~JOBJECT() {
        if (local && env) {
            env->DeleteLocalRef(local);
        }
        local = NULL;
        env = NULL;
    }
};

struct JCLASS {
    jclass local;
    JNIEnv* env;
    
    JCLASS(JNIEnv* pEnv, jclass object) {
        local = object;
        env = pEnv;
    }
    operator jclass() {
        return local;
    }
    ~JCLASS() {
        if (local && env) {
          env->DeleteLocalRef(local);
        }
        local = NULL;
        env = NULL;
    }
};

struct JSTRING {
    jstring local;
    JNIEnv* env;
    
    JSTRING(JNIEnv* pEnv, jstring object) {
        local = object;
        env = pEnv;
    }
    operator jstring() {
        return local;
    }
    ~JSTRING() {
        if (local && env) {
          env->DeleteLocalRef(local);
        }
        local = NULL;
        env = NULL;
    }
};

inline
bool GetClass(JNIEnv* env, const char* className, jclass* pClass) {
    if (*pClass) {
        return true;
    }
    lock<std::mutex> lock(g_lock);
    if (*pClass) {
        return true;
    }
    JCLASS localRef = JCLASS(env, env->FindClass (className));
    if (localRef) {
        *pClass = (jclass)env->NewGlobalRef(localRef);
    }
    return (*pClass) != NULL;
}

inline
bool GetMethod(JNIEnv* env, jclass clazz, const char* methodName, const char* methodSig, jmethodID* pMethod) {
    if (!env) {
        return false;
    }
    if (*pMethod) {
        return true;
    }
    lock<std::mutex> lock(g_lock);
    if (*pMethod) {
        return true;
    }
    *pMethod = env->GetMethodID(clazz, methodName, methodSig);
    return (*pMethod) != NULL;
}

const char* CallToString(JNIEnv* env, jobject localRef) ;

const char* GetJNIExceptionDescription(JNIEnv* env, jthrowable exception );

void DebugLog(const char *);

template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

inline
void LogIfException(JNIEnv* env) {
    if (env->ExceptionCheck() == JNI_TRUE) {
        DebugLog(GetJNIExceptionDescription(env, env->ExceptionOccurred()));
        env->ExceptionClear();
    }
}

#endif
