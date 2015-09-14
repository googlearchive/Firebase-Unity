using UnityEngine;
using System.Collections;
using System;

#if UNITY_ANDROID
internal class FirebaseErrorAndroidImpl  {
	object firebaseErrorRef;
	
	public FirebaseErrorAndroidImpl(object nativeReference) {
		if (nativeReference == null) {
			throw new System.ArgumentNullException ("reference");
		}
		firebaseErrorRef = nativeReference;
	}
	
	
	private AndroidJavaObject GetJavaObject() {
		return (AndroidJavaObject)firebaseErrorRef;
	}
	
	
	public int Code {
		get {
			return GetJavaObject ().Call<int> ("getCode");
		}
	}
	
	public string Message {
		get {
			return GetJavaObject ().Call<string> ("getMessage");
		}
	}
	
	public string Details {
		get {
			return GetJavaObject ().Call<string> ("getDetails");
		}
	}
	
	public override string ToString() {
		return "FirebaseError: " + Message;
	}

}
#endif
