#ifndef TESTSEVERACCOUNT_H
#define TESTSEVERACCOUNT_H

#include <pjsua2.hpp>
using namespace pj;

class TestSeverAccount : public Account
{
public:
    std::vector<Call *> calls;

public:
    // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    TestSeverAccount();
    ~TestSeverAccount();

    static int sendCounter;
    virtual void onInstantMessageStatus(OnInstantMessageStatusParam &prm);
    virtual void onIncomingCall(OnIncomingCallParam &iprm);
    virtual void onRegState(OnRegStateParam &prm);

    void removeCall(Call *call);
};

#endif // TESTSEVERACCOUNT_H
