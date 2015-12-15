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
//  jnistub_ValueEventListenerStub.cpp
//  XCodePlugin
//
//  Created by benwu on 8/24/15.
//
//

#include <stdio.h>
#include "jnistub_ValueEventListenerStub.h"
#include "JniDataSnapshot.h"
#include "plugin.pch"
#include "JniEventProcessor.h"
#include "JniEvent.h"
#include "JniFirebaseError.h"

/*
 * Class:     jnistub_ValueEventListenerStub
 * Method:    onDataChange
 * Signature: (JLcom/firebase/client/DataSnapshot;)V
 */
JNIEXPORT void JNICALL Java_jnistub_ValueEventListenerStub_onDataChange
(JNIEnv *, jobject, jlong cookie, jobject snapshot) {
    JniEventProcessor::GetInstance()->EnqueueEvent( new ValueChangedEvent(
                                                      new JniDataSnapshot(snapshot, (void*)cookie, JniDataSnapshot::ValueChanged)));
}

/*
 * Class:     jnistub_ValueEventListenerStub
 * Method:    onCancelled
 * Signature: (Lcom/firebase/client/FirebaseError;)V
 */
JNIEXPORT void JNICALL Java_jnistub_ValueEventListenerStub_onCancelled
(JNIEnv *, jobject, jlong cookie, jobject firebaseError) {
    JniFirebaseError error = JniFirebaseError(firebaseError);
    JniEventProcessor::GetInstance()->EnqueueEvent(
                                                   new ErrorEvent((void*)cookie, error.GetCode(),
                                                                  error.GetErrorMessage(), error.GetDetails()));
    
}
