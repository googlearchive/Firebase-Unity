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
