# firebase-unity
Firebase inside Unity3d!

Welcome to firebase (www.firebase.com) on Unity3d.
This plugin supports:
 * iOS
 * Android
 * The Unity Player

The unity player is now functional for both Mac and Windows.

How to get started.
 1. Read about Firebase at www.firebase.com
 2. Open the empty unity3d project included here (...or import the firebase.unitypackage into your existing app).
 3. Open the MainScene and the single TestScript.cs and start coding against Firebase!

API Differences.<p/>
You will find the API is very similar to the Java/iOS versions except that:
 1. You create a new Firebase with
    IFirebase ref = Firebase.CreateNew("path....").  
    This gives you the proper platform-dependent implementation
 2. Certain parts of the API have been made C#-idiomatic with C# properties and events.  So instead of getKey,
   you'll find a property Key.
   
Trouble Shooting.
 * You are not receiving events, its very likely you allowed your Firebase reference to GC.
  So instead of this:<p/>
   new Firebase("mypath").ValueChanged += (...) => { .... };<p/>
  do this:<p/>
   classMember = new Firebase("mypath");  //holds a reference until the behavior is released<p/>
   classMember.ValueChanged += (...) => {...} ;<p/>
 * iOS: XCode fails to link.  Please follow instructions located at: https://www.firebase.com/docs/ios/alternate-setup.html  You will need to do this again if you do a full build/replace from Unity, but an incremental build will keep these settings.<p/>
 * Mac: The plugin does not appear to be working at all for the player, but works for iOS and Android<p/>
  It could be that you have not installed the Java6 legacy runtime https://support.apple.com/kb/DL1572?locale=en_US
  It may also be that the plugin cannot locate Java on your machine.  To patch the Java location, open
  the Fireproxy.bundle, cd into Contents/MacOS and run this command:<p/>
  install_name_tool -change "/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/lib/server/libjvm.dylib" << newpath >> FirebaseProxy <p/>
  where newpath is the location of libjvm.dylib under Java6.
 * Windows: The plugin does not work in the player (or you get a nullref/crash) but works for iOS and Android.<p/>  Make sure you have installed the *64bit* version of Java and that you've updated your PATH environment variable to include jvm.dll.  Note that the error you see will be something to the effect of "could not locate Firebaseproxy.dll".  This happens because FirebaseProxy.dll depends on jvm.dll which could not be located.
