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
using UnityEngine;
using System.Collections;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

#if UNITY_ANDROID
internal class FirebaseAndroidImpl : QueryAndroidImpl, IFirebase
{
	static bool initialized = false;
	
	public FirebaseAndroidImpl (string path)
		: base(CreateNativeFirebase(path))
	{
	}

	public FirebaseAndroidImpl (AndroidJavaObject nativeReference)
		:base(nativeReference)
	{
	}
	
	private static AndroidJavaObject CreateNativeFirebase (string path)
	{
		if (!initialized) {
			try {
				initialized = true;
#if UNITY_ANDROID
				Debug.Log ("Firebase: attempting to initialize");
				AndroidJavaClass unityPlayer = new AndroidJavaClass ("com.unity3d.player.UnityPlayer"); 
				AndroidJavaObject currentContext = unityPlayer.GetStatic<AndroidJavaObject> ("currentActivity");
				AndroidJavaClass firebaseClass = new AndroidJavaClass ("com.firebase.client.Firebase"); 
				firebaseClass.CallStatic ("setAndroidContext", currentContext);
				Debug.Log ("Firebase: set Context!");
#endif
			} catch (Exception e) {
				Debug.Log (e.ToString ());
			}
		}
		return new AndroidJavaObject ("com.firebase.client.Firebase", path);
	}
	
	public IFirebase Child (string name)
	{
		return new FirebaseAndroidImpl(GetJavaObject().Call<AndroidJavaObject>("child", name));
	}
	
	public IFirebase Parent 
	{
		get {
			return new FirebaseAndroidImpl (GetJavaObject ().Call<AndroidJavaObject> ("getParent"));
		}
	}
	
	public IFirebase Root
	{
		get {
			return new FirebaseAndroidImpl (GetJavaObject ().Call<AndroidJavaObject> ("getRoot"));
		}
	}
	
	public string Key
	{
		get {
			return GetJavaObject ().Call<string> ("getKey");
		}
	}
	
	public IFirebase Push ()
	{
		return new FirebaseAndroidImpl(GetJavaObject().Call<AndroidJavaObject>("push"));
	}
	
	public void SetValue (string value)
	{
		GetJavaObject().Call ("setValue", value);
	}
	
	public void SetValue (string value, string priority, Action<FirebaseError, IFirebase> listener)
	{
		GetJavaObject().Call ("setValue", value, priority, new CompletionListener(listener));
	}
	
	public void SetValue (float value)
	{
		GetJavaObject().Call ("setValue", new AndroidJavaObject ("java.lang.Float", value));
	}

	public void SetValue (IDictionary<string, object> value) {
		string jsonString = MiniJSON.Json.Serialize (value);
		AndroidJavaObject jsonObject = GetObjectMapper ().Call<AndroidJavaObject> ("readValue", jsonString, GetObjectClass ());
		GetJavaObject().Call ("setValue", jsonObject);
	}
	
	public void SetValue (float value, string priority, Action<FirebaseError, IFirebase> listener)
	{
		GetJavaObject().Call ("setValue", new AndroidJavaObject ("java.lang.Float", value), priority, new CompletionListener(listener));
	}
	
	public void SetPriority (string priority)
	{
		GetJavaObject().Call ("setPriority", priority);
	}
	
	public void SetPriority (string priority, Action<FirebaseError,IFirebase> listener)
	{
		GetJavaObject().Call ("setPriority", priority, new CompletionListener(listener));
	}

	#region IFirebase implementation
	public void AuthWithCustomToken (string token, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		GetJavaObject ().Call ("authWithCustomToken", token, new AuthResultListener (onSuccess, onError));
	}

	public void AuthAnonymously (Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		GetJavaObject ().Call ("authAnonymously", new AuthResultListener (onSuccess, onError));
	}

	public void AuthWithPassword (string email, string password, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		GetJavaObject ().Call ("authWithPassword", email, password, new AuthResultListener (onSuccess, onError));
	}

	public void AuthWithOAuthToken (string provider, string token, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		GetJavaObject ().Call ("authWithOAuthToken", provider, token, new AuthResultListener (onSuccess, onError));
	}

	public void UnAuth ()
	{
		GetJavaObject ().Call ("unauth");
	}

	public AuthData Auth {
		get {
			string token = null;
			string uid = null;
			long expiration = 0;
			AndroidJavaObject authData = GetJavaObject().Call<AndroidJavaObject>("getAuth");

			if (authData != null) {
				token = authData.Call<string>("getToken");
				uid = authData.Call<string>("getUid");
				expiration = authData.Call<long>("getExpires");
			}

			return new AuthData(token, uid, expiration);
		}
	}
	#endregion	
	
	class CompletionListener : AndroidJavaProxy {
		private Action<FirebaseError,IFirebase> completionListener;
		
		public CompletionListener(Action<FirebaseError,IFirebase> listener)
			:base("com.firebase.client.Firebase$CompletionListener")
		{
			completionListener = listener;
		}
		
		void onComplete(AndroidJavaObject error, AndroidJavaObject reference) {
			if (completionListener != null) {
				completionListener (null, new FirebaseAndroidImpl (reference));
			}
		} 
	}

	class AuthResultListener : AndroidJavaProxy {
		Action<AuthData> onSuccess;
		Action<FirebaseError> onError;

		public AuthResultListener(Action<AuthData> onSuccess, Action<FirebaseError> onError)
			:base("com.firebase.client.Firebase$AuthResultHandler")
		{
			this.onError = onError;
			this.onSuccess = onSuccess;
		}

	    void onAuthenticated(AndroidJavaObject authData) {
			string token = null;
			string uid = null;
			long expiration = 0;
			if (authData != null) {
				token = authData.Call<string>("getToken");
				uid = authData.Call<string>("getUid");
				expiration = authData.Call<long>("getExpires");
			}
			if (onSuccess != null) {
				onSuccess(new AuthData( token, uid, expiration));
			}
		}
		

		void onAuthenticationError(AndroidJavaObject firebaseError) {
			FirebaseErrorAndroidImpl errorImpl = new FirebaseErrorAndroidImpl (firebaseError);
			if (onError != null) {
				onError(new FirebaseError( errorImpl.Code, 
      					errorImpl.Message, errorImpl.Details));
			}
		}
	}

	public class Factory : IFirebaseFactory
	{
		#region IFirebaseFactory implementation
		public IFirebase TryCreate (string path)
		{
			if (Application.platform == RuntimePlatform.Android) {
				return new FirebaseAndroidImpl (path);
			}

			return null;
		}
		#endregion
	}
}
#endif
