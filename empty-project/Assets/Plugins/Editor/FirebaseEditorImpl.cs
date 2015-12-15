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
﻿using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using System.Collections.Generic;
using AOT;
using System.Threading;

#if UNITY_EDITOR
using UnityEditor;

public class FirebaseEditorImpl : QueryEditorImpl, IFirebase {
	static bool initialized = false;
	static object auth_sync = new object ();
	static AuthCallInstanceEntry activeCallEntry;

	public FirebaseEditorImpl (IntPtr nativeReference)
		:base(nativeReference)
	{
	}

	public static FirebaseEditorImpl CreateNewFirebaseEditorImpl(IntPtr nativeReference) {
		return QueryEditorImpl.GetOrCreateCachedInstance (nativeReference, () => {
			return new FirebaseEditorImpl(nativeReference);
		});
	}
	
	private static IntPtr CreateNativeFirebase (string path)
	{
		return _FirebaseNew (path);
	}

	~FirebaseEditorImpl() {
		IntPtr nativeReference = GetEditorObject ();
		if (nativeReference != IntPtr.Zero) {
			_FirebaseRelease (nativeReference);
		}
	}

	#region Imports
	[DllImport ("FirebaseProxy")]
	private static extern IntPtr _FirebaseNew (string path);
	
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseRelease(IntPtr firebase);

	[DllImport ("FirebaseProxy")]
	private static extern IntPtr _FirebaseChild (IntPtr firebase, string path);
	
	[DllImport ("FirebaseProxy")]
	private static extern IntPtr _FirebaseParent (IntPtr firebase);
	
	[DllImport ("FirebaseProxy")]
	private static extern IntPtr _FirebaseRoot (IntPtr firebase);
	
	[DllImport ("FirebaseProxy")]
	private static extern string _FirebaseGetKey(IntPtr firebase);
	
	[DllImport ("FirebaseProxy")]
	private static extern IntPtr _FirebasePush (IntPtr firebase);
	
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseSetString (IntPtr firebase, string value);

	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseSetJson (IntPtr firebase, string json);

	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseSetFloat (IntPtr firebase, float value);
	
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseSetPriority (IntPtr firebase, string value);

	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseAuthWithCustomToken (IntPtr firebase, string token, long callback);
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseAuthAnonymously (IntPtr firebase, long callback);
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseAuthWithPassword (IntPtr firebase, string email,
	                                string password, long callback);
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseAuthWithOAuthToken (IntPtr firebase, string provider,
	                                  string token, long callback);
	
	[DllImport ("FirebaseProxy")]
	private static extern string _FirebaseGetAuthToken(IntPtr firebase);
	[DllImport ("FirebaseProxy")]
	private static extern string _FirebaseGetAuthUid(IntPtr firebase);
	[DllImport ("FirebaseProxy")]
	private static extern long _FirebaseGetAuthExpiration(IntPtr firebase);
	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseUnAuth(IntPtr firebase);
	
	#endregion

	#region IFirebase implementation
	public IFirebase Child (string name)
	{
		return CreateNewFirebaseEditorImpl (_FirebaseChild (GetEditorObject(), name));
	}

	public IFirebase Parent
	{
		get
		{
			return CreateNewFirebaseEditorImpl (_FirebaseParent (GetEditorObject ()));
		}
	}

	public IFirebase Root
	{
		get 
		{
			return CreateNewFirebaseEditorImpl (_FirebaseRoot (GetEditorObject ()));
		}
	}

	public string Key
	{
		get
		{
			return _FirebaseGetKey (GetEditorObject ());
		}
	}

	public IFirebase Push ()
	{
		return CreateNewFirebaseEditorImpl( _FirebasePush(GetEditorObject ()));
	}

	public void SetValue (string value)
	{
		_FirebaseSetString (GetEditorObject (), value);
	}

	public void SetValue (IDictionary<string, object> value) {
		string jsonString = MiniJSON.Json.Serialize (value);
		_FirebaseSetJson (GetEditorObject (), jsonString);
	}

	public void SetValue (float value)
	{
		_FirebaseSetFloat (GetEditorObject (), value);
	}

	public void SetPriority (string priority)
	{
		_FirebaseSetPriority (GetEditorObject (), priority);
	}
	
	public void AuthWithCustomToken (string token, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		lock (auth_sync) {
			long newnumber = activeCallEntry != null ? (activeCallEntry.Instance + 1) % long.MaxValue - 1 : 0;
			activeCallEntry = new AuthCallInstanceEntry() { Instance = newnumber, OnSuccess = onSuccess, OnError = onError};
			_FirebaseAuthWithCustomToken(GetEditorObject(), token, activeCallEntry.Instance);
		}
	}

	public void AuthAnonymously (Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		lock (auth_sync) {
			long newnumber = activeCallEntry != null ? (activeCallEntry.Instance + 1) % long.MaxValue - 1 : 0;
			activeCallEntry = new AuthCallInstanceEntry() { Instance = newnumber, OnSuccess = onSuccess, OnError = onError};
			_FirebaseAuthAnonymously(GetEditorObject(), activeCallEntry.Instance);
		}
	}

	public void AuthWithPassword (string email, string password, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		lock (auth_sync) {
			long newnumber = activeCallEntry != null ? (activeCallEntry.Instance + 1) % long.MaxValue - 1 : 0;
			activeCallEntry = new AuthCallInstanceEntry() { Instance = newnumber, OnSuccess = onSuccess, OnError = onError};
			_FirebaseAuthWithPassword(GetEditorObject(), email, password, activeCallEntry.Instance);
		}
	}

	public void AuthWithOAuthToken (string provider, string token, Action<AuthData> onSuccess, Action<FirebaseError> onError)
	{
		lock (auth_sync) {
			long newnumber = activeCallEntry != null ? (activeCallEntry.Instance + 1) % long.MaxValue - 1 : 0;
			activeCallEntry = new AuthCallInstanceEntry() { Instance = newnumber, OnSuccess = onSuccess, OnError = onError};
			_FirebaseAuthWithOAuthToken(GetEditorObject(), provider, token, activeCallEntry.Instance);
		}
	}

	public void UnAuth ()
	{
		_FirebaseUnAuth (GetEditorObject ());
	}

	public AuthData Auth {
		get {
			return new AuthData(_FirebaseGetAuthToken(GetEditorObject()), _FirebaseGetAuthUid(GetEditorObject()),
			                     _FirebaseGetAuthExpiration(GetEditorObject()));
		}
	}
	
	#endregion

	[DllImport ("FirebaseProxy")]
	private static extern void _FirebaseSetCallbacks(onValueChangedEventHandler valueChanged,
	                                                 onValueChangedEventHandler childAdded,
	                                                 onValueChangedEventHandler childRemoved,
	                                                 onValueChangedEventHandler childChanged,
	                                                 onValueChangedEventHandler childMoved,
	                                                 debugLogHandler debugLogger,
	                                                 onAuthSuccessHandler authSuccess,
	                                                 onAuthCancelHandler authFailure,
	                                                 onErrorHandler errorHandler);

	static onAuthSuccessHandler authSuccessHandler = new onAuthSuccessHandler(onAuthSuccess);
	static onAuthCancelHandler authFailureHandler = new onAuthCancelHandler(onAuthCancel);

	public static void Initialize() {
		if (!initialized) {
			initialized = true;
			_FirebaseSetCallbacks(valueChangedHandler, childAddedHandler, childRemovedHandler,
			                      childChangedHandler, childMovedHandler,  debugLog, 
			                      authSuccessHandler, authFailureHandler, errorHandler);
		}
	}

	public static void Shutdown() {
		if (initialized) {
			initialized = false;
			_FirebaseSetCallbacks(null, null, null, null, null, null, null, null, null);
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onAuthSuccessHandler))]
	static void onAuthSuccess(long reference, String token, String uid, long expiration) {
		Action<AuthData> callback = null;
		lock (auth_sync) {
			if (activeCallEntry != null && activeCallEntry.Instance == reference) {
				callback = activeCallEntry.OnSuccess;
				activeCallEntry = null;
			}
		}
		if (callback != null) {
			callback(new AuthData(token, uid, expiration));
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onAuthCancelHandler))]
	static void onAuthCancel(long reference, int code, String message, String details) {
		Action<FirebaseError> callback = null;
		lock (auth_sync) {
			if (activeCallEntry != null && activeCallEntry.Instance == reference) {
				callback = activeCallEntry.OnError;
				activeCallEntry = null;
			}
		}
		if (callback != null) {
			callback(new FirebaseError(code, message, details));
		}
	}

	public class Factory : IFirebaseFactory
	{
		#region IFirebaseFactory implementation
		public IFirebase TryCreate (string path)
		{
			return CreateNewFirebaseEditorImpl (CreateNativeFirebase(path));
		}
		#endregion
	}

	class AuthCallInstanceEntry {
			public long Instance { get; set; }
			public Action<AuthData> OnSuccess { get; set; }
			public Action<FirebaseError> OnError { get; set;}
	}
	
}
#endif
