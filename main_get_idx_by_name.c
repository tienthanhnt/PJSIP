#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_config.h"

#define 	PJMEDIA_SND_DEFAULT_REC_LATENCY   1000
#define 	PJMEDIA_SND_DEFAULT_PLAY_LATENCY   1400

#define THIS_FILE "APP"

pj_pool_t *pool;
pjmedia_port *conf;
pj_str_t get_num_from_call(char *contact){
	pj_str_t val;
	char *temp;
	temp = trim(contact, find(contact, ":") + 1, find(contact, "@"));
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
	pj_str_t contact_number;

	pjsua_call_get_info(call_id, &ci);
	contact_number = get_num_from_call(ci.local_contact.ptr);
	printf("=========================mediacnt = %d\n",ci.media_cnt);

	//When media is active, connect call to sound device.
	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		if (strcmp(contact_number.ptr, ar_config[0].username) == 0) {
			pjsua_conf_connect(ci.conf_slot, ar_config[0].slot);
			pjsua_conf_connect(ar_config[0].slot, ci.conf_slot);
			//pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[0].slot_1);
			//pjsua_conf_connect(ar_config[0].slot_1, pjsua_call_get_conf_port(call_id));
		}
		else if (strcmp(contact_number.ptr, ar_config[1].username) == 0) {
			pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[1].slot);
			pjsua_conf_connect(ar_config[1].slot, pjsua_call_get_conf_port(call_id));
			//pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[1].slot_1);
			//pjsua_conf_connect(ar_config[1].slot_1, pjsua_call_get_conf_port(call_id));
		}
		else if (strcmp(contact_number.ptr, ar_config[2].username) == 0) {
			pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[2].slot);
			pjsua_conf_connect(ar_config[2].slot, pjsua_call_get_conf_port(call_id));
		}
		else if (strcmp(contact_number.ptr, ar_config[3].username) == 0) {
			pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[3].slot);
			pjsua_conf_connect(ar_config[3].slot, pjsua_call_get_conf_port(call_id));
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

		// config pj_media
		pjsua_media_config   media_cfg;
		pjsua_media_config_default(&media_cfg);
		//media_cfg.no_vad = PJ_TRUE;
		media_cfg.quality = 10;
		media_cfg.clock_rate = 48000;
		media_cfg.audio_frame_ptime = 20;
		media_cfg.channel_count = 2;
		media_cfg.ec_options = PJMEDIA_ECHO_USE_SW_ECHO;

		pjsua_config_default(&cfg);
		cfg.cb.on_incoming_call = &on_incoming_call;
		cfg.cb.on_call_media_state = &on_call_media_state;
		cfg.cb.on_call_state = &on_call_state;
		cfg.max_calls = 32;
		pj_str_t codec_id;
		//codec_id = pj_str((char*)"");
		//codec_id = pj_str((char*)"G729/8000/1");;
		//status = pjsua_codec_set_priority(&codec_id, PJMEDIA_CODEC_PRIO_HIGHEST);
		//if (status != PJ_SUCCESS) printf("=============== set codec failed");
		//pjmedia_endpt endpt;`
		//pjmedia_codec_param param;

		pjsua_logging_config_default(&log_cfg);
		log_cfg.console_level = 4;

		status = pjsua_init(&cfg, &log_cfg, NULL);
		if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
	}

	/* Add UDP transport. */
	pjsua_transport_id cfg_transport_id[2];
	pjsua_transport_config cfg_transport[2];

	pjsua_transport_config_default(&cfg_transport[0]);
	cfg_transport[0].port = 5060;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg_transport[0], &cfg_transport_id[0]);
	if (status != PJ_SUCCESS) error_exit("Error creating transport", status);

	pjsua_transport_config_default(&cfg_transport[1]);
	cfg_transport[1].port = 5061;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg_transport[1], &cfg_transport_id[1]);
	if (status != PJ_SUCCESS) error_exit("Error creating transport", status);

	/* Initialization is done, now start pjsua */
	status = pjsua_start();

	pool = pjsua_pool_create("pool", 2048, 2048);
	conf = pjsua_set_no_snd_dev();
	char devAudioName_0[30];
	char devAudioName_1[30];
	char drvName[5];
	pj_ansi_strcpy(devAudioName_0, "sysdefault:CARD=Device");
	pj_ansi_strcpy(devAudioName_1, "sysdefault:CARD=Device_1");
	pj_ansi_strcpy(drvName, "alsa");

	for (int i = 0; i < 2; i++){
		printf("init sound device %d", i);
		ar_config[i].snd_port = NULL;
		switch (i) {
			case 0:
				printf("====================== case i = %d \n", i);
				pj_ansi_strcpy(ar_config[i].devName, "sysdefault:CARD=Device");
				break;
			case 1:
				printf("====================== case i = %d \n", i);
				pj_ansi_strcpy(ar_config[i].devName, "sysdefault:CARD=Device_1");
				break;
			default:
				printf("======================= no match \n");	
		}

		pjmedia_aud_dev_lookup(drvName, ar_config[i].devName, &ar_config[i].playback_dev_id);
		printf("=================================== index = %d\n", ar_config[i].playback_dev_id);

		status = pjmedia_snd_port_create(pool, ar_config[i].playback_dev_id, ar_config[i].playback_dev_id, 
				16000,
				2 /* stereo */,
				2 * 320,
				16,
				0, &ar_config[i].snd_port);

		pjmedia_snd_port_set_ec(ar_config[i].snd_port, pool, 300, PJMEDIA_ECHO_SIMPLE);
		// Create stereo-mono splitter/combiner
		status = pjmedia_splitcomb_create(pool, 
				16000,
				2 /* stereo */,
				2 * 320,
				16,
				0, &ar_config[i].sc);
//		printf("=======================> pjmedia_port clock rate = %d \n", PJMEDIA_PIA_SRATE(&conf->info));
//		printf("=======================> pjmedia_port sample per frame = %d \n", PJMEDIA_PIA_SPF(&conf->info));
//		printf("=======================> pjmedia_port number bit per sample = %d \n", PJMEDIA_PIA_BITS(&conf->info));

		/* Connect channel0 (left channel?) to conference port slot0 */
		status = pjmedia_splitcomb_set_channel(ar_config[i].sc, 0, 0, conf);
		
		/* Create reverse channel for channel1 (right channel?)... */
		status = pjmedia_splitcomb_create_rev_channel(pool, ar_config[i].sc, 1, 0, &ar_config[i].rev);

		//status = pjmedia_splitcomb_set_channel(ar_config[i].sc, 1, 0, conf);
		//status = pjmedia_splitcomb_create_rev_channel(pool, ar_config[i].sc, 0, 0, &ar_config[i].rev_1);

		pjsua_conf_add_port(pool, ar_config[i].rev, &ar_config[i].slot);
		printf("===================================== slot = %d", ar_config[i].slot);
		//pjsua_conf_add_port(pool, ar_config[i].rev_1, &ar_config[i].slot_1);
		pjmedia_snd_port_connect(ar_config[i].snd_port, ar_config[i].sc);

		/* Create pjmedia conf and add to pjsua */
		/*pjmedia_conf_create(pool, 3, PJMEDIA_PIA_SRATE(&conf->info),
							2, 2 * PJMEDIA_PIA_SPF(&conf->info), 16, 
							PJMEDIA_CONF_NO_DEVICE, &ar_config[i].media_conf_2);
		ar_config[i].mp_2 = pjmedia_conf_get_master_port(ar_config[i].media_conf_1);
		status = pjmedia_splitcomb_create(pool, 
				PJMEDIA_PIA_SRATE(&conf->info),
				1,
				2 * PJMEDIA_PIA_SPF(&conf->info),
				PJMEDIA_PIA_BITS(&conf->info),
				0, &ar_config[i].sc_2);*/

		/* Create pjmedia conf and add port, then connect */
		/*
		pjmedia_conf_create(pool, 3, PJMEDIA_PIA_SRATE(&conf->info),
							2, 2 * PJMEDIA_PIA_SPF(&conf->info), 16, 
							PJMEDIA_CONF_NO_DEVICE, &ar_config[i].media_conf);
		ar_config[i].mp = pjmedia_conf_get_master_port(ar_config[i].media_conf);
		pjmedia_snd_port_connect(ar_config[i].snd_port, ar_config[i].mp);
		pjmedia_conf_add_port(ar_config[i].media_conf, pool, 
							ar_config[i].sc, NULL, &ar_config[i].p_slot);
		pjmedia_conf_connect_port(ar_config[i].media_conf, 0, ar_config[i].p_slot, 0);
		pjmedia_conf_connect_port(ar_config[i].media_conf, ar_config[i].p_slot, 0, 0);
		*/
	}
	for(i = 0; i < 2; i++){
		pjsua_acc_config cfg;
		pjsua_acc_config_default(&cfg);
		char id[100];
		char reg_uri[100];
		sprintf(id,"sip:%s@%s", ar_config[i].username, ar_config[i].domain);
		cfg.id = pj_str(id);
		cfg.transport_id = cfg_transport_id[i]; /* Map account to a config which has difference SIP port*/
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
		//status = pjsua_acc_set_transport(cfg, cfg_transport_id[i]);
		//if (status != PJ_SUCCESS) error_exit("Error set transpoet ID", status);

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
			pj_str_t uri1 = pj_str("sip:958253@86.10.86.181");
			status = pjsua_call_make_call(ar_config[0].acc_id, &uri1, 0, NULL, NULL, NULL);
		}
		if (option[0] == 'p'){
			pj_str_t uri1 = pj_str("sip:958254@86.10.86.181");
			status = pjsua_call_make_call(ar_config[1].acc_id, &uri1, 0, NULL, NULL, NULL);
		}
		if (option[0] == 'o'){
			pj_str_t uri1 = pj_str("sip:996893@192.168.1.198");
			status = pjsua_call_make_call(ar_config[0].acc_id, &uri1, 0, NULL, NULL, NULL);
		}
		if (option[0] == 'h')
			pjsua_call_hangup_all();
	}
}
