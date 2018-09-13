#include "pjsiptestserver.h"
#include <sstream>

#define THIS_FILE 	"pjsiptestserver.cpp"
#define PJSIP_SIP_PORT  5060
#define SEND_TIMES      2

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
    ans[0].rdata.srv.port = PJSIP_SIP_PORT;
    ans[0].rdata.srv.target = pj_str("sip.3402000000");

    pj_dns_resolver_add_entry( resv, &pkt, PJ_FALSE);

    ar[0].name = pj_str("sip.3402000000");
    ar[0].type = PJ_DNS_TYPE_A;
    ar[0].dnsclass = PJ_DNS_CLASS_IN;
    ar[0].ttl = 3600;
    ar[0].rdata.a.ip_addr = pj_inet_addr(pj_cstr(&tmp, "192.168.1.241"));

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

static pj_status_t lookup(pj_pool_t *pool,const pj_str_t *realm,const pj_str_t *acc_name,pjsip_cred_info *cred_info)
{
    pjsip_cred_info cred;
    pj_bzero(&cred, sizeof(cred));
    cred.realm = pj_str("3402000000");
    cred.scheme = pj_str("digest");
    cred.username = pj_str("34020000001110000001");
    cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cred.data = pj_str("12345678");
    *cred_info = cred;
    return PJ_SUCCESS;
}

static void aregistrar(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    const pjsip_authorization_hdr *auth;
    auth = (pjsip_authorization_hdr *)pjsip_msg_find_hdr(rdata->msg_info.msg,PJSIP_H_AUTHORIZATION, NULL);
    if (auth == NULL)
    {
        pjsip_endpt_create_response(pjsua_get_pjsip_endpt(),rdata, 401, NULL, &tdata);
        pjsip_www_authenticate_hdr *hdr;
        char nonce_buf[16];
        pj_str_t random;
        random.ptr = nonce_buf;
        random.slen = sizeof(nonce_buf);
        /* Create the header. */
        hdr = pjsip_www_authenticate_hdr_create(tdata->pool);
        hdr->scheme = pj_str("Digest");
        hdr->challenge.digest.algorithm = pj_str("md5");
        pj_create_random_string(nonce_buf, sizeof(nonce_buf));
        pj_strdup(tdata->pool, &hdr->challenge.digest.nonce, &random);
        pj_str_t auth = pj_str("3402000000");
        pj_strdup(tdata->pool, &hdr->challenge.digest.realm, &auth);
        pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*)hdr);
    }
    else
    {
        pjsip_auth_srv auth_srv;
        pjsip_auth_lookup_cred lookup;
        pjsip_auth_lookup_cred *lookuptmp = lookup;
        pjsip_auth_srv_init(rdata->tp_info.pool, &auth_srv, &auth->credential.digest.realm, lookuptmp, 0);
        int status_code;
        pjsip_auth_srv_verify(&auth_srv, rdata, &status_code);
        pjsip_endpt_create_response(pjsua_get_pjsip_endpt(), rdata, status_code, NULL, &tdata);
    }
    pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(),rdata, tdata, NULL, NULL);
}

static pj_bool_t default_mod_on_rx_request(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_register_method) == 0)
    {
        aregistrar(rdata);
        PJ_LOG(3, (THIS_FILE, "********** REGISTRATAR ***********\n\n\n"));
        return PJ_TRUE;
    }
    pjsip_endpt_create_response(pjsua_get_pjsip_endpt(), rdata, 200, NULL, &tdata);
    pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(), rdata, tdata, NULL, NULL);
    return PJ_TRUE;
}

static pjsip_module mod_default_handler =
{
    NULL, NULL,				/* prev, next.		*/
    { "mod-default-handler", 19 },	/* Name.		*/
    -1,					/* Id			*/
    PJSIP_MOD_PRIORITY_APPLICATION + 99,	/* Priority	        */
    NULL,				/* load()		*/
    NULL,				/* start()		*/
    NULL,				/* stop()		*/
    NULL,				/* unload()		*/
    &default_mod_on_rx_request,		/* on_rx_request()	*/
    NULL,				/* on_rx_response()	*/
    NULL,				/* on_tx_request.	*/
    NULL,				/* on_tx_response()	*/
    NULL,				/* on_tsx_state()	*/
};


int PjsipTestServer::sendCounter = 0;
int main()
{
    //// init
    Endpoint ep;
    ep.libCreate();

    EpConfig ep_cfg;
    ep_cfg.logConfig.level = 5;
    ep_cfg.uaConfig.userAgent = "server";
    ep.libInit(ep_cfg);

    pjsip_endpt_register_module(pjsua_get_pjsip_endpt(),&mod_default_handler);

    TransportConfig tcfg;
    int transportId;
    tcfg.port = 5060;
    transportId = ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

    //Add account
    AccountConfig acc_cfg;
    acc_cfg.idUri = "sip:34020000001200000001@3402000000";
    PjsipTestServer *acc = new PjsipTestServer;
    acc->create(acc_cfg);
    pj_thread_sleep(1000);

    //// run
    ep.libStart();
    std::cout << "********** PJSUA2 STARTING ***********" << std::endl;
    pj_thread_sleep(1000);

    //add buddy
    pjsip_endpoint *endpt = pjsua_get_pjsip_endpt();
    pj_dns_resolver *resv;
    pj_str_t nameserver;
    pj_uint16_t port = PJSIP_SIP_PORT;
    pjsip_endpt_create_resolver(endpt, &resv);
    nameserver = pj_str("192.168.1.241");
    pj_dns_resolver_set_ns(resv, 1, &nameserver, &port);
    pjsip_endpt_set_resolver(endpt, resv);
    add_dns_entries(resv);

    BuddyConfig cfg;
    cfg.uri = "sip:34020000001110000001@3402000000";
    cfg.subscribe = false;
    MyBuddy buddy;
    buddy.create(*acc, cfg);
    buddy.subscribePresence(false);
    buddy.onBuddyState();
    //Message Dragzoomin
    SendInstantMessageParam sprm;
    sprm.contentType = "Application/MANSCDP+xml";
    std::ostringstream strContent;
    strContent << "<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n";
    strContent << "<Control>\n";
    strContent << "    <CmdType>DeviceControl</CmdType>\n";
    strContent << "    <SN>" << 1 << "</SN>\n";
    strContent << "    <DeviceID>34020000001320000001</DeviceID>\n";
    strContent << "    <DragZoomIn>\n";
    strContent << "    </DragZoomIn>\n";
    strContent << "</Control>\n";
    sprm.content = strContent.str();

    std::cout << std::endl << "********** MESSAGE START ***********" << std::endl << std::endl;
    for(int i=0; i<SEND_TIMES; i++)
    {
        buddy.sendInstantMessage(sprm);
        std::cout << std::endl << "********** i = " << i << " ***********" << std::endl << std::endl;
    }
//    pj_thread_sleep(1000);
    std::cout << std::endl << "********** MESSAGE END ***********" << std::endl << std::endl;

    //// destroy
    ep.libDestroy();
    std::cout << std::endl<< "********** DESTORY ***********" << std::endl << std::endl;

    return 0;
}

