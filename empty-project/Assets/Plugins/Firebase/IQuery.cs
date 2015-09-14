using UnityEngine;
using System.Collections;
using System;

public interface IQuery  {
	event EventHandler<ChangedEventArgs> ValueUpdated;
	event EventHandler<ChangedEventArgs> ChildAdded;
	event EventHandler<ChangedEventArgs> ChildRemoved;
	event EventHandler<ChangedEventArgs> ChildChanged;
	event EventHandler<ChangedEventArgs> ChildMoved;
	event EventHandler<ErrorEventArgs> Error;
}

public class ChangedEventArgs : EventArgs
{
	public IDataSnapshot DataSnapshot { get; set; }
}

public class ErrorEventArgs : EventArgs 
{
	public FirebaseError Error { get; set; }
}

