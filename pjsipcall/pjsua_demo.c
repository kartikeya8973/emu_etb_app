/* $Id: main.c 4752 2014-02-19 08:57:22Z ming $ */
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include "pjsua_app.h"
#include "common.h"

#define THIS_FILE	"main.c"

extern info_t info_active;
//extern int button_flag;

static pj_bool_t	    running = PJ_TRUE;
static pj_status_t	    receive_end_sig;
static pj_thread_t	    *sig_thread;
static pjsua_app_cfg_t	    cfg;

/* Called when CLI (re)started */
void on_app_started(pj_status_t status, const char *msg)
{
	pj_perror(3, THIS_FILE, status, (msg)?msg:"");
}

void on_app_stopped(pj_bool_t restart, int argc, char** argv)
{
	if (argv) {
		cfg.argc = argc;
		cfg.argv = argv;
	}

	running = restart;
}

#if defined(PJ_WIN32) && PJ_WIN32!=0
#include <windows.h>

static pj_thread_desc handler_desc;

static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{   
	switch (fdwCtrlType)
	{
	// Handle the CTRL+C signal.

	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		pj_thread_register("ctrlhandler", handler_desc, &sig_thread);
		PJ_LOG(3,(THIS_FILE, "Ctrl-C detected, quitting.."));
		receive_end_sig = PJ_TRUE;
		pjsua_app_destroy();
		ExitProcess(1);
		PJ_UNREACHED(return TRUE;)

	default:

		return FALSE;
	}
}

static void setup_socket_signal()
{
}

static void setup_signal_handler(void)
{
	SetConsoleCtrlHandler(&CtrlHandler, TRUE);
}

#else
#include <signal.h>

static void setup_socket_signal()
{
	signal(SIGPIPE, SIG_IGN);
}

static void setup_signal_handler(void) {}
#endif

int argc=10;
char *agrs[10];

//char *argv[] = {"",
//			"--registrar=sip:192.168.1.22",
//			"--id=sip:105@192.168.1.22",
//			"--realm=*",
//			"--username=105",
//			"--password=105",
//			"--add-buddy=sip:101@192.168.1.22",
//			"--add-buddy=sip:102@192.168.1.22",
//			"--add-buddy=sip:103@192.168.1.22",
//			"--add-buddy=sip:104@192.168.1.22"};

int main_func()
{
	char ptr [5], id[80], registrar[80], realm[80], uname[80], passwd[30], buddy_1[80], buddy_2[80], buddy_3[80], buddy_4[80];
	//	sprintf(ptr1,"%s",);
	//	agrs[0]=ptr1;

	//Assigning and passing dynamic values as arguments
	sprintf(ptr," ");
	agrs[0]=ptr;
	sprintf(id,"--id=sip:%s@%s", info_active.e_info.sip_user_1, info_active.e_info.svr_ip_1);
	agrs[1]=id;
	//	printf("*\n\n\n\%s\n\n\n*\n",id);
	sprintf(registrar,"--registrar=sip:%s", info_active.e_info.svr_ip_1);
	agrs[2]=registrar;
	//	printf("*\n\n\n\%s\n\n\n*\n",registrar);
	sprintf(realm,"--realm=*");
	agrs[3]=realm;
	//	printf("*\n\n\n\%s\n\n\n*\n",realm);
	sprintf(uname,"--username=%s", info_active.e_info.sip_user_1);
	agrs[4]=uname;
	//	printf("*\n\n\n\%s\n\n\n*\n",uname);
	sprintf(passwd,"--password=%s", info_active.e_info.sip_pass_1);
	agrs[5]=passwd;
	//	printf("*\n\n\n\%s\n\n\n*\n",passwd);
	sprintf(buddy_1,"--add-buddy=sip:%s@%s",info_active.e_info.emerg_ext_s1_1,info_active.e_info.svr_ip_1);
	agrs[6]=buddy_1;
	//	printf("*\n\n\n\%s\n\n\n*\n", buddy_1);
	sprintf(buddy_2,"--add-buddy=sip:%s@%s",info_active.e_info.emerg_ext_s1_2,info_active.e_info.svr_ip_1);
	agrs[7]=buddy_2;
	//	printf("*\n\n\n\%s\n\n\n*\n", buddy_2);
	sprintf(buddy_3,"--add-buddy=sip:%s@%s",info_active.e_info.test_ext_s1,info_active.e_info.svr_ip_1);;
	agrs[8]=buddy_3;
	//	printf("*\n\n\n\%s\n\n\n*\n", buddy_3);
	sprintf(buddy_4,"--add-buddy=sip:%s@%s",info_active.e_info.adm_ext_s1,info_active.e_info.svr_ip_1);
	agrs[9]=buddy_4;
	//	printf("*\n\n\n\%s\n\n\n*\n", buddy_4);

	pj_status_t status = PJ_TRUE;

	pj_bzero(&cfg, sizeof(cfg));
	cfg.on_started = &on_app_started;
	cfg.on_stopped = &on_app_stopped;
	cfg.argc = argc;
	cfg.argv = agrs;

	setup_signal_handler();
	setup_socket_signal();

	while (running) {
		status = pjsua_app_init(&cfg);
		if (status == PJ_SUCCESS) {
			status = pjsua_app_run(PJ_TRUE);

		} else {
			running = PJ_FALSE;
		}

		if (!receive_end_sig) {
			pjsua_app_destroy();

			/* This is on purpose */
			pjsua_app_destroy();
		} else {
			pj_thread_join(sig_thread);
		}
	}
	return 0;
}

int *pjsip_call_thread()
{
	//call button flag (disabling button for the duration of the call)
	//	button_flag = 0;
	return pj_run_app(&main_func, argc, agrs, 0);
}
