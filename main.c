#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spi.h"
#include "read_config.h"

#define THIS_FILE "APP"

#define USE_CUCM 0
#define CHANNEL_NUM 2

pj_pool_t *pool;
pjmedia_port *conf;
struct config ar_config[24];
pj_mutex_t *mutex;
pj_str_t call_number;
pj_str_t get_num_from_call(char *contact){
	pj_str_t val;
	char *temp;
	temp = trim(contact, find(contact, ":") + 1, find(contact, "@"));
	val = pj_str(temp);
}

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
		pjsip_rx_data *rdata)
{
	pj_str_t remote_acc, local_acc;
	pjsua_call_info ci;
	int i;
	PJ_UNUSED_ARG(acc_id);
	PJ_UNUSED_ARG(rdata);

	pjsua_call_get_info(call_id, &ci);
	remote_acc = get_num_from_call(ci.remote_info.ptr);
	local_acc = get_num_from_call(ci.local_contact.ptr);
  for (i = 0; i < CHANNEL_NUM; i++){
    if (strcmp(local_acc.ptr, ar_config[i].username) == 0) {
        if (ar_config[i].state > 0) goto busy;
        ar_config[i].call_id = call_id;
        ar_config[i].call_number = remote_acc;
        printf("call id %d = %d \n", i, ar_config[i].call_id); 
        ar_config[i].state = 0x02;
        pj_mutex_lock (mutex);
        transfer(fd1, (uint8_t)((i << 3) | 0x02));
        pj_mutex_unlock(mutex);
        break;
	  }
  }
  	pj_mutex_lock (mutex);
          transfer(fd1, (uint8_t)((remote_acc.slen << 4) | (remote_acc.ptr[0] - 48)));
          for (i = 1; i < remote_acc.slen; i = i + 2){
                  if (i + 1 < remote_acc.slen) {
                          transfer(fd1, (uint8_t)(((remote_acc.ptr[i] - 48) << 4) | (remote_acc.ptr[i+1] - 48)));
                          printf("%d \n", (uint8_t)(((remote_acc.ptr[i] - 48) << 4) | (remote_acc.ptr[i+1] - 48)));}
                  else if (i < remote_acc.slen) {
                          transfer(fd1, (uint8_t)((remote_acc.ptr[i] - 48) << 4));
                          printf("=====gia tri cuoi %d\n", (uint8_t)((remote_acc.ptr[i] - 48) << 4));
                  }
          }
  	pj_mutex_unlock (mutex);

	PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
				(int)ci.remote_info.slen,
				ci.remote_info.ptr));
	/* send ringing 180*/
	pjsua_call_answer(call_id, PJSIP_SC_RINGING , NULL, NULL);
	return;
  	busy :
        printf("go to busy\n");
        pjsua_call_hangup(call_id, 0, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	pjsua_call_info ci;
	pjsua_call_get_info(call_id, &ci);
  pj_str_t local_acc, remote_acc;
  int i;
	printf("============%s", ci.local_contact.ptr);
	local_acc = get_num_from_call(ci.local_contact.ptr);
  remote_acc = get_num_from_call(ci.remote_info.ptr);
	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);
	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
				    (int)ci.state_text.slen,
				    ci.state_text.ptr));
  for (i = 0; i < CHANNEL_NUM; i++){
    if (strcmp(local_acc.ptr, ar_config[i].username) == 0) {
      if ((ar_config[i].state == 0x02) && (ci.state == 0x06) || (ci.state == 0x04)) goto out;
      if ((ar_config[i].state != 0) || (ci.state != 0x06)){
        ar_config[i].call_number = remote_acc;
        printf("khong phai disconnect %d\n", ar_config[i].state);
        ar_config[i].state = ci.state;
        ar_config[i].call_id = call_id;
        pj_mutex_lock (mutex);
        transfer(fd1, (uint8_t)((i << 3) | ci.state));
        pj_mutex_unlock (mutex);
        break;
      }
    }
  }
  out :
        printf(" ");
 }
/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info ci;
	pj_str_t a;
  int i;
	pjsua_call_get_info(call_id, &ci);
	a = get_num_from_call(ci.local_contact.ptr);

	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
    for (i = 0; i < CHANNEL_NUM; i++){
      if ((strcmp(a.ptr, ar_config[i].username) == 0)) {
        ar_config[i].call_id = call_id; 
        printf("===========================match phone 2\n");
        pjsua_conf_connect(pjsua_call_get_conf_port(call_id), ar_config[i].slot);
        pjsua_conf_connect(ar_config[i].slot, pjsua_call_get_conf_port(call_id));
        break;
	    }
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
  	pjsua_codec_info codec_inf[100];
 	int count = 100;

	read_config(ar_config,"config");

	/* init PJLIB */
	status = pj_init();
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

	/* Create pjsua first! */
	status = pjsua_create();
	/* init gpio and spi */
	fd1 = init_spi();
	init_gpio();
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

	/* Init pjsua */
	{
		pjsua_config cfg;
		pjsua_logging_config log_cfg;

		pjsua_media_config   media_cfg;
		pjsua_media_config_default(&media_cfg);
		media_cfg.quality = 10;
        media_cfg.ec_options = PJMEDIA_ECHO_USE_SW_ECHO;
        media_cfg.clock_rate = 8000;
        media_cfg.channel_count = CHANNEL_NUM;

		pjsua_config_default(&cfg);
		cfg.cb.on_incoming_call = &on_incoming_call;
		cfg.cb.on_call_media_state = &on_call_media_state;
		cfg.cb.on_call_state = &on_call_state;
		cfg.max_calls = 32;
		pjsua_logging_config_default(&log_cfg);
		log_cfg.console_level = 4;

		status = pjsua_init(&cfg, &log_cfg, &media_cfg);
		if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
	}

	/* Add UDP transport. */
	pjsua_transport_id cfg_transport_id[24];
	pjsua_transport_config cfg_transport[24];
	for(i = 0; i < CHANNEL_NUM; i++){
		pjsua_transport_config_default(&cfg_transport[i]);
		cfg_transport[i].port = ar_config[i].sip_port;
		status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg_transport[i], &cfg_transport_id[i]);
	}

	/* Initialization is done, now start pjsua */
	status = pjsua_start();

	pool = pjsua_pool_create("pool", 2048, 2048);
	
	status = pj_mutex_create_simple (pool, "my_lock", &mutex);
	if (status != PJ_SUCCESS) error_exit("Error creat mutex", status);
	conf = pjsua_set_no_snd_dev();

	/* list codec */
 	status = pjsua_enum_codecs(codec_inf, &count);

	for (int i = 0; i < 10; i++) {
	    printf("============================i = %d\n", i);
	    printf("============================ codec_id 0 = %s\n", codec_inf[i].codec_id.ptr);
	    printf("============================ codec_pri 0 = %d\n", codec_inf[i].priority);
	}

	for (i = 0; i < CHANNEL_NUM; i++){
		printf("init sound device %d", i);
		status = pjmedia_snd_port_create(pool, ar_config[i].playback_dev_id, ar_config[i].capture_dev_id, 
				PJMEDIA_PIA_SRATE(&conf->info),
				2 /* stereo */,
				2 * PJMEDIA_PIA_SPF(&conf->info),
				PJMEDIA_PIA_BITS(&conf->info),
				0, &ar_config[i].snd_port);

		pjmedia_snd_port_set_ec(ar_config[i].snd_port, pool, 300, PJMEDIA_ECHO_SIMPLE); // Remove this
		// Create stereo-mono splitter/combiner
		status = pjmedia_splitcomb_create(pool, 
				PJMEDIA_PIA_SRATE(&conf->info),
				2 /* stereo */,
				2 * PJMEDIA_PIA_SPF(&conf->info),
				PJMEDIA_PIA_BITS(&conf->info),
				0, &ar_config[i].sc);
		
		/* Connect channel0 (left channel?) to conference port slot0 */
		status = pjmedia_splitcomb_set_channel(ar_config[i].sc, 0, 0, conf);
		
		/* Create reverse channel for channel1 (right channel?)... */
		status = pjmedia_splitcomb_create_rev_channel(pool, ar_config[i].sc, 1, 0, &ar_config[i].rev);

		pjmedia_snd_port_connect(ar_config[i].snd_port, ar_config[i].sc);
		pjsua_conf_add_port(pool, ar_config[i].rev, &ar_config[i].slot);
	}

	for(i = 0; i < CHANNEL_NUM; i++){
		pjsua_acc_config cfg;
		pjsua_acc_config_default(&cfg);
		char id[100];
		char reg_uri[100];
		sprintf(id,"sip:%s@%s", ar_config[i].username, ar_config[i].domain);
		cfg.id = pj_str(id);

		if (USE_CUCM == 0) cfg.transport_id = cfg_transport_id[0]; /* Map account to a config which has difference SIP port */
	    else cfg.transport_id = cfg_transport_id[i];

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
	fd2 = open("/sys/class/gpio/gpio340/value", O_RDONLY);
	pfd1.fd = fd2;
	pfd1.events = POLLPRI;
	while (1)
	{
		lseek(fd2, 0, SEEK_SET);
		ret = poll(&pfd1, 1, -1);
		read(fd2, &c, 1);
		if(ret == 0)
			printf("Timeout\n");
		else if(c == '0')
			printf("Push\n");
		else{
				char call_uri[100];
				char flag;
				char count;
				pj_str_t uri;
				printf("Release\n");
				pj_mutex_lock (mutex);
				count = read_spi(fd1, rd_buf, &flag);
				call_number = pj_str(rd_buf);
				sprintf(call_uri,"sip:%s@%s", call_number.ptr, ar_config[0].domain);
				uri = pj_str(call_uri);
				printf("chuoi %s va len %ld\n", uri.ptr, uri.slen);
				pj_mutex_unlock(mutex);
				printf("flag = %d, count = %d \n", flag, count);
				if (flag == 0x01) {
					int i;
					for (i = 0; i < CHANNEL_NUM; i++){
						if (strcmp(ar_config[i].call_number.ptr, call_number.ptr) == 0){
              goto quit;
              break;
						}
					}
					pjsua_call_make_call(ar_config[count].acc_id, &uri, 0, NULL, NULL, NULL);
          quit :
            printf(" ");
					}
				else if (flag == 0x04) pjsua_call_answer(ar_config[count].call_id, 200, NULL, NULL);
				else if (flag == 0x07) {
						ar_config[count].state = 0;
            ar_config[count].call_number = pj_str("idle");
						pjsua_call_hangup(ar_config[count].call_id, 0, NULL, NULL);
						printf("idle state\n"); 
				}
			}
	}

  pjsua_call_hangup_all();
}