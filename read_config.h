struct config{
  /* sip */
    char contact[100];
    int sip_port;
    int sip_tcp_port;
    int sip_tls_port;
    int inc_timeout;
    int in_call_timeout;
    int delayed_timeout;
    /* rtp */
    int audio_rtp_port;
    int text_rtp_port;
    /* sound */
    char ringer_dev_id[100];
    char playback_dev_id[100];
    char capture_dev_id[100];
    float playback_gain_db;
    float mic_gain_db;
    /* proxy */
    char reg_proxy[100];
    char reg_identity[100];
    /* auth_info */
    char username[100];
    char secret[100];
    char ha1[100];
    char realm[100];
    char domain[100];
};
inline void del_line(char *src);
int find(char *src, char *key);
char *trim(char *src, int start, int end);
void read_config(struct config *ar_config, char *dir);