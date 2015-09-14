//
//  JniDataSnapshot.h
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#ifndef __XCodePlugin__JniDataSnapshot__
#define __XCodePlugin__JniDataSnapshot__

#include <stdio.h>
#include <map>
#include <mutex>

#include "JniFirebase.h"
#include "jni.h"

class JniDataSnapshot {
public:
    enum EventSource
    {
        None = 0,
        ValueChanged,
        ChildAdded,
        ChildChanged,
        ChildRemoved,
        ChildMoved,
        Error
    };
    
    JniDataSnapshot(jobject localRef, void* cookie, EventSource source);
    ~JniDataSnapshot();
    
    float GetFloatValue();
    const char* GetStringValue();
    const char* GetDictionaryJSON();
    JniDataSnapshot* GetChild (const char* path);
    bool HasChild (const char* path);
    bool Exists ();
    const char* GetKey ();
    const char* GetPriority ();
    JniFirebase* GetRef ();
    void* GetCookie();
    EventSource GetSource();
    
private:
    jobject m_globalRef;
    void* m_cookie;
    EventSource m_source;
    
    static jclass s_class;
    static jmethodID s_getValue;
    static jmethodID s_getChild;
    static jmethodID s_hasChild;
    static jmethodID s_exists;
    static jmethodID s_getKey;
    static jmethodID s_getPriority;
    static jmethodID s_getRef;

    static jclass s_jsonClass;
    static jmethodID s_jsonCtor;

};



#endif /* defined(__XCodePlugin__JniDataSnapshot__) */
