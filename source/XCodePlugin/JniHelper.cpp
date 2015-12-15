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
//  JniHelper.cpp
//  XCodePlugin
//
//  Created by benwu on 8/23/15.
//
//

#include <stdio.h>
#include <string>
#include <mutex>
#include "JniHelper.h"
#include "Plugin.pch"
#include "jnistub_ValueEventListenerStub.h"
#include "jnistub_ChildEventListenerStub.h"
#include "jnistub_AuthAnonymousStub.h"
#include "jnistub_AuthCustomTokenStub.h"
#include "jnistub_AuthOAuthStub.h"
#include "jnistub_AuthPasswordStub.h"
#if _WIN64
#include <Windows.h>
#endif

std::mutex g_lock;
JavaVM *g_jvm = NULL;

std::map<jclass, jmethodID> s_toStringMap;
std::mutex s_toStringlock;

const int NUM_ENTRIES = 10;
const char* classpathentries[] = {
    "firebase-client-jvm-2.3.1.jar",
    "jackson-core-2.6.0.jar",
    "commons-logging-1.1.1.jar",
    "jackson-databind-2.6.0.jar",
    "httpclient-4.0.1.jar",
    "httpcore-4.0.1.jar",
    "jackson-annotations-2.6.0.jar",
    "jnistub.jar",
    "json-20090211.jar",
    "tubesock-0.0.11.jar"};

JNINativeMethod g_valueChangedMethods[] = {
    {(char*)"onDataChange",    (char*)"(JLcom/firebase/client/DataSnapshot;)V",
        (void *)&Java_jnistub_ValueEventListenerStub_onDataChange},
    {(char*)"onCancelled",        (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_ValueEventListenerStub_onCancelled},
};

JNINativeMethod g_childChangedMethods[] = {
    {(char*)"onChildAdded",        (char*)"(JLcom/firebase/client/DataSnapshot;Ljava/lang/String;)V",
        (void *)&Java_jnistub_ChildEventListenerStub_onChildAdded},
    {(char*)"onChildChanged",        (char*)"(JLcom/firebase/client/DataSnapshot;Ljava/lang/String;)V",
        (void *)&Java_jnistub_ChildEventListenerStub_onChildChanged},
    {(char*)"onChildRemoved",        (char*)"(JLcom/firebase/client/DataSnapshot;)V",
        (void *)&Java_jnistub_ChildEventListenerStub_onChildRemoved},
    {(char*)"onChildMoved",        (char*)"(JLcom/firebase/client/DataSnapshot;Ljava/lang/String;)V",
        (void *)&Java_jnistub_ChildEventListenerStub_onChildMoved},
    {(char*)"onCancelled",        (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_ChildEventListenerStub_onCancelled},
};

JNINativeMethod g_authAnonMethods[] = {
    {(char*)"onAuthenticated",    (char*)"(JLcom/firebase/client/AuthData;)V",
        (void *)&Java_jnistub_AuthAnonymousStub_onAuthenticated},
    {(char*)"onAuthenticationError",  (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_AuthAnonymousStub_onAuthenticationError},
};
JNINativeMethod g_authCustomTokenMethods[] = {
    {(char*)"onAuthenticated",    (char*)"(JLcom/firebase/client/AuthData;)V",
        (void *)&Java_jnistub_AuthCustomTokenStub_onAuthenticated},
    {(char*)"onAuthenticationError",  (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_AuthCustomTokenStub_onAuthenticationError},
};
JNINativeMethod g_authOAuthMethods[] = {
    {(char*)"onAuthenticated",    (char*)"(JLcom/firebase/client/AuthData;)V",
        (void *)&Java_jnistub_AuthOAuthStub_onAuthenticated},
    {(char*)"onAuthenticationError",  (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_AuthOAuthStub_onAuthenticationError},
};
JNINativeMethod g_authPasswordMethods[] = {
    {(char*)"onAuthenticated",    (char*)"(JLcom/firebase/client/AuthData;)V",
        (void *)&Java_jnistub_AuthPasswordStub_onAuthenticated},
    {(char*)"onAuthenticationError",  (char*)"(JLcom/firebase/client/FirebaseError;)V",
        (void *)&Java_jnistub_AuthPasswordStub_onAuthenticationError},
};

JNIEnv* getEnv() {
    JNIEnv *env;
    
    if (g_jvm == NULL){
        lock<std::mutex> lock(g_lock);
        if (g_jvm == NULL) {
            JavaVMInitArgs vm_args;
            JavaVMOption* options = new JavaVMOption[1];
#if _WIN64
            //classpath WIN64
            char path[MAX_PATH];
            HMODULE hm = NULL;

            if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)&_FirebaseSetString,
                &hm))
            {
                int ret = GetLastError();
                fprintf(stderr, "GetModuleHandle returned %d\n", ret);
                return NULL;
            }
            GetModuleFileNameA(hm, path, sizeof(path));

            char* lastFolder = strrchr(path, '\\');
            int pathsize = (int)(lastFolder - path + 1);
            if (pathsize <= 0) {
               return NULL;
            }

            int bufSize = (FILENAME_MAX + pathsize + 1) * NUM_ENTRIES + 18;
            char* classpath = new char[bufSize];
            *classpath = '\0';
            strcat_s(classpath, bufSize, "-Djava.class.path=");
            for(int i = 0; i < NUM_ENTRIES; i++) {
               strncat_s(classpath, bufSize, path, pathsize);
               strcat_s(classpath, bufSize, classpathentries[i]);
               if (i != NUM_ENTRIES - 1) {
                strcat_s(classpath, bufSize, ";");
               }
            }

            options[0].optionString = classpath;
#else
            Dl_info info;
            if (!dladdr((void*)_FirebaseSetString, &info)) {
                return NULL;
            }
            char* lastFolder = strrchr(info.dli_fname, '/');
            int pathsize = (lastFolder - info.dli_fname + 1);
            if (pathsize <= 0) {
                return NULL;
            }

            char* classpath = new char[(FILENAME_MAX + pathsize + 1) * NUM_ENTRIES + 18];
            *classpath = '\0';
            classpath = strcat(classpath, "-Djava.class.path=");
            for(int i = 0; i < NUM_ENTRIES; i++) {
                classpath = strncat(classpath, info.dli_fname, pathsize);
                classpath = strcat(classpath, classpathentries[i]);
                if (i != NUM_ENTRIES - 1) {
                    classpath = strcat(classpath, ":");
                }
            }

            options[0].optionString = classpath;
#endif

            vm_args.version = JNI_VERSION_1_6;
            vm_args.nOptions = 1;
            vm_args.options = options;
            vm_args.ignoreUnrecognized = false;
            
            jint rc = JNI_CreateJavaVM(&g_jvm, (void**)&env, &vm_args);
            delete options;
            delete classpath;
            if (rc != 0) {
                return NULL;
            }
            
            jclass valueChangedStubClass = env->FindClass("jnistub/ValueEventListenerStub");
            if (valueChangedStubClass == NULL) {
                return NULL;
            }
            env->RegisterNatives(valueChangedStubClass, g_valueChangedMethods,
                                 sizeof(g_valueChangedMethods)/sizeof(g_valueChangedMethods[0]));
            
            jclass childChangedStubClass = env->FindClass("jnistub/ChildEventListenerStub");
            if (childChangedStubClass == NULL) {
                return NULL;
            }
            env->RegisterNatives(childChangedStubClass, g_childChangedMethods,
                                 sizeof(g_childChangedMethods)/sizeof(g_childChangedMethods[0]));
            
            jclass authAnon = env->FindClass("jnistub/AuthAnonymousStub");
            if (authAnon == NULL) {
                return NULL;
            }
            env->RegisterNatives(authAnon, g_authAnonMethods,
                                 sizeof(g_authAnonMethods)/sizeof(g_authAnonMethods[0]));
            
            jclass authCustom = env->FindClass("jnistub/AuthCustomTokenStub");
            if (authCustom == NULL) {
                return NULL;
            }
            env->RegisterNatives(authCustom, g_authCustomTokenMethods,
                                 sizeof(g_authCustomTokenMethods)/sizeof(g_authCustomTokenMethods[0]));
            
            jclass authOAuth = env->FindClass("jnistub/AuthOAuthStub");
            if (authOAuth == NULL) {
                return NULL;
            }
            env->RegisterNatives(authOAuth, g_authOAuthMethods,
                                 sizeof(g_authOAuthMethods)/sizeof(g_authOAuthMethods[0]));
            
            jclass authPassword = env->FindClass("jnistub/AuthPasswordStub");
            if (authPassword == NULL) {
                return NULL;
            }
            env->RegisterNatives(authPassword, g_authPasswordMethods,
                                 sizeof(g_authPasswordMethods)/sizeof(g_authPasswordMethods[0]));
            
            return env;
        }
    }
    
    int status = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(status < 0) {
        status = g_jvm->AttachCurrentThread((void**)&env, NULL);
        if(status < 0) {
            DebugLog("FirebasePlugin: could not attach thread to jvm");
            return NULL;
        }
    }
    return env;
}

const char* CallToString(JNIEnv* env, jobject localRef) {
    if (localRef == NULL) {
        return NULL;
    }
    JCLASS clazz = JCLASS(env, env->GetObjectClass(localRef));
    jmethodID toStringofClazz = NULL;
    {
        lock<std::mutex> lock(s_toStringlock);
        std::map<jclass, jmethodID>::iterator it = s_toStringMap.find((jclass)clazz);
        if (it == s_toStringMap.end()) {
            if (!GetMethod(env, (jclass)clazz, "toString", "()Ljava/lang/String;", &toStringofClazz)) {
                return NULL;
            }
            s_toStringMap.insert(std::map<jclass, jmethodID>::value_type((jclass)clazz, toStringofClazz));
        }
        else {
            toStringofClazz = it->second;
        }
    }
    JSTRING java_string = JSTRING(env, (jstring)env->CallObjectMethod(localRef, toStringofClazz));
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

jclass throwable_class = NULL;
jclass frame_class = NULL;
jmethodID mid_throwable_getCause = NULL;
jmethodID mid_throwable_getStackTrace = NULL;
jmethodID mid_throwable_toString = NULL;
jmethodID mid_frame_toString = NULL;

void _append_exception_trace_messages(
                                      JNIEnv&      a_jni_env,
                                      std::string& a_error_msg,
                                      jthrowable   a_exception,
                                      jmethodID    a_mid_throwable_getCause,
                                      jmethodID    a_mid_throwable_getStackTrace,
                                      jmethodID    a_mid_throwable_toString,
                                      jmethodID    a_mid_frame_toString)
{
    a_error_msg.clear();
    
    // Get the array of StackTraceElements.
    jobjectArray frames =
    (jobjectArray) a_jni_env.CallObjectMethod(
                                              a_exception,
                                              a_mid_throwable_getStackTrace);
    jsize frames_length = a_jni_env.GetArrayLength(frames);
    
    // Add Throwable.toString() before descending
    // stack trace messages.
    if (0 != frames)
    {
        jstring msg_obj =
        (jstring) a_jni_env.CallObjectMethod(a_exception,
                                             a_mid_throwable_toString);
        const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);
        
        // If this is not the top-of-the-trace then
        // this is a cause.
        if (!a_error_msg.empty())
        {
            a_error_msg += "\nCaused by: ";
            a_error_msg += msg_str;
        }
        else
        {
            a_error_msg = msg_str;
        }
        
        a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
        a_jni_env.DeleteLocalRef(msg_obj);
    }
    
    // Append stack trace messages if there are any.
    if (frames_length > 0)
    {
        jsize i = 0;
        for (i = 0; i < frames_length; i++)
        {
            // Get the string returned from the 'toString()'
            // method of the next frame and append it to
            // the error message.
            jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
            jstring msg_obj =
            (jstring) a_jni_env.CallObjectMethod(frame,
                                                 a_mid_frame_toString);
            
            const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);
            
            a_error_msg += "\n    ";
            a_error_msg += msg_str;
            
            a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
            a_jni_env.DeleteLocalRef(msg_obj);
            a_jni_env.DeleteLocalRef(frame);
        }
    }
    
    // If 'a_exception' has a cause then append the
    // stack trace messages from the cause.
    if (0 != frames)
    {
        jthrowable cause =
        (jthrowable) a_jni_env.CallObjectMethod(
                                                a_exception,
                                                a_mid_throwable_getCause);
        if (0 != cause)
        {
            _append_exception_trace_messages(a_jni_env,
                                             a_error_msg,
                                             cause,
                                             a_mid_throwable_getCause,
                                             a_mid_throwable_getStackTrace,
                                             a_mid_throwable_toString,
                                             a_mid_frame_toString);
        }
    }
}

std::string error_msg;
const char* GetJNIExceptionDescription(JNIEnv* env, jthrowable exception ) {
    if (!GetClass(env, "java/lang/Throwable", &throwable_class)) {
        return NULL;
    }
    if (!GetMethod(env, throwable_class, "getCause", "()Ljava/lang/Throwable;", &mid_throwable_getCause)) {
        return NULL;
    }
    if (!GetMethod(env, throwable_class, "getStackTrace", "()[Ljava/lang/StackTraceElement;",
                   &mid_throwable_getStackTrace)) {
        return NULL;
    }
    if (!GetMethod(env, throwable_class, "toString", "()Ljava/lang/String;",
                   &mid_throwable_toString)) {
        return NULL;
    }
    if (!GetClass(env, "java/lang/StackTraceElement", &frame_class)) {
        return NULL;
    }
    if (!GetMethod(env, frame_class, "toString", "()Ljava/lang/String;",
                   &mid_frame_toString)) {
        return NULL;
    }
    
    _append_exception_trace_messages(*env,
                                     error_msg,
                                     exception,
                                     mid_throwable_getCause,
                                     mid_throwable_getStackTrace,
                                     mid_throwable_toString,
                                     mid_frame_toString);
    return error_msg.c_str();
}

void DebugLog(const char *message) {
    if (g_debugLog != NULL) {
        g_debugLog(message);
    }
}

