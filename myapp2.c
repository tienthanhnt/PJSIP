/* $Id$ */
 /*
3  * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
4  * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
5  *
6  * This program is free software; you can redistribute it and/or modify
7  * it under the terms of the GNU General Public License as published by
8  * the Free Software Foundation; either version 2 of the License, or
9  * (at your option) any later version.
10  *
11  * This program is distributed in the hope that it will be useful,
12  * but WITHOUT ANY WARRANTY; without even the implied warranty of
13  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
14  * GNU General Public License for more details.
15  *
16  * You should have received a copy of the GNU General Public License
17  * along with this program; if not, write to the Free Software
18  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
19  */
#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_config.h"
#define THIS_FILE "APP"
#define THIS_FILE "APP"

 /* read config file */
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
 
  pjsua_call_get_info(call_id, &ci);
 
  if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
  // When media is active, connect call to sound device.
  pjsua_conf_connect(ci.conf_slot, 0);
  pjsua_conf_connect(0, ci.conf_slot);
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
  int i;
 
  /* Create pjsua first! */
  status = pjsua_create();
  if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);
 
  /* If argument is specified, it's got to be a valid SIP URL */
  if (argc > 1) {
  status = pjsua_verify_url(argv[1]);
  if (status != PJ_SUCCESS) error_exit("Invalid URL in argv", status);
  }
 
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
  read_config(ar_config, "config");
  printf("%s\n", ar_config[0].username);
  for (i = 0; i < 24 ; i++)
  {
  pjsua_acc_config cfg;
 
  pjsua_acc_config_default(&cfg);
  char id[100];
  char reg_uri[100] = "sip:";
  printf("account %d username %s", i, ar_config[i].username);
  sprintf(id,"sip:%s@%s", ar_config[i].username, ar_config[i].domain);
  cfg.id = pj_str(id);
  printf("%s\n" , cfg.id.ptr);
  sprintf(reg_uri, "sip:%s", ar_config[i].domain);
  cfg.reg_uri = pj_str(reg_uri);
  printf("%s\n" , cfg.reg_uri.ptr);
  cfg.cred_count = 1;
  cfg.cred_info[0].realm =  pj_str((char *)"*");
  cfg.cred_info[0].scheme = pj_str((char *)"digest");
  cfg.cred_info[0].username = pj_str(ar_config[i].username);
  cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  cfg.cred_info[0].data = pj_str(ar_config[i].secret);
 
  status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
  if (status != PJ_SUCCESS) error_exit("Error adding account", status);
  }
 
  /* If URL is specified, make call to the URL. */
  if (argc > 1) {
  pj_str_t uri = pj_str(argv[1]);
  status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
  if (status != PJ_SUCCESS) error_exit("Error making call", status);
  }
 
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