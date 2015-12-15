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
//
//  JniEvent.h
//  XCodePlugin
//
//  Created by benwu on 9/11/15.
//
//

#ifndef __XCodePlugin__JniEvent__
#define __XCodePlugin__JniEvent__

#include <stdio.h>
#include "JniDataSnapshot.h"

class JniEvent {
public:
    virtual void Process();
};

class ValueChangedEvent : public JniEvent {
public:
    ValueChangedEvent(JniDataSnapshot* snapshot);
    void Process();
private:
    JniDataSnapshot* m_snapshot;
};

class ChildAddedEvent : public JniEvent {
public:
    ChildAddedEvent(JniDataSnapshot* snapshot);
    void Process();
private:
    JniDataSnapshot* m_snapshot;
};

class ChildRemovedEvent : public JniEvent {
public:
    ChildRemovedEvent(JniDataSnapshot* snapshot);
    void Process();
private:
    JniDataSnapshot* m_snapshot;
};

class ChildChangedEvent : public JniEvent {
public:
    ChildChangedEvent(JniDataSnapshot* snapshot);
    void Process();
private:
    JniDataSnapshot* m_snapshot;
};

class ChildMovedEvent : public JniEvent {
public:
    ChildMovedEvent(JniDataSnapshot* snapshot);
    void Process();
private:
    JniDataSnapshot* m_snapshot;
};

class AuthSuccessEvent : public JniEvent {
public:
    AuthSuccessEvent(uint64_t cookie, const char* token, const char*uid, uint64_t expiration);
    void Process();
private:
	uint64_t m_cookie;
    const char* m_token;
    const char* m_uid;
	uint64_t m_expiration;
};

class AuthFailureEvent : public JniEvent {
public:
    AuthFailureEvent(uint64_t cookie, int code, const char* message, const char* detail);
    void Process();
private:
	uint64_t m_cookie;
    int m_code;
    const char* m_message;
    const char* m_detail;
};

class ErrorEvent : public JniEvent {
public:
    ErrorEvent(void* cookie, int code, const char* message, const char* detail);
    void Process();
private:
    void* m_cookie;
    int m_code;
    const char* m_message;
    const char* m_detail;
};


#endif /* defined(__XCodePlugin__JniEvent__) */
