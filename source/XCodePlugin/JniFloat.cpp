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
//  JniFloat.cpp
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#include "JniFloat.h"
#include "JniHelper.h"

jclass JniFloat::s_floatClass = NULL;
const char* JniFloat::s_floatClassName = "java/lang/Float";

jmethodID JniFloat::s_floatCtor = NULL;
const char* JniFloat::s_floatCtorName = "<init>";
const char* JniFloat::s_floatCtorSig = "(F)V";

jmethodID JniFloat::s_value = NULL;
const char* JniFloat::s_valueName = "floatValue";
const char* JniFloat::s_vaueSig = "()F";

JniFloat::JniFloat(float value) {
    auto env = getEnv();
    
    if (!GetClass(env, s_floatClassName, &s_floatClass)) {
        m_floatObject = NULL;
        return;
    }
    if (!GetMethod(env, s_floatClass, s_floatCtorName, s_floatCtorSig, &s_floatCtor)) {
        m_floatObject = NULL;
        return;
    }
    
    JOBJECT localRef = JOBJECT(env, env->NewObject(s_floatClass, s_floatCtor, value ));
    m_floatObject = env->NewGlobalRef(localRef);
    m_env = NULL;
}

JniFloat::JniFloat(JNIEnv* env, float value) {
    if (!GetClass(env, s_floatClassName, &s_floatClass)) {
        m_floatObject = NULL;
        return;
    }
    if (!GetMethod(env, s_floatClass, s_floatCtorName, s_floatCtorSig, &s_floatCtor)) {
        m_floatObject = NULL;
        return;
    }
    
    m_floatObject = env->NewObject(s_floatClass, s_floatCtor, value );
    m_env = env;
}

JniFloat::JniFloat(jobject object) {
    auto env = getEnv();
    if (!GetClass(env, s_floatClassName, &s_floatClass)) {
        m_floatObject = NULL;
        return;
    }
    m_floatObject = env->NewGlobalRef(object);
}

JniFloat::JniFloat(JNIEnv* env, jobject object) {
    if (!GetClass(env, s_floatClassName, &s_floatClass)) {
        m_floatObject = NULL;
        return;
    }
    m_floatObject = object;
    m_env = env;
}

JniFloat::~JniFloat() {
    if (m_env == NULL) {
        auto env = getEnv();
        env->DeleteGlobalRef(m_floatObject);
    } else {
        m_env->DeleteLocalRef(m_floatObject);
    }
}

JniFloat::operator jobject() {
    return m_floatObject;
}

float JniFloat::Value(JNIEnv* env) {
    if (!m_floatObject) {
        return 0;
    }
    
    if (!GetClass(env, s_floatClassName, &s_floatClass)) {
        return 0;
    }
    
    if (!GetMethod(env, s_floatClass, s_valueName, s_vaueSig, &s_value)) {
        return 0;;
    }
    
    if (!env->IsSameObject(s_floatClass, (jclass)JCLASS(env, env->GetObjectClass(m_floatObject)))) {
        return 0;
    }
    
    return env->CallFloatMethod(m_floatObject, s_value);
}

float JniFloat::Value() {
    if (m_env!= NULL) {
        return Value(m_env);
    }
    else {
        return Value(getEnv());
    }
}

