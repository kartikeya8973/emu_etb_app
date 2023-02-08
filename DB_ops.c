/*
 * DB_ops.c
 *
 *  Created on: Feb 1, 2022
 *      Author: Kartikeya
 */


//All operations of DB are performed in this C file
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "common.h"


#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

extern info_t info_active;
extern info_t info_recv;

// For getting host's IP address
extern struct ifreq ifr;


//For hiding the debugging printf statements
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif



//*******************************CALLBACKS FUNCTIONS************************************
//Callback for inserting data
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i = 0; i<argc; i++)
	{
		(printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"));
	}
	D(printf("\n"));
	return 0;
}

//Callback for displaying data
static int call_back(void *data, int argc, char **argv, char **azColName)
{
	int i;
	(fprintf(stderr, "%s: ", (const char*)data));

	for(i = 0; i<argc; i++)
	{
		(printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"));
	}

	(printf("\n"));
	return 0;
}

//Callback for P_SNO to get current value from DB
static int callback_readpsno(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.prod_sno, argv[0]);
	D(printf("%s\n", info_active.p_info.prod_sno));
	return 0;
}
//Callback for PRJ to get current value from DB
static int callback_readprj(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.project, argv[0]);
	D(printf("%s\n", info_active.p_info.project));
	return 0;
}
//Callback for MFR to get current value from DB
static int callback_readmfr(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.mfr, argv[0]);
	D(printf("%s\n", info_active.p_info.mfr));
	return 0;
}
//Callback for MFR_DATE to get current value from DB
static int callback_readmfrdate(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.mfr_date, argv[0]);
	D(printf("%s\n", info_active.p_info.mfr_date));
	return 0;
}
//Callback for HW_VER to get current value from DB
static int callback_readhwver(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.hw_ver, argv[0]);
	D(printf("%s\n", info_active.p_info.hw_ver));
	return 0;
}
//Callback for SW_VER to get current value from DB
static int callback_readswver(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.p_info.sw_ver, argv[0]);
	D(printf("%s\n", info_active.p_info.sw_ver));
	return 0;
}
//Callback for ETH_MODE to get current value from DB
static int callback_readethmode(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.n_info.eth_mode, argv[0]);
	D(printf("%s\n", info_active.n_info.eth_mode));
	return 0;
}
//Callback for IPv4 to get current value from DB
static int callback_readipv4(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.n_info.ipv4_add, argv[0]);
	D(printf("%s\n", info_active.n_info.ipv4_add));
	return 0;
}
//Callback for SUB_MASK to get current value from DB
static int callback_readsub(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.n_info.subnet_mask, argv[0]);
	D(printf("%s\n", n_info.subnet_mask));
	return 0;
}
//Callback for GW to get current value from DB
static int callback_readgw(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.n_info.gateway, argv[0]);
	D(printf("%s\n", info_active.n_info.gateway));
	return 0;
}
//Callback for DNS to get current value from DB
static int callback_readdns(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.n_info.dns, argv[0]);
	D(printf("%s\n", info_active.n_info.dns));
	return 0;
}
//Callback for SIP_EXT_ETB to get current value from DB
static int callback_readsipxetb(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.sip_exten_etb, argv[0]);
	D(printf("%s\n", info_active.e_info.sip_exten_etb));
	return 0;
}
//Callback for SIP_USER_1 to get current value from DB
static int callback_readsipu1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.sip_user_1, argv[0]);
	(printf("%s\n", info_active.e_info.sip_user_1));
	return 0;
}
//Callback for SIP_PASS_1 to get current value from DB
static int callback_readsipp1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.sip_pass_1, argv[0]);
	D(printf("%s\n", info_active.e_info.sip_pass_1));
	return 0;
}
//Callback for SIP_USER_2 to get current value from DB
static int callback_readsipu2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.sip_user_2, argv[0]);
	D(printf("%s\n", info_active.e_info.sip_user_2));
	return 0;
}
//Callback for SIP_PASS_2 to get current value from DB
static int callback_readsipp2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.sip_pass_2, argv[0]);
	D(printf("%s\n", info_active.e_info.sip_pass_2));
	return 0;
}
//Callback for ETB_PORT to get current value from DB
static int callback_readsipetbp(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.etb_port, argv[0]);
	D(printf("%s\n", info_active.e_info.etb_port));
	return 0;
}
//Callback for SRV_IP_1 to get current value from DB
static int callback_readsrv1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.svr_ip_1, argv[0]);
	D(printf("%s\n", info_active.e_info.svr_ip_1));
	return 0;
}
//Callback for EMERG_EXT_S1_1 to get current value from DB
static int callback_reademergs1e1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.emerg_ext_s1_1, argv[0]);
	D(printf("%s\n", info_active.e_info.emerg_ext_s1_1));
	return 0;
}
//Callback for EMERG_EXT_S1_2 to get current value from DB
static int callback_reademergs1e2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.emerg_ext_s1_2, argv[0]);
	D(printf("%s\n", info_active.e_info.emerg_ext_s1_2));
	return 0;
}
//Callback for TEST_EXT_S1 to get current value from DB
static int callback_readte1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.test_ext_s1, argv[0]);
	D(printf("%s\n", info_active.e_info.test_ext_s1));
	return 0;
}
//Callback for ADM_EXT_S1 to get current value from DB
static int callback_readae1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.adm_ext_s1, argv[0]);
	D(printf("%s\n", info_active.e_info.adm_ext_s1));
	return 0;
}
//Callback for SRV_IP_2 to get current value from DB
static int callback_readsrv2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.svr_ip_2, argv[0]);
	D(printf("%s\n", info_active.e_info.svr_ip_2));
	return 0;
}
//Callback for EMERG_EXT_S2_1 to get current value from DB
static int callback_reademergs2e1(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.emerg_ext_s2_1, argv[0]);
	D(printf("%s\n", info_active.e_info.emerg_ext_s2_1));
	return 0;
}
//Callback for EMERG_EXT_S2_2 to get current value from DB
static int callback_reademergs2e2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.emerg_ext_s2_2, argv[0]);
	D(printf("%s\n", info_active.e_info.emerg_ext_s2_2));
	return 0;
}
//Callback for TEST_EXT_S2 to get current value from DB
static int callback_readte2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.test_ext_s2, argv[0]);
	D(printf("%s\n", info_active.e_info.test_ext_s2));
	return 0;
}
//Callback for ADM_EXT_S2 to get current value from DB
static int callback_readae2(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.adm_ext_s2, argv[0]);
	D(printf("%s\n", info_active.e_info.adm_ext_s2));
	return 0;
}
//Callback for REC_LOC to get current value from DB
static int callback_readrecloc(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.rec_loc, argv[0]);
	D(printf("%s\n", info_active.e_info.rec_loc));
	return 0;
}
//Callback for REC_FLG to get current value from DB
static int callback_readrecflag(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.rec_flg, argv[0]);
	D(printf("%s\n", info_active.e_info.rec_flg));
	return 0;
}
//Callback for MIC_VOL to get current value from DB
static int callback_readmicvol(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.mic_vol, argv[0]);
	D(printf("%s\n",info_active.e_info.mic_vol));
	return 0;
}
//Callback for SPK_VOL to get current value from DB
static int callback_readspkvol(void *NotUsed, int argc, char **argv, char **azColName)
{
	strcpy (info_active.e_info.spk_vol, argv[0]);
	D(printf("%s\n",info_active.e_info.spk_vol));
	return 0;
}

//*******************************END OF CALLBACKS FUNCTIONS****************************

//Function for creation of the database and/or evaluation of the database if it already exists
void db_func(void){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	const char* data = "";

	//1. Creating and/or opening the DATABASE
	rc = sqlite3_open("ETB.db", &db); //
	if( rc )
	{
		D(fprintf(stderr, "Can't open database: %s\n\n", sqlite3_errmsg(db)));
	}
	else
	{
		D(fprintf(stdout, "\nETB.db database opened successfully\n\n"));
	}

	//2.a) Checking if P_INFO exists in the database
	sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='P_INFO'";

	//2.b) Executing SQL query for checking if P_INFO exist
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		D(fprintf(stdout, "P_INFO already exists\n\n"));
	}
	else
	{
		//3.a) Creating Project Information Table in the SQL Statement
		sql = "CREATE TABLE P_INFO(P_SNO CHAR(10) PRIMARY KEY NOT NULL,PRJ CHAR(20) NOT NULL,MFR CHAR(20) NOT NULL,MFR_DATE CHAR(20) NOT NULL,HW_VER CHAR(20) NOT NULL, SW_VER CHAR(20) NOT NULL)";

		//3.b) Executing SQL Statement for creating the P_INFO Table
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if( rc != SQLITE_OK )
		{
			D(fprintf(stderr, "%s\n\n", zErrMsg));
			sqlite3_free(zErrMsg);
		}
		else
		{
			D(fprintf(stdout, "Table P_INFO created successfully\n\n"));
			//4.a) SQL query for inserting values in P_INFO
			sql = "INSERT INTO P_INFO (P_SNO, PRJ, MFR, MFR_DATE, HW_VER, SW_VER) VALUES ('1' , 'ETB', 'CSEMI','2022-02-02', 'testvalue', 'testvalue')";
			//4.b) Executing SQL Statement for inserting values the P_INFO Table
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			if( rc != SQLITE_OK )
			{
				D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
				sqlite3_free(zErrMsg);
			}
			else
			{
				D(fprintf(stdout, "Default Settings added in table P_INFO successfully\n\n"));
			}
		}


	}
	//5.a) Checking if N_INFO exists in the database
	sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='N_INFO'";

	//5.b) Executing SQL query for checking if N_INFO exist
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		D(fprintf(stdout, "N_INFO already exists\n\n"));
	}
	else
	{
		//6.a) Creating Network Information Table in the SQL Statement
		sql = "CREATE TABLE N_INFO(ETH_MODE CHAR(10) PRIMARY KEY NOT NULL,IPv4 CHAR(10) NOT NULL,SUB_MASK CHAR(15) NOT NULL,GW CHAR(15) NOT NULL,DNS CHAR(15) NOT NULL)";

		//6.b) Executing SQL Statement for creating the N_INFO Table
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if( rc != SQLITE_OK )
		{
			D(fprintf(stderr, "%s\n\n", zErrMsg));
			sqlite3_free(zErrMsg);
		}
		else
		{
			D(fprintf(stdout, "Table N_INFO created successfully\n\n"));
			//7.a) SQL query for inserting values in N._INFO
//			sql = "INSERT INTO N_INFO (ETH_MODE, IPv4, SUB_MASK, GW, DNS) VALUES ('static' , '192.168.1.50', '255.255.255.0', '192.168.1.1', '8.8.8.8')";

			getting_ip_of_host();

			// defining a char array in order to execute the sql query using sprintf
			char sql_dynamic[1024];
			sprintf (sql_dynamic, "INSERT INTO N_INFO (ETH_MODE, IPv4, SUB_MASK, GW, DNS) VALUES ('static' , '%s', '255.255.255.0', '192.168.1.1', '8.8.8.8')", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

			//7.b) Executing SQL Statement for inserting values the N_INFO Table
			rc = sqlite3_exec(db, sql_dynamic, callback, 0, &zErrMsg);
			if( rc != SQLITE_OK )
			{
				D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
				sqlite3_free(zErrMsg);
			}
			else
			{
				D(fprintf(stdout, "Default Settings added in table N_INFO successfully\n\n"));
			}
		}


	}


	//8.a) Checking if E_INFO exists in the database
	sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='E_INFO'";

	//8.b) Executing SQL query for checking if E_INFO exist
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		D(fprintf(stdout, "E_INFO already exists\n\n"));
	}
	else
	{
		//9.a) Creating Extensions Information Table in the SQL Statement
		sql = "CREATE TABLE E_INFO (SIP_EXT_ETB CHAR(20) PRIMARY KEY NOT NULL,SIP_USER_1 CHAR(10) NOT NULL,SIP_PASS_1 CHAR(10) NOT NULL,SIP_USER_2 CHAR(10) NOT NULL," \
				"SIP_PASS_2 CHAR(10) NOT NULL, ETB_PORT CHAR(10) NOT NULL,SRV_IP_1 CHAR(20) NOT NULL,EMERG_EXT_S1_1 CHAR(10) NOT NULL ,EMERG_EXT_S1_2 CHAR(10) NOT NULL," \
				"TEST_EXT_S1 CHAR(10) NOT NULL,ADM_EXT_S1 CHAR(10) NOT NULL,SRV_IP_2 CHAR(20) NOT NULL,EMERG_EXT_S2_1 CHAR(10) NOT NULL ,EMERG_EXT_S2_2 CHAR(10) NOT NULL," \
				"TEST_EXT_S2 CHAR(10) NOT NULL,ADM_EXT_S2 CHAR(10) NOT NULL,REC_LOC CHAR(50) NOT NULL,REC_FLG CHAR(10) NOT NULL,MIC_VOL CHAR(10) NOT NULL,SPK_VOL CHAR(10) NOT NULL); ";

		//9.b) Executing SQL Statement for creating the E_INFO Table
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if( rc != SQLITE_OK )
		{
			D(fprintf(stderr, "%s\n\n", zErrMsg));
			sqlite3_free(zErrMsg);
		}
		else
		{
			D(fprintf(stdout, "Table E_INFO created successfully\n\n"));
			//10.a) SQL query for inserting values in E_INFO
			sql = 	"INSERT INTO E_INFO (SIP_EXT_ETB ,SIP_USER_1 ,SIP_PASS_1 ,SIP_USER_2, SIP_PASS_2 , ETB_PORT , SRV_IP_1, EMERG_EXT_S1_1,EMERG_EXT_S1_2,TEST_EXT_S1,ADM_EXT_S1,SRV_IP_2 ,EMERG_EXT_S2_1," \
					"EMERG_EXT_S2_2,TEST_EXT_S2 ,ADM_EXT_S2 ,REC_LOC,REC_FLG, MIC_VOL, SPK_VOL) VALUES ('sip:60@192.168.1.22' , '60', '60','201','201','2244','192.168.1.22','100','101','105','106','192.168.1.24','901','902','903','904','/home/Recordings','Enable', '31', '63')";
			//10.b) Executing SQL Statement for inserting values the E_INFO Table
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			if( rc != SQLITE_OK )
			{
				D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
				sqlite3_free(zErrMsg);
			}
			else
			{
				D(fprintf(stdout, "Default Settings added in table E_INFO successfully\n\n"));
			}
		}


	}


	//11.a) Checking if CH_INFO exists in the database
	sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='CH_INFO'";

	//11.b) Executing SQL query for checking if CH_INFO exist
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		D(fprintf(stdout, "CH_INFO already exists\n\n"));
	}
	else
	{
		//12.a) Creating Call History Information Table in the SQL Statement
		sql = "CREATE TABLE CH_INFO (CALL_DIR CHAR(20) PRIMARY KEY NOT NULL,TIME_STAMP CHAR(20) NOT NULL,START_TIME CHAR(20) NOT NULL,END_TIME CHAR(20) NOT NULL,SRC CHAR(20) NOT NULL ,DEST CHAR(20) NOT NULL ,STATUS CHAR(20) NOT NULL) ";

		//12.b) Executing SQL Statement for creating the CH_INFO Table
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		if( rc != SQLITE_OK )
		{
			D(fprintf(stderr, "%s\n\n", zErrMsg));
			sqlite3_free(zErrMsg);
		}
		else
		{
			D(fprintf(stdout, "Table CH_INFO created successfully\n\n"));
			//13.a) SQL query for inserting values in E_INFO
			sql = 	"INSERT INTO CH_INFO (CALL_DIR ,TIME_STAMP,START_TIME,END_TIME,SRC ,DEST ,STATUS) VALUES ('Outgoing' , '2022-02-02 15:34:55', '15:35:43','15:37:23','101','102','SUCCESS')";
			//13.b) Executing SQL Statement for inserting values the E_INFO Table
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			if( rc != SQLITE_OK )
			{
				D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
				sqlite3_free(zErrMsg);
			}
			else
			{
				D(fprintf(stdout, "Default Settings added in table CH_INFO successfully\n\n"));
			}
		}


	}

	//14.a) SQL query for displaying the values in P_INFO
	sql = "SELECT * from P_INFO";
	//14.b) Executing SQL Statement for displaying values present in P_INFO Table
	printf("Product Information Table contents - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else
	{
		//(fprintf(stdout, "^ PRODUCT INFORMATION TABLE\n\n"));
	}

	//15.a) SQL query for displaying the values in N_INFO
	sql = "SELECT * from N_INFO";
	//15.b) Executing SQL Statement for displaying values present in N_INFO Table
	printf("Network Information Table contents - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else
	{
		//(fprintf(stdout, "^ NETWORK TABLE\n\n"));
	}

	//16.a) SQL query for displaying the values in E_INFO
	sql = "SELECT * from E_INFO";
	//16.b) Executing SQL Statement for displaying values present in E_INFO Table
	printf("Extension Information Table contents - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else
	{
		//(fprintf(stdout, "^ EXTENSIONS TABLE\n\n"));
	}

	//17.a) SQL query for displaying the values in CH_INFO
	sql = "SELECT * from CH_INFO";
	//17.b) Executing SQL Statement for displaying values present in CH_INFO Table
	printf("Call History Information Table created with the following contents - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else
	{
		//(fprintf(stdout, "^ EXTENSIONS TABLE\n\n"));
	}
	printf("Closing ETB.db...\n");
	sqlite3_close(db);
}





//Function to read the current values from the database

void read_from_db(void){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	//Opening the database
	printf("Opening ETB.db...\n\nReading information from ETB.db...\n\n");
	rc = sqlite3_open("ETB.db", &db);
	if( rc ){D(fprintf(stderr, "Can't open database: %s\n\n", sqlite3_errmsg(db)));}
	else{D(fprintf(stdout, "\nETB.db database opened successfully\n\n"));
	//1. Saving P_SNO to variable info_active.p_info.prod_sno
	sql = "SELECT P_SNO FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readpsno, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "P_SNO selection query executed\n"));}

	//2. Saving PRJ to variable info_active.p_info.project
	sql = "SELECT PRJ FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readprj, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "PRJ selection query executed\n"));}

	//3. Saving MFR to variable info_active.p_info.mfr
	sql = "SELECT MFR FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readmfr, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "MFR selection query executed\n"));}

	//4. Saving MFR_DATE to variable info_active.p_info.mfrdate
	sql = "SELECT MFR_DATE FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readmfrdate, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "MFR_DATE selection query executed\n"));}

	//5. Saving HW_VER to variable info_active.p_info.hwver
	sql = "SELECT HW_VER FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readhwver, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "HW_VER selection query executed\n"));}

	//6. Saving SW_VER to variable info_active.p_info.swver
	sql = "SELECT SW_VER FROM P_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readswver, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SW_VER selection query executed\n"));}

	//7. Saving ETH_MODE to variable info_active.n_info.eth_mode
	sql = "SELECT ETH_MODE FROM N_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readethmode, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "ETH_MODE selection query executed\n"));}

	//8. Saving IPv4 to variable info_active.n_info.ipv4_add
	sql = "SELECT IPv4 FROM N_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readipv4, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "IPv4 selection query executed\n"));}

	//9. Saving SUB_MASK to variable info_active.n_info.subnet_mask
	sql = "SELECT SUB_MASK FROM N_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsub, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SUB_MASK selection query executed\n"));}

	//10. Saving GW to variable info_active.n_info.gateway
	sql = "SELECT GW FROM N_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readgw, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "GW selection query executed\n"));}

	//11. Saving DNS to variable info_active.n_info.dns
	sql = "SELECT DNS FROM N_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readdns, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "DNS selection query executed\n"));}

	//12. Saving SIP_EXT_ETB to variable info_active.e_info.sip_exten_etb
	sql = "SELECT SIP_EXT_ETB FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipxetb, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SIP_EXT_ETB selection query executed\n"));}

	//13. Saving SIP_USER_1 to variable info_active.e_info.sip_user_1
	sql = "SELECT SIP_USER_1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipu1, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SIP_USER_1 selection query executed\n"));}

	//14. Saving SIP_PASS_1 to variable info_active.e_info.sip_pass_1
	sql = "SELECT SIP_PASS_1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipp1, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SIP_PASS_1 selection query executed\n"));}

	//15. Saving SIP_USER_2 to variable info_active.e_info.sip_user_2
	sql = "SELECT SIP_USER_2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipu2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SIP_USER_2 selection query executed\n"));}

	//16. Saving SIP_PASS_2 to variable info_active.e_info.sip_pass_2
	sql = "SELECT SIP_PASS_2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipp2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SIP_PASS_2 selection query executed\n"));}

	//17. Saving ETB_PORT to variable info_active.e_info.etb_port
	sql = "SELECT ETB_PORT FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsipetbp, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "ETB_PORT selection query executed\n"));}

	//18. Saving SRV_IP_1  to variable info_active.e_info.svr_ip_1
	sql = "SELECT SRV_IP_1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsrv1, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SRV_IP_1 selection query executed\n"));}

	//19. Saving EMERG_EXT_S1_1 to variable info_active.e_info.emerg_ext_s1_1
	sql = "SELECT EMERG_EXT_S1_1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_reademergs1e1, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "EMERG_EXT_S1_1 selection query executed\n"));}

	//20. Saving EMERG_EXT_S1_2  to variable info_active.e_info.emerg_ext_s1_2
	sql = "SELECT EMERG_EXT_S1_2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_reademergs1e2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "EMERG_EXT_S1_2 selection query executed\n"));}

	//21. Saving TEST_EXT_S1 to variable info_active.e_info.test_ext_s1
	sql = "SELECT TEST_EXT_S1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readte1 , 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "TEST_EXT_S1 selection query executed\n"));}

	//22. Saving ADM_EXT_S1 to variable info_active.e_info.adm_ext_s1
	sql = "SELECT ADM_EXT_S1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readae1 , 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "ADM_EXT_S1 selection query executed\n"));}

	//23. Saving SRV_IP_2 to variable info_active.e_info.svr_ip_2
	sql = "SELECT SRV_IP_2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readsrv2 , 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SRV_IP_2  selection query executed\n"));}

	//24. Saving EMERG_EXT_S2_1 to variable info_active.e_info.emerg_ext_s2_1
	sql = "SELECT EMERG_EXT_S2_1 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_reademergs2e1, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "EMERG_EXT_S2_1  selection query executed\n"));}

	//25. Saving EMERG_EXT_S2_2 to variable info_active.e_info.emerg_ext_s2_2
	sql = "SELECT EMERG_EXT_S2_2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_reademergs2e2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "EMERG_EXT_S2_2  selection query executed\n"));}

	//26. Saving TEST_EXT_S2 to variable info_active.e_info.test_ext_s2
	sql = "SELECT TEST_EXT_S2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readte2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "TEST_EXT_S2   selection query executed\n"));}

	//27. Saving ADM_EXT_S2 to variable info_active.e_info.adm_ext_s2
	sql = "SELECT ADM_EXT_S2 FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readae2, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "ADM_EXT_S2   selection query executed\n"));}

	//28. Saving REC_LOC to variable info_active.e_info.rec_loc
	sql = "SELECT REC_LOC FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readrecloc, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "REC_LOC   selection query executed\n"));}

	//29. Saving REC_FLG to variable info_active.e_info.rec_flg
	sql = "SELECT REC_FLG FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readrecflag, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "REC_LOC   selection query executed\n"));}


	//30. Saving MIC_VOL to variable info_active.e_info.mic_vol
	sql = "SELECT MIC_VOL FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readmicvol, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "MIC_VOL   selection query executed\n"));}

	//31. Saving SPK_VOL to variable info_active.e_info.spk_vol
	sql = "SELECT SPK_VOL FROM E_INFO";
	//Executing the SQL query
	rc = sqlite3_exec(db, sql, callback_readspkvol, 0, &zErrMsg);
	if( rc != SQLITE_OK ){(fprintf(stderr, "%s\n\n", zErrMsg));sqlite3_free(zErrMsg);}
	else{D(fprintf(stdout, "SPK_VOL   selection query executed\n"));}

	}

	printf("Closing ETB.db...\n\n");
	sqlite3_close(db);
}




// Function to update the information in ETB.db database

void write_to_db(void){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[1024];
	const char* data = "";

	//	1.Opening the database
	rc = sqlite3_open("ETB.db", &db);
	if( rc ){D(fprintf(stderr, "Can't open database: %s\n\n", sqlite3_errmsg(db)));}
	else{(fprintf(stdout, "\nETB.db database opened successfully\n\n"));

	//	2. a) Updating P_INFO by inserting the values received from database into the table
	sprintf( sql,"UPDATE P_INFO SET P_SNO = '%s', PRJ = '%s', MFR = '%s', MFR_DATE = '%s', HW_VER = '%s', SW_VER = '%s'",info_recv.p_info.prod_sno , info_recv.p_info.project, info_recv.p_info.mfr, info_recv.p_info.mfr_date, info_recv.p_info.hw_ver, info_recv.p_info.sw_ver);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		D(fprintf(stdout, "P_INFO table updated successfully\n\n"));
	}

	//	printf("%s\n%s\n%s\n%s\n", info_recv.p_info.prod_sno, info_recv.p_info.project, info_recv.p_info.mfr, info_recv.p_info.mfr_date);

	//  2. b) Displaying the contents of P_INFO
	sprintf(sql, "SELECT * from P_INFO");
	//	2. c) Executing SQL Statement for displaying values present in P_INFO Table
	printf("Product Information Table - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		//(fprintf(stdout, "^ PRODUCT INFORMATION TABLE\n\n"));
	}


	//	3. a) Updating N_INFO by inserting the values received from database into the table

	sprintf( sql,"UPDATE N_INFO SET ETH_MODE = '%s', IPv4 = '%s', SUB_MASK = '%s', GW = '%s', DNS = '%s'",info_recv.n_info.eth_mode , info_recv.n_info.ipv4_add,info_recv.n_info.subnet_mask, info_recv.n_info.gateway, info_recv.n_info.dns);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		D(fprintf(stdout, "N_INFO table updated successfully\n\n"));
	}

	//	printf("%s\n%s\n%s\n%s\n%s\n", info_recv.n_info.eth_mode, info_recv.n_info.ipv4_add, info_recv.n_info.subnet_mask, info_recv.n_info.gateway, info_recv.n_info.dns);

	//  3. b) Displaying the contents of N_INFO
	sprintf(sql, "SELECT * FROM N_INFO");
	//	3. c) Executing SQL Statement for displaying values present in N_INFO Table
	printf("Network Information Table - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		//(fprintf(stdout, "^ NETWORK INFORMATION TABLE\n\n"));
	}

	//	4. a) Updating E_INFO by inserting the values received from database into the table
	sprintf( sql,"UPDATE E_INFO SET SIP_EXT_ETB = '%s', SIP_USER_1 = '%s', SIP_PASS_1 = '%s', SIP_USER_2 = '%s', SIP_PASS_2 = '%s' ,ETB_PORT = '%s', SRV_IP_1 = '%s', EMERG_EXT_S1_1 = '%s', EMERG_EXT_S1_2 = '%s', TEST_EXT_S1 = '%s', ADM_EXT_S1 = '%s', SRV_IP_2 = '%s', EMERG_EXT_S2_1 = '%s', EMERG_EXT_S2_2 = '%s', TEST_EXT_S2 = '%s', ADM_EXT_S2 = '%s', REC_LOC = '%s', REC_FLG = '%s', MIC_VOL = '%s', SPK_VOL = '%s'", info_recv.e_info.sip_exten_etb, info_recv.e_info.sip_user_1, info_recv.e_info.sip_pass_1, info_recv.e_info.sip_user_2, info_recv.e_info.sip_pass_2, info_recv.e_info.etb_port, info_recv.e_info.svr_ip_1, info_recv.e_info.emerg_ext_s1_1, info_recv.e_info.emerg_ext_s1_2, info_recv.e_info.test_ext_s1, info_recv.e_info.adm_ext_s1, info_recv.e_info.svr_ip_2, info_recv.e_info.emerg_ext_s2_1, info_recv.e_info.emerg_ext_s2_2, info_recv.e_info.test_ext_s2, info_recv.e_info.adm_ext_s2, info_recv.e_info.rec_loc, info_recv.e_info.rec_flg, info_recv.e_info.mic_vol, info_recv.e_info.spk_vol);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		D(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		D(fprintf(stdout, "N_INFO table updated successfully\n\n"));
	}

	//	printf("%s\n%s\n%s\n%s\n%s\n", info_recv.n_info.eth_mode, info_recv.n_info.ipv4_add, info_recv.n_info.subnet_mask, info_recv.n_info.gateway, info_recv.n_info.dns);

	//  4. b) Displaying the contents of E_INFO
	sprintf(sql, "SELECT * FROM E_INFO");
	//	4. c) Executing SQL Statement for displaying values present in E_INFO Table
	printf("Network Information Table - \n");
	rc = sqlite3_exec(db, sql, call_back, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
		(fprintf(stderr, "SQL error: %s\n\n", zErrMsg));
		sqlite3_free(zErrMsg);
	}
	else{
		//(fprintf(stdout, "^ EXTENSIONS INFORMATION TABLE\n\n"));
	}
	}
	printf("Closing ETB.db...\n");
	sqlite3_close(db);

}
