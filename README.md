# firebase-unity

* 9/27 Update. September is almost over, and we’ve been hard at work on improving Unity support for Firebase. We’re now at the point where we’re putting the final touches on our API and ensuring the documentation meets your needs.  However, there is still some work to get done, so we’ll be pushing the release out approximately two weeks to mid-October.
 * We realize this is disappointing, but we’ll use the extra time enhancing the usability of the framework and documentation. We do apologize for the inconvenience and appreciate your continued support. We are doing our best to deliver the best of Firebase to you.
* 9/2 Update. Now that September is here, I thought it would be good to give you an update of where we are with the promised update to this repository. 
 * We are still on track for a September update to this repo.  It will be an update that supports the new version of Firebase Realtime Database.  We want to make sure the update meets your expectations and there is additional testing and fixes we want to do. To make sure that we’ve set expectations appropriately with you, this will be a BETA quality release with a general availability (release) date to follow.
 * I look forward to showing you what we’ve been up to!

The Firebase Real-Time Database Unity plugin does not currently support the latest Firebase SDK.  We are working on an update which should be available around September 2016.  Follow this project on github for updates.

Firebase inside Unity3d!

Welcome to Firebase (www.firebase.com) on Unity3d plugin, which supports:
 * iOS
 * Android
 * The Unity Editor Player (currently works on Mac and Windows)

## How to get started (As base or standalone application)
 1. Read about Firebase at www.firebase.com
 2. Clone this git repo, then form Unity open the Unity project in the `empty-project` folder
 3. Open the `MainScene` and the example `TestScript.cs` to start coding against Firebase!

## How to get started (If you already have an existing project that requires Firebase)
1. Read about Firebase at www.firebase.com
2. Import the package "firebase.unitypackage"
3. If you already have an existing AndroidManifest.xml file, simply add the permission ``<uses-permission android:name="android.permission.INTERNET" />  ``. In this case you do not have to import the AndroidManifest.xml file because it might overwrite your own. 

## API Differences
You will find the API is very similar to the Java/iOS versions except that:
 1. You create a new Firebase reference with:
    ```C#
    IFirebase ref = Firebase.CreateNew("https://your-project-id.firebaseIO.com");
    ```
    
    This gives you the proper platform-dependent implementation.
 2. Certain parts of the API have been made C#-idiomatic with C# properties and events.
    For example, instead of getKey, you'll find a `Key` property.
    ```C#
    IFirebase root;
    IFirebase foo;

    void Awake() {
      root = Firebase.CreateNew("https://your-project-id.firebaseIO.com");
      foo = ref.Child("foo");
      foo.Child (DateTime.Now.ToString ("yyyy-MM-dd-HH-mm-ss-zzz")).SetValue ("Awake()");
    }

    void OnEnable() {
      foo.ChildAdded += FooChildAdded;
    }

    void OnDisable() {
      foo.ChildAdded -= FooChildAdded;
    }

    void FooChildAdded (object sender, FirebaseChangedEventArgs e) {
      Debug.Log("FooChildAdded: " + e.DataSnapshot.Key);
    }
    ```
   
## Troubleshooting
 * If you are not receiving events, it can either be because your event response call contains an exception, or because Garbage Collection is removing the references to your firebase. You can solve this by storing the firebase objects in variables. 
 
   First, make sure you are referencing objects correctly. 

   Instead of this:
   ```C#
    new Firebase("mypath").ValueChanged += (...) => { .... };
    ```

   do this:
   ```C#
   classMember = new Firebase("mypath");  //holds a reference until the behavior is released
   classMember.ValueChanged += (...) => {...} ;
   ```

   But beware, if you are going to access child objects you need to store these in variables too, otherwise GC will delete the reference as well. So instead of this:
  ```C#
   classMember = new Firebase("mypath");  //holds a reference until the behavior is released
   classMember.Child("somechild").ValueChanged += (...) => {...} ;
   ```

   do this

   ```C#
   IFirebase fb;
   IFirebase sampleChild;
   void someFunction() {
       fb = new Firebase("mypath");  //holds a reference until the behavior is released
       sampleChild = fb.Child("somechild"); // holds another child reference so GC doens't remove the reference to the child
       sampleChild.ValueChanged += (...) => {...} ;
   }
   ```

 If you are referencing objects correctly, you will likely need to re-open your Unity project because your event response function contained an error. At this moment an error in the event response function causes Firebase Unity to stop working during your entire Unity Editor session. You can debug this by catching the exception on the highest level, re-opening your Unity project, and running your scene again. You can now see the error and fix it. Note that you will need to re-open your project on every attempt until we fix this bug. After you have resolved your exception, your firebase event should be received normally again. 

 For example: 
   ```C#
   IFirebase fb;
   IFirebase sampleChild;
   void someFunction() {
       try {
           fb = new Firebase("mypath");  //holds a reference until the behavior is released
           sampleChild = fb.Child("somechild"); // holds another child reference so GC doens't remove the reference to the child
           sampleChild.ValueChanged += (...) => {...} ;
           someBadFunction();
      } catch(Exception e) {
        Debug.LogError(e);
      }
   }
   ```

 * Threading: Unity does not allow game object modifications from any thread other than the Main thread. Firebase operates on a separate thread for performance reasons, and so you can not directly edit game objects from your firebase event responses. You can solve this by adding a Queue with actions to be fulfilled on the main thread. You can use the following (unofficial) class for doing so: https://github.com/PimDeWitte/UnityMainThreadDispatcher


  You can now execute function on the main thread like this:

  ```C#
	public IEnumerator ThisWillBeExecutedOnTheMainThread() {
		Debug.Log ("This is executed from the main thread");
		yield return null;
	}
	public void ExampleMainThreadCall() {
		UnityMainThreadDispatcher.Instance().Enqueue(ThisWillBeExecutedOnTheMainThread()); 
	}
```


 * iOS: XCode fails to link.  Please follow instructions located at: https://www.firebase.com/docs/ios/alternate-setup.html  You will need to do this again if you do a full build/replace from Unity, but an incremental build will keep these settings.
 * Mac: The plugin does not appear to be working at all for the player, but works for iOS and Android<p/>
  It could be that you have not installed the Java6 legacy runtime https://support.apple.com/kb/DL1572?locale=en_US
  It may also be that the plugin cannot locate Java on your machine.  To patch the Java location, open
  the Fireproxy.bundle, cd into Contents/MacOS and run this command:
   ```bash
   install_name_tool -change "/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib/server/libjvm.dylib" << newpath >> FirebaseProxy
   ```

   where `newpath` is the location of `libjvm.dylib` under Java6.
 * Windows: The plugin does not work in the player (or you get a nullref/crash) but works for iOS and Android. Make sure you have installed the *64bit* version of Java and that you've updated your `PATH` environment variable to include the path to `jvm.dll`.  Note that the error you see will be something to the effect of `could not locate Firebaseproxy.dll`.  This happens because FirebaseProxy.dll depends on jvm.dll which could not be located.
