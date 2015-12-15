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
//  jnistub_AuthOAuthStub.cpp
//  XCodePlugin
//
//  Created by benwu on 9/10/15.
//
//

#include <stdio.h>
#include "jnistub_AuthOAuthStub.h"
#include "JniAuth.h"
#include "JniEventProcessor.h"
#include "JniFirebaseError.h"

/*
 * Class:     jnistub_AuthOAuthStub
 * Method:    onAuthenticated
 * Signature: (JLcom/firebase/client/AuthData;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthOAuthStub_onAuthenticated
(JNIEnv *, jobject, jlong cookie, jobject authData) {
    JniAuth auth = JniAuth(authData);
    JniEventProcessor::GetInstance()->EnqueueEvent(
                                                   new AuthSuccessEvent(cookie, auth.GetAuthToken(),
                                                                        auth.GetAuthUid(), auth.GetAuthExpiration()));
}

/*
 * Class:     jnistub_AuthOAuthStub
 * Method:    onAuthenticationError
 * Signature: (JLcom/firebase/client/FirebaseError;)V
 */
JNIEXPORT void JNICALL Java_jnistub_AuthOAuthStub_onAuthenticationError
    (JNIEnv *, jobject, jlong cookie, jobject firebaseError) {
        JniFirebaseError error = JniFirebaseError(firebaseError);
        JniEventProcessor::GetInstance()->EnqueueEvent(
                                                       new AuthFailureEvent(cookie, error.GetCode(),
                                                                            error.GetErrorMessage(), error.GetDetails()));
}
