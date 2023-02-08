/*
 * server.c
 *
 *  Created on: Jan 29, 2022
 *      Author: Kartikeya
 */

//	sprintf(json, "{\"CONFIGURATION\":\"P_INFO\":{\"P_SNO\":\"1\",\"PRJ\":\"ETB\",\"MFR\":\"CSEMI\",\"MFR_DATE\":\"27.01.2022\"},\"N_INFO\":{\"IPv4\":\"192.168.1.50\",\"SUB_MASK\":\"255.255.255.0\",\"GW\":\"192.168.1.1\",\"DNS\":\"8.8.8.8\"},\"E_INFO\":{\"SIP_EXT_ETB\":\"sip:101@192.168.1.50\",\"SIP_USER_1\":\"101\",\"SIP_PASS_1\":\"101\",\"SIP_USER_2\":\"102\",\"SIP_PASS_2\":\"102\",\"ETB_PORT\":\"2244\",\"SRV_IP_1\":\"192.168.1.50\",\"EMERG_EXT_S1_1\":\"801\",\"EMERG_EXT_S1_2\":\"802\",\"TEST_EXT_S1\":\"803\",\"ADM_EXT_S1\":\"804\",\"SRV_IP_2\":\"192.168.1.200\",\"EMERG_EXT_S2_1\":\"901\",\"EMERG_EXT_S2_2\":\"902\",\"TEST_EXT_S2\":\"903\",\"ADM_EXT_S2\":\"904\",\"REC_LOC\":\"/home/Recordings\",\"REC_FLG\":\"Enable\"},\"CH_INFO:\"{\"CALL_DIR\":\"Outgoing\",\"TIME_STAMP\":\"28.01.2022 15:34:55\",\"START_TIME\":\"15:35:55\",\"END_TIME\":\"15:37:23\",\"SRC\":\"101\",\DEST\":\"102\",\"STATUS\":\"SUCCESS\"}");

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "common.h"
#include "DB_ops.h"
#include<json-c/json.h>

extern info_t info_active;
extern info_t info_recv;
extern info_t info_prev;

//Thread function for the TCP server


void *tcp_server_thread(void){

			read_from_db();
			printf("IP address for the server - %s\n",info_active.n_info.ipv4_add);
			char address[16];
			strcpy(address, info_active.n_info.ipv4_add);
			int port = 5566;

			int server_sock, client_sock;
			struct sockaddr_in server_addr, client_addr;
			socklen_t addr_size;
			char buffer[2048]; //variable to store infomation transfered between client and server
			int n;

			//Creating a TCP server socket

			server_sock = socket(AF_INET, SOCK_STREAM, 0);
			if(server_sock < 0){
				perror("[-]Socket error...\n");
				exit(1);
			}
			printf("[+]TCP server socket created...\n");

			memset(&server_addr, '\0', sizeof(server_addr));

			//assigning port and IP
			server_addr.sin_family = AF_INET;             // this means IPv4
			server_addr.sin_port =htons(port);
			//server_addr.sin_port = port;				  // port we have defined
			server_addr.sin_addr.s_addr = inet_addr(address);  // local ip address

			//Binding the newly created socket to the given IP address
			n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if(n<0){
				perror("[-]Socket bind failed...\n");
				exit(1);
			}
			printf("[+]Socket successfully binded to the port %d\n",port);

			//Now the server is ready to listen
			listen(server_sock,5);
			while(1){
			printf("Listening...\n");

			//Accepting connection from client
			addr_size =sizeof(client_addr);
			client_sock =accept(server_sock, (struct sockaddr*)&client_addr,&addr_size);
			printf("[+]Client connected...\n");
			//Clearing the buffer
			bzero(buffer, 2048);
			//Receive message from client
			recv(client_sock, buffer, sizeof(buffer),0);
			printf("Client: %s\n",buffer);

//WAITING STATE - The tcp server is open and waits for the clients action

			char get_info[20] = "GETINFO";   				 // String for the validation of getting config process
			int get_value;									 // to compare to strings
			get_value = strcmp(buffer,get_info);			 // client sends "GETINFO" string in the buffer
			char write_info[20] = "WRITEINFO";				 // String for the validation of writing config to DB process
			int write_value; 								 // to compare to strings
			write_value = strcmp(buffer,write_info);		 // client sends "WRITEINFO" string in the buffer
			char ping_info[20] = "PING";					 // String for the validating if client is connected or not
			int ping_value; 								 // to compare to strings
			ping_value = strcmp(buffer,ping_info);		 	 // client sends "PING" string in the buffer
			char verify_info[20] = "VERIFY";				 // String to verify sent and received json client
			int verify_value;								 // to compare to strings
			verify_value = strcmp(buffer,verify_info);		 // client sends "VERIFY" string in the buffer


			if(get_value == 0){
//					//Accepting connection from client
//					addr_size =sizeof(client_addr);
//					client_sock =accept(server_sock, (struct sockaddr*)&client_addr,&addr_size);
//					printf("[+]Client connected...\n");
					 //Clearing the buffer
//					bzero(buffer, 2048);
					 //Receive message from client
//					recv(client_sock, buffer, sizeof(buffer),0);
//					printf("Client: %s\n",buffer);

					 //Calling function that read the configurations from the database
					 read_from_db();

					   //JSON creation
					   char json[2048];
					   printf("Creating JSON file\n");
					   sprintf(json,
					"{\"configurations\":"
							"{\"P_INFO\":"
								"{\"P_SNO\":\"%s\","
								"\"PRJ\":\"%s\","
								"\"MFR\":\"%s\","
							   	"\"MFR_DATE\":\"%s\","
							   	"\"HW_VER\":\"%s\","
								"\"SW_VER\":\"%s\"},"
							"\"N_INFO\":"
								"{\"ETH_MODE\":\"%s\","
								"\"IPv4\":\"%s\","
								"\"SUB_MASK\":\"%s\","
								"\"GW\":\"%s\","
								"\"DNS\":\"%s\"},"
							"\"E_INFO\":"
								"{\"SIP_EXT_ETB\":\"%s\","
								"\"SIP_USER_1\":\"%s\","
								"\"SIP_PASS_1\":\"%s\","
								"\"SIP_USER_2\":\"%s\","
								"\"SIP_PASS_2\":\"%s\","
								"\"ETB_PORT\":\"%s\","
								"\"SRV_IP_1\":\"%s\","
								"\"EMERG_EXT_S1_1\":\"%s\","
								"\"EMERG_EXT_S1_2\":\"%s\","
								"\"TEST_EXT_S1\":\"%s\","
								"\"ADM_EXT_S1\":\"%s\","
								"\"SRV_IP_2\":\"%s\","
								"\"EMERG_EXT_S2_1\":\"%s\","
								"\"EMERG_EXT_S2_2\":\"%s\","
								"\"TEST_EXT_S2\":\"%s\","
								"\"ADM_EXT_S2\":\"%s\","
								"\"REC_LOC\":\"%s\","
								"\"REC_FLG\":\"%s\","
								"\"MIC_VOL\":\"%s\","
								"\"SPK_VOL\":\"%s\"},"
							"}"
					 "}",info_active.p_info.prod_sno, info_active.p_info.project, info_active.p_info.mfr, info_active.p_info.mfr_date,info_active.p_info.hw_ver,info_active.p_info.sw_ver,
					 	 info_active.n_info.eth_mode,info_active.n_info.ipv4_add, info_active.n_info.subnet_mask, info_active.n_info.gateway, info_active.n_info.dns, info_active.e_info.sip_exten_etb,
						 info_active.e_info.sip_user_1, info_active.e_info.sip_pass_1, info_active.e_info.sip_user_2, info_active.e_info.sip_pass_2, info_active.e_info.etb_port,
						 info_active.e_info.svr_ip_1, info_active.e_info.emerg_ext_s1_1, info_active.e_info.emerg_ext_s1_2, info_active.e_info.test_ext_s1,
						 info_active.e_info.adm_ext_s1, info_active.e_info.svr_ip_2, info_active.e_info.emerg_ext_s2_1, info_active.e_info.emerg_ext_s2_2, info_active.e_info.test_ext_s2,
						 info_active.e_info.adm_ext_s2, info_active.e_info.rec_loc, info_active.e_info.rec_flg, info_active.e_info.mic_vol, info_active.e_info.spk_vol);
					 	 printf("%s\n", json);
					 	 //Clearing the buffer
						 bzero(buffer, 2048);
						 strcpy(buffer,json);
						 printf("Sending the JSON file to the client\n");
//						 printf("Server: %s\n", buffer);
						 //Sending message to the client
						 send(client_sock,buffer,strlen(buffer),0);
						 close(client_sock);
						 printf("[+]Client disconnected...\n\n");
			}

				 else if(write_value == 0){

					 	 bzero(buffer, 2048);
					 	 //receiving the json sent by client in a char array called buffer
					 	 recv(client_sock, buffer, sizeof(buffer),0);
//						 printf("CLIENT SENT \n%s\n\n", buffer);

					 	 //Parsing Configuration string of the JSON
					 	 parsed_json = json_tokener_parse(buffer);
					 	 json_object_object_get_ex(parsed_json, "configurations", &configurations);
//						 printf("configurations: %s\n\n", json_object_get_string(configurations));

					 	 //Parsing P_INFO string of the JSON
					 	 bzero(buffer, 2048);
					 	 strcpy(buffer, json_object_get_string(configurations));
//					 	 printf("%s\n\n", buffer);
					 	 parsed_json_conf = json_tokener_parse(buffer);
					 	 json_object_object_get_ex(parsed_json_conf, "P_INFO", &P_INFO);
//						 printf("P_INFO: %s\n\n", json_object_get_string(P_INFO));

					 	 //Saving the info received from client JSON in info_recv type variables fot P_INFO table
					 	 bzero(buffer, 2048);
					 	 strcpy(buffer, json_object_get_string(P_INFO));
//						 printf("%s\n", buffer);
					 	 parsed_json_pinfo = json_tokener_parse(buffer);
					 	 json_object_object_get_ex(parsed_json_pinfo, "P_SNO", &json_obj);
//		 			 	 printf("P_SNO: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.prod_sno,json_object_get_string(json_obj));
					 	 json_object_object_get_ex(parsed_json_pinfo, "PRJ", &json_obj);
//					 	 printf("PRJ: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.project,json_object_get_string(json_obj));
					 	 json_object_object_get_ex(parsed_json_pinfo, "MFR", &json_obj);
//					 	 printf("MFR: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.mfr,json_object_get_string(json_obj));
					 	 json_object_object_get_ex(parsed_json_pinfo, "MFR_DATE", &json_obj);
//					 	 printf("MFR_DATE: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.mfr_date,json_object_get_string(json_obj));
					 	 json_object_object_get_ex(parsed_json_pinfo, "HW_VER", &json_obj);
//	 					 printf("HW_VER: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.hw_ver,json_object_get_string(json_obj));
					 	 json_object_object_get_ex(parsed_json_pinfo, "SW_VER", &json_obj);
//	 					 printf("SW_VER: %s\n\n", json_object_get_string(json_obj));
					 	 sprintf(info_recv.p_info.sw_ver,json_object_get_string(json_obj));

						 //Parsing N_INFO string of the JSON
						 bzero(buffer, 2048);
						 strcpy(buffer, json_object_get_string(configurations));
	//					 printf("%s\n\n", buffer);
						 parsed_json_ninfo = json_tokener_parse(buffer);
						 json_object_object_get_ex(parsed_json_conf, "N_INFO", &N_INFO);
	//					 printf("N_INFO: %s\n\n", json_object_get_string(N_INFO));

						 //Saving the info received from client JSON in info_recv type variables fot N_INFO table
						 bzero(buffer, 2048);
						 strcpy(buffer, json_object_get_string(N_INFO));
	//					 printf("%s\n", buffer);
						 parsed_json_ninfo = json_tokener_parse(buffer);
						 json_object_object_get_ex(parsed_json_ninfo, "ETH_MODE", &json_obj);
//						 printf("ETH_MODE: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.n_info.eth_mode,json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_ninfo, "IPv4", &json_obj);
//						 printf("IPv4: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.n_info.ipv4_add,json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_ninfo, "SUB_MASK", &json_obj);
//						 printf("SUB_MASK: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.n_info.subnet_mask,json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_ninfo, "GW", &json_obj);
//						 printf("GW: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.n_info.gateway,json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_ninfo, "DNS", &json_obj);
//						 printf("DNS: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.n_info.dns,json_object_get_string(json_obj));

						 //Parsing E_INFO string of the JSON
						 bzero(buffer, 2048);
						 strcpy(buffer, json_object_get_string(configurations));
	//					 printf("%s\n\n", buffer);
						 parsed_json_einfo = json_tokener_parse(buffer);
						 json_object_object_get_ex(parsed_json_conf, "E_INFO", &E_INFO);
	//					 printf("E_INFO: %s\n\n", json_object_get_string(E_INFO));

						 //Saving the info received from client JSON in info_recv type variables fot E_INFO table
						 bzero(buffer, 2048);
						 strcpy(buffer, json_object_get_string(E_INFO));
	//					 printf("%s\n", buffer);
						 parsed_json_einfo = json_tokener_parse(buffer);
						 json_object_object_get_ex(parsed_json_einfo, "SIP_EXT_ETB", &json_obj);
//						 printf("SIP_EXT_ETB: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.sip_exten_etb, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SIP_USER_1", &json_obj);
//						 printf("SIP_USER_1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.sip_user_1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SIP_PASS_1", &json_obj);
//						 printf("SIP_PASS_1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.sip_pass_1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SIP_USER_2", &json_obj);
//						 printf("SIP_USER_2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.sip_user_2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SIP_PASS_2", &json_obj);
//						 printf("SIP_PASS_2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.sip_pass_2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "ETB_PORT", &json_obj);
//						 printf("ETB_PORT: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.etb_port, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SRV_IP_1", &json_obj);
//						 printf("SRV_IP_1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.svr_ip_1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "EMERG_EXT_S1_1", &json_obj);
//						 printf("EMERG_EXT_S1_1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.emerg_ext_s1_1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "EMERG_EXT_S1_2", &json_obj);
//						 printf("EMERG_EXT_S1_2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.emerg_ext_s1_2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "TEST_EXT_S1", &json_obj);
//						 printf("TEST_EXT_S1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.test_ext_s1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "ADM_EXT_S1", &json_obj);
//						 printf("ADM_EXT_S1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.adm_ext_s1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SRV_IP_2", &json_obj);
//						 printf("SRV_IP_2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.svr_ip_2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "EMERG_EXT_S2_1", &json_obj);
//						 printf("EMERG_EXT_S2_1: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.emerg_ext_s2_1, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "EMERG_EXT_S2_2", &json_obj);
//						 printf("EMERG_EXT_S2_2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.emerg_ext_s2_2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "TEST_EXT_S2", &json_obj);
//						 printf("TEST_EXT_S2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.test_ext_s2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "ADM_EXT_S2", &json_obj);
//						 printf("ADM_EXT_S2: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.adm_ext_s2, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "REC_LOC", &json_obj);
//						 printf("REC_LOC: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.rec_loc, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "REC_FLG", &json_obj);
//						 printf("REC_FLG: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.rec_flg, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "MIC_VOL", &json_obj);
//						 printf("MIC_VOL: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.mic_vol, json_object_get_string(json_obj));
						 json_object_object_get_ex(parsed_json_einfo, "SPK_VOL", &json_obj);
//						 printf("SPK_VOL: %s\n\n", json_object_get_string(json_obj));
						 sprintf(info_recv.e_info.spk_vol, json_object_get_string(json_obj));
						 close(client_sock);
						 printf("[+]Client disconnected...\n\n");

						 //Read the last configuration
						 read_from_db();
						 //Saving previous configuration in info_prev datatype
						 //Saving previous state of P_INFO in info_prev
						 printf("Previous Configuration - \n");

						 strcpy(info_prev.p_info.prod_sno, info_active.p_info.prod_sno);
						 printf("%s\n", info_prev.p_info.prod_sno);
						 strcpy(info_prev.p_info.project, info_active.p_info.project);
						 printf("%s\n", info_prev.p_info.project);
						 strcpy(info_prev.p_info.mfr, info_active.p_info.mfr);
						 printf("%s\n", info_prev.p_info.mfr);
						 strcpy(info_prev.p_info.mfr_date, info_active.p_info.mfr_date);
						 printf("%s\n", info_prev.p_info.mfr_date);
						 strcpy(info_prev.p_info.hw_ver, info_active.p_info.hw_ver);
						 printf("%s\n", info_prev.p_info.hw_ver);
						 strcpy(info_prev.p_info.sw_ver, info_active.p_info.sw_ver);
						 printf("%s\n", info_prev.p_info.sw_ver);

						 //Saving previous state of N_INFO in info_prev
						 strcpy(info_prev.n_info.eth_mode, info_active.n_info.eth_mode);
						 printf("%s\n", info_prev.n_info.eth_mode);
						 strcpy(info_prev.n_info.ipv4_add, info_active.n_info.ipv4_add);
						 printf("%s\n", info_prev.n_info.ipv4_add);
						 strcpy(info_prev.n_info.subnet_mask, info_active.n_info.subnet_mask);
						 printf("%s\n", info_prev.n_info.subnet_mask);
						 strcpy(info_prev.n_info.gateway, info_active.n_info.gateway);
						 printf("%s\n", info_prev.n_info.gateway);
						 strcpy(info_prev.n_info.dns, info_active.n_info.dns);
						 printf("%s\n", info_prev.n_info.dns);

						 //Saving previous state of E_INFO in info_prev
						 strcpy(info_prev.e_info.sip_exten_etb, info_active.e_info.sip_exten_etb);
						 printf("%s\n", info_prev.e_info.sip_exten_etb);
						 strcpy(info_prev.e_info.sip_user_1, info_active.e_info.sip_user_1);
						 printf("%s\n", info_prev.e_info.sip_user_1);
						 strcpy(info_prev.e_info.sip_pass_1, info_active.e_info.sip_pass_1);
						 printf("%s\n", info_prev.e_info.sip_pass_1);
						 strcpy(info_prev.e_info.sip_user_2, info_active.e_info.sip_user_2);
						 printf("%s\n", info_prev.e_info.sip_user_2);
						 strcpy(info_prev.e_info.sip_pass_2, info_active.e_info.sip_pass_2);
						 printf("%s\n", info_prev.e_info.sip_pass_2);
						 strcpy(info_prev.e_info.etb_port, info_active.e_info.etb_port);
						 printf("%s\n", info_prev.e_info.etb_port);
						 strcpy(info_prev.e_info.svr_ip_1, info_active.e_info.svr_ip_1);
						 printf("%s\n", info_prev.e_info.svr_ip_1);
						 strcpy(info_prev.e_info.emerg_ext_s1_1, info_active.e_info.emerg_ext_s1_1);
						 printf("%s\n", info_prev.e_info.emerg_ext_s1_1);
						 strcpy(info_prev.e_info.emerg_ext_s1_2, info_active.e_info.emerg_ext_s1_2);
						 printf("%s\n", info_prev.e_info.emerg_ext_s1_2);
						 strcpy(info_prev.e_info.test_ext_s1, info_active.e_info.test_ext_s1);
						 printf("%s\n", info_prev.e_info.test_ext_s1);
						 strcpy(info_prev.e_info.adm_ext_s1, info_active.e_info.adm_ext_s1);
						 printf("%s\n", info_prev.e_info.adm_ext_s1);
						 strcpy(info_prev.e_info.svr_ip_2, info_active.e_info.svr_ip_2);
						 printf("%s\n", info_prev.e_info.svr_ip_2);
						 strcpy(info_prev.e_info.emerg_ext_s2_1, info_active.e_info.emerg_ext_s2_1);
						 printf("%s\n", info_prev.e_info.emerg_ext_s2_1);
						 strcpy(info_prev.e_info.emerg_ext_s2_2, info_active.e_info.emerg_ext_s2_2);
						 printf("%s\n", info_prev.e_info.emerg_ext_s2_2);
						 strcpy(info_prev.e_info.test_ext_s2, info_active.e_info.test_ext_s2);
						 printf("%s\n", info_prev.e_info.test_ext_s2);
						 strcpy(info_prev.e_info.adm_ext_s2, info_active.e_info.adm_ext_s2);
						 printf("%s\n", info_prev.e_info.adm_ext_s2);
						 strcpy(info_prev.e_info.rec_loc, info_active.e_info.rec_loc);
						 printf("%s\n", info_prev.e_info.rec_loc);
						 strcpy(info_prev.e_info.rec_flg, info_active.e_info.rec_flg);
						 printf("%s\n", info_prev.e_info.rec_flg);
						 strcpy(info_prev.e_info.mic_vol, info_active.e_info.mic_vol);
						 printf("%s\n", info_prev.e_info.mic_vol);
						 strcpy(info_prev.e_info.spk_vol, info_active.e_info.spk_vol);
						 printf("%s\n", info_prev.e_info.spk_vol);

						 /*Calling the write function to write the config pf json send by
						  * client to the ETB.db database
						  */
						 write_to_db();

						 //Creating new interfaces script
						 char mode[50]; //static or dhcp only
						 char address[50];
						 char netmask[50];
						 char network[50];
						 char gateway[50];
						 char mic[50];
						 char spk[50];
						 system("./script_for_eth1");  //in mys-6ulx eth1 is the ethernet that is active when codec is connected
						 sleep(1);
						 sprintf(mode,"echo ""iface eth1 inet %s"" >> /etc/network/interfaces", info_recv.n_info.eth_mode);
						 system(mode);
//						 printf("%s\n", mode);
						 sleep(1);
						 sprintf(address,"echo ""address %s"" >> /etc/network/interfaces", info_recv.n_info.ipv4_add);
						 system(address);
//					 	 printf("%s\n", address);
						 sleep(1);
						 sprintf(netmask,"echo ""netmask %s"" >> /etc/network/interfaces", info_recv.n_info.subnet_mask);
						 system(netmask);
//					     printf("%s\n", netmask);
						 sleep(1);
						 sprintf(network,"echo ""network %s"" >> /etc/network/interfaces", info_recv.n_info.gateway);
						 system(network);
//					   	 printf("%s\n", network);
						 sleep(1);
						 sprintf(gateway,"echo ""gateway %s"" >> /etc/network/interfaces", info_recv.n_info.gateway);
						 system(gateway);
//						 printf("%s\n", gateway);
						 sleep(1);
						 sprintf(mic, "amixer set 'Capture' %s", info_recv.e_info.mic_vol);
						 system(mic);
						 sleep(1);
//						 printf("%s\n", mic);
						 sprintf(spk, "amixer set 'Headphone' %s", info_recv.e_info.spk_vol);
						 system(spk);

				 }

				 else if(ping_value == 0){
					 //Clearing the buffer
					 bzero(buffer, 2048);
					 strcpy(buffer,"CON");
					 printf("Server: %s\n", buffer);
					 //Sending message to the client
					 send(client_sock,buffer,strlen(buffer),0);
					 close(client_sock);
					 printf("[+]Client disconnected...\n\n");

				 }

				 else if(verify_value == 0){
					 //Clearing the buffer
					 bzero(buffer, 2048);
					 //Sending json back to client for verfication
					 //Calling function that read the configurations from the database
						 read_from_db();

					 //JSON creation
						 char json[2048];
						 printf("Creating JSON file\n");
						 sprintf(json,
								 "{\"configurations\":"
								 "{\"P_INFO\":"
								 "{\"P_SNO\":\"%s\","
								 "\"PRJ\":\"%s\","
								 "\"MFR\":\"%s\","
								 "\"MFR_DATE\":\"%s\","
								 "\"HW_VER\":\"%s\","
								 "\"SW_VER\":\"%s\"},"
								 "\"N_INFO\":"
								 "{\"ETH_MODE\":\"%s\","
								 "\"IPv4\":\"%s\","
								 "\"SUB_MASK\":\"%s\","
								 "\"GW\":\"%s\","
								 "\"DNS\":\"%s\"},"
								 "\"E_INFO\":"
								 "{\"SIP_EXT_ETB\":\"%s\","
								 "\"SIP_USER_1\":\"%s\","
								 "\"SIP_PASS_1\":\"%s\","
								 "\"SIP_USER_2\":\"%s\","
								 "\"SIP_PASS_2\":\"%s\","
								 "\"ETB_PORT\":\"%s\","
								 "\"SRV_IP_1\":\"%s\","
								 "\"EMERG_EXT_S1_1\":\"%s\","
								 "\"EMERG_EXT_S1_2\":\"%s\","
								 "\"TEST_EXT_S1\":\"%s\","
								 "\"ADM_EXT_S1\":\"%s\","
								 "\"SRV_IP_2\":\"%s\","
								 "\"EMERG_EXT_S2_1\":\"%s\","
								 "\"EMERG_EXT_S2_2\":\"%s\","
								 "\"TEST_EXT_S2\":\"%s\","
								 "\"ADM_EXT_S2\":\"%s\","
								 "\"REC_LOC\":\"%s\","
								 "\"REC_FLG\":\"%s\","
								 "\"MIC_VOL\":\"%s\","
								 "\"SPK_VOL\":\"%s\"},"
								 "}"
								 "}",info_active.p_info.prod_sno, info_active.p_info.project, info_active.p_info.mfr, info_active.p_info.mfr_date,info_active.p_info.hw_ver,info_active.p_info.sw_ver,
								 info_active.n_info.eth_mode,info_active.n_info.ipv4_add, info_active.n_info.subnet_mask, info_active.n_info.gateway, info_active.n_info.dns, info_active.e_info.sip_exten_etb,
								 info_active.e_info.sip_user_1, info_active.e_info.sip_pass_1, info_active.e_info.sip_user_2, info_active.e_info.sip_pass_2, info_active.e_info.etb_port,
								 info_active.e_info.svr_ip_1, info_active.e_info.emerg_ext_s1_1, info_active.e_info.emerg_ext_s1_2, info_active.e_info.test_ext_s1,
								 info_active.e_info.adm_ext_s1, info_active.e_info.svr_ip_2, info_active.e_info.emerg_ext_s2_1, info_active.e_info.emerg_ext_s2_2, info_active.e_info.test_ext_s2,
								 info_active.e_info.adm_ext_s2, info_active.e_info.rec_loc, info_active.e_info.rec_flg, info_active.e_info.mic_vol, info_active.e_info.spk_vol);
						 printf("%s\n", json);

						 //Clearing the buffer
						 bzero(buffer, 2048);
						 strcpy(buffer,json);
						 printf("Sending the JSON file to the client\n");
						 //						 printf("Server: %s\n", buffer);
						 //Sending message to the client
						 send(client_sock,buffer,strlen(buffer),0);
						 close(client_sock);
						 printf("[+]Client disconnected...\n\n");

						 sleep(5);
						 printf("\n*\n*\n*\n*\nREBOOTING SYSTEM NOW\n*\n*\n*\n*\n");
						 system("reboot");
				 }

				 }

			return 0;

	return NULL;
}
