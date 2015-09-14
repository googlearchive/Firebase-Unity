package jnistub;

import com.firebase.client.ChildEventListener;
import com.firebase.client.DataSnapshot;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;

/**
 * Created by benwu on 8/23/15.
 */
public class ChildEventListenerStub implements ChildEventListener {
    private long token;
    private Firebase firebase;

    public ChildEventListenerStub(Firebase reference, long refToken) {
        this.token = refToken;
        this.firebase = reference;
        this.firebase.addChildEventListener(this);
    }

    public void release() {
        this.firebase.removeEventListener(this);
    }

    @Override
    public void onChildAdded(DataSnapshot dataSnapshot, String previousChildName) {
        onChildAdded(token, dataSnapshot, previousChildName);
    }

    @Override
    public void onChildChanged(DataSnapshot dataSnapshot, String previousChildName) {
        onChildChanged(token, dataSnapshot, previousChildName);
    }

    @Override
    public void onChildRemoved(DataSnapshot dataSnapshot) {
        onChildRemoved(token, dataSnapshot);
    }

    @Override
    public void onChildMoved(DataSnapshot dataSnapshot, String previousChildName) {
        onChildMoved(token, dataSnapshot, previousChildName);
    }

    @Override
    public void onCancelled(FirebaseError firebaseError) {
        onCancelled(token, firebaseError);
    }

    public native void onChildAdded(long token, DataSnapshot dataSnapshot, String previousChildName);

    public native void onChildChanged(long token, DataSnapshot dataSnapshot, String previousChildName);

    public native void onChildRemoved(long token, DataSnapshot dataSnapshot);

    public native void onChildMoved(long token, DataSnapshot dataSnapshot, String previousChildName);

    public native void onCancelled(long token, FirebaseError firebaseError);

}
