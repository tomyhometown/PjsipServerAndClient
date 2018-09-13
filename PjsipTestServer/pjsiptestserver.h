#ifndef PJSIPTESTSERVER_H
#define PJSIPTESTSERVER_H

#include <pjsua2.hpp>
#include <iostream>
#include <string>
#include <sstream>
using namespace pj;

class PjsipTestServer : public Account
{
public:
    static int sendCounter;
public:
    PjsipTestServer()
    {
        sip_user = "server";
    }
    ~PjsipTestServer(){}
    std::string getUser()
    {
        return sip_user;
    }
    virtual void onInstantMessageStatus(OnInstantMessageStatusParam &prm)
    {
       std::cout << std::endl<< "********** SendMessage:code = " << prm.code << " ***********"<< std::endl << std::endl;
       if(200 == prm.code)
       {
           sendCounter++;
       }
       std::cout << std::endl<< "********** sendCounter = " << sendCounter << " ***********"<< std::endl << std::endl;
    }

private:
    std::string sip_user;
};

class MyBuddy : public Buddy
{
public:
    MyBuddy() {}
    ~MyBuddy() {}

    virtual void onBuddyState()
    {
        BuddyInfo bi = getInfo();
        std::cout << "Buddy " << bi.uri << " is " << bi.presStatus.statusText << std::endl;
    }
};

#endif // PJSIPTESTSERVER_H
