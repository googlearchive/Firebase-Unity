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