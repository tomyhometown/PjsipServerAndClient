#ifndef TESTSERVERBUDDY_H
#define TESTSERVERBUDDY_H

#include <pjsua2.hpp>
using namespace pj;

class TestServerBuddy : public Buddy
{
public:
    // Constructor needs implement, if not, there will be a error "undefined reference to constructor".
    TestServerBuddy();
    ~TestServerBuddy();
};

#endif // TESTSERVERBUDDY_H
