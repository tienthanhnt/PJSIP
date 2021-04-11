 /* $Id$ */
 /*
  * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
  * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */
 
 #include <pjsua-lib/pjsua.h>
 
 #define THIS_FILE "APP"
 
 #define SIP_DOMAIN "192.168.1.198"
 #define SIP_USER_1 "996891"
 #define SIP_PASSWD_1 "Pin996891"

 #define SIP_USER_2 "996892"
 #define SIP_PASSWD_2 "Pin996892"

 #define SND_DEV_IDX_1 14
 
// cut chuoi string

int find(char *src, char *key){
  int c;
  int b;
  char *str;
  str = strstr(src,key);
  if (str != NULL){
    c = (int) src;
    b = (int) str;
  return b-c;
  }
  else return -1;
}
char *trim(char *src, int start, int end){
  char *val;
  val = malloc(end-start+1);
  int i;
  for (i = start; i < end; i++){
    val[i-start] = (char)src[i]; 
  }
  return val;
}
pj_str_t cat_chuoi(char *src){
  pj_str_t val;
  char *temp;
  temp = trim(src, find(src, ":") + 1, find(src, "@"));
  val = pj_str(temp);
}

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
  printf("=================local_contact = %s \n",ci.local_contact);
  printf("=================remote_contact = %s \n", ci.remote_contact);
  a = cat_chuoi(ci.local_contact.ptr);
  printf("=========================a = %s\n", a);

  //When media is active, connect call to sound device.
  //pjsua_get_conference() // get_master_port
  if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE && (strcmp(a.ptr, SIP_USER_1) == 0)) {
	  if (strcmp(a.ptr, SIP_USER_1) == 0) {
  	 	printf("===========================match phone 1");

                pjmedia_conf *conf_1 = NULL; // tao 1 pj media NULL
		pj_pool_t *pjsua_pool_1 = NULL;
  		pjsua_pool_1 = pjsua_pool_create("phone_pool_1", 512, 512); // tao pool de su dung
		pj_status_t status;

		//pjmedia_pool_1 =
		pjmedia_port *mp_1 = NULL; 
	        status = pjmedia_conf_create(pjsua_pool_1, 3, 16000, 1, 320, 16, PJMEDIA_CONF_NO_DEVICE, &conf_1); // tao ra 1 pj media conf
		if (status != PJ_SUCCESS) printf("Error in pjmedia_conf_create\n");
		mp_1 = pjmedia_conf_get_master_port(conf_1); // lay master port tu conf vua tao

		// tap ra 1 port media NULL va gan port do vao conf o tren
		pjmedia_port *sc_1 = NULL;
		unsigned int sc_1_slot;
	        status = pjmedia_conf_add_port(conf_1, pjsua_pool_1, sc_1, NULL, &sc_1_slot);
		if (status != PJ_SUCCESS) printf("Error in pjmedia_conf_add_port\n");

		// tao ra sound device port tu device index
		pjmedia_snd_port *snd_port_1 = NULL;
		status = pjmedia_snd_port_create(pjsua_pool_1, SND_DEV_IDX_1, SND_DEV_IDX_1, 8000, 2, 320, 16, 0, &snd_port_1);
		if (status != PJ_SUCCESS) printf("Error in pjmedia_snd_port_create");

                // connect port master cua conference voi port cua sound device 
		status = pjmedia_snd_port_connect(snd_port_1, mp_1);
		if (status != PJ_SUCCESS) printf("Error in pjmedia_snd_port_connect");
		
		// connect port cho sound device va port sc_1
		pjmedia_conf_connect_port(conf_1, sc_1, 0, 128);
		pjmedia_conf_connect_port(conf_1, 0, sc_1, 128);

		// Phan conf cho pjsua
		// lay ra source slot tu call id
		pjsua_conf_port_id source_slot = pjsua_call_get_conf_port(ci.id);
		// gan source slot day vao conf cua pjsua
		pjsua_conf_port_id p_id;
		pjsua_conf_connect(p_id, source_slot);

		// Phan gan p_id va sc_1
		//pjsua_conf_add_port(pjsua_pool_1, sc_1, &p_id);
	//	pjsua_conf_connect(p_id, source_slot);
	//        pjsua_conf_connect(source_slot, p_id);

	  } else {
	        printf("===========================match phone 2");
               // pjsua_conf_connect(ci.conf_slot, 0);
               // pjsua_conf_connect(0, ci.conf_slot);
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
 
 /*
  * main()
  *
  * argv[1] may contain URL to call.
  */
 int main(int argc, char *argv[])
 {
  pjsua_acc_id acc_id;
  pj_status_t status;

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
  if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);
 
  /* Register to SIP server by creating SIP account. */
  {
  pjsua_acc_config cfg[2];
 
  pjsua_acc_config_default(&cfg[0]);
  pjsua_acc_config_default(&cfg[1]);

  cfg[0].id = pj_str("sip:" SIP_USER_1 "@" SIP_DOMAIN);
  cfg[0].reg_uri = pj_str("sip:" SIP_DOMAIN);
  cfg[0].cred_count = 1;
  cfg[0].cred_info[0].realm = pj_str((char *)"*");
  cfg[0].cred_info[0].scheme = pj_str((char *)"digest");
  cfg[0].cred_info[0].username = pj_str(SIP_USER_1);
  cfg[0].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  cfg[0].cred_info[0].data = pj_str(SIP_PASSWD_1);
 
  cfg[1].id = pj_str("sip:" SIP_USER_2 "@" SIP_DOMAIN);
  cfg[1].reg_uri = pj_str("sip:" SIP_DOMAIN);
  cfg[1].cred_count = 1;
  cfg[1].cred_info[0].realm = pj_str((char *)"*");
  cfg[1].cred_info[0].scheme = pj_str((char *)"digest");
  cfg[1].cred_info[0].username = pj_str(SIP_USER_2);
  cfg[1].cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  cfg[1].cred_info[0].data = pj_str(SIP_PASSWD_2);


  status = pjsua_acc_add(&cfg[0], PJ_TRUE, &acc_id);
  if (status != PJ_SUCCESS) error_exit("Error adding account 0", status);
  status = pjsua_acc_add(&cfg[1], PJ_TRUE, &acc_id);
  if (status != PJ_SUCCESS) error_exit("Error adding account 1", status);
  }
 
  /* If URL is specified, make call to the URL. */
  // thanhnt: make 1 call succees
  /*
  PJ_LOG(1, (__FILE__, "Make call now"));
  pjsua_call_id call_id = PJSUA_INVALID_ID;
  char phone_c[100];
  pjsua_msg_data msg_data;
  pj_ansi_sprintf(phone_c, "<sip:%s@%s:%d>", "522601", "192.168.1.198", 5060);
  pj_str_t dst_uri = pj_str(phone_c);

  pjsua_msg_data_init(&msg_data);
  status = pjsua_call_make_call(acc_id, &dst_uri, 0, NULL, &msg_data, &call_id);
  if (status != PJ_SUCCESS) error_exit("Error making call", status);
  */
  // thanhnt

  //if (argc > 1) {
  //pj_str_t uri = pj_str(argv[1]);
  //status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
  //if (status != PJ_SUCCESS) error_exit("Error making call", status);
  //}
 
  /* Wait until user press "q" to quit. */
  for (;;) {
  char option[10];
 
  puts("Press 'h' to hangup all calls, 'q' to quit");
  if (fgets(option, sizeof(option), stdin) == NULL) {
  puts("EOF while reading stdin, will quit now..");
  break;
  }
 
  if (option[0] == 'q')
  break;
 
  if (option[0] == 'h')
  pjsua_call_hangup_all();
  }
 
  /* Destroy pjsua */
  pjsua_destroy();
 
  return 0;
 }
