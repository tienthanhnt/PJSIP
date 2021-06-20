#include <pjsua-lib/pjsua.h>

struct call_data
{
  pj_pool_t           *pool;
  pjmedia_conf        *conf;
  pjmedia_port        *cport;
  int                 call_slot_0;
  int 		      call_slot_1;	
};
struct config{
	/* sip */
	int sip_port;
	int sip_tcp_port;
	int sip_tls_port;
	int inc_timeout;
	int in_call_timeout;
	int delayed_timeout;

	/* sound */
	int sound_id;
	char sound_name[100];

	/* proxy */
	char reg_proxy[100];
	char reg_identity[100];

	/* auth_info */
	char username[100];
	char secret[100];
	char ha1[100];
	char realm[100];
	char domain[100];
	pjmedia_port *sc, *sc_2;
	pjmedia_port* rev;
	pjmedia_port* rev_1;
	int slot, slot_1;
	pjmedia_snd_port *snd_port;
	pjsua_acc_id acc_id;
	/* add pjmedia conf */
	pjmedia_conf *media_conf, *media_conf_2;
	pjmedia_port *mp, *mp_2;
	int p_slot;
	struct call_data *cd;

};
void del_line(char *src);
int find(char *src, char *key);
char *trim(char *src, int start, int end);
void read_config(struct config *ar_config, char *dir);
