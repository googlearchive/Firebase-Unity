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
#include <stdio.h>
#include <map>
#include <thread>
#include "Plugin.pch"
#include "jni.h"
#include "JniHelper.h"
#include "JniFirebase.h"
#include "JniValueEventStub.h"
#include "JniChildEventStub.h"
#include "JniDataSnapshot.h"
#include "JniEventProcessor.h"

void OnValueChangedNoop(void*, void* pOrphanedSnapshot) {
    if (pOrphanedSnapshot != NULL) {
        JniDataSnapshot* snapshot = (JniDataSnapshot*)pOrphanedSnapshot;
        delete snapshot;
    }
}
void OnDebugLogNoop(const char*) {}

void OnAuthSuccessNoop(uint64_t, const char*, const char*, uint64_t) {}
void OnAuthCancelNoop(uint64_t, int , const char*, const char*) {}
void OnErrorNoop(void*, int , const char*, const char*) {}

OnValueChanged g_valueChanged = &OnValueChangedNoop;
OnValueChanged g_childAdded = &OnValueChangedNoop;
OnValueChanged g_childRemoved = &OnValueChangedNoop;
OnValueChanged g_childChanged = &OnValueChangedNoop;
OnValueChanged g_childMoved = &OnValueChangedNoop;
OnDebugLog g_debugLog = &OnDebugLogNoop;
OnAuthSuccess g_authSuccess = &OnAuthSuccessNoop;
OnAuthCancel g_authCancel = &OnAuthCancelNoop;
OnError g_Error = &OnErrorNoop;

void DestroyJvm() {
    JniEventProcessor::GetInstance()->TerminateThread();
}

float GetJvmVersion() {
    JNIEnv* env = getEnv();
    if (env == NULL) {
        return 0;
    }
    jint ver = env->GetVersion();
    return ((ver>>16) & 0x0f) + ((float)(ver & 0x0f)) /10 ;
}

void _FirebaseSetCallbacks(OnValueChanged valueChanged,
                           OnValueChanged childAdded,
                           OnValueChanged childRemoved,
                           OnValueChanged childChanged,
                           OnValueChanged childMoved,
                           OnDebugLog debugLog,
                           OnAuthSuccess authSuccess,
                           OnAuthCancel authFailure,
                           OnError onError) {

    //we never let the callback pointers be null because we want to noop on thread
    //race conditions where the value might suddenly change underneath a callback
    //We prevent bad callbacks by unregistering aggressively in the unity editor as soon
    // as the player is stopped (before the appdomain shuts down).
    // hooking appdomain shutdown isnt better and creates complications because unity
    // delays appdomain shutdown until the next run and it can overlay the newer appdomain
    // running the unity player
    g_valueChanged = valueChanged != NULL ? valueChanged : &OnValueChangedNoop;
    g_childAdded = childAdded != NULL ? childAdded : &OnValueChangedNoop;
    g_childChanged = childChanged != NULL ? childChanged : &OnValueChangedNoop;
    g_childRemoved = childRemoved != NULL ? childRemoved : &OnValueChangedNoop;
    g_childMoved = childMoved != NULL ? childMoved : &OnValueChangedNoop;

    g_debugLog = debugLog != NULL ? debugLog : &OnDebugLogNoop;
    g_authSuccess = authSuccess != NULL ? authSuccess : &OnAuthSuccessNoop;;
    g_authCancel = authFailure != NULL ? authFailure : &OnAuthCancelNoop;;

    g_Error = onError != NULL ? onError : &OnErrorNoop;;

    auto env = getEnv();
    if (env != NULL) {
        DebugLog("FireBasePlugin: initialized");
    }
}

void* _FirebaseNew (const char *path)
{
    return new JniFirebase(path);
}

void _FirebaseRelease(void* firebase) {
    if (!firebase) return;
    JniFirebase* localRef = (JniFirebase*)firebase;
    delete localRef;
}

void _FirebaseSetString(void* firebase, const char *value) {
    if (!firebase) return;
    JniFirebase* localRef = (JniFirebase*)firebase;
    localRef->SetString(value);
}

void _FirebaseSetFloat(void* firebase, float value) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->SetFloat(value);
}

void _FirebaseSetJson(void* firebase, const char *value) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->SetJson(value);
}

void _FirebaseSetPriority(void* firebase, const char *value) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->SetPriority(value);
}

void* _FirebaseChild(void* firebase, const char *path)
{
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->Child(path);
}

void* _FirebaseParent(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->Parent();
}

void* _FirebaseRoot(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->Root();
}

void* _FirebasePush(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->Push();
}

const char* _FirebaseGetKey(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;

    return jniFirebase->GetKey();
}

// It's worth talking a bit about how events work with c#->c++->jni
// C# has pinned static callback methods for each method on each callback interface
// To register an event, c# calls into a c++ method like this one, passing the native firebase to hook.
// C++ will then use jni to create a jobject stub listener initialized with the firebase ref.
// It returns the created stub jobject casted to a void* after leaving open an global reference
// When an event happens, the created stub calls into C++ with the firebase pointer.  C++ then calls back
// into the pinned static methods in mono with that firebase ptr.  the c# code then has to properly route the call
// based on the firebase ref sent.
// To release an event, mono will send in the returned stub.  C++ casts it to its stub jni jobject
// and then calls release() on it and also releases the jni global ref.
void* _FirebaseObserveValueChange( void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return new JniValueEventStub(jniFirebase, (jlong)firebase);
}

void* _FirebaseObserveChildEvents( void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return new JniChildEventStub(jniFirebase, (jlong)firebase);
}

void _FirebaseRemoveValueChange(void* stubReference) {
    if (!stubReference) return;
    JniValueEventStub* jniStub = (JniValueEventStub*) stubReference;
    delete jniStub;
}

void _FirebaseRemoveChildEvents(void* stubReference) {
    if (!stubReference) return;
    JniChildEventStub* jniStub = (JniChildEventStub*) stubReference;
    delete jniStub;
}

void _FirebaseAuthWithCustomToken (void* firebase, const char* token, long callback) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->AuthWithCustomToken(token, callback);
}

void _FirebaseAuthAnonymously (void* firebase, long callback) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->AuthAnonymously(callback);
}

void _FirebaseAuthWithPassword (void* firebase, const char* email,
                                const char* password, long callback) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->AuthWithPassword(email, password, callback);
}

void _FirebaseAuthWithOAuthToken (void* firebase, const char* provider,
                                  const char* token, long callback) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->AuthWithOAuthToken(provider, token, callback);
}

const char* _FirebaseGetAuthToken(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->GetAuthToken();
}

const char* _FirebaseGetAuthUid(void* firebase) {
    if (!firebase) return NULL;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->GetAuthUid();
}

uint64_t _FirebaseGetAuthExpiration(void* firebase) {
    if (!firebase) return 0;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    return jniFirebase->GetAuthExpiration();
}

void _FirebaseUnAuth(void* firebase) {
    if (!firebase) return;
    JniFirebase* jniFirebase = (JniFirebase*) firebase;
    jniFirebase->UnAuth();
}

float _DataSnapshotGetFloatValue (void* datasnapshot) {
    if (!datasnapshot) return 0;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetFloatValue();
}

const char* _DataSnapshotGetStringValue (void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetStringValue();
}

const char* _DataSnapshotGetDictionary(void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetDictionaryJSON();
}

void* _DataSnapshotGetChild (void* datasnapshot, const char* path) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetChild(path);
}

void* _DataSnapshotHasChild (void* datasnapshot, const char* path) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    if (jniDataSnapshot->HasChild(path)) {
        return (void*)(1);
    }
    return NULL;
}

void* _DataSnapshotExists (void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    if (jniDataSnapshot->Exists()) {
        return (void*)(1);
    }
    return NULL;
}

const char* _DataSnapshotGetKey (void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetKey();
}

const char* _DataSnapshotGetPriority (void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetPriority();
}

void* _DataSnapshotGetRef (void* datasnapshot) {
    if (!datasnapshot) return NULL;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    return jniDataSnapshot->GetRef();
}

void _DataSnapshotRelease(void* datasnapshot) {
    if (!datasnapshot) return;
    JniDataSnapshot* jniDataSnapshot = (JniDataSnapshot*)datasnapshot;
    delete jniDataSnapshot;
}


