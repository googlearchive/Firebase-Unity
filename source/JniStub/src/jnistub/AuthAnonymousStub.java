package jnistub;

import com.firebase.client.AuthData;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;

/**
 * Created by benwu on 9/10/15.
 */
public class AuthAnonymousStub {
    public AuthAnonymousStub(Firebase ref, final long callbackToken) {
        ref.authAnonymously(new Firebase.AuthResultHandler() {
            @Override
            public void onAuthenticated(AuthData authData) {
                AuthAnonymousStub.this.onAuthenticated(callbackToken, authData);
            }

            @Override
            public void onAuthenticationError(FirebaseError firebaseError) {
                AuthAnonymousStub.this.onAuthenticationError(callbackToken, firebaseError);
            }
        });

    }

    public native void onAuthenticated(long token, AuthData authData);

    public native void onAuthenticationError(long token, FirebaseError firebaseError);

}
