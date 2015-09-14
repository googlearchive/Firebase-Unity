using UnityEngine;
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
