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
//  JniFirebase.h
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#ifndef __XCodePlugin__JniFirebase__
#define __XCodePlugin__JniFirebase__

#include <stdio.h>
#include <mutex>
#include "jni.h"

class JniFirebase {
public:
    JniFirebase(const char* path);
    JniFirebase(jobject object);

    ~JniFirebase();
    
    void SetString(const char *value);
    void SetFloat(float value);
    void SetJson(const char *value);
    void SetPriority(const char *value);
    JniFirebase* Child(const char *value);
    JniFirebase* Parent();
    JniFirebase* Root();
    JniFirebase* Push();
    char* GetKey();
    
    void AuthWithCustomToken (const char* token, long callback);
    void AuthAnonymously (long callback);
    void AuthWithPassword (const char* email, const char* password, long callback);
    void AuthWithOAuthToken (const char* provider, const char* token, long callback);
    
    const char* GetAuthToken();
    const char* GetAuthUid();
	uint64_t GetAuthExpiration();
    
    void UnAuth();
    
    jobject getJniObject() {
        return m_firebase;
    }

private:
    jobject m_firebase;
    
    static jclass s_firebaseClass;
    static const char* s_firebaseClassName;
    
    static jmethodID s_firebaseCtor;
    static const char* s_firebaseCtorName;
    static const char* s_firebaseCtorSig;
    
    static jmethodID s_firebaseSetValue;
    static const char* s_firebaseSetValueName;
    static const char* s_firebaseSetValueSig;
    
    static jmethodID s_firebaseSetPriority;
    static const char* s_firebaseSetPriorityName;
    static const char* s_firebaseSetPrioritySig;
    
    static jmethodID s_firebaseChild;
    static const char* s_firebaseChildName;
    static const char* s_firebaseChildSig;
    
    static jmethodID s_firebaseParent;
    static const char* s_firebaseParentName;
    static const char* s_firebaseParentSig;
    
    static jmethodID s_firebaseRoot;
    static const char* s_firebaseRootName;
    static const char* s_firebaseRootSig;
    
    static jmethodID s_firebasePush;
    static const char* s_firebasePushName;
    static const char* s_firebasePushSig;
    
    static jmethodID s_firebaseGetKey;
    static const char* s_firebaseGetKeyName;
    static const char* s_firebaseGetKeySig;
    
    static jclass s_objectMapperClass;
    static const char* s_objectMapperClassName;
    static jclass s_objectClass;
    
    static jmethodID s_objectMapperReadValue;
    static const char* s_objectMapperReadValueName;
    static const char* s_objectMapperReadValueSig;
    
    static std::mutex s_objectMapperLock;
    static jobject s_objectMapperInstance;
    
    static jclass s_anonymousClass;
    static jmethodID s_anonymousCtor;
    
    static jclass s_customTokenClass;
    static jmethodID s_customTokenCtor;
    
    static jclass s_oauthClass;
    static jmethodID s_oauthCtor;
    
    static jclass s_passwordClass;
    static jmethodID s_passwordCtor;
    
    static jmethodID s_firebaseGetAuth;

    
    static jmethodID s_unAuth;
};
#endif /* defined(__XCodePlugin__JniFirebase__) */
