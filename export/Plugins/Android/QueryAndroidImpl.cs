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

#if UNITY_ANDROID
internal class QueryAndroidImpl : IQuery {
	AndroidJavaObject queryRef;
	EventHandler<FirebaseChangedEventArgs> valueUpdatedEvent, childAddedEvent, 
									childRemovedEvent, childChangedEvent, childMovedEvent;
	ValueEventListener valueupdateListener;
	ChildEventListener childListener;
	static AndroidJavaObject objectmapper;
	static AndroidJavaClass objectclass;

	protected QueryAndroidImpl(AndroidJavaObject nativeReference) {
		queryRef = nativeReference;
	}
	
	protected AndroidJavaObject GetJavaObject() {
		return queryRef;
	}

	internal static AndroidJavaObject GetObjectMapper() {
		// it isn't critical that this is protected against multithreaded access.
		if (objectmapper == null) {
			objectmapper = new AndroidJavaObject ("com.fasterxml.jackson.databind.ObjectMapper");
		}
		return objectmapper;
	}

	internal static AndroidJavaClass GetObjectClass() {
		if (objectclass == null) {
			objectclass = new AndroidJavaClass("java.lang.Object");
		}
		return objectclass;
	}

	public event EventHandler<FirebaseChangedEventArgs> ValueUpdated {
		add {
			valueUpdatedEvent += value;
			
			if (valueupdateListener == null) {
				valueupdateListener = new ValueEventListener(this);
				GetJavaObject().Call<AndroidJavaObject>("addValueEventListener", valueupdateListener);
			}
			
		}
		remove {
			valueUpdatedEvent -= value;
			
			if (valueUpdatedEvent == null) {
				
				GetJavaObject().Call("removeEventListener", valueupdateListener);
				valueupdateListener = null;
			}
			
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildAdded {
		add {
			childAddedEvent += value;
			
			if (childListener == null) {
				childListener = new ChildEventListener(this);
				GetJavaObject().Call<AndroidJavaObject>("addChildEventListener", childListener);
			}
			
		}
		remove {
			childAddedEvent -= value;
			
			if (childAddedEvent == null && childRemovedEvent == null
			    && childChangedEvent == null && childMovedEvent == null) {
				GetJavaObject().Call("removeEventListener", childListener);
				childListener = null;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildRemoved {
		add {
			childRemovedEvent += value;
			
			if (childListener == null) {
				childListener = new ChildEventListener(this);
				GetJavaObject().Call<AndroidJavaObject>("addChildEventListener", childListener);
			}
			
		}
		remove {
			childRemovedEvent -= value;
			
			if (childAddedEvent == null && childRemovedEvent == null
			    && childChangedEvent == null && childMovedEvent == null) {
				GetJavaObject().Call("removeEventListener", childListener);
				childListener = null;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildChanged {
		add {
			childChangedEvent += value;
			
			if (childListener == null) {
				childListener = new ChildEventListener(this);
				GetJavaObject().Call<AndroidJavaObject>("addChildEventListener", childListener);
			}
			
		}
		remove {
			childChangedEvent -= value;
			
			if (childAddedEvent == null && childRemovedEvent == null
			    && childChangedEvent == null && childMovedEvent == null) {
				GetJavaObject().Call("removeEventListener", childListener);
				childListener = null;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildMoved {
		add {
			childMovedEvent += value;
			
			if (childListener == null) {
				childListener = new ChildEventListener(this);
				GetJavaObject().Call<AndroidJavaObject>("addChildEventListener", childListener);
			}
			
		}
		remove {
			childMovedEvent -= value;
			
			if (childAddedEvent == null && childRemovedEvent == null
			    && childChangedEvent == null && childMovedEvent == null) {
				GetJavaObject().Call("removeEventListener", childListener);
				childListener = null;
			}
		}
	}

	public event EventHandler<FirebaseErrorEventArgs> Error;
	
	void OnValueUpdated(DataSnapshotAndroidImpl snapshot) {
		EventHandler<FirebaseChangedEventArgs> handler = valueUpdatedEvent;
		if (handler != null)
		{
			handler(this, new FirebaseChangedEventArgs() { DataSnapshot = snapshot });
		}
	}

	void OnChildAdded(DataSnapshotAndroidImpl snapshot) {
		EventHandler<FirebaseChangedEventArgs> handler = childAddedEvent;
		if (handler != null)
		{
			handler(this, new FirebaseChangedEventArgs() { DataSnapshot = snapshot });
		}
	}

	void OnChildRemoved(DataSnapshotAndroidImpl snapshot) {
		EventHandler<FirebaseChangedEventArgs> handler = childRemovedEvent;
		if (handler != null)
		{
			handler(this, new FirebaseChangedEventArgs() { DataSnapshot = snapshot });
		}
	}

	void OnChildChanged(DataSnapshotAndroidImpl snapshot) {
		EventHandler<FirebaseChangedEventArgs> handler = childChangedEvent;
		if (handler != null)
		{
			handler(this, new FirebaseChangedEventArgs() { DataSnapshot = snapshot });
		}
	}

	void OnChildMoved(DataSnapshotAndroidImpl snapshot) {
		EventHandler<FirebaseChangedEventArgs> handler = childMovedEvent;
		if (handler != null)
		{
			handler(this, new FirebaseChangedEventArgs() { DataSnapshot = snapshot });
		}
	}
	
	class ValueEventListener : AndroidJavaProxy {
		QueryAndroidImpl parent;
		
		public ValueEventListener(QueryAndroidImpl parent)
			:base("com.firebase.client.ValueEventListener")
		{
			this.parent = parent;
		}
		
		void onDataChange(AndroidJavaObject dataSnapshot) {
			parent.OnValueUpdated (new DataSnapshotAndroidImpl (dataSnapshot));
		}
		
		void onCancelled(AndroidJavaObject error) {
			FirebaseErrorAndroidImpl errorImpl = new FirebaseErrorAndroidImpl (error);
			EventHandler<FirebaseErrorEventArgs> handler = parent.Error;
			if (handler != null) {
				handler(this, new FirebaseErrorEventArgs() { 
					Error = new FirebaseError(errorImpl.Code, errorImpl.Message,  errorImpl.Details)
				});
			}		
		}
	}

	class ChildEventListener : AndroidJavaProxy {
		QueryAndroidImpl parent;
		
		public ChildEventListener(QueryAndroidImpl parent)
			:base("com.firebase.client.ChildEventListener")
		{
			this.parent = parent;
		}

		void onCancelled(AndroidJavaObject error) {
			FirebaseErrorAndroidImpl errorImpl = new FirebaseErrorAndroidImpl (error);
			EventHandler<FirebaseErrorEventArgs> handler = parent.Error;
			if (handler != null) {
				handler(this, new FirebaseErrorEventArgs() { Error = new FirebaseError(
					errorImpl.Code, errorImpl.Message, errorImpl.Details)});
			}
		}

		void onChildAdded(AndroidJavaObject snapshot, string previousChildName) {
			parent.OnChildAdded(new DataSnapshotAndroidImpl(snapshot));
		}

		void onChildAdded(AndroidJavaObject snapshot, AndroidJavaObject previousChildName) {
			parent.OnChildAdded(new DataSnapshotAndroidImpl(snapshot));
		}

		void onChildChanged(AndroidJavaObject snapshot, string previousChildName) {
			parent.OnChildChanged (new DataSnapshotAndroidImpl (snapshot));
		}

		void onChildChanged(AndroidJavaObject snapshot, AndroidJavaObject previousChildName) {
			parent.OnChildChanged (new DataSnapshotAndroidImpl (snapshot));
		}

		void onChildMoved(AndroidJavaObject snapshot, string previousChildName) {
			parent.OnChildMoved (new DataSnapshotAndroidImpl (snapshot));
		}

		void onChildRemoved(AndroidJavaObject snapshot) {
			parent.OnChildRemoved (new DataSnapshotAndroidImpl (snapshot));
		}

		bool equals (AndroidJavaObject other)
		{
			return other.Equals (this);
		}
	}
}
#endif


