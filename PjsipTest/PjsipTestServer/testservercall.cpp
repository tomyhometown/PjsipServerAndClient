#include "testservercall.h"
#include <iostream>
#define THIS_FILE	"sdp.c"
#define TOKEN		"!#$%&'*+-.^_`{|}~"

TestServerCall::TestServerCall(Account &acc, int call_id)
: Call(acc, call_id)
{
    std::cout << "TestServerCall is created!" << std::endl;
    myAcc = (TestSeverAccount *)&acc;
}

TestServerCall::~TestServerCall()
{
}

void TestServerCall::onCallState(OnCallStateParam &prm)
{
    PJ_UNUSED_ARG(prm);

    CallInfo ci = getInfo();
    std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
              << "]" << std::endl;

//    for (unsigned i = 0; i < ci.provMedia.size(); i++)
//    {
//        std::cout << ci.provMedia[i].index << " : " << ci.provMedia[i].status << std::endl;
//    }

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
        myAcc->removeCall(this);
        /* Delete the call */
        delete this;
    }
}

//void onCallTsxState(OnCallTsxStateParam &prm)
//{

//}

void TestServerCall::onCallMediaState(OnCallMediaStateParam &/*prm*/)
{
    CallInfo ci = getInfo();
    // Iterate all the call medias
    for (unsigned i = 0; i < ci.media.size(); i++)
    {
        std::cout << ci.media[i].index << " : " << ci.media[i].status << std::endl;
    }
}

void TestServerCall::onCallSdpCreated(OnCallSdpCreatedParam &prm)
{
    prm.sdp.wholeSdp =
            "v=0\n"
            "o=34020000002020000001 0 0 IN IP4 192.168.1.212\n"
            "s=Play\n"
            "c=IN IP4 192.168.1.212\n"
            "t=0 0\n"
            "m=video 6000 RTP/AVP 96\n"
            "a=recvonly\n"
            "a=rtpmap:96 PS/90000\n";
}

//    pj_pool_t *pool = pjsua_pool_create("pjsua-app", 1000, 1000);

//    pjmedia_sdp_session *new_sdp;
//    pj_str_t dup_new_sdp;
//    pj_str_t new_sdp_str = {(char*)prm.sdp.wholeSdp.c_str(),
//                (pj_ssize_t)prm.sdp.wholeSdp.size()};

//    pj_strdup(pool, &dup_new_sdp, &new_sdp_str);

//    sdp_parse(pool, dup_new_sdp.ptr,
//                       dup_new_sdp.slen, &new_sdp);

static int is_initialized;
static pj_cis_buf_t cis_buf;
static pj_cis_t cs_digit, cs_token;

enum {
    SKIP_WS = 0,
    SYNTAX_ERROR = 1,
};

typedef struct parse_context
{
    pj_status_t last_error;
} parse_context;

static void pj_scan_syntax_err(pj_scanner *scanner)
{
    (*scanner->callback)(scanner);
}

static void init_sdp_parser(void)
{
    if (is_initialized != 0)
    return;

//    pj_enter_critical_section();

//    if (is_initialized != 0) {
//    pj_leave_critical_section();
//    return;
//    }

    pj_cis_buf_init(&cis_buf);

    pj_cis_init(&cis_buf, &cs_token);
    pj_cis_add_alpha(&cs_token);
    pj_cis_add_num(&cs_token);
    pj_cis_add_str(&cs_token, TOKEN);

    pj_cis_init(&cis_buf, &cs_digit);
    pj_cis_add_num(&cs_digit);

    is_initialized = 1;
//    pj_leave_critical_section();
}

#define PJ_SCAN_IS_PROBABLY_SPACE(c)	((c) <= 32)

PJ_DEF(void) scan_get( pj_scanner *scanner,
              const pj_cis_t *spec, pj_str_t *out)
{
    register char *s = scanner->curptr;

    pj_assert(pj_cis_match(spec,0)==0);

    /* EOF is detected implicitly */
    if (!pj_cis_match(spec, *s)) {
    pj_scan_syntax_err(scanner);
    return;
    }

    do {
    ++s;
    } while (pj_cis_match(spec, *s));
    /* No need to check EOF here (PJ_SCAN_CHECK_EOF(s)) because
     * buffer is NULL terminated and pj_cis_match(spec,0) should be
     * false.
     */

    pj_strset3(out, scanner->curptr, s);

    scanner->curptr = s;

    if (PJ_SCAN_IS_PROBABLY_SPACE(*s) && scanner->skip_ws) {
    pj_scan_skip_whitespace(scanner);
    }
}

static void on_scanner_error(pj_scanner *scanner)
{
    PJ_UNUSED_ARG(scanner);

    PJ_THROW(SYNTAX_ERROR);
}

static void parse_version(pj_scanner *scanner, parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINVER;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return;
    }

    /* check version is 0 */
    if (*(scanner->curptr+2) != '0') {
    on_scanner_error(scanner);
    return;
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}

static void parse_origin(pj_scanner *scanner, pjmedia_sdp_session *ses,
             parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINORIGIN;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return;
    }

    /* o= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* username. */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.user);
    pj_scan_get_char(scanner);

    /* id */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->origin.id = pj_strtoul(&str);
    pj_scan_get_char(scanner);

    /* version */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->origin.version = pj_strtoul(&str);
    pj_scan_get_char(scanner);

    /* network-type */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.net_type);
    pj_scan_get_char(scanner);

    /* addr-type */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.addr_type);
    pj_scan_get_char(scanner);

    /* address */
    pj_scan_get_until_chr(scanner, " \t\r\n", &ses->origin.addr);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);

}

static void parse_time(pj_scanner *scanner, pjmedia_sdp_session *ses,
               parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINTIME;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return;
    }

    /* t= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* start time */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->time.start = pj_strtoul(&str);

    pj_scan_get_char(scanner);

    /* stop time */
    pj_scan_get_until_chr(scanner, " \t\r\n", &str);
    ses->time.stop = pj_strtoul(&str);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}

static void parse_generic_line(pj_scanner *scanner, pj_str_t *str,
                   parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINSDP;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return;
    }

    /* x= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* get anything until newline (including whitespaces). */
    pj_scan_get_until_chr(scanner, "\r\n", str);

    /* newline. */
    pj_scan_get_newline(scanner);
}

static void parse_connection_info(pj_scanner *scanner, pjmedia_sdp_conn *conn,
                  parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINCONN;

    /* c= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* network-type */
    pj_scan_get_until_ch(scanner, ' ', &conn->net_type);
    pj_scan_get_char(scanner);

    /* addr-type */
    pj_scan_get_until_ch(scanner, ' ', &conn->addr_type);
    pj_scan_get_char(scanner);

    /* address. */
    pj_scan_get_until_chr(scanner, "/ \t\r\n", &conn->addr);
    PJ_TODO(PARSE_SDP_CONN_ADDRESS_SUBFIELDS);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}

static void parse_bandwidth_info(pj_scanner *scanner, pjmedia_sdp_bandw *bandw,
                  parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINBANDW;

    /* b= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* modifier */
    pj_scan_get_until_ch(scanner, ':', &bandw->modifier);
    pj_scan_get_char(scanner);

    /* value */
    pj_scan_get_until_chr(scanner, " \t\r\n", &str);
    bandw->value = pj_strtoul(&str);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}

static void parse_media(pj_scanner *scanner, pjmedia_sdp_media *med,
            parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINMEDIA;

    /* check the equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return;
    }

    /* m= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* type */
    pj_scan_get_until_ch(scanner, ' ', &med->desc.media);
    pj_scan_get_char(scanner);

    /* port */
    scan_get(scanner, &cs_token, &str);
    med->desc.port = (unsigned short)pj_strtoul(&str);
    if (*scanner->curptr == '/') {
    /* port count */
    pj_scan_get_char(scanner);
    pj_scan_get(scanner, &cs_token, &str);
    med->desc.port_count = pj_strtoul(&str);

    } else {
    med->desc.port_count = 0;
    }

    if (pj_scan_get_char(scanner) != ' ') {
    PJ_THROW(SYNTAX_ERROR);
    }

    /* transport */
    pj_scan_get_until_chr(scanner, " \t\r\n", &med->desc.transport);

    /* format list */
    med->desc.fmt_count = 0;
    while (*scanner->curptr == ' ') {
    pj_str_t fmt;

    pj_scan_get_char(scanner);

    /* Check again for the end of the line */
    if ((*scanner->curptr == '\r') || (*scanner->curptr == '\n'))
        break;

    pj_scan_get(scanner, &cs_token, &fmt);
    if (med->desc.fmt_count < PJMEDIA_MAX_SDP_FMT)
        med->desc.fmt[med->desc.fmt_count++] = fmt;
    else
        PJ_PERROR(2,(THIS_FILE, PJ_ETOOMANY,
                 "Error adding SDP media format %.*s, "
             "format is ignored",
             (int)fmt.slen, fmt.ptr));
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}

static pjmedia_sdp_attr *parse_attr( pj_pool_t *pool, pj_scanner *scanner,
                    parse_context *ctx)
{
    pjmedia_sdp_attr *attr;

    ctx->last_error = PJMEDIA_SDP_EINATTR;

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
    on_scanner_error(scanner);
    return NULL;
    }

    /* skip a= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* get attr name. */
    pj_scan_get(scanner, &cs_token, &attr->name);

    if (*scanner->curptr && *scanner->curptr != '\r' &&
    *scanner->curptr != '\n')
    {
    /* skip ':' if present. */
    if (*scanner->curptr == ':')
        pj_scan_get_char(scanner);

    /* get value */
    if (*scanner->curptr != '\r' && *scanner->curptr != '\n') {
        pj_scan_get_until_chr(scanner, "\r\n", &attr->value);
    } else {
        attr->value.ptr = NULL;
        attr->value.slen = 0;
    }

    } else {
    attr->value.ptr = NULL;
    attr->value.slen = 0;
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);

    return attr;
}

PJ_DEF(pj_status_t) TestServerCall::sdp_parse( pj_pool_t *pool,
                       char *buf, pj_size_t len,
                       pjmedia_sdp_session **p_sdp)
{
    pj_scanner scanner;
    pjmedia_sdp_session *session;
    pjmedia_sdp_media *media = NULL;
    pjmedia_sdp_attr *attr;
    pjmedia_sdp_conn *conn;
    pjmedia_sdp_bandw *bandw;
    pj_str_t dummy;
    int cur_name = 254;
    parse_context ctx;
    PJ_USE_EXCEPTION;

    ctx.last_error = PJ_SUCCESS;

    init_sdp_parser();

    pj_scan_init(&scanner, buf, len, 0, &on_scanner_error);
    session = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_session);
    PJ_ASSERT_RETURN(session != NULL, PJ_ENOMEM);

    /* Ignore leading newlines */
    while (*scanner.curptr=='\r' || *scanner.curptr=='\n')
    pj_scan_get_char(&scanner);

    PJ_TRY {
    while (!pj_scan_is_eof(&scanner)) {
        cur_name = *scanner.curptr;
        switch (cur_name) {
        case 'a':
            attr = parse_attr(pool, &scanner, &ctx);
            if (attr) {
            if (media) {
                if (media->attr_count < PJMEDIA_MAX_SDP_ATTR)
                pjmedia_sdp_media_add_attr(media, attr);
                else
                PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
                          "Error adding media attribute, "
                          "attribute is ignored"));
            } else {
                if (session->attr_count < PJMEDIA_MAX_SDP_ATTR)
                pjmedia_sdp_session_add_attr(session, attr);
                else
                PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
                          "Error adding session attribute"
                          ", attribute is ignored"));
            }
            }
            break;
        case 'o':
            parse_origin(&scanner, session, &ctx);
            break;
        case 's':
            parse_generic_line(&scanner, &session->name, &ctx);
            break;
        case 'c':
            conn = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_conn);
            parse_connection_info(&scanner, conn, &ctx);
            if (media) {
            media->conn = conn;
            } else {
            session->conn = conn;
            }
            break;
        case 't':
            parse_time(&scanner, session, &ctx);
            break;
        case 'm':
            media = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_media);
            parse_media(&scanner, media, &ctx);
            if (session->media_count < PJMEDIA_MAX_SDP_MEDIA)
            session->media[ session->media_count++ ] = media;
            else
            PJ_PERROR(2,(THIS_FILE, PJ_ETOOMANY,
                     "Error adding media, media is ignored"));
            break;
        case 'v':
            parse_version(&scanner, &ctx);
            break;
        case 13:
        case 10:
            pj_scan_get_char(&scanner);
            /* Allow empty newlines at the end of the message */
            while (!pj_scan_is_eof(&scanner)) {
            if (*scanner.curptr != 13 && *scanner.curptr != 10) {
                ctx.last_error = PJMEDIA_SDP_EINSDP;
                on_scanner_error(&scanner);
            }
            pj_scan_get_char(&scanner);
            }
            break;
        case 'b':
            bandw = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_bandw);
            parse_bandwidth_info(&scanner, bandw, &ctx);
            if (media) {
            if (media->bandw_count < PJMEDIA_MAX_SDP_BANDW)
                media->bandw[media->bandw_count++] = bandw;
            else
                PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
                      "Error adding media bandwidth "
                      "info, info is ignored"));
            } else {
            if (session->bandw_count < PJMEDIA_MAX_SDP_BANDW)
                session->bandw[session->bandw_count++] = bandw;
            else
                PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
                      "Error adding session bandwidth "
                      "info, info is ignored"));
            }
            break;
        default:
            if (cur_name >= 'a' && cur_name <= 'z')
                parse_generic_line(&scanner, &dummy, &ctx);
            else  {
            ctx.last_error = PJMEDIA_SDP_EINSDP;
            on_scanner_error(&scanner);
            }
            break;
        }
    }

    ctx.last_error = PJ_SUCCESS;

    }
    PJ_CATCH_ANY {

    char errmsg[PJ_ERR_MSG_SIZE];
    pj_strerror(ctx.last_error, errmsg, sizeof(errmsg));

    PJ_LOG(4, (THIS_FILE, "Error parsing SDP in line %d col %d: %s",
           scanner.line, pj_scan_get_col(&scanner),
           errmsg));

    session = NULL;

    pj_assert(ctx.last_error != PJ_SUCCESS);
    }
    PJ_END;

    pj_scan_fini(&scanner);

//    if (session)
//    apply_media_direction(session);

    *p_sdp = session;
    return ctx.last_error;
}

//void TestServerCall::onStreamCreated(OnStreamCreatedParam &prm)
//{

//}
