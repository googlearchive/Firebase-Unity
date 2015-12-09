//
//  JniFirebaseError.cpp
//  XCodePlugin
//
//  Created by benwu on 9/11/15.
//
//

#include "JniFirebaseError.h"

jclass JniFirebaseError::s_errorClass = NULL;
jmethodID JniFirebaseError::s_getCode = NULL;
jmethodID JniFirebaseError::s_getMessage = NULL;
jmethodID JniFirebaseError::s_getDetails = NULL;

JniFirebaseError::JniFirebaseError(jobject errorObject) {
    auto env = getEnv();
    if (!GetClass(env, "com/firebase/client/FirebaseError", &s_errorClass)) {
        return;
    }
    if (errorObject != NULL) {
        m_error = env->NewGlobalRef(errorObject);
    }
    else {
        m_error = NULL;
    }
}

JniFirebaseError::~JniFirebaseError() {
    auto env = getEnv();
    if (env != NULL && m_error != NULL) {
        env->DeleteGlobalRef((jobject)m_error);
    }
    m_error = NULL;
}

int JniFirebaseError::GetCode() {
    auto env = getEnv();
    if (m_error == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_errorClass, "getCode", "()I",
                   &s_getCode)) {
        return NULL;
    }
    jint code = env->CallIntMethod(m_error, s_getCode);
    return code;
}

const char* JniFirebaseError::GetErrorMessage() {
    auto env = getEnv();
    if (m_error == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_errorClass, "getMessage", "()Ljava/lang/String;",
                   &s_getMessage)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_error, s_getMessage));
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

const char* JniFirebaseError::GetDetails() {
    auto env = getEnv();
    if (m_error == NULL) {
        return NULL;
    }
    if (!GetMethod(env, s_errorClass, "getDetails", "()Ljava/lang/String;",
                   &s_getDetails)) {
        return NULL;
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(m_error, s_getDetails));
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


