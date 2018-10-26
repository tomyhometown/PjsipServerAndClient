#ifndef TESTCLIENTACCOUNT_H
#define TESTCLIENTACCOUNT_H

#include <pjsua2.hpp>

using namespace pj;
class TestClientAccount : public Account
{
public:
    std::vector<Call *> calls;

public:
     // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    TestClientAccount();
    ~TestClientAccount();

    void removeCall(Call *call);
    virtual void onRegState(OnRegStateParam &prm);
};

#endif // TESTCLIENTACCOUNT_H
