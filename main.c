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

#define SND_DEV_IDX_1 8
#define SND_DEV_IDX_2 12
#define SND_DEV_IDX_3 16
#define SND_DEV_IDX_4 20

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
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
    pjsua_call_get_info(call_id, &ci); 
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
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