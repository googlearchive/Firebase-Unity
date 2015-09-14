//
//  JniChildEventStub.h
//  XCodePlugin
//
//  Created by benwu on 8/26/15.
//
//

#ifndef __XCodePlugin__JniChildEventStub__
#define __XCodePlugin__JniChildEventStub__

#include <stdio.h>
#include "JniFirebase.h"
#include "jni.h"


class JniChildEventStub {
public:
    JniChildEventStub(JniFirebase* firebase, jlong cookie);
    ~JniChildEventStub();
    
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


#endif /* defined(__XCodePlugin__JniChildEventStub__) */
