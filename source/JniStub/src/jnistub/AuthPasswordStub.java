package jnistub;

import com.firebase.client.AuthData;
import com.firebase.client.Firebase;
import com.firebase.client.FirebaseError;

/**
 * Created by benwu on 9/10/15.
 */
public class AuthPasswordStub {
    public AuthPasswordStub(Firebase ref, final long callbackToken, String email, String password) {
        ref.authWithPassword(email, password, new Firebase.AuthResultHandler() {
            @Override
            public void onAuthenticated(AuthData authData) {
                AuthPasswordStub.this.onAuthenticated(callbackToken, authData);
            }

            @Override
            public void onAuthenticationError(FirebaseError firebaseError) {
                AuthPasswordStub.this.onAuthenticationError(callbackToken, firebaseError);
            }
        });
    }

    public native void onAuthenticated(long token, AuthData authData);

    public native void onAuthenticationError(long token, FirebaseError firebaseError);
}
