//
//  JniValueEventStub.h
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#ifndef __XCodePlugin__JniValueEventStub__
#define __XCodePlugin__JniValueEventStub__

#include <stdio.h>
#include "jni.h"
#include "JniFirebase.h"

class JniValueEventStub {
public:
    JniValueEventStub(JniFirebase* firebase, jlong cookie);
    ~JniValueEventStub();

    jobject GetStub() {
        return m_stub;
    }
    
private:
    jobject m_stub;

    static jclass s_stubClass;
    static const char* s_stubClassName;
    
    static jmethodID s_stubCtor;
    static const char* s_stubCtorName;
    static const char* s_stubCtorSig;
    
    static jmethodID s_stubRelease;
    static const char* s_stubReleaseName;
    static const char* s_stubReleaseSig;
};


#endif /* defined(__XCodePlugin__JniValueEventStub__) */
