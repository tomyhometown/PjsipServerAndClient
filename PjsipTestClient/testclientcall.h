#ifndef TESTCLIENTCALL_H
#define TESTCLIENTCALL_H

#include <pjsua2.hpp>
#include "testclientaccount.h"

using namespace pj;

class TestClientAccount;
class TestClientCall : public Call
{
private:
    TestClientAccount *myAcc;
public:
    TestClientCall(Account &acc, int call_id = PJSUA_INVALID_ID);
    ~TestClientCall();

    virtual void onCallState(OnCallStateParam &prm);
};

#endif // TESTCLIENTCALL_H
