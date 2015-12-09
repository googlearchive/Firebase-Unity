//
//  jnistub_AuthPasswordStub.cpp
//  XCodePlugin
//
//  Created by benwu on 9/10/15.
//
//

#include <stdio.h>
#include "jnistub_AuthPasswordStub.h"
#include "JniAuth.h"
#include "JniEventProcessor.h"
#include "JniFirebaseError.h"

/*
 * Class:     jnistub_AuthPasswordStub
 * Method:    onAuthenticated
 * Signature: (JLcom/firebase/client/AuthData;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthPasswordStub_onAuthenticated
(JNIEnv *, jobject, jlong cookie, jobject authData) {
    JniAuth auth = JniAuth(authData);
    JniEventProcessor::GetInstance()->EnqueueEvent(
                                                   new AuthSuccessEvent(cookie, auth.GetAuthToken(),
                                                                        auth.GetAuthUid(), auth.GetAuthExpiration()));
}

/*
 * Class:     jnistub_AuthPasswordStub
 * Method:    onAuthenticationError
 * Signature: (JLcom/firebase/client/FirebaseError;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthPasswordStub_onAuthenticationError
    (JNIEnv *, jobject, jlong cookie, jobject firebaseError) {
        JniFirebaseError error = JniFirebaseError(firebaseError);
        JniEventProcessor::GetInstance()->EnqueueEvent(
                                                       new AuthFailureEvent(cookie, error.GetCode(),
                                                                            error.GetErrorMessage(), error.GetDetails()));
}