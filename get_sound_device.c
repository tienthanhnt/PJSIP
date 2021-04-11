#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>

#define SND_DEFAULT "default"
#define SND_DEVICE_0 "hw:0"
#define SND_DEVICE_1 "hw:1"
#define SND_DEVICE_2 "hw:2"
#define SND_DEVICE_3 "hw:3"
#define SND_DEVICE_4 "hw:4"
#define SND_DEVICE_5 "hw:5"
#define SND_DEVICE_6 "hw:6"
#define SND_DEVICE_7 "hw:7"
#define SND_DEVICE_8 "hw:8"
#define SND_DEFAULT_SPEAKER "Master"
#define SND_DEFAULT_CAPTURE "Capture"
#define SND_SPEAKER "PCM"
#define SND_CAPTURE "Mic"

#define MAX_TIME_OUT 4000

// VCS migrate to call flow
	pj_init();
	pj_caching_pool_init(&cp, NULL, 1024);
    pjmedia_endpt_create(&cp.factory, NULL, 1, &med_endpt);
	pjsua_create();

	//!CONFIG DEFAULT
	pjsua_config cfg[24];
	pjsua_logging_config log_cfg[24];
	for (int i=0; i<24; i++) {
		pjsua_config_default(&cfg[i]);
	}

	pjsua_config_default(&cfg);
	pjsua_media_config_default(&media_cfg);
	media_cfg.no_vad = PJ_TRUE;
	media_cfg.snd_auto_close_time = -1;
	media_cfg.clock_rate = 8000;
	media_cfg.snd_clock_rate = 8000;
	media_cfg.channel_count = 1;
	media_cfg.quality = 10;
	media_cfg.ec_options = PJMEDIA_ECHO_WEBRTC;

	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 2;


// migrate sound device
	pj_ansi_strcpy(devAudioName_1, "sysdefault:CARD=Device");
	pj_ansi_strcpy(devAudioName_2, "sysdefault:CARD=Device_1");
	pj_ansi_strcpy(devAudioName_3, "sysdefault:CARD=Device_2");
	pj_ansi_strcpy(devAudioName_4, "sysdefault:CARD=Device_3");
    pj_ansi_strcpy(devAudioName_5, "sysdefault:CARD=Device_4");
    pj_ansi_strcpy(drvName, "alsa");

    int deviceIDx = [24];
    memset(deviceIDx, 0, 24*sizeof(deviceIDx[0]));

    status = pjmedia_aud_dev_lookup(drvName, devAudioName_1, &deviceIdx[0]);
    status1 = pjmedia_aud_dev_lookup(drvName, devAudioName_1, &deviceIdx[1]);
    status2 = pjmedia_aud_dev_lookup(drvName, devAudioName_1, &deviceIdx[2]);
    status3 = pjmedia_aud_dev_lookup(drvName, devAudioName_1, &deviceIdx[3]);

    if ((status || status1 || status2 || status3 ) != PJ_SUCESS) {
    	printf("look up sound device failed\n");
    	return 1
    }