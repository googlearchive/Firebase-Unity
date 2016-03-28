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
using AOT;
using System.Collections.Generic;
using System.Threading;

#if UNITY_EDITOR
public class QueryEditorImpl : IQuery {
	IntPtr nativeReference;
	EventHandler<FirebaseChangedEventArgs> valueUpdatedEvent, childAddedEvent, 
									childRemovedEvent, childChangedEvent, childMovedEvent;
	IntPtr stubValueChanged = IntPtr.Zero;
	IntPtr stubChildEvent = IntPtr.Zero;
	
	static protected onValueChangedEventHandler valueChangedHandler = new onValueChangedEventHandler(onValueChanged);
	static protected onValueChangedEventHandler childAddedHandler = new onValueChangedEventHandler(onChildAdded);
	static protected onValueChangedEventHandler childRemovedHandler = new onValueChangedEventHandler(onChildRemoved);
	static protected onValueChangedEventHandler childChangedHandler = new onValueChangedEventHandler(onChildChanged);
	static protected onValueChangedEventHandler childMovedHandler = new onValueChangedEventHandler(onChildMoved);
	static protected onErrorHandler errorHandler = new onErrorHandler (OnError);
	static protected debugLogHandler debugLog = new debugLogHandler(DebugLog);

	static Dictionary<IntPtr, WeakReference> global_table = new Dictionary<IntPtr, WeakReference> ();
	static object _sync = new object ();

	public QueryEditorImpl(IntPtr nativeReference) {
		this.nativeReference = nativeReference;
		lock (_sync) {
			global_table[nativeReference] = new WeakReference(this);
		}
	}

	~QueryEditorImpl() {
		if (nativeReference != IntPtr.Zero) {
			lock(_sync) {
				global_table.Remove (nativeReference);
			}
			if (stubChildEvent != IntPtr.Zero) {
				_FirebaseRemoveChildEvents(stubChildEvent);
			}
			if (stubValueChanged != IntPtr.Zero) {
				Debug.Log ("releasing value changed: " + stubValueChanged.ToString());
				_FirebaseRemoveValueChange(stubValueChanged);
			}
		}
	}

	/// <summary>
	/// TODO: when we support Queries directly (not as Firebase*), we'll have to make sure
	/// this pointer gets stored as a Query versus Firebase as its vtable will be different.
	/// </summary>
	/// <returns>The OS object.</returns>
	protected IntPtr GetEditorObject() {
		return nativeReference;
	}

	internal static object GetCachedInstance(IntPtr reference) {
		lock (_sync) {
			WeakReference weakReference;
			if (global_table.TryGetValue(reference, out weakReference)) {
				if (weakReference.Target == null) {
					Debug.Log ("FirebasePlugin: target of event has GC'd");
				}
				return weakReference.Target;
			}
		}
		return null;
	}

	internal static T GetOrCreateCachedInstance<T>(IntPtr reference, Func<T> createFunction) where T:class {
		lock (_sync) {
			WeakReference weakReference;
			if (global_table.TryGetValue(reference, out weakReference)) {
				T result = weakReference.Target as T;
				if (result != null) {
					return result;
				}
			}
			return createFunction();
		}
	}

	public delegate void onValueChangedEventHandler( IntPtr dataSnapshot, IntPtr referenceId );
	public delegate void debugLogHandler( String log );
	public delegate void onAuthSuccessHandler(long reference, String token, String uid, long expiration);
	public delegate void onAuthCancelHandler(long reference, int code, String message, String details);
	public delegate void onErrorHandler(IntPtr reference, int code, String message, String details);

	[DllImport("FirebaseProxy")]
	private static extern IntPtr _FirebaseObserveValueChange( IntPtr firebase);

	[DllImport("FirebaseProxy")]
	private static extern IntPtr _FirebaseObserveChildEvents (IntPtr firebase);

	[DllImport("FirebaseProxy")]
	private static extern void _FirebaseRemoveValueChange(IntPtr stub);

	[DllImport("FirebaseProxy")]
	private static extern void _FirebaseRemoveChildEvents(IntPtr stub);

	#region IQuery implementation
	public event EventHandler<FirebaseChangedEventArgs> ValueUpdated {
		add {
			valueUpdatedEvent += value;
			
			if (stubValueChanged == IntPtr.Zero) {
				this.stubValueChanged = _FirebaseObserveValueChange(GetEditorObject());
				Debug.Log ("subscribed value changed: " + stubValueChanged.ToString());
			}
			
		}
		remove {
			valueUpdatedEvent -= value;
			if (valueUpdatedEvent == null) {
				_FirebaseRemoveValueChange(stubValueChanged);
				stubValueChanged = IntPtr.Zero;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildAdded {
		add {
			childAddedEvent += value;
			
			if (stubChildEvent == IntPtr.Zero) {
				this.stubChildEvent = _FirebaseObserveChildEvents(GetEditorObject());
			}
			
		}
		remove {
			childAddedEvent -= value;
			if (childAddedEvent == null && childRemovedEvent == null 
			    && childChangedEvent == null && childMovedEvent == null) {
				_FirebaseRemoveChildEvents(stubChildEvent);
				stubChildEvent = IntPtr.Zero;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildRemoved
	{
		add {
			childRemovedEvent += value;
			
			if (stubChildEvent == IntPtr.Zero) {
				this.stubChildEvent = _FirebaseObserveChildEvents(GetEditorObject());
			}
			
		}
		remove {
			childRemovedEvent -= value;
			if (childAddedEvent == null && childRemovedEvent == null 
			    && childChangedEvent == null && childMovedEvent == null) {
				_FirebaseRemoveChildEvents(stubChildEvent);
				stubChildEvent = IntPtr.Zero;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildChanged
	{
		add {
			childChangedEvent += value;
			
			if (stubChildEvent == IntPtr.Zero) {
				this.stubChildEvent = _FirebaseObserveChildEvents(GetEditorObject());
			}
			
		}
		remove {
			childChangedEvent -= value;
			if (childAddedEvent == null && childRemovedEvent == null 
			    && childChangedEvent == null && childMovedEvent == null) {
				_FirebaseRemoveChildEvents(stubChildEvent);
				stubChildEvent = IntPtr.Zero;
			}
		}
	}

	public event EventHandler<FirebaseChangedEventArgs> ChildMoved
	{
		add {
			childMovedEvent += value;
			
			if (stubChildEvent == IntPtr.Zero) {
				this.stubChildEvent = _FirebaseObserveChildEvents(GetEditorObject());
			}
			
		}
		remove {
			childMovedEvent -= value;
			if (childAddedEvent == null && childRemovedEvent == null 
			    && childChangedEvent == null && childMovedEvent == null) {
				_FirebaseRemoveChildEvents(stubChildEvent);
				stubChildEvent = IntPtr.Zero;
			}
		}
	}

	public event System.EventHandler<FirebaseErrorEventArgs> Error;
	#endregion

	[MonoPInvokeCallbackAttribute(typeof(onValueChangedEventHandler))]
	static void onValueChanged(IntPtr reference, IntPtr snapshot) {
		if (snapshot == IntPtr.Zero) {
			return;
		}
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			Debug.Log ("FirebasePlugin: unable to locate target for value callback.  Make sure you hold a reference to the firebase object.");
			return;
		}
		EventHandler<FirebaseChangedEventArgs> handler = target.valueUpdatedEvent;
		if (handler != null)
		{
			handler(target, new FirebaseChangedEventArgs() { DataSnapshot = new DataSnapshotEditorImpl(snapshot) });
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onValueChangedEventHandler))]
	static void onChildAdded(IntPtr reference, IntPtr snapshot) {
		if (snapshot == IntPtr.Zero) {
			return;
		}
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			Debug.Log ("FirebasePlugin: unable to locate target for child event onChildAdded.  Make sure you hold a reference to the firebase object.");
			return;
		}
		EventHandler<FirebaseChangedEventArgs> handler = target.childAddedEvent;
		if (handler != null)
		{
			handler(target, new FirebaseChangedEventArgs() { DataSnapshot = new DataSnapshotEditorImpl(snapshot) });
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onValueChangedEventHandler))]
	static void onChildRemoved(IntPtr reference, IntPtr snapshot) {
		if (snapshot == IntPtr.Zero) {
			return;
		}
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			Debug.Log ("FirebasePlugin: unable to locate target for child event onChildRemoved.  Make sure you hold a reference to the firebase object.");
			return;
		}
		EventHandler<FirebaseChangedEventArgs> handler = target.childRemovedEvent;
		if (handler != null)
		{
			handler(target, new FirebaseChangedEventArgs() { DataSnapshot = new DataSnapshotEditorImpl(snapshot) });
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onValueChangedEventHandler))]
	static void onChildChanged(IntPtr reference, IntPtr snapshot) {
		if (snapshot == IntPtr.Zero) {
			return;
		}
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			Debug.Log ("FirebasePlugin: unable to locate target for child event onChildChanged.  Make sure you hold a reference to the firebase object.");
			return;
		}
		EventHandler<FirebaseChangedEventArgs> handler = target.childChangedEvent;
		if (handler != null)
		{
			handler(target, new FirebaseChangedEventArgs() { DataSnapshot = new DataSnapshotEditorImpl(snapshot) });
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onValueChangedEventHandler))]
	static void onChildMoved(IntPtr reference, IntPtr snapshot) {
		if (snapshot == IntPtr.Zero) {
			return;
		}
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			Debug.Log ("FirebasePlugin: unable to locate target for child event onChildMoved.  Make sure you hold a reference to the firebase object.");
			return;
		}
		EventHandler<FirebaseChangedEventArgs> handler = target.childMovedEvent;
		if (handler != null)
		{
			handler(target, new FirebaseChangedEventArgs() { DataSnapshot = new DataSnapshotEditorImpl(snapshot) });
		}
	}

	[MonoPInvokeCallbackAttribute(typeof(onErrorHandler))]
	static void OnError(IntPtr reference, int code, String message, String details) {
		QueryEditorImpl target = (QueryEditorImpl)GetCachedInstance(reference);
		if (target == null) {
			return;
		}
		EventHandler<FirebaseErrorEventArgs> handler = target.Error;
		if (handler != null)
		{
			handler(target, new FirebaseErrorEventArgs() { Error = new FirebaseError(code, message, details) });
		}	
	}

	[MonoPInvokeCallbackAttribute(typeof(debugLogHandler))]
	static void DebugLog(String message) {
		Debug.Log (message);
	}
	
}
#endif
