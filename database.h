#pragma once


#include <ctime>
#include <stdlib.h>
#include "mysql.h"
#include <string>
using namespace std;

typedef struct _db_bist
{
	char worker_id[32];
	char time[32];
	char flash[32];
	char rgb[32];
	char ir[32];
	char tec[32];
	char ld[32];
	char ldp[32];
	char codec[32];
}db_bist;

typedef struct _db_part
{
	char worker_id[32];
	char time[32];
	char res[32];
}db_part;

typedef struct _db_item
{
	char pn[512];
	char sn[512];

	db_bist bist;

	db_part preheat;
	db_part depth_calib;
	db_part d2c_calib;
	db_part depth_quality;

}db_item;


class database
{
public:
	database(void);
	~database(void);

	db_item cur_item;
	char    next_pn[12];
	char    next_sn[12];

	int init_database();
	int open_database(const char *host,   //host
		const char *user,   //user
		const char *passwd, //passewd
		const char *db,     //database
		const char *table,
		unsigned int port);
	int close_database();

	int init_item(db_item *item);

	int get_next_valid_pn_by_worker(int worker);
	int get_next_valid_sn_by_worker(int worker);

	int insert_item(db_item *item);
	int update_item(db_item *item);

	int get_item_by_pn(char * pn);
	int get_item_by_sn(char * sn);

	int update_bist_by_pn(char *pn, int worker_id, db_bist *bist);
	int update_preheat_by_pn(char *pn, int worker_id, int val);
	int update_depthcalib_by_pn(char *pn, int worker_id, int val);
	int update_d2ccalib_by_pn(char *pn, int worker_id, int val);
	int update_depthquality_by_pn(char *pn, int worker_id, int val);

	int update_sn_by_pn(char *pn, int worker_id, char *sn);

	int count_bist(int year, int month, int day, int worker_id, int * success, int *failed);
	int count_preheat(int year, int month, int day, int worker_id, int * success, int *failed);
	int count_depthcalib(int year, int month, int day, int worker_id, int * success, int *failed);
	int count_d2ccalib(int year, int month, int day, int worker_id, int * success, int *failed);
	int count_depthquality(int year, int month, int day, int worker_id, int * success, int *failed);


	int check_bist_by_pn(char *pn);
	int check_preheat_by_pn(char *pn);
	int check_depthcalib_by_pn(char *pn);
	int check_d2ccalib_by_pn(char *pn);
	int check_depthquality_by_pn(char *pn);

private:

	MYSQL m_mysql;
	char  tablename[128];


};


