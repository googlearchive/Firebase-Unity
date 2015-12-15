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
//  JniAuth.cpp
//  XCodePlugin
//
//  Created by benwu on 9/11/15.
//
//

#include "JniAuth.h"

jclass JniAuth::s_authClass = NULL;
jmethodID JniAuth::s_authGetToken = NULL;
jmethodID JniAuth::s_getUid = NULL;
jmethodID JniAuth::s_getExpires = NULL;

JniAuth::JniAuth(jobject authobject) {
    auto env = getEnv();
    if (!GetClass(env, "com/firebase/client/AuthData", &s_authClass)) {
        return;
    }
    if (authobject != NULL) {
        m_auth = env->NewGlobalRef(authobject);
    }
    else {
        m_auth = NULL;
    }
}

JniAuth::~JniAuth() {
    auto env = getEnv();
    if (env != NULL && m_auth != NULL) {
        env->DeleteGlobalRef((jobject)m_auth);
    }
    m_auth = NULL;
}

const char* JniAuth::GetAuthToken() {
    auto env = getEnv();
    if (m_auth == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_authClass, "getToken", "()Ljava/lang/String;",
                   &s_authGetToken)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_auth, s_authGetToken));
    if((jstring)java_string == NULL){
        return NULL;
    }
    const char* utf_string = env->GetStringUTFChars(java_string, NULL);
#if _WIN64
    char* result = _strdup(utf_string);
#else
    char* result = strdup(utf_string);
#endif
    env->ReleaseStringUTFChars(java_string, utf_string);
    return result;
}

const char* JniAuth::GetAuthUid() {
    auto env = getEnv();
    if (m_auth == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_authClass, "getUid", "()Ljava/lang/String;",
                   &s_getUid)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_auth, s_getUid));
    if((jstring)java_string == NULL){
        return NULL;
    }
    const char* utf_string = env->GetStringUTFChars(java_string, NULL);
    #if _WIN64
        char* result = _strdup(utf_string);
    #else
        char* result = strdup(utf_string);
    #endif
    env->ReleaseStringUTFChars(java_string, utf_string);
    return result;
}

uint64_t JniAuth::GetAuthExpiration() {
    auto env = getEnv();
    if (m_auth == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_authClass, "getExpires", "()J",
                   &s_getExpires)) {
        return NULL;
    }
    jlong data = env->CallLongMethod(m_auth, s_getExpires);
    return data;
}

