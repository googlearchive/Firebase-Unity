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

#import "Firebase/Firebase.h"

// Converts C style string to NSString
NSString* CreateNSString (const char* string)
{
    if (string)
        return [NSString stringWithUTF8String: string];
    else
        return [NSString stringWithUTF8String: ""];
}

// Helper method to create C string copy
char* MakeStringCopy (const char* string)
{
    if (string == NULL)
        return NULL;
    
    char* res = (char*)malloc(strlen(string) + 1);
    strcpy(res, string);
    return res;
}

extern "C" {
    typedef void (*OnValueChanged)(void*, void* );
    typedef void (*OnAuthSuccessHandler)(long reference, const char* token, const char* uid, long expiration);
    typedef void (*OnAuthCancelHandler)(long reference, int code, const char* message, const char* details);
    
    void* _FirebaseNew (const char *path)
    {
        // Create a reference to a Firebase location
        Firebase *myFirebaseRef = [[Firebase alloc] initWithUrl:CreateNSString(path)];
        return (void*)CFBridgingRetain(myFirebaseRef);
    }
    
    void _FirebaseRelease(void* firebase) {
        Firebase *myFirebaseRef = (__bridge_transfer Firebase*) (firebase);
    }
    
    void* _FirebaseChild(void* firebase, const char *path)
    {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        Firebase *child = [myFirebaseRef childByAppendingPath:CreateNSString(path)];
        if (child == nil) {
            return nil;
        }
        return (void*)CFBridgingRetain(child);
    }
    
    void* _FirebaseParent(void* firebase)
    {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        Firebase *parent = [myFirebaseRef parent];
        if (parent == nil) {
            return nil;
        }
        return (void*)CFBridgingRetain(parent);
    }
    
    void* _FirebaseRoot(void* firebase)
    {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        Firebase *root = [myFirebaseRef root];
        if (root == nil) {
            return nil;
        }
        return (void*)CFBridgingRetain(root);
    }
    
    const char* _FirebaseGetKey(void* firebase)
    {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        return MakeStringCopy([[myFirebaseRef key] UTF8String]);
    }
    
    void* _FirebasePush(void* firebase)
    {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        Firebase *push = [myFirebaseRef childByAutoId];
        if (push == nil) {
            return nil;
        }
        return (void*)CFBridgingRetain(push);
    }
    
    void _FirebaseSetString(void* firebase, const char *value) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef setValue:CreateNSString(value)];
    }
    
    void _FirebaseSetJson(void* firebase, const char *value) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        NSData *data = [CreateNSString(value) dataUsingEncoding:NSUTF8StringEncoding];
        id json = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
        
        [myFirebaseRef setValue:json];
    }
    
    void _FirebaseSetFloat(void* firebase, float value) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef setValue:[NSNumber numberWithFloat: value]];
    }
    
    void _FirebaseSetPriority(void* firebase, const char *value) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef setPriority:CreateNSString(value)];
    }
    
    void _FirebaseObserveValueChange( void* firebase, OnValueChanged onChanged, void* refId) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef observeEventType:FEventTypeValue withBlock:^(FDataSnapshot *snapshot) {
            onChanged((void*)CFBridgingRetain(snapshot), refId);
        }];
    }
    
    void _FirebaseObserveChildAdded( void* firebase, OnValueChanged onChanged, void* refId) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef observeEventType:FEventTypeChildAdded withBlock:^(FDataSnapshot *snapshot) {
            onChanged((void*)CFBridgingRetain(snapshot), refId);
        }];
    }
    
    void _FirebaseObserveChildRemoved( void* firebase, OnValueChanged onChanged, void* refId) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef observeEventType:FEventTypeChildRemoved withBlock:^(FDataSnapshot *snapshot) {
            onChanged((void*)CFBridgingRetain(snapshot), refId);
        }];
    }
    
    void _FirebaseObserveChildChanged( void* firebase, OnValueChanged onChanged, void* refId) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef observeEventType:FEventTypeChildChanged withBlock:^(FDataSnapshot *snapshot) {
            onChanged((void*)CFBridgingRetain(snapshot), refId);
        }];
    }
    
    void _FirebaseObserveChildMoved( void* firebase, OnValueChanged onChanged, void* refId) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef observeEventType:FEventTypeChildMoved withBlock:^(FDataSnapshot *snapshot) {
            onChanged((void*)CFBridgingRetain(snapshot), refId);
        }];
    }
    
    void _FirebaseRemoveObservers( void* firebase) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef removeAllObservers];
    }
    
    void _FirebaseAuthWithCustomToken (void* firebase, const char* token,
                                       OnAuthSuccessHandler success, OnAuthCancelHandler cancel,
                                       long callback) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef authWithCustomToken:CreateNSString(token)
                       withCompletionBlock:^(NSError *error, FAuthData *authData) {
                           if (error) {
                               cancel(callback, [error code],
                                      MakeStringCopy([[error localizedFailureReason] UTF8String]),
                                      MakeStringCopy([[error localizedDescription] UTF8String]));
                           } else {
                               // user is logged in, check authData for data
                               success(callback, MakeStringCopy([[authData token] UTF8String]),
                                       MakeStringCopy([[authData uid] UTF8String]), [[authData expires] longValue]);
                           }
                       }];
    }
                           
    void _FirebaseAuthAnonymously (void* firebase,
                                   OnAuthSuccessHandler success, OnAuthCancelHandler cancel,
                                   long callback) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef authAnonymouslyWithCompletionBlock:^(NSError *error, FAuthData *authData) {
            if (error) {
                cancel(callback, [error code],
                       MakeStringCopy([[error localizedFailureReason] UTF8String]),
                       MakeStringCopy([[error localizedDescription] UTF8String]));
            } else {
                // user is logged in, check authData for data
                success(callback, MakeStringCopy([[authData token] UTF8String]),
                        MakeStringCopy([[authData uid] UTF8String]), [[authData expires] longValue]);
            }
        }];
    }
    
    void _FirebaseAuthWithPassword (void* firebase, const char* email,
                                    const char* password,
                                    OnAuthSuccessHandler success, OnAuthCancelHandler cancel,
                                    long callback) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef authUser:CreateNSString(email) password:CreateNSString(password)
            withCompletionBlock:^(NSError *error, FAuthData *authData) {
                if (error) {
                    cancel(callback, [error code],
                           MakeStringCopy([[error localizedFailureReason] UTF8String]),
                           MakeStringCopy([[error localizedDescription] UTF8String]));
                } else {
                    // user is logged in, check authData for data
                    success(callback, MakeStringCopy([[authData token] UTF8String]),
                            MakeStringCopy([[authData uid] UTF8String]), [[authData expires] longValue]);
                }
            }];
    }
    
    void _FirebaseAuthWithOAuthToken (void* firebase, const char* provider,
                                      const char* token,
                                      OnAuthSuccessHandler success, OnAuthCancelHandler cancel,
                                      long callback) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef authWithOAuthProvider:CreateNSString(provider) token:CreateNSString(token)
                         withCompletionBlock:^(NSError *error, FAuthData *authData) {
                             if (error) {
                                 cancel(callback, [error code],
                                        MakeStringCopy([[error localizedFailureReason] UTF8String]),
                                        MakeStringCopy([[error localizedDescription] UTF8String]));
                             } else {
                                 // user is logged in, check authData for data
                                 success(callback, MakeStringCopy([[authData token] UTF8String]),
                                         MakeStringCopy([[authData uid] UTF8String]), [[authData expires] longValue]);
                             }
                         }];
    }
    
    const char* _FirebaseGetAuthToken(void* firebase) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        FAuthData* authData = [myFirebaseRef authData];
        if (authData == nil) {
            return nil;
        }

        return MakeStringCopy([[authData token] UTF8String]);
    }
                               
    const char* _FirebaseGetAuthUid(void* firebase) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        FAuthData* authData = [myFirebaseRef authData];
        if (authData == nil) {
            return nil;
        }
        
        return MakeStringCopy([[authData uid] UTF8String]);
    }
                               
    long _FirebaseGetAuthExpiration(void* firebase) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        FAuthData* authData = [myFirebaseRef authData];
        if (authData == nil) {
            return 0;
        }
        
        return [[authData expires] longValue];
    }
    
    void _FirebaseUnAuth( void* firebase) {
        Firebase *myFirebaseRef = (__bridge Firebase*) (firebase);
        [myFirebaseRef unauth];
    }
    
    float _DataSnapshotGetFloatValue (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return [[snapshotRef value] floatValue];
    }
    
    const char* _DataSnapshotGetStringValue (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return MakeStringCopy([[snapshotRef value] UTF8String]);
    }
    
    const char* _DataSnapshotGetDictionary(void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        id value = [snapshotRef value];
        if([value isKindOfClass:[NSDictionary class]]
           && [NSJSONSerialization isValidJSONObject:value]) {
            NSError *error;
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:value
                                                               options:0
                                                                 error:&error];
            if (! jsonData) {
                return nil;
            } else {
                NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
                return MakeStringCopy([jsonString UTF8String]);
            }
        }
        return nil;
    }
    
    void* _DataSnapshotGetChild (void* datasnapshot, const char* path) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return (void*)CFBridgingRetain([snapshotRef childSnapshotForPath:CreateNSString(path)]);
    }
    
    void* _DataSnapshotHasChild (void* datasnapshot, const char* path) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        if ([snapshotRef hasChild:CreateNSString(path)]) {
            return (void*)(1);
        }
        return nil;
    }
    
    void* _DataSnapshotExists (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        if (snapshotRef.value != [NSNull null]) {
            return (void*)1;
        }
        return nil;
    }
    
    const char* _DataSnapshotGetKey (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return MakeStringCopy([[snapshotRef key] UTF8String]);
    }
    
    const char* _DataSnapshotGetPriority (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return MakeStringCopy([[snapshotRef priority] UTF8String]);
    }
    
    void* _DataSnapshotGetRef (void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge FDataSnapshot*) (datasnapshot);
        return (void*)CFBridgingRetain([snapshotRef ref]);
    }
    
    void _DataSnapshotRelease(void* datasnapshot) {
        FDataSnapshot *snapshotRef = (__bridge_transfer FDataSnapshot*) (datasnapshot);
    }
}

