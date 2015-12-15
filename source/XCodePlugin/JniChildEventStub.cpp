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
//  JniChildEventStub.cpp
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#include "JniChildEventStub.h"
#include "JniHelper.h"

//TODO the stub classes are almost identical and could be merged.

jclass JniChildEventStub::s_stubClass = NULL;
const char* JniChildEventStub::s_stubClassName = "jnistub/ChildEventListenerStub";

jmethodID JniChildEventStub::s_stubCtor = NULL;
const char* JniChildEventStub::s_stubCtorName = "<init>";
const char* JniChildEventStub::s_stubCtorSig = "(Lcom/firebase/client/Firebase;J)V";

jmethodID JniChildEventStub::s_stubRelease = NULL;
const char* JniChildEventStub::s_stubReleaseName = "release";
const char* JniChildEventStub::s_stubReleaseSig = "()V";

JniChildEventStub::JniChildEventStub(JniFirebase* firebase, jlong cookie) {
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
    
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_stubClass, s_stubCtor, firebase->getJniObject(),
                                              cookie));
    m_stub = env->NewGlobalRef(localRef);
}

JniChildEventStub::~JniChildEventStub() {
    auto env = getEnv();
    if (env != NULL && m_stub != NULL) {
        if (s_stubRelease != NULL) {
            env->CallVoidMethod(m_stub, s_stubRelease);
        }
        env->DeleteGlobalRef((jobject)m_stub);
    }
    m_stub = NULL;
}
