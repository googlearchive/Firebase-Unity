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
package jnistub;

import com.firebase.client.DataSnapshot;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;
import com.firebase.client.ValueEventListener;

/**
 * Created by benwu on 8/23/15.
 */
public class ValueEventListenerStub implements ValueEventListener {
    private long token;
    private Firebase firebase;

    public ValueEventListenerStub(Firebase reference, long refToken) {
        this.token = refToken;
        this.firebase = reference;
        this.firebase.addValueEventListener(this);
    }

    public void release() {
        this.firebase.removeEventListener(this);
    }

    @Override
    public void onDataChange(DataSnapshot dataSnapshot) {
        onDataChange(token, dataSnapshot);
    }

    public native void onDataChange(long token, DataSnapshot dataSnapshot);

    @Override
    public void onCancelled(FirebaseError firebaseError) {
        onCancelled(token, firebaseError);
    }

    public native void onCancelled(long token, FirebaseError firebaseError);

}
