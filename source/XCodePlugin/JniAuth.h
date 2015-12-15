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
//  JniAuth.h
//  XCodePlugin
//
//  Created by benwu on 9/11/15.
//
//

#ifndef __XCodePlugin__JniAuth__
#define __XCodePlugin__JniAuth__

#include <stdio.h>
#include "JniHelper.h"

class JniAuth {
public:
    JniAuth(jobject authobject);
    ~JniAuth();
    
    const char* GetAuthToken();
    const char* GetAuthUid();
	uint64_t GetAuthExpiration();
    
private:
    jobject m_auth;
    
    static jclass s_authClass;
    static jmethodID s_authGetToken;
    static jmethodID s_getUid;
    static jmethodID s_getExpires;
};

#endif /* defined(__XCodePlugin__JniAuth__) */
