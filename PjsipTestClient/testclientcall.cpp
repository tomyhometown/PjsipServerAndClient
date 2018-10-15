#include "testclientcall.h"
#include <iostream>

TestClientCall::TestClientCall(Account &acc, int call_id)
: Call(acc, call_id)
{
    std::cout << "TestClientCall is created!" << std::endl;
    myAcc = (TestClientAccount *)&acc;
}

TestClientCall::~TestClientCall()
{
}

void TestClientCall::onCallState(OnCallStateParam &prm)
{
    PJ_UNUSED_ARG(prm);

    CallInfo ci = getInfo();
    std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
              << "]" << std::endl;

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        myAcc->removeCall(this);
        /* Delete the call */
        delete this;
    }
}
