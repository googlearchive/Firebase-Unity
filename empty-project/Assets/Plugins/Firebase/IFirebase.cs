using UnityEngine;
using System.Collections;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

public interface IFirebase : IQuery
{
	IFirebase Child (string name);
	IFirebase Parent { get; }
	IFirebase Root { get; }
	string Key { get; }

	IFirebase Push ();
	void SetValue (string value);
	void SetValue (float value);
	void SetValue (IDictionary<string, object> value);
	void SetPriority (string priority);

	void AuthWithCustomToken(string token, Action<AuthData> onSuccess, Action<FirebaseError> onError);
	void AuthAnonymously(Action<AuthData> onSuccess, Action<FirebaseError> onError);
	void AuthWithPassword(string email, string password, Action<AuthData> onSuccess, Action<FirebaseError> onError);
	void AuthWithOAuthToken(string provider, string token, Action<AuthData> onSuccess, Action<FirebaseError> onError);

	void UnAuth();
	AuthData Auth { get; }
}
