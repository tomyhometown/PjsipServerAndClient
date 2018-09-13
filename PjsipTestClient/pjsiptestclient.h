#ifndef PJSIPTESTCLIENT_H
#define PJSIPTESTCLIENT_H

#include <pjsua2.hpp>
#include <iostream>
#include <string>
using namespace pj;

class PjsipTestClient : public Account
{
public:
    // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    PjsipTestClient()
    {
        std::cout << "PjsipTestClient is being created!" << std::endl;
    }

    ~PjsipTestClient()
    {
        std::cout << "PjsipTestClient is being deleted!" << std::endl;
     }

    virtual void onRegState(OnRegStateParam &prm)
    {
    AccountInfo ai = getInfo();
    std::cout << std::endl<<   (ai.regIsActive? "************* Register: code=" :
                                                "************* Unregister: code=")  << prm.code << "*************" << std::endl << std::endl;
    }
};

#endif // PJSIPTESTCLIENT_H
