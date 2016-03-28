# firebase-unity
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
        Debug.Log("Firebase Event Exception:");
        Debug.Log(e);
      }
   }
   ```

 * Threading: Unity does not allow game object modifications from any thread other than the Main thread. Firebase operates on a separate thread for performance reasons, and so you can not directly edit game objects from your firebase event responses. You can solve this by adding a Queue with actions to be fulfilled on the main thread. Create a new object inside Unity, add it to the game object you just created, and insert this code:

  ```C#
  using UnityEngine;
  using System.Collections;
  using System.Collections.Generic;
  using System;

  public class ExampleMainThreadQueue : MonoBehaviour {

    public readonly static Queue<Action> ExecuteOnMainThread = new Queue<Action>();

    public void Update()
    {
      // dispatch stuff on main thread
      while (ExecuteOnMainThread.Count > 0)
      {
        ExecuteOnMainThread.Dequeue().Invoke();
      }
    }
  }

  ```

  You can now execute function on the main thread using coroutines. 

  ```C#
  ExampleMainThreadQueue.ExecuteOnMainThread.Enqueue(() => {  
      StartCoroutine(somecoroutine);
  } );
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
