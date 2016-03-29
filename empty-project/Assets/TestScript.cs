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

public class TestScript : MonoBehaviour {
	IFirebase firebase;

	// Use this for initialization
	void Start () {
	    firebase = Firebase.CreateNew ("https://get-wrecked.firebaseio.com");


		firebase.ChildAdded += (object sender, FirebaseChangedEventArgs e) => {
			Debug.Log ("Child added with value: " + e.DataSnapshot.StringValue);
		};

		firebase.ChildRemoved += (object sender, FirebaseChangedEventArgs e) => {
			Debug.Log ("Child removed!");
		};

		//firebase.SetValue ("SetValue working?");
		Dictionary<string, object> testDictionary = new Dictionary<string, object>()
		{
			{ "key1", "value1"},
			{ "key2", "value2"},
			{ "key3", "value3"}
		};
		//firebase.SetValue(testDictionary);
		firebase.SetJsonValue("{\"example_child\":{\"child_working\" : true}}");
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
