using UnityEngine;
using System.Collections;

public interface IFirebaseFactory  {
	IFirebase TryCreate(string path);
}
