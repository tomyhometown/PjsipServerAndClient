#include "pjsiptestclient.h"

static void add_dns_entries(pj_dns_resolver *resv)
{
    /* Inject DNS SRV entry */
    pj_dns_parsed_packet pkt;
    pj_dns_parsed_query q;
    pj_dns_parsed_rr ans[2];
    pj_dns_parsed_rr ar[2];
    pj_str_t tmp;

    pj_bzero(&pkt, sizeof(pkt));
    pj_bzero(ans, sizeof(ans));
    pj_bzero(ar, sizeof(ar));

    pkt.hdr.flags = PJ_DNS_SET_QR(1);
    pkt.hdr.anscount = PJ_ARRAY_SIZE(ans);
    pkt.hdr.arcount = 0;
    pkt.ans = ans;
    pkt.arr = ar;

    ans[0].name = pj_str("_sip._udp.3402000000");
    ans[0].type = PJ_DNS_TYPE_SRV;
    ans[0].dnsclass = PJ_DNS_CLASS_IN;
    ans[0].ttl = 3600;
    ans[0].rdata.srv.prio = 0;
    ans[0].rdata.srv.weight = 0;
    ans[0].rdata.srv.port = 5060;
    ans[0].rdata.srv.target = pj_str("sip.3402000000");

    pj_dns_resolver_add_entry( resv, &pkt, PJ_FALSE);

    ar[0].name = pj_str("sip.3402000000");
    ar[0].type = PJ_DNS_TYPE_A;
    ar[0].dnsclass = PJ_DNS_CLASS_IN;
    ar[0].ttl = 3600;
    ar[0].rdata.a.ip_addr = pj_inet_addr(pj_cstr(&tmp, "192.168.1.145"));

    pj_bzero(&pkt, sizeof(pkt));
    pkt.hdr.flags = PJ_DNS_SET_QR(1);
    pkt.hdr.qdcount = 1;
    pkt.q = &q;
    q.name = ar[0].name;
    q.type = ar[0].type;
    q.dnsclass = PJ_DNS_CLASS_IN;
    pkt.hdr.anscount = 1;
    pkt.ans = &ar[0];

    pj_dns_resolver_add_entry( resv, &pkt, PJ_FALSE);

}

int main()
{
    //// init
    Endpoint ep;
    ep.libCreate();

    EpConfig ep_cfg;
    ep_cfg.logConfig.level = 5;
    ep_cfg.uaConfig.userAgent = "client";
    ep.libInit(ep_cfg);

    TransportConfig tcfg;
    int transportId;
    tcfg.port = 5060;
    transportId = ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

    //// run
    ep.libStart();
    std::cout << "********** PJSUA2 STARTING ***********" << std::endl;

    pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();
    pj_dns_resolver *resv;
    pj_str_t nameserver;
    pj_uint16_t port = 5060;
    pjsip_endpt_create_resolver(endpt, &resv);
    nameserver = pj_str("192.168.1.145");
    pj_dns_resolver_set_ns(resv, 1, &nameserver, &port);
    pjsip_endpt_set_resolver(endpt, resv);
    add_dns_entries(resv);

    //Add account
    AccountConfig acc_cfg;
    acc_cfg.idUri = "sip:34020000001110000001@3402000000";
    acc_cfg.regConfig.registrarUri = "sip:34020000001110000001@3402000000";
    AuthCredInfo cred("digest", "*", "34020000001110000001", 0, "12345678");
    acc_cfg.sipConfig.authCreds.push_back(cred);
    PjsipTestClient  *acc = new PjsipTestClient;
    acc->create(acc_cfg);
    std::cout << std::endl<< "********** REGISTER ***********" << std::endl << std::endl;

    pj_thread_sleep(10000);
    std::cout << std::endl<< "********** SLEEP END ***********" << std::endl << std::endl;

    //// destroy
    ep.libDestroy();
    std::cout << std::endl<< "********** DESTORY ***********" << std::endl << std::endl;

    return 0;
}
