using UnityEngine;
using System.Collections;

public class TestScript : MonoBehaviour {
	IFirebase firebase;

	// Use this for initialization
	void Start () {
	    firebase = Firebase.CreateNew ("https://incandescent-torch-2575.firebaseio.com/testing");
		firebase.AuthWithPassword ("benwu@google.com", "password", (AuthData auth) => {
			Debug.Log ("auth success!!" + auth.Uid);
		}, (FirebaseError e) => {
			Debug.Log ("auth failure!!");
		});

		firebase.ChildAdded += (object sender, ChangedEventArgs e) => {
			Debug.Log ("Child added!");
		};

		firebase.ChildRemoved += (object sender, ChangedEventArgs e) => {
			Debug.Log ("Child removed!");
		};

		//firebase.SetValue ("working?");
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
