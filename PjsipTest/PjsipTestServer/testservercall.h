#ifndef TESTSERVERCALL_H
#define TESTSERVERCALL_H

#include <pjsua2.hpp>
#include "testseveraccount.h"

using namespace pj;

class TestSeverAccount;
class TestServerCall : public Call
{
private:
    TestSeverAccount *myAcc;
public:
    TestServerCall(Account &acc, int call_id = PJSUA_INVALID_ID);
    ~TestServerCall();

    virtual void onCallState(OnCallStateParam &prm);
    virtual void onCallMediaState(OnCallMediaStateParam &/*prm*/);
    virtual void onCallSdpCreated(OnCallSdpCreatedParam &prm);
//    virtual void onCallTsxState(OnCallTsxStateParam &prm);
    PJ_DEF(pj_status_t) sdp_parse( pj_pool_t *pool,
                           char *buf, pj_size_t len,
                           pjmedia_sdp_session **p_sdp);
//    virtual void onStreamCreated(OnStreamCreatedParam &prm);

};

#endif // TESTSERVERCALL_H
