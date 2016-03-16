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
//using UnityEditor;
//using UnityEditor.Callbacks;
//using UnityEditor.iOS.Xcode; 
using System.Collections;
using System.IO;

class XcodeProjectConfig  {

//	static string GetPBXProjectPath (string buildPath)
//	{
//		return Path.Combine (buildPath, "Unity-iPhone.xcodeproj/project.pbxproj");
//	}
//
//	static void AddDyLibToProject(PBXProject proj, string targetGuid, string lib, bool weak)
//	{
//		string fileGuid = proj.AddFile( "usr/lib/"+lib, "Frameworks/"+lib, PBXSourceTree.Sdk);
//		proj.AddFileToBuild (targetGuid, fileGuid);
//	}
//
//	[PostProcessBuild]
//	public static void OnPostprocessBuild(BuildTarget buildTarget, string path)
//	{
//		if (buildTarget == BuildTarget.iOS)
//		{
//			string projPath = GetPBXProjectPath(path);
//			PBXProject proj = new PBXProject();
//			proj.ReadFromString(File.ReadAllText(projPath));
//			string target = proj.TargetGuidByName("Unity-iPhone");
//
////			FFECC7051B1F3AC200DE8239 /* libc++.dylib */ = {isa = PBXFileReference; 
////				lastKnownFileType = "compiled.mach-o.dylib"; name = "libc++.dylib";
////				path = "Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator8.1.sdk/usr/lib/libc++.dylib"; 
////				sourceTree = DEVELOPER_DIR; };
//
//			AddDyLibToProject(proj, target, "libc++.dylib", false);
//			AddDyLibToProject(proj, target, "libicucore.dylib", false);
//			File.WriteAllText(projPath, proj.WriteToString());
//		}
//	}
}
