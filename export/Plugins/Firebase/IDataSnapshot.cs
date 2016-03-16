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
using System.Collections.Generic;

public interface IDataSnapshot  {
	IDataSnapshot Child (string path);
	bool HasChild (string path);

	bool Exists  { get; }
	string Key  { get; }
	object Priority { get; }
	IFirebase Ref { get; }

	float FloatValue { get; }
	string StringValue { get; }
	Dictionary<string, object> DictionaryValue { get; }
}
