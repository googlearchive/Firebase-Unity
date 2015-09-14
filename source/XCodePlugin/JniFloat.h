//
//  JniFloat.h
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#ifndef __XCodePlugin__JniFloat__
#define __XCodePlugin__JniFloat__

#include <memory>
#include <stdio.h>
#include "jni.h"

class JniFloat {
public:
    JniFloat(float value);
    JniFloat(jobject object);
    
    JniFloat(JNIEnv* env, float value);
    JniFloat(JNIEnv* env, jobject object);
    
    ~JniFloat();

    operator jobject();
    float Value();

private:
    jobject m_floatObject;
    JNIEnv* m_env;
    
    static jclass s_floatClass;
    static const char* s_floatClassName;
    
    static jmethodID s_floatCtor;
    static const char* s_floatCtorName;
    static const char* s_floatCtorSig;
    
    static jmethodID s_value;
    static const char* s_valueName;
    static const char* s_vaueSig;
    
    float Value(JNIEnv*);
};
#endif /* defined(__XCodePlugin__JniFloat__) */
