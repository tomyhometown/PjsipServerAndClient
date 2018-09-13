#ifndef PJSIPTESTSERVER_H
#define PJSIPTESTSERVER_H

#include <pjsua2.hpp>
#include <iostream>
#include <string>
using namespace pj;

class PjsipTestServer : public Account
{
public:
    static int sendCounter;
public:
    // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    PjsipTestServer()
    {
        std::cout << "PjsipTestServer is created!" << std::endl;
    }

    ~PjsipTestServer()
    {
        std::cout << "PjsipTestServer is deleted!" << std::endl;
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
};

class MyBuddy : public Buddy
{
public:
    // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    MyBuddy()
    {
         std::cout << "MyBuddy is created!" << std::endl;
    }

    // Desstructor needs implement, if not, there will be a error "undefined reference to destructor".
    ~MyBuddy()
    {
         std::cout << "MyBuddy is deleted!" << std::endl;
    }

    virtual void onBuddyState()
    {
        BuddyInfo bi = getInfo();
        std::cout << "Buddy " << bi.uri << " is " << bi.presStatus.statusText << std::endl;
    }
};

#endif // PJSIPTESTSERVER_H
