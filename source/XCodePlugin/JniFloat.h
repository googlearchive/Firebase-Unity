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
