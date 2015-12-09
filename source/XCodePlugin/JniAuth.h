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
