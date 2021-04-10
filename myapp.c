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

#define SIP_DOMAIN "example.com"
#define SIP_USER "alice"
#define SIP_PASSWD "secret"
 
 
struct config ar_config[24];


int main(){
    read_config(ar_config,"config");
    pjsua_acc_config cfg;
    //char *tmp = strcat(ar_config[0].username, "@");
    //char *tmp2 = strcat(tmp, ar_config[0].domain);
    
    //cfg.id = pj_str(strcat(a, tmp2));
    //cfg.reg_uri = pj_str(strcats(a,ar_config[0].domain));
    int i;
    for (i = 0; i < 24; i++ ){
        char id[100];
        char reg_uri[100] = "sip:";
        sprintf(id,"sip:%s@%s", ar_config[i].username, ar_config[i].domain);
        cfg.id = pj_str(id);
        printf("%s\n" , cfg.id.ptr);
        sprintf(reg_uri, "sip:%s", ar_config[i].domain);
        cfg.reg_uri = pj_str(reg_uri);
        printf("%s\n" , cfg.reg_uri.ptr);
    }
    return 0;
 }