#include "testseveraccount.h"
#include "testservercall.h"
#include <iostream>
#include <fstream>

int TestSeverAccount::sendCounter = 0;

TestSeverAccount::TestSeverAccount()
{
    std::cout << "TestSeverAccount is created!" << std::endl;
}

TestSeverAccount::~TestSeverAccount()
{
    std::cout << "TestSeverAccount is deleted!" << std::endl;
}

void TestSeverAccount::onInstantMessageStatus(OnInstantMessageStatusParam &prm)
{
   std::cout << std::endl<< "********** SendMessage:code = " << prm.code << " ***********"<< std::endl << std::endl;
   if(200 == prm.code)
   {
       sendCounter++;
   }
   std::cout << std::endl<< "********** SendCounter = " << sendCounter << " ***********"<< std::endl << std::endl;
}

void TestSeverAccount::onIncomingCall(OnIncomingCallParam &iprm)
{
    Call *call = new TestServerCall(*this, iprm.callId);
    CallInfo ci = call->getInfo();
    CallOpParam prm;

    std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
              << ci.stateText << "]" << std::endl;

    calls.push_back(call);
    prm.statusCode = (pjsip_status_code)200;
    call->answer(prm);
}

void TestSeverAccount::onRegState(OnRegStateParam &prm)
{
    AccountInfo ai = getInfo();
    std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
          << prm.code << std::endl;
}

void TestSeverAccount::removeCall(Call *call)
{
    for (std::vector<Call *>::iterator it = calls.begin();
         it != calls.end(); ++it)
    {
        if (*it == call) {
            calls.erase(it);
            break;
        }
    }
}
