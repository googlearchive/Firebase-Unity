# firebase-unity
Firebase inside Unity3d!

Welcome to Firebase (www.firebase.com) on Unity3d plugin, which supports:
 * iOS
 * Android
 * The Unity Editor Player (currently works on Mac and Windows)

## How to get started
 1. Read about Firebase at www.firebase.com
 2. Clone this git repo, then form Unity open the Unity project in the `empty-project` folder
 3. Open the `MainScene` and the example `TestScript.cs` to start coding against Firebase!

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

    void FooChildAdded (object sender, ChangedEventArgs e) {
      Debug.Log("FooChildAdded: " + e.DataSnapshot.Key);
    }
    ```
   
## Troubleshooting
 * You are not receiving events, its very likely you allowed your Firebase reference to GC.

   Instead of this:
   ```C#
    new Firebase("mypath").ValueChanged += (...) => { .... };
    ```

   do this:
   ```C#
   classMember = new Firebase("mypath");  //holds a reference until the behavior is released
   classMember.ValueChanged += (...) => {...} ;
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
