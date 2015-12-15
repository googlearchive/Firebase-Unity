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
//  JniDataSnapshot.cpp
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#include "JniDataSnapshot.h"
#include "JniHelper.h"
#include "JniFloat.h"

jclass JniDataSnapshot::s_class = NULL;

JniDataSnapshot::JniDataSnapshot(jobject localRef, void* cookie, JniDataSnapshot::EventSource source) {
    m_cookie = cookie;
    m_source = source;
    auto env = getEnv();
    if ((JNIEnv*)env == NULL) {
        DebugLog("FireBasePlugin: unable to attach to Java");
        return;
    }
    
    if (env->IsSameObject(localRef, NULL)) {
        m_globalRef = NULL;
        return;
    }

    if (env->ExceptionCheck() == JNI_TRUE) {
        DebugLog(GetJNIExceptionDescription(env, env->ExceptionOccurred()));
        env->ExceptionClear();
    }
    
    if (!GetClass(env, "com/firebase/client/DataSnapshot", &s_class)) {
        return;
    }
    m_globalRef = env->NewGlobalRef(localRef);
}

JniDataSnapshot::~JniDataSnapshot() {
    auto env = getEnv();
    if (env != NULL && m_globalRef != NULL) {
        env->DeleteGlobalRef((jobject)m_globalRef);
    }
    m_globalRef = NULL;

}

void* JniDataSnapshot::GetCookie() {
    return m_cookie;
}

JniDataSnapshot::EventSource JniDataSnapshot::GetSource() {
    return m_source;
}


jmethodID JniDataSnapshot::s_getValue = NULL;
float JniDataSnapshot::GetFloatValue() {
    if (!m_globalRef) {
        return 0;
    }
    
    auto env = getEnv();
    
    if (!GetMethod(env, s_class, "getValue", "()Ljava/lang/Object;", &s_getValue)) {
        return 0;
    }
    JniFloat* floatRef = new JniFloat(env, env->CallObjectMethod(m_globalRef, s_getValue));
    float result = floatRef->Value();
    delete floatRef;
    return result;
}

// We use a map of jclass to methodId so we can cache all overridden tostring methods
// on a per class basis.
const char* JniDataSnapshot::GetStringValue() {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    if (env == NULL) {
        return NULL;
    }
    
    if (!GetMethod(env, s_class, "getValue", "()Ljava/lang/Object;", &s_getValue)) {
        return NULL;
    }
    
    return CallToString(env, JOBJECT(env, env->CallObjectMethod(m_globalRef, s_getValue)));
}

jclass JniDataSnapshot::s_jsonClass = NULL;
jmethodID JniDataSnapshot::s_jsonCtor = NULL;
const char* JniDataSnapshot::GetDictionaryJSON() {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();

    if (!GetMethod(env, s_class, "getValue", "()Ljava/lang/Object;", &s_getValue)) {
        return NULL;
    }

    JOBJECT localRef = JOBJECT(env, env->CallObjectMethod(m_globalRef, s_getValue));
    if ((jobject)localRef == NULL) {
        return NULL;
    }

    if (!GetClass(env, "org/json/JSONObject", &s_jsonClass)) {
        return NULL;
    }

    if (!GetMethod(env, s_jsonClass, "<init>", "(Ljava/util/Map;)V", &s_jsonCtor)) {
        return NULL;
    }
    
    JOBJECT tempJSON = JOBJECT(env, env->NewObject(s_jsonClass, s_jsonCtor, (jobject)localRef));
    LogIfException(env);
    const char* result = CallToString(env, tempJSON);
    return result;
}

jmethodID JniDataSnapshot::s_getChild = NULL;
JniDataSnapshot* JniDataSnapshot::GetChild (const char* path) {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_class, "child", "(Ljava/lang/String;)Lcom/firebase/client/DataSnapshot;", &s_getChild)) {
        return NULL;
    }
    JOBJECT childSnapshot = JOBJECT(env, env->CallObjectMethod(m_globalRef, s_getChild,
                                                               (jstring)JSTRING(env, env->NewStringUTF(path))));

    return new JniDataSnapshot(childSnapshot, 0, None);
}

jmethodID JniDataSnapshot::s_hasChild = NULL;;
bool JniDataSnapshot::HasChild (const char* path) {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_class, "hasChild", "(Ljava/lang/String;)Z", &s_hasChild)) {
        return false;
    }
    jboolean result = env->CallBooleanMethod(m_globalRef, s_hasChild, (jstring)JSTRING(env, env->NewStringUTF(path)));
    
    return (bool)(result != JNI_FALSE);
}

jmethodID JniDataSnapshot::s_exists = NULL;;
bool JniDataSnapshot::Exists () {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_class, "exists", "()Z", &s_exists)) {
        return false;
    }
    jboolean result = env->CallBooleanMethod(m_globalRef, s_exists);
    
    return (bool)(result != JNI_FALSE);
}

jmethodID JniDataSnapshot::s_getKey = NULL;;
const char* JniDataSnapshot::GetKey () {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    if (!GetMethod(env, s_class, "getKey", "()Ljava/lang/String;", &s_getKey)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_globalRef, s_getKey));
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

jmethodID JniDataSnapshot::s_getPriority = NULL;;
const char* JniDataSnapshot::GetPriority () {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    if (!GetMethod(env, s_class, "getPriority", "()Ljava/lang/String;", &s_getPriority)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_globalRef, s_getPriority));
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

jmethodID JniDataSnapshot::s_getRef = NULL;;
JniFirebase* JniDataSnapshot::GetRef () {
    if (!m_globalRef) {
        return NULL;
    }
    auto env = getEnv();
    if (!GetMethod(env, s_class, "getRef", "()Lcom/firebase/client/Firebase;", &s_getRef)) {
        return NULL;
    }
    JOBJECT localRef = JOBJECT(env, env->CallObjectMethod(m_globalRef, s_getRef));
    if((jobject)localRef == NULL){
        return NULL;
    }
    return new JniFirebase(localRef);
}

