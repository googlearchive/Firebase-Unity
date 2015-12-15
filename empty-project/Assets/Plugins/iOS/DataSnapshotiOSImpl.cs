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
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

#if UNITY_IOS
internal class DataSnapshotiOSImpl : IDataSnapshot {
	IntPtr dataSnapshotReference;

	public DataSnapshotiOSImpl(IntPtr dataSnapshotReference) {
		this.dataSnapshotReference = dataSnapshotReference;
	}

	private IntPtr GetSnapShotReference() {
		return dataSnapshotReference;
	}

	~DataSnapshotiOSImpl() {
		IntPtr nativeSnapshot = GetSnapShotReference ();
		if (nativeSnapshot != IntPtr.Zero) {
			_DataSnapshotRelease(nativeSnapshot);
		}
	}

	#region Imports
	[DllImport ("__Internal")]
	private static extern float _DataSnapshotGetFloatValue (IntPtr datasnapshot);
	
	[DllImport ("__Internal")]
	private static extern string _DataSnapshotGetStringValue (IntPtr datasnapshot);

	[DllImport ("__Internal")]
	private static extern IntPtr _DataSnapshotGetChild (IntPtr datasnapshot, string path);
	
	[DllImport ("__Internal")]
	private static extern IntPtr _DataSnapshotHasChild (IntPtr datasnapshot, string path);
	
	[DllImport ("__Internal")]
	private static extern IntPtr _DataSnapshotExists (IntPtr datasnapshot);
	
	[DllImport ("__Internal")]
	private static extern string _DataSnapshotGetKey (IntPtr datasnapshot);
	
	[DllImport ("__Internal")]
	private static extern string _DataSnapshotGetPriority (IntPtr datasnapshot);
	
	[DllImport ("__Internal")]
	private static extern IntPtr _DataSnapshotGetRef (IntPtr datasnapshot);

	[DllImport ("__Internal")]
	private static extern string _DataSnapshotGetDictionary (IntPtr datasnapshot);

	[DllImport ("__Internal")]
	private static extern void _DataSnapshotRelease (IntPtr datasnapshot);
	#endregion

	#region IDataSnapshot implementation
	public IDataSnapshot Child (string path)
	{
		return new DataSnapshotiOSImpl (_DataSnapshotGetChild (GetSnapShotReference (), path));
	}

	public bool Exists
	{
		get {
			return _DataSnapshotExists (GetSnapShotReference ()) != IntPtr.Zero;
		}
	}

	public string Key 
	{
		get {
			return _DataSnapshotGetKey (GetSnapShotReference ());
		}
	}

	public object Priority
	{
		get {
			return _DataSnapshotGetPriority (GetSnapShotReference ());
		}
	}

	public IFirebase Ref
	{
		get {
			return new FirebaseiOSImpl (_DataSnapshotGetRef (GetSnapShotReference ()));
		}
	}

	public float FloatValue
	{
		get {
			return _DataSnapshotGetFloatValue (GetSnapShotReference ());
		}
	}

	public string StringValue
	{
		get {
			return _DataSnapshotGetStringValue (GetSnapShotReference ());
		}
	}

	public Dictionary<string, object> DictionaryValue {
		get {
			string dictionaryJSON = _DataSnapshotGetDictionary (GetSnapShotReference ());
			if (String.IsNullOrEmpty (dictionaryJSON)) {
				return null;
			}
			return MiniJSON.Json.Deserialize (dictionaryJSON) as Dictionary<string,object>;
		}
	}

	public bool HasChild (string path)
	{
		return _DataSnapshotHasChild (GetSnapShotReference (), path) != IntPtr.Zero;
	}
	
	#endregion
}
#endif
