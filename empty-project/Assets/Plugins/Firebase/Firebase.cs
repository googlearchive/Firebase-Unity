using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

public class Firebase  {
	private static IList<IFirebaseFactory> firebasePlatforms = new List<IFirebaseFactory>();

	private Firebase() {
	}

	static Firebase() {
#if UNITY_IOS
		Register (new FirebaseiOSImpl.Factory ());
#endif

#if UNITY_ANDROID 
		Register (new FirebaseAndroidImpl.Factory ());
#endif
	}

	public static void Register(IFirebaseFactory factory) {
		firebasePlatforms.Insert(0, factory);
	}
	
	public static IFirebase CreateNew(string path) {
		foreach (IFirebaseFactory factory in firebasePlatforms) {
			IFirebase firebase = factory.TryCreate(path);
			if (firebase != null) {
				return firebase;
			}
		}
		return null;
	}
}
