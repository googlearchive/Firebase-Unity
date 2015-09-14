using UnityEngine;
using System.Collections;

public class AuthData {
	public AuthData(string token, string uid, long expiration) {
		this.Token = token;
		this.Uid = uid;
		this.Expiration = expiration;
	}
	public string Token { get; private set;}
	public string Uid { get; private set;}
	public long Expiration { get; private set;}
}
