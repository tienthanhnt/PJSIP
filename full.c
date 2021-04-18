#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_config.h"

#define THIS_FILE "APP"

pj_pool_t *pool;
pjmedia_port *conf;
pj_str_t cat_chuoi(char *src){
  pj_str_t val;
  char *temp;
  temp = trim(src, find(src, ":") + 1, find(src, "@"));
  val = pj_str(temp);
}
struct config ar_config[24];
 /* Callback called by the library upon receiving incoming call */
 static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
  pjsip_rx_data *rdata)
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
  pj_str_t a;
 
  pjsua_call_get_info(call_id, &ci);
  printf("=================pjsua_acc_id = %d \n", ci.acc_id);
  printf("=================local_contact = %s \n",ci.local_contact.ptr);
  printf("=================remote_contact = %s \n", ci.remote_contact.ptr);
  a = cat_chuoi(ci.local_contact.ptr);
  printf("=========================a = %s\n", a.ptr);
  printf("=========================mediacnt = %d\n",ci.media_cnt);

  //When media is active, connect call to sound device.
  if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
	  if (strcmp(a.ptr, ar_config[0].username) == 0) {
  	 	printf("===========================match phone 1\n");
        pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[0].slot);
        pjsua_conf_connect(ar_config[0].slot, pjsua_call_get_conf_port(call_id));
	  }
      else if (strcmp(a.ptr, ar_config[1].username) == 0) {
        printf("===========================match phone 2\n");
        pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[1].slot);
        pjsua_conf_connect(ar_config[1].slot, pjsua_call_get_conf_port(call_id));
	  }
  	}
 }
 /* Display error and exit application */
 static void error_exit(const char *title, pj_status_t status)
 {
  pjsua_perror(THIS_FILE, title, status);
  pjsua_destroy();
  exit(1);
 }
 int main(int argc, char *argv[])
 {
  pj_status_t status;
  int i;
  char errmsg[PJ_ERR_MSG_SIZE];

  // Read config
  read_config(ar_config,"config");

  // init PJLIB
  status = pj_init();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

  /* Create pjsua first! */
  status = pjsua_create();
  if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

  /* Init pjsua */
  {
  pjsua_config cfg;
  pjsua_logging_config log_cfg;
  //pjsua_media_config   media_cfg;
  //pjsua_media_config_default(&media_cfg);
  //media_cfg.channel_count=2;

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

  for (i = 0; i < 1; i++){
        printf("init sound device %d", i);
        status = pjmedia_snd_port_create(pool, ar_config[i].playback_dev_id, ar_config[i].capture_dev_id, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &ar_config[i].snd_port);
        if (status != PJ_SUCCESS) {
                pj_strerror(status, errmsg, sizeof(errmsg));
                PJ_LOG(1, (__FILE__, "Status %d: %s\n", status, errmsg));
                return ;
        }
        else {
                PJ_LOG(1, (__FILE__, "pjmedia_snd_port_create...OK"));
        }
        // Create stereo-mono splitter/combiner
        status = pjmedia_splitcomb_create(pool, 
                PJMEDIA_PIA_SRATE(&conf->info),
                2 /* stereo */,
                2 * PJMEDIA_PIA_SPF(&conf->info),
                PJMEDIA_PIA_BITS(&conf->info),
                0, &ar_config[i].sc);
        if (status != PJ_SUCCESS) {
                pj_strerror(status, errmsg, sizeof(errmsg));
                PJ_LOG(1, (__FILE__, "Status %d: %s\n", status, errmsg));
                return ;
        }
        else {
                PJ_LOG(1, (__FILE__, "pjmedia_splitcomb_create OK"));
        }
        /* Connect channel0 (left channel?) to conference port slot0 */
        status = pjmedia_splitcomb_set_channel(ar_config[i].sc, 1/* ch0 */,0 /*options*/, conf);
        if (status != PJ_SUCCESS) {
                pj_strerror(status, errmsg, sizeof(errmsg));
                PJ_LOG(1, (__FILE__, "Status %d: %s\n", status, errmsg));
                return 0;
        }
        else {
                PJ_LOG(1, (__FILE__, "pjmedia_splitcomb_set_channel OK"));
        }
        /* Create reverse channel for channel1 (right channel?)... */
        status = pjmedia_splitcomb_create_rev_channel(pool, ar_config[i].sc, 0, 0, &ar_config[i].rev);
        if (status != PJ_SUCCESS) {
                pj_strerror(status, errmsg, sizeof(errmsg));
                PJ_LOG(1, (__FILE__, "Status %d: %s\n", status, errmsg));
                return 0;
        }
        else {
                PJ_LOG(1, (__FILE__, "pjmedia_splitcomb_create_rev_channel...OK"));
        }
        pjsua_conf_add_port(pool, ar_config[i].rev, &ar_config[i].slot);
        pjmedia_snd_port_connect(ar_config[i].snd_port, ar_config[i].sc);
  }
  for(i = 0; i < 1; i++){
        pjsua_acc_config cfg;
        pjsua_acc_config_default(&cfg);
        char id[100];
        char reg_uri[100];
        sprintf(id,"sip:%s@%s", ar_config[i].username, ar_config[i].domain);
        cfg.id = pj_str(id);
        sprintf(reg_uri, "sip:%s", ar_config[i].domain);
        cfg.reg_uri = pj_str(reg_uri);
        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str((char *)"*");
        cfg.cred_info[0].scheme = pj_str((char *)"digest");
        cfg.cred_info[0].username = pj_str(ar_config[i].username);
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(ar_config[i].secret);
        printf ("id : %s, reg_uri : %s, secret : %s \n",cfg.id.ptr,cfg.reg_uri.ptr,cfg.cred_info[0].data.ptr);
        status = pjsua_acc_add(&cfg, PJ_TRUE, &ar_config[i].acc_id);
        if (status != PJ_SUCCESS) error_exit("Error adding account 0", status);
  }
  for (;;) {
  char option[10];
 
  puts("Press 'h' to hangup all calls,'m' to make a call, 'q' to quit");
  if (fgets(option, sizeof(option), stdin) == NULL) {
  puts("EOF while reading stdin, will quit now..");
  break;
  }
 
  if (option[0] == 'q')
  break;
  if (option[0] == 'm'){
    pj_str_t uri1 = pj_str("sip:996892@192.168.1.50");
    status = pjsua_call_make_call(ar_config[0].acc_id, &uri1, 0, NULL, NULL, NULL);
  }
 
  if (option[0] == 'h')
  pjsua_call_hangup_all();
 }
}