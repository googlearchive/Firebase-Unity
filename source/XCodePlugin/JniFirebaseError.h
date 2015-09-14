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
    const char* GetMessage();
    const char* GetDetails();
    
private:
    jobject m_error;
    
    static jclass s_errorClass;
    static jmethodID s_getCode;
    static jmethodID s_getMessage;
    static jmethodID s_getDetails;
};

#endif /* defined(__XCodePlugin__JniFirebaseError__) */
