using UnityEngine;
using UnityEditor;
using System;
using System.Runtime.InteropServices;
using System.Threading;

[InitializeOnLoad]
public class Startup {

	[DllImport("FirebaseProxy")]
	private static extern void DestroyJvm( );
	
	static bool hasInitialized = false;
	static Startup()
	{
		if (!hasInitialized) {
			hasInitialized = true;
			EditorApplication.CallbackFunction oldCallback = EditorApplication.playmodeStateChanged;
			EditorApplication.playmodeStateChanged = delegate {
				if (!EditorApplication.isPlaying) {
					DestroyJvm();

					FirebaseEditorImpl.Shutdown();
				}
				else {
					FirebaseEditorImpl.Initialize();
				}

				if (oldCallback != null) {
					oldCallback.Invoke();
				}
			};
			FirebaseEditorImpl.Initialize();
			Firebase.Register (new FirebaseEditorImpl.Factory ());
		}
	}
}
