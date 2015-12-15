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
//  JniFirebaseError.h
//  XCodePlugin
//
//  Created by benwu on 9/11/15.
//
//

#ifndef __XCodePlugin__JniFirebaseError__
#define __XCodePlugin__JniFirebaseError__

#include <stdio.h>
#include "JniHelper.h"

class JniFirebaseError {
public:
    JniFirebaseError(jobject errorObject);
    ~JniFirebaseError();
    
    int GetCode();
    const char* GetErrorMessage();
    const char* GetDetails();
    
private:
    jobject m_error;
    
    static jclass s_errorClass;
    static jmethodID s_getCode;
    static jmethodID s_getMessage;
    static jmethodID s_getDetails;
};

#endif /* defined(__XCodePlugin__JniFirebaseError__) */
