#include "testclientaccount.h"
#include <iostream>

TestClientAccount::TestClientAccount()
{
    std::cout << "TestClientAccount is created!" << std::endl;
}

TestClientAccount::~TestClientAccount()
{
    std::cout << "TestClientAccount is deleted!" << std::endl;
 }

void TestClientAccount::removeCall(Call *call)
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

void TestClientAccount::onRegState(OnRegStateParam &prm)
{
AccountInfo ai = getInfo();
std::cout << std::endl<<   "************* Register: code= "  << prm.code << "*************" << std::endl << std::endl;
}
