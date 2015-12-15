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
﻿using UnityEngine;
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

