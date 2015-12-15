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
//  JniValueEventStub.cpp
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#include "JniValueEventStub.h"
#include "JniHelper.h"
#include "Plugin.pch"

jclass JniValueEventStub::s_stubClass = NULL;
const char* JniValueEventStub::s_stubClassName = "jnistub/ValueEventListenerStub";

jmethodID JniValueEventStub::s_stubCtor = NULL;
const char* JniValueEventStub::s_stubCtorName = "<init>";
const char* JniValueEventStub::s_stubCtorSig = "(Lcom/firebase/client/Firebase;J)V";

jmethodID JniValueEventStub::s_stubRelease = NULL;
const char* JniValueEventStub::s_stubReleaseName = "release";
const char* JniValueEventStub::s_stubReleaseSig = "()V";

JniValueEventStub::JniValueEventStub(JniFirebase* firebase, jlong cookie) {
    auto env = getEnv();
    
    if (!GetClass(env, s_stubClassName, &s_stubClass)) {
        return;
    }
    
    if (!GetMethod(env, s_stubClass, s_stubCtorName, s_stubCtorSig, &s_stubCtor)) {
        return;
    }
    
    if (!GetMethod(env, s_stubClass, s_stubReleaseName, s_stubReleaseSig, &s_stubRelease)) {
        return;
    }
    g_debugLog("created stub");
    
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_stubClass, s_stubCtor, firebase->getJniObject(),
                                              cookie));
    m_stub = env->NewGlobalRef(localRef);
}


JniValueEventStub::~JniValueEventStub() {
    auto env = getEnv();
    if (env != NULL && m_stub != NULL) {
        if (s_stubRelease != NULL) {
            g_debugLog("released stub");

            env->CallVoidMethod(m_stub, s_stubRelease);
        }
        env->DeleteGlobalRef((jobject)m_stub);
    }
    m_stub = NULL;
}
