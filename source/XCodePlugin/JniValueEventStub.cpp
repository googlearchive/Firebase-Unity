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