#include <pjsua-lib/pjsua.h> 

#define THIS_FILE "APP" 

#define SIP_DOMAIN "192.168.1.198"
#define SIP_USER_0 "996891"
#define SIP_PASSWD_0 "Pin996891"
#define SIP_USER_1 "996892"
#define SIP_PASSWD_1 "Pin996892"
#define SIP_USER_2 "996893"
#define SIP_PASSWD_2 "Pin996893"
#define SIP_USER_3 "996894"
#define SIP_PASSWD_3 "Pin996894"

#define SND_DEV_IDX_0 8
#define SND_DEV_IDX_1 12
#define SND_DEV_IDX_2 16
#define SND_DEV_IDX_3 20

pj_pool_t *pool;
pjmedia_port *conf;
/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}
int find(char *src, char *key)
{
    int c;
    int b;
    char *str;
    str = strstr(src,key);
    if (str != NULL){
        c = (int) src;
        b = (int) str;
        return b-c;
    }
    return -1;
}
char *trim(char *src, int start, int end)
{
    char *val;
    val = malloc(end-start+1);
    for (int i = start; i < end; i++){
        val[i-start] = (char)src[i];
    }
    return val;
    }
/* Get call number from call info */
pj_str_t get_num_from_call(char *contact)
{
    pj_str_t val;
    char *temp;
    temp = trim(contact, find(contact, ":") + 1, find(contact, "@"));
    val = pj_str(temp);
}
/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);
    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
    (int)ci.remote_info.slen,
    ci.remote_info.ptr));
    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}
/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci; 
    PJ_UNUSED_ARG(e); 
    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
    (int)ci.state_text.slen,
    ci.state_text.ptr));
}
/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;
    pj_str_t contact_number;
    pjsua_call_get_info(call_id, &ci);
    contact_number = get_num_from_call(ci.local_contact.ptr);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        if (strcmp(contact_number.ptr, SIP_USER_0) == 0) {
            //PJ_LOG(1, (__FILE__, "=========== Match contact 0\n"));
            pj_status_t status;
            pjmedia_port *sc;
            pjmedia_port* rev;
            char errmsg[PJ_ERR_MSG_SIZE];
            int slot;

            //CREATE SOUND PORT
            pjmedia_snd_port *snd_port;
                status = pjmedia_snd_port_create(pool, SND_DEV_IDX_0, SND_DEV_IDX_0,
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &snd_port);

            // Create stereo-mono splitter/combiner
            status = pjmedia_splitcomb_create(pool, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &sc);

            /* Connect channel0 (left channel?) to conference port slot0 */
            status = pjmedia_splitcomb_set_channel(sc, 1/* ch0 */,0 /*options*/, conf);

            /* Create reverse channel for channel1 (right channel?)... */
            status = pjmedia_splitcomb_create_rev_channel(pool, sc, 0, 0, &rev);

            pjsua_conf_add_port(pool, rev, &slot);
            pjsua_conf_connect(pjsua_call_get_conf_port(call_id), slot);
            pjsua_conf_connect(slot, pjsua_call_get_conf_port(call_id));
            pjmedia_snd_port_connect(snd_port, sc);

        } else if (strcmp(contact_number.ptr, SIP_USER_1) == 0) {
            //PJ_LOG(1, (__FILE__, "=========== Match contact 1\n"));
            pj_status_t status;
            pjmedia_port *sc1;
            pjmedia_port* rev1;
            char errmsg[PJ_ERR_MSG_SIZE];
            int slot1;

            //CREATE SOUND PORT
            pjmedia_snd_port *snd_port1;
            status = pjmedia_snd_port_create(pool, SND_DEV_IDX_1, SND_DEV_IDX_1,
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &snd_port1);
            // Create stereo-mono splitter/combiner
            status = pjmedia_splitcomb_create(pool, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &sc1);

            /* Connect channel0 (left channel?) to conference port slot0 */
            status = pjmedia_splitcomb_set_channel(sc1, 1/* ch0 */,0 /*options*/, conf);
            /* Create reverse channel for channel1 (right channel?)... */
            status = pjmedia_splitcomb_create_rev_channel(pool, sc1, 0, 0, &rev1);

            pjsua_conf_add_port(pool, rev1, &slot1);
            pjsua_conf_connect(pjsua_call_get_conf_port(call_id), slot1);
            pjsua_conf_connect(slot1, pjsua_call_get_conf_port(call_id));
            pjmedia_snd_port_connect(snd_port1, sc1);

        } else if (strcmp(contact_number.ptr, SIP_USER_2) == 0) {
            //PJ_LOG(1, (__FILE__, "=========== Match contact 2\n"));
            pj_status_t status;
            pjmedia_port *sc2;
            pjmedia_port* rev2;
            char errmsg[PJ_ERR_MSG_SIZE];
            int slot2;

            //CREATE SOUND PORT
            pjmedia_snd_port *snd_port2;
            status = pjmedia_snd_port_create(pool, SND_DEV_IDX_2, SND_DEV_IDX_2,
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &snd_port2);
            if (status != PJ_SUCCESS) error_exit("Error pjmedia_snd_port_create \n", status);
            // Create stereo-mono splitter/combiner
            status = pjmedia_splitcomb_create(pool, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &sc2);

            if (status != PJ_SUCCESS) error_exit("Error pjmedia_splitcomb_create \n", status);
            /* Connect channel0 (left channel?) to conference port slot0 */
            status = pjmedia_splitcomb_set_channel(sc2, 1/* ch0 */,0 /*options*/, conf);
            if (status != PJ_SUCCESS) error_exit("Error pjmedia_splitcomb_set_channel \n", status);
            /* Create reverse channel for channel1 (right channel?)... */
            status = pjmedia_splitcomb_create_rev_channel(pool, sc2, 0, 0, &rev2);
            if (status != PJ_SUCCESS) error_exit("Error pjmedia_splitcomb_create_rev_channel\n", status);
            pjsua_conf_add_port(pool, rev2, &slot2);
            pjsua_conf_connect(pjsua_call_get_conf_port(call_id), slot2);
            pjsua_conf_connect(slot2, pjsua_call_get_conf_port(call_id));
            pjmedia_snd_port_connect(snd_port2, sc2);

        } else if (strcmp(contact_number.ptr, SIP_USER_3) == 0) {
            //PJ_LOG(1, (__FILE__, "=========== Match contact 3\n"));
            pj_status_t status;
            pjmedia_port *sc3;
            pjmedia_port* rev3;
            char errmsg[PJ_ERR_MSG_SIZE];
            int slot3;

            //CREATE SOUND PORT
            pjmedia_snd_port *snd_port3;
            status = pjmedia_snd_port_create(pool, SND_DEV_IDX_3, SND_DEV_IDX_3,
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &snd_port3);

            // Create stereo-mono splitter/combiner
            status = pjmedia_splitcomb_create(pool, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &sc3);

            /* Connect channel0 (left channel?) to conference port slot0 */
            status = pjmedia_splitcomb_set_channel(sc3, 1/* ch0 */,0 /*options*/, conf);
            /* Create reverse channel for channel1 (right channel?)... */
            status = pjmedia_splitcomb_create_rev_channel(pool, sc3, 0, 0, &rev3);

            pjsua_conf_add_port(pool, rev3, &slot3);
            pjsua_conf_connect(pjsua_call_get_conf_port(call_id), slot3);
            pjsua_conf_connect(slot3, pjsua_call_get_conf_port(call_id));
            pjmedia_snd_port_connect(snd_port3, sc3);

        } else {
            PJ_LOG(1, (__FILE__, "=========== Match no contact \n"));
        }
    }
}

int main(int argc, char *argv[])
{
    pjsua_acc_id acc_id;
    pj_status_t status;
    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* Init pjsua */
    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;
        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &on_incoming_call;
        cfg.cb.on_call_media_state = &on_call_media_state;
        cfg.cb.on_call_state = &on_call_state;
        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = 4;
        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }
    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;
        pjsua_transport_config_default(&cfg);
        cfg.port = 5060;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }
    /* Initialization is done, now start pjsua */
    status = pjsua_start();

    /* Create Pool*/
    pool = pjsua_pool_create("pool", 512, 512);

    /* Create conf*/
    conf = pjsua_set_no_snd_dev();

    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg[4];

        pjsua_acc_config_default(&cfg[0]);
        pjsua_acc_config_default(&cfg[1]);
        pjsua_acc_config_default(&cfg[2]);
        pjsua_acc_config_default(&cfg[3]);

        cfg[0].id = pj_str("sip:" SIP_USER_0 "@" SIP_DOMAIN);
        cfg[0].reg_uri = pj_str("sip:" SIP_DOMAIN);
        cfg[0].cred_count = 1;
        cfg[0].cred_info[0].realm = pj_str((char *)"*");
        cfg[0].cred_info[0].scheme = pj_str((char *)"digest");
        cfg[0].cred_info[0].username = pj_str(SIP_USER_0);
        cfg[0].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg[0].cred_info[0].data = pj_str(SIP_PASSWD_0);

        cfg[1].id = pj_str("sip:" SIP_USER_1 "@" SIP_DOMAIN);
        cfg[1].reg_uri = pj_str("sip:" SIP_DOMAIN);
        cfg[1].cred_count = 1;
        cfg[1].cred_info[0].realm = pj_str((char *)"*");
        cfg[1].cred_info[0].scheme = pj_str((char *)"digest");
        cfg[1].cred_info[0].username = pj_str(SIP_USER_1);
        cfg[1].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg[1].cred_info[0].data = pj_str(SIP_PASSWD_1);

        cfg[2].id = pj_str("sip:" SIP_USER_2 "@" SIP_DOMAIN);
        cfg[2].reg_uri = pj_str("sip:" SIP_DOMAIN);
        cfg[2].cred_count = 1;
        cfg[2].cred_info[0].realm = pj_str((char *)"*");
        cfg[2].cred_info[0].scheme = pj_str((char *)"digest");
        cfg[2].cred_info[0].username = pj_str(SIP_USER_2);
        cfg[2].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg[2].cred_info[0].data = pj_str(SIP_PASSWD_2);

        cfg[3].id = pj_str("sip:" SIP_USER_3 "@" SIP_DOMAIN);
        cfg[3].reg_uri = pj_str("sip:" SIP_DOMAIN);
        cfg[3].cred_count = 1;
        cfg[3].cred_info[0].realm = pj_str((char *)"*");
        cfg[3].cred_info[0].scheme = pj_str((char *)"digest");
        cfg[3].cred_info[0].username = pj_str(SIP_USER_3);
        cfg[3].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg[3].cred_info[0].data = pj_str(SIP_PASSWD_3);

        status = pjsua_acc_add(&cfg[0], PJ_TRUE, &acc_id);
        status = pjsua_acc_add(&cfg[1], PJ_TRUE, &acc_id);
        status = pjsua_acc_add(&cfg[2], PJ_TRUE, &acc_id);
        status = pjsua_acc_add(&cfg[3], PJ_TRUE, &acc_id);
    }

    /* Wait until user press "q" to quit. */
    for (;;) {
        char option[10]; 
        puts("Press 'h' to hangup all calls, 'q' to quit");
        if (fgets(option, sizeof(option), stdin) == NULL) {
            puts("EOF while reading stdin, will quit now..");
            break;
        }
        if (option[0] == 'q') break; 
        if (option[0] == 'h') pjsua_call_hangup_all();
    }
    /* Destroy pjsua */
    pjsua_destroy(); 
    return 0;
}