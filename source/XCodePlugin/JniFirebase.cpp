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
//  JniFirebase.cpp
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#include "JniFirebase.h"
#include "JniHelper.h"
#include "JniFloat.h"
#include "JniAuth.h"

jclass JniFirebase::s_firebaseClass = NULL;
jmethodID JniFirebase::s_firebaseCtor = NULL;
JniFirebase::JniFirebase(const char* path) {
    auto env = getEnv();
    
    if (!GetClass(env, "com/firebase/client/Firebase", &s_firebaseClass)) {
        return;
    }
        
    if (!GetMethod(env, s_firebaseClass, "<init>", "(Ljava/lang/String;)V", &s_firebaseCtor)) {
        return;
    }
        
    JOBJECT localRef = JOBJECT(env, env->NewObject(s_firebaseClass, s_firebaseCtor,
                                                  (jstring)JSTRING(env, env->NewStringUTF(path))));
    m_firebase = env->NewGlobalRef(localRef);
}

JniFirebase::JniFirebase(jobject object) {
    auto env = getEnv();
    if (env == NULL) {
        m_firebase = NULL;
    }
    else {
        m_firebase = env->NewGlobalRef(object);
    }
}

JniFirebase::~JniFirebase() {
    auto env = getEnv();
    if (env != NULL && m_firebase != NULL) {
        env->DeleteGlobalRef((jobject)m_firebase);
    }
    m_firebase = NULL;
}

jmethodID JniFirebase::s_firebaseSetValue = NULL;
const char* JniFirebase::s_firebaseSetValueName="setValue";
const char* JniFirebase::s_firebaseSetValueSig="(Ljava/lang/Object;)V";
void JniFirebase::SetString(const char *value) {
    if (!m_firebase) {
        return;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseSetValueName, s_firebaseSetValueSig, &s_firebaseSetValue)) {
        return;
    }
    env->CallVoidMethod(m_firebase, s_firebaseSetValue, (jstring)JSTRING(env, env->NewStringUTF(value)));
}


void JniFirebase::SetFloat(float value) {
    if (!m_firebase) {
        return;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseSetValueName, s_firebaseSetValueSig, &s_firebaseSetValue)) {
        return;
    }
    JniFloat* jniFloat = new JniFloat(env, value);
    env->CallVoidMethod(m_firebase, s_firebaseSetValue, (jobject) (*jniFloat));
    delete jniFloat;
}

jclass JniFirebase::s_objectMapperClass = NULL;
const char* JniFirebase::s_objectMapperClassName = "com/fasterxml/jackson/databind/ObjectMapper";
jobject JniFirebase::s_objectMapperInstance = NULL;
jclass JniFirebase::s_objectClass = NULL;

jmethodID JniFirebase::s_objectMapperReadValue = NULL;
std::mutex JniFirebase::s_objectMapperLock;
const char* JniFirebase::s_objectMapperReadValueName = "readValue";
const char* JniFirebase::s_objectMapperReadValueSig = "(Ljava/lang/String;Ljava/lang/Class;)Ljava/lang/Object;";
void JniFirebase::SetJson(const char *value) {
    auto env = getEnv();
    
    if (s_objectMapperInstance == NULL) {
        lock<std::mutex> lock(s_objectMapperLock);
        if (s_objectMapperInstance == NULL) {
            if (!GetClass(env, "java/util/HashMap", &s_objectClass)) {
                return;
            }

            if (!GetClass(env, s_objectMapperClassName, &s_objectMapperClass)) {
                return;
            }

            jmethodID ctor = NULL;
            if (!GetMethod(env, s_objectMapperClass, "<init>", "()V", &ctor))
            {
                return;
            }

            if (!GetMethod(env, s_objectMapperClass, s_objectMapperReadValueName, s_objectMapperReadValueSig, &s_objectMapperReadValue)) {
                return;
            }

            JOBJECT localRef = JOBJECT(env, env->NewObject(s_objectMapperClass, ctor));
            s_objectMapperInstance = env->NewGlobalRef(localRef);
        }
    }
    
    JOBJECT localValue = JOBJECT(env,
                                env->CallObjectMethod(s_objectMapperInstance, s_objectMapperReadValue,
                                                      (jstring)JSTRING(env, env->NewStringUTF(value)), s_objectClass));

    if (env->ExceptionCheck() == JNI_TRUE) {
        SetString(GetJNIExceptionDescription(env, env->ExceptionOccurred()));
        return;
    }
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseSetValueName, s_firebaseSetValueSig, &s_firebaseSetValue)) {
        return;
    }
    
    env->CallVoidMethod(m_firebase, s_firebaseSetValue, (jobject)localValue);
}

jmethodID JniFirebase::s_firebaseSetPriority = NULL;
const char* JniFirebase::s_firebaseSetPriorityName="setPriority";
const char* JniFirebase::s_firebaseSetPrioritySig="(Ljava/lang/String;)V";
void JniFirebase::SetPriority(const char *value) {
    if (!m_firebase) {
        return;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseSetPriorityName, s_firebaseSetPrioritySig, &s_firebaseSetPriority)) {
        return;
    }
    env->CallVoidMethod(m_firebase, s_firebaseSetPriority, (jstring)JSTRING(env, env->NewStringUTF(value)));
}

jmethodID JniFirebase::s_firebaseChild = NULL;
const char* JniFirebase::s_firebaseChildName="child";
const char* JniFirebase::s_firebaseChildSig="(Ljava/lang/String;)Lcom/firebase/client/Firebase;";
JniFirebase* JniFirebase::Child(const char *value) {
    if (!m_firebase) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseChildName, s_firebaseChildSig, &s_firebaseChild)) {
        return NULL;
    }
    JOBJECT childFirebase = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseChild, (jstring)JSTRING(env, env->NewStringUTF(value))));
    return new JniFirebase(childFirebase);
}

jmethodID JniFirebase::s_firebaseParent = NULL;
const char* JniFirebase::s_firebaseParentName="getParent";
const char* JniFirebase::s_firebaseParentSig="()Lcom/firebase/client/Firebase";
JniFirebase* JniFirebase::Parent() {
    if (!m_firebase) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseParentName, s_firebaseParentSig, &s_firebaseParent)) {
        return NULL;
    }
    JOBJECT parentFirebase = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseParent));
    return new JniFirebase(parentFirebase);
}

jmethodID JniFirebase::s_firebaseRoot = NULL;
const char* JniFirebase::s_firebaseRootName="getRoot";
const char* JniFirebase::s_firebaseRootSig="()Lcom/firebase/client/Firebase;";
JniFirebase* JniFirebase::Root() {
    if (!m_firebase) {
        return NULL;
    }
    auto env = getEnv();
    
    if (!GetMethod(env, s_firebaseClass, s_firebaseRootName, s_firebaseRootSig, &s_firebaseRoot)) {
        return NULL;
    }
    JOBJECT rootFirebase = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseRoot));
    return new JniFirebase(rootFirebase);
}

jmethodID JniFirebase::s_firebaseGetKey = NULL;
const char* JniFirebase::s_firebaseGetKeyName = "getKey";
const char* JniFirebase::s_firebaseGetKeySig = "()Ljava/lang/String;";
char* JniFirebase::GetKey()
{
    if (!m_firebase) {
        return NULL;
    }
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, s_firebaseGetKeyName, s_firebaseGetKeySig, &s_firebaseGetKey)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_firebase, s_firebaseGetKey));
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

jmethodID JniFirebase::s_firebasePush = NULL;
const char* JniFirebase::s_firebasePushName="push";
const char* JniFirebase::s_firebasePushSig="()Lcom/firebase/client/Firebase;";
JniFirebase* JniFirebase::Push() {
    if (!m_firebase) {
        return NULL;
    }
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, s_firebasePushName, s_firebasePushSig, &s_firebasePush)) {
        return NULL;
    }
    return new JniFirebase(env->CallObjectMethod(m_firebase, s_firebasePush));
}

jclass JniFirebase::s_anonymousClass = NULL;
jmethodID JniFirebase::s_anonymousCtor = NULL;
void JniFirebase::AuthAnonymously (long callback) {
    auto env = getEnv();
    if (!GetClass(env, "jnistub/AuthAnonymousStub", &s_anonymousClass)) {
        return;
    }
    
    if (!GetMethod(env, s_anonymousClass, "<init>",
                   "(Lcom/firebase/client/Firebase;J)V", &s_anonymousCtor)) {
        return;
    }
        
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_anonymousClass, s_anonymousCtor, getJniObject(),
                                              (jlong)callback));
}

jclass JniFirebase::s_customTokenClass = NULL;
jmethodID JniFirebase::s_customTokenCtor = NULL;
void JniFirebase::AuthWithCustomToken (const char* token, long callback) {
    auto env = getEnv();
    if (!GetClass(env, "jnistub/AuthCustomTokenStub", &s_customTokenClass)) {
        return;
    }
    
    if (!GetMethod(env, s_customTokenClass, "<init>",
                   "(Lcom/firebase/client/Firebase;JLjava/lang/String;)V", &s_customTokenCtor)) {
        return;
    }
    
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_customTokenClass, s_customTokenCtor, getJniObject(),
                                              (jlong)callback,
                                              (jstring)JSTRING(env, env->NewStringUTF(token))));
}

jclass JniFirebase::s_oauthClass = NULL;
jmethodID JniFirebase::s_oauthCtor = NULL;
void JniFirebase::AuthWithOAuthToken (const char* provider, const char* token, long callback) {
    auto env = getEnv();
    if (!GetClass(env, "jnistub/AuthOAuthStub", &s_oauthClass)) {
        return;
    }
    
    if (!GetMethod(env, s_oauthClass, "<init>",
                   "(Lcom/firebase/client/Firebase;JLjava/lang/String;Ljava/lang/String;)V",
                   &s_oauthCtor)) {
        return;
    }
    
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_oauthClass, s_oauthCtor, getJniObject(),
                                              (jlong)callback,
                                              (jstring)JSTRING(env, env->NewStringUTF(provider)),
                                              (jstring)JSTRING(env, env->NewStringUTF(token))));
}

jclass JniFirebase::s_passwordClass = NULL;
jmethodID JniFirebase::s_passwordCtor = NULL;
void JniFirebase::AuthWithPassword (const char* email, const char* password, long callback) {
    auto env = getEnv();
    if (!GetClass(env, "jnistub/AuthPasswordStub", &s_passwordClass)) {
        return;
    }
    
    if (!GetMethod(env, s_passwordClass, "<init>",
                   "(Lcom/firebase/client/Firebase;JLjava/lang/String;Ljava/lang/String;)V", &s_passwordCtor)) {
        return;
    }
    
    JOBJECT localRef = JOBJECT(env,
                               env->NewObject(s_passwordClass, s_passwordCtor, getJniObject(),
                                              (jlong)callback,
                                              (jstring)JSTRING(env, env->NewStringUTF(email)),
                                              (jstring)JSTRING(env, env->NewStringUTF(password))));
}

jmethodID JniFirebase::s_firebaseGetAuth = NULL;

const char* JniFirebase::GetAuthToken() {
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, "getAuth", "()Lcom.firebase.client.AuthData;",
                   &s_firebaseGetAuth)) {
        return NULL;
    }
    
    JOBJECT authData = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseGetAuth));
    JniAuth auth = JniAuth(authData);
    return auth.GetAuthToken();
}

const char* JniFirebase::GetAuthUid() {
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, "getAuth", "()Lcom.firebase.client.AuthData;",
                   &s_firebaseGetAuth)) {
        return NULL;
    }
    
    JOBJECT authData = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseGetAuth));
    JniAuth auth = JniAuth(authData);
    return auth.GetAuthToken();
}

uint64_t JniFirebase::GetAuthExpiration() {
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, "getAuth", "()Lcom.firebase.client.AuthData;",
                   &s_firebaseGetAuth)) {
        return NULL;
    }
    
    JOBJECT authData = JOBJECT(env, env->CallObjectMethod(m_firebase, s_firebaseGetAuth));
    JniAuth auth = JniAuth(authData);
    return auth.GetAuthExpiration();
}

jmethodID JniFirebase::s_unAuth = NULL;
void JniFirebase::UnAuth() {
    auto env = getEnv();
    if (!GetMethod(env, s_firebaseClass, "unauth", "()V",
                   &s_unAuth)) {
        return;
    }
    env->CallVoidMethod(m_firebase, s_unAuth);
}


