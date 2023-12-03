#ifndef __MESSAGES_H__
#define __MESSAGES_H__

enum MsgTypes {
    NONE,
    LOGIN_REQUEST,
    LOGIN_RESPONSE,
    BALANCE_REQUEST,
    BALANCE_RESPONSE,
    WITHDRAW_REQUEST,
    WITHDRAW_RESPONSE,
    QUIT,
};

struct GenericMessage {
    long msgType;
    char msgResult[100];
};

#endif
