using UnityEngine;
using System.Collections;
using System;

public class FirebaseError {
	public FirebaseError(int code, string message, string details) {
		this.Code = code;
		this.Message = message;
		this.Details = details;
	}
	public int Code { get; private set; }
	public string Message { get; private set; }
	public string Details { get; private set;}
}
