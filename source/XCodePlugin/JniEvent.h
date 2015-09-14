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
    AuthSuccessEvent(long cookie, const char* token, const char*uid, long expiration);
    void Process();
private:
    long m_cookie;
    const char* m_token;
    const char* m_uid;
    long m_expiration;
};

class AuthFailureEvent : public JniEvent {
public:
    AuthFailureEvent(long cookie, int code, const char* message, const char* detail);
    void Process();
private:
    long m_cookie;
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
