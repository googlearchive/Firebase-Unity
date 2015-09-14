//
//  jnistub_AuthCustomTokenStub.cpp
//  XCodePlugin
//
//  Created by benwu on 9/10/15.
//
//

#include <stdio.h>
#include "jnistub_AuthCustomTokenStub.h"
#include "JniAuth.h"
#include "JniEventProcessor.h"
#include "JniFirebaseError.h"

/*
 * Class:     jnistub_AuthCustomTokenStub
 * Method:    onAuthenticated
 * Signature: (JLcom/firebase/client/AuthData;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthCustomTokenStub_onAuthenticated
(JNIEnv *, jobject, jlong cookie, jobject authData) {
    JniAuth auth = JniAuth(authData);
    JniEventProcessor::GetInstance()->EnqueueEvent(
                                                   new AuthSuccessEvent(cookie, auth.GetAuthToken(),
                                                                        auth.GetAuthUid(), auth.GetAuthExpiration()));
}

/*
 * Class:     jnistub_AuthCustomTokenStub
 * Method:    onAuthenticationError
 * Signature: (JLcom/firebase/client/FirebaseError;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthCustomTokenStub_onAuthenticationError
    (JNIEnv *, jobject, jlong cookie, jobject firebaseError) {
        JniFirebaseError error = JniFirebaseError(firebaseError);
        JniEventProcessor::GetInstance()->EnqueueEvent(
                                                       new AuthFailureEvent(cookie, error.GetCode(),
                                                                            error.GetMessage(), error.GetDetails()));
   
}
