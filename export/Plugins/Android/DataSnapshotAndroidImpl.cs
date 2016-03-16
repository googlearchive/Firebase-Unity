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
using System.Collections.Generic;

#if UNITY_ANDROID
internal class DataSnapshotAndroidImpl : IDataSnapshot  {
	object snapshotNativeReference;
	
	public DataSnapshotAndroidImpl (object reference)
	{
		snapshotNativeReference = reference;
	}
	
	public IDataSnapshot Child (string path)
	{
		return new DataSnapshotAndroidImpl(getJavaObject().Call<AndroidJavaObject>("child", path));
	}
	
	private AndroidJavaObject getJavaObject() 
	{
		return (AndroidJavaObject)snapshotNativeReference;
	}

	public bool Exists
	{
		get 
		{
			return snapshotNativeReference != null && getJavaObject ().Call<bool> ("exists");
		}
	}
	
	public string Key
	{
		get 
		{
			return getJavaObject ().Call<string> ("getKey");
		}
	}
	
	public object Priority
	{
		get 
		{
			return getJavaObject ().Call<object> ("getPriority");
		}
	}

	public IFirebase Ref
	{
		get 
		{
			return new FirebaseAndroidImpl (getJavaObject ().Call<AndroidJavaObject> ("getRef"));
		}
	}
	
	public float FloatValue
	{
		get 
		{
			AndroidJavaObject javaObject = getJavaObject ().Call<AndroidJavaObject> ("getValue");
			return javaObject != null ? javaObject.Call<float> ("floatValue") : 0f;
		}
	}
	
	public string StringValue
	{
		get 
		{
			AndroidJavaObject javaObject = getJavaObject ().Call<AndroidJavaObject> ("getValue");
			return javaObject != null ? javaObject.Call<string> ("toString") : string.Empty;
		}
	}

	public Dictionary<string, object> DictionaryValue 
	{
		get 
		{
			AndroidJavaObject valueObject = getJavaObject ().Call<AndroidJavaObject> ("getValue");
			if (valueObject == null) 
			{
				return null;
			}
			AndroidJavaObject json = new AndroidJavaObject ("org.json.JSONObject", valueObject);
			return MiniJSON.Json.Deserialize (json.Call<string> ("toString")) as Dictionary<string,object>;
		}
	}

	public bool HasChild (string path)
	{
		return getJavaObject().Call<bool>("hasChild", path);
	}
}
#endif
