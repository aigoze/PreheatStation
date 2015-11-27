#include "database.h"


static int sn_is_valid(unsigned char * sn)
{
	unsigned int i = 0;
	for (i = 0; i<11; i++)
	{
		if ((sn[i]<0x30) || (sn[i]>0x39))
		{
			return -1;
		}
	}
	return 0;
}

database::database(void)
{
}

database::~database(void)
{
}

int database::init_database()
{
	MYSQL* res_mysql = NULL;
	res_mysql = mysql_init(&m_mysql);
	if (res_mysql == NULL)
	{
		printf_s("Initialize MYSQL Failed\r\n");
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	return 0;
}

int database::open_database(const char *host,   //host
	const char *user,   //user
	const char *passwd, //passewd
	const char *db,     //database
	const char *table,
	unsigned int port)  //port
{

	int ret = 0;
	char sql[1024];
	MYSQL* res_mysql = NULL;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	res_mysql = mysql_real_connect(&m_mysql, host, user, passwd, db, port, NULL, 0);
	if (res_mysql == NULL)
	{
		printf_s("Connect host : %s Database : %s  : Failed\r\n", host, db);
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	printf_s("Connect host : %s Database : %s  Success\r\n", host, db);
	//check the database find the table
	sprintf_s(sql, "SHOW TABLES like '%s'", table);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	rowcount = mysql_num_rows(result);
	fieldcount = mysql_num_fields(result);
	row = mysql_fetch_row(result);
	if (rowcount == 0)
	{
		//create new table
		printf_s("Create new Table\n");
		sprintf_s(sql, "create table %s (PN char(11) primary key,SN char(15),BIST_WORKER_ID integer,Time text,Flash bit(8),RGB bit(8),IR bit(8),TEC bit(8),LD bit(8),LDP bit(8),Codec bit(8),PreHeat_WORKER_ID integer,PreHeat_Time text,PreHeat integer,Depth_Calib_WORKER_ID integer,Depth_Calib_Time text,Depth_Calib integer ,D2C_Calib_WORKER_ID integer,D2C_Calib_Time text,D2C_Calib integer,Depth_Quality_WORKER_ID integer,Depth_Quality_Time text,Depth_Quality integer)", table);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
	}
	else
	{
		printf("Table %s exist in %s at %s\r\n", table, db, host);
	}
	strcpy_s(tablename, table);

	return 0;


}

int database::close_database()
{
	mysql_close(&m_mysql);
	return 0;
}

int database::init_item(db_item *item)
{
	strcpy_s(item->pn, "FFFFFFFFFFF");
	strcpy_s(item->sn, "FFFFFFFFFFF");
	strcpy_s(item->bist.flash, "-1");
	strcpy_s(item->bist.tec, "-1");
	strcpy_s(item->bist.ir, "-1");
	strcpy_s(item->bist.rgb, "-1");
	strcpy_s(item->bist.ldp, "-1");
	strcpy_s(item->bist.ld, "-1");
	strcpy_s(item->bist.codec, "-1");
	strcpy_s(item->bist.worker_id, "0");

	strcpy_s(item->preheat.worker_id, "0");
	strcpy_s(item->preheat.res, "-1");

	strcpy_s(item->depth_calib.worker_id, "0");
	strcpy_s(item->depth_calib.res, "-1");

	strcpy_s(item->d2c_calib.worker_id, "0");
	strcpy_s(item->d2c_calib.res, "-1");

	strcpy_s(item->depth_quality.worker_id, "0");
	strcpy_s(item->depth_quality.res, "-1");
	return 0;
}

int database::count_bist(int year, int month, int day,int worker_id,int * success, int *failed)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;
	int all_count;
	sprintf_s(sql, "select * from %s where time like '%%%02d%%-%02d%%-%02d%%' and bist_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	all_count = mysql_num_rows(result);
	if (all_count == 0)
	{
		printf_s("No Device be tested\r\n");
		*success = 0;
		*failed = 0;
		return 0;
	}
	else
	{
		sprintf_s(sql, "select * from %s item where time like '%%%02d%%-%02d%%-%02d%%'  and bist_worker_id = %d and \
					   mod(item.flash,2) = 1  and mod(item.ir,2) = 1 and mod(item.rgb,2) = 1  and mod(item.tec,2) = 1  and mod(item.ldp,2) = 1  and mod(item.ld,2) = 1  and mod(item.codec,2) = 1", tablename, year, month, day,worker_id);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		*success = mysql_num_rows(result);
		*failed = all_count - *success;
		return 0;
	}
	return 0;
}

int database::count_preheat(int year, int month, int day, int worker_id, int * success, int *failed)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;
	int all_count;
	sprintf_s(sql, "select * from %s where PreHeat_Time like '%%%02d%%-%02d%%-%02d%%' and preheat=0  and preheat_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*failed = mysql_num_rows(result);

	sprintf_s(sql, "select * from %s where PreHeat_Time like '%%%02d%%-%02d%%-%02d%%' and preheat=1 and preheat_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*success = mysql_num_rows(result);

	return 0;
}

int database::count_depthcalib(int year, int month, int day, int worker_id, int * success, int *failed)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;
	int all_count;
	sprintf_s(sql, "select * from %s where Depth_Calib_Time like '%%%02d%%-%02d%%-%02d%%' and depth_calib=0 and depth_calib_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*failed = mysql_num_rows(result);

	sprintf_s(sql, "select * from %s where Depth_Calib_Time like '%%%02d%%-%02d%%-%02d%%' and depth_calib=1 and depth_calib_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*success = mysql_num_rows(result);

	return 0;
}

int database::count_d2ccalib(int year, int month, int day, int worker_id, int * success, int *failed)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;
	int all_count;
	sprintf_s(sql, "select * from %s where D2C_Calib_Time like '%%%02d%%-%02d%%-%02d%%' and d2c_calib=0 and d2c_calib_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*failed = mysql_num_rows(result);

	sprintf_s(sql, "select * from %s where D2C_Calib_Time like '%%%02d%%-%02d%%-%02d%%' and d2c_calib=1 and d2c_calib_worker_id = %d", tablename, year, month, day,worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*success = mysql_num_rows(result);

	return 0;
}

int database::count_depthquality(int year, int month, int day, int worker_id, int * success, int *failed)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;
	int all_count;
	sprintf_s(sql, "select * from %s where Depth_Quality_Time like '%%%02d%%-%02d%%-%02d%%' and depth_quality=0 and depth_quality_worker_id = %d", tablename, year, month, day, worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*failed = mysql_num_rows(result);

	sprintf_s(sql, "select * from %s where Depth_Quality_Time like '%%%02d%%-%02d%%-%02d%%' and depth_quality=1 and depth_quality_worker_id = %d", tablename, year, month, day, worker_id);
	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);
	*success = mysql_num_rows(result);

	return 0;
}

int database::get_next_valid_pn_by_worker(int worker)
{
	time_t t;
	struct tm tm1;
	char sql[512];
	char pn_buf[12];
	int  ret = 0;
	unsigned long  year = 0, month = 0, day = 0, pn = 0;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row = NULL;

	t = time(&t);
	localtime_s(&tm1, &t);

	year = (tm1.tm_year + 1900) % 2000;
	month = tm1.tm_mon + 1;
	day = tm1.tm_mday;

	sprintf_s(sql, "select * from (select * from %s where PN like '%2.2d%2.2d%2.2d%1.1d____') a where \
				   	PN=(select max(PN) as max_pn from (select * from %s where PN like '%2.2d%2.2d%2.2d%1.1d____') b)"\
					, tablename, year, month, day, worker, tablename, year, month, day, worker);


	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);

	if (mysql_num_rows(result) == 0)
	{
		sprintf_s(pn_buf, "%2.2d%2.2d%2.2d%1.1d%4.4d", year, month, day, worker, 0);
		strcpy_s(next_pn, pn_buf);
		return 0;
	}
	else
	{
		rowcount = mysql_num_rows(result);
		fieldcount = mysql_num_fields(result);
		row = mysql_fetch_row(result);

		strcpy_s(pn_buf, &row[0][7]);
		pn = strtoul(pn_buf, (char**)NULL, 10);

		if (pn>9999)
		{
			printf_s("PN Overflow!SN:%d\r\n", pn);
			return -1;
		}
		pn = pn + 1;
		sprintf_s(pn_buf, "%2.2d%2.2d%2.2d%1.1d%4.4d"
			, year, month, day, worker, pn);
		strcpy_s(next_pn, pn_buf);
		return 0;

	}

	return 0;
}

int database::get_next_valid_sn_by_worker(int worker)
{
	time_t t;
	struct tm tm1;
	char sql[512];
	char sn_buf[12];
	int  ret = 0;
	unsigned long  year = 0, month = 0, day = 0, sn = 0;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row = NULL;

	t = time(&t);
	localtime_s(&tm1, &t);

	year = (tm1.tm_year + 1900) % 2000;
	month = tm1.tm_mon + 1;
	day = tm1.tm_mday;

	sprintf_s(sql, "select * from (select * from %s where SN like '%2.2d%2.2d%2.2d%1.1d____') a where \
				   	SN=(select max(SN) as max_sn from (select * from %s where SN like '%2.2d%2.2d%2.2d%1.1d____') b)"\
					,tablename, year, month, day, worker, tablename, year, month, day, worker);


	ret = mysql_query(&m_mysql, sql);
	if (ret != 0)
	{
		printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
		return -1;
	}
	result = mysql_store_result(&m_mysql);

	if (mysql_num_rows(result) == 0)
	{
		sprintf_s(sn_buf, "%2.2d%2.2d%2.2d%1.1d%4.4d", year, month, day, worker, 0);
		strcpy_s(next_sn, sn_buf);
		return 0;
	}
	else
	{
		rowcount = mysql_num_rows(result);
		fieldcount = mysql_num_fields(result);
		row = mysql_fetch_row(result);

		strcpy_s(sn_buf, &row[1][7]);
		sn = strtoul(sn_buf, (char**)NULL, 10);

		if (sn>9999)
		{
			printf_s("SN Overflow!SN:%d\r\n", sn);
			return -1;
		}
		sn = sn + 1;
		sprintf_s(sn_buf, "%2.2d%2.2d%2.2d%1.1d%4.4d"
			, year, month, day, worker, sn);
		strcpy_s(next_sn, sn_buf);
		return 0;

	}

	return 0;

}

int database::insert_item(db_item *item)
{

	int ret = 0;
	char sql[1024*10];

	//check the input valid
	if ((sn_is_valid((unsigned char *)item->pn) == 0)&(sn_is_valid((unsigned char *)item->pn) == 0))
	{
		//set the item
		sprintf_s(sql, "insert into %s values(%s,%s,%s,current_timestamp,%s,%s,%s,%s,%s,%s,%s,\
					   %s,current_timestamp,%s,\
					   %s,current_timestamp,%s,\
					   %s,current_timestamp,%s,\
					   %s,current_timestamp,%s)", \
					   tablename,item->pn, item->sn, item->bist.worker_id, item->bist.flash, item->bist.rgb, item->bist.ir, item->bist.tec, item->bist.ld, item->bist.ldp, item->bist.codec, \
					   item->preheat.worker_id, item->preheat.res, \
					   item->depth_calib.worker_id, item->depth_calib.res, \
					   item->d2c_calib.worker_id, item->d2c_calib.res, \
					   item->depth_quality.worker_id, item->depth_quality.res);
		
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		//check the item
		ret = get_item_by_pn(item->pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}
		return 0;
	}
	else
	{
		printf_s("PN or SN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::get_item_by_pn(char * pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);

		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		else
		{
			rowcount = mysql_num_rows(result);
			fieldcount = mysql_num_fields(result);
			row = mysql_fetch_row(result);

			//copy the result
			strcpy_s(cur_item.pn, row[0]);
			strcpy_s(cur_item.sn, row[1]);

			strcpy_s(cur_item.bist.worker_id, row[2]);
			strcpy_s(cur_item.bist.time, row[3]);

			strcpy_s(cur_item.bist.flash, row[4]);
			strcpy_s(cur_item.bist.rgb, row[5]);
			strcpy_s(cur_item.bist.ir, row[6]);
			strcpy_s(cur_item.bist.tec, row[7]);
			strcpy_s(cur_item.bist.ld, row[8]);
			strcpy_s(cur_item.bist.ldp, row[9]);
			strcpy_s(cur_item.bist.codec, row[10]);

			strcpy_s(cur_item.preheat.worker_id, row[11]);
			strcpy_s(cur_item.preheat.time, row[12]);
			strcpy_s(cur_item.preheat.res, row[13]);

			strcpy_s(cur_item.depth_calib.worker_id, row[14]);
			strcpy_s(cur_item.depth_calib.time, row[15]);
			strcpy_s(cur_item.depth_calib.res, row[16]);

			strcpy_s(cur_item.d2c_calib.worker_id, row[17]);
			strcpy_s(cur_item.d2c_calib.time, row[18]);
			strcpy_s(cur_item.d2c_calib.res, row[19]);

			strcpy_s(cur_item.depth_quality.worker_id, row[20]);
			strcpy_s(cur_item.depth_quality.time, row[21]);
			strcpy_s(cur_item.depth_quality.res, row[22]);

			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::get_item_by_sn(char * sn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)sn) == 0)
	{
		sprintf_s(sql, "select * from %s where sn=%s", tablename, sn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);

		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		else
		{
			rowcount = mysql_num_rows(result);
			fieldcount = mysql_num_fields(result);
			row = mysql_fetch_row(result);

			//copy the result
			strcpy_s(cur_item.pn, row[0]);
			strcpy_s(cur_item.sn, row[1]);

			strcpy_s(cur_item.bist.worker_id, row[2]);
			strcpy_s(cur_item.bist.time, row[3]);

			strcpy_s(cur_item.bist.flash, row[4]);
			strcpy_s(cur_item.bist.rgb, row[5]);
			strcpy_s(cur_item.bist.ir, row[6]);
			strcpy_s(cur_item.bist.tec, row[7]);
			strcpy_s(cur_item.bist.ld, row[8]);
			strcpy_s(cur_item.bist.ldp, row[9]);
			strcpy_s(cur_item.bist.codec, row[10]);

			strcpy_s(cur_item.preheat.worker_id, row[11]);
			strcpy_s(cur_item.preheat.time, row[12]);
			strcpy_s(cur_item.preheat.res, row[13]);

			strcpy_s(cur_item.depth_calib.worker_id, row[14]);
			strcpy_s(cur_item.depth_calib.time, row[15]);
			strcpy_s(cur_item.depth_calib.res, row[16]);

			strcpy_s(cur_item.d2c_calib.worker_id, row[17]);
			strcpy_s(cur_item.d2c_calib.time, row[18]);
			strcpy_s(cur_item.d2c_calib.res, row[19]);

			strcpy_s(cur_item.depth_quality.worker_id, row[20]);
			strcpy_s(cur_item.depth_quality.time, row[21]);
			strcpy_s(cur_item.depth_quality.res, row[22]);

			return 0;
		}
	}
	else
	{
		printf_s("SN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_item(db_item *item)
{
	int ret = 0;
	char sql[1024];
	MYSQL_RES *result = NULL;
	//check the input valid
	if ((sn_is_valid((unsigned char *)item->pn) == 0)&(sn_is_valid((unsigned char *)item->pn) == 0))
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, item->pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}

		sprintf_s(sql, "update %s set BIST_WORKER_ID = %s,Time = current_timestamp,Flash = %s,RGB = %s,IR = %s,TEC = %s,LD = %s,LDP = %s,Codec = %s,\
					   	PreHeat_WORKER_ID = %s,PreHeat_Time = current_timestamp,PreHeat = %s,\
						Depth_Calib_WORKER_ID = %s,Depth_Calib_Time = current_timestamp,Depth_Calib = %s,\
						D2C_Calib_WORKER_ID = %s,D2C_Calib_Time = current_timestamp,D2C_Calib = %s,\
						Depth_Quality_WORKER_ID = %s,Depth_Quality_Time=current_timestamp,Depth_Quality = %s  where pn = %s",
						tablename,item->bist.worker_id, item->bist.flash, item->bist.rgb, item->bist.ir, item->bist.tec, item->bist.ld, item->bist.ldp, item->bist.codec, \
						item->preheat.worker_id, item->preheat.res, \
						item->depth_calib.worker_id, item->depth_calib.res, \
						item->d2c_calib.worker_id, item->d2c_calib.res, \
						item->depth_quality.worker_id, item->depth_quality.res,item->pn);

		//printf_s(sql);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}

		//check the item
		ret = get_item_by_pn(item->pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}
		return 0;

	}
	else
	{
		printf_s("SN or PN is illegal\r\n");
		return -1;
	}
	return 0;

}

int database::update_bist_by_pn(char *pn, int worker_id, db_bist *bist)
{
	int ret = 0;
	char sql[1024];
	MYSQL_RES *result = NULL;

	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set BIST_WORKER_ID = %s,Time = current_timestamp,Flash = %s,RGB = %s,IR = %s,TEC = %s,LD = %s,LDP = %s,Codec = %s where pn = %s", \
			tablename, bist->worker_id, bist->flash, bist->rgb, bist->ir, bist->tec, bist->ld, bist->ldp, bist->codec, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}

		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}
		return 0;
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_preheat_by_pn(char *pn, int worker_id, int val)
{
	int ret = 0;
	char sql[1024];
	MYSQL_RES *result = NULL;

	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set PreHeat_WORKER_ID = %d ,PreHeat_Time=current_timestamp, PreHeat=%d  where pn = %s", tablename, worker_id, val, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}

		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}
		return 0;
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_depthcalib_by_pn(char *pn, int worker_id, int val)
{
	int ret;
	char sql[1024];
	MYSQL_RES *result = NULL;
	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set Depth_Calib_WORKER_ID = %d ,Depth_Calib_Time=current_timestamp, Depth_Calib=%d  where pn = %s", tablename, worker_id, val, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}

		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}
		return 0;

	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_d2ccalib_by_pn(char *pn, int worker_id, int val)
{

	int ret;
	char sql[1024];
	MYSQL_RES *result = NULL;
	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set D2C_Calib_WORKER_ID = %d ,D2C_Calib_Time=current_timestamp,D2C_Calib=%d  where pn = %s", tablename, worker_id, val, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}

		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}

		return 0;

	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_depthquality_by_pn(char *pn, int worker_id, int val)
{

	int ret;
	char sql[1024];
	MYSQL_RES *result = NULL;
	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set Depth_Quality_WORKER_ID = %d ,Depth_Quality_Time=current_timestamp,Depth_Quality=%d  where pn = %s", tablename, worker_id, val, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}


		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}

		return 0;

	}
	else
	{
		printf_s("sn is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::update_sn_by_pn(char *pn, int worker_id, char *sn)
{

	int ret;
	char sql[1024];
	MYSQL_RES *result = NULL;
	//check the input valid
	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		//set the item
		sprintf_s(sql, "select * from %s where pn=%s", tablename, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		if (mysql_num_rows(result) == 0)
		{
			printf_s("Can't found the item\r\n");
			return -1;
		}
		//set the item
		sprintf_s(sql, "update %s set sn=%s where pn = %s", tablename, sn, pn);
		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}


		//check the item
		ret = get_item_by_pn(pn);
		if (ret != 0)
		{
			printf_s("Write database error\r\n");
			return -1;
		}

		return 0;

	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
	return 0;
}

int database::check_bist_by_pn(char *pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s item where pn=%s and \
					   	mod(item.flash,2) = 1  and mod(item.ir,2) = 1 and mod(item.rgb,2) = 1  and mod(item.tec,2) = 1  and mod(item.ldp,2) = 1  and mod(item.ld,2) = 1  and mod(item.codec,2) = 1", tablename, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		rowcount = mysql_num_rows(result);
		if (rowcount == 0)
		{
			printf_s("Check BIST Failed\r\n");
			return -1;
		}
		else
		{
			printf_s("Check BIST Success\r\n");
			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
}

int database::check_preheat_by_pn(char *pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s item where pn=%s  and preheat = 1",tablename, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		rowcount = mysql_num_rows(result);
		if (rowcount == 0)
		{
			printf_s("Check PreHeat Failed\r\n");
			return -1;
		}
		else
		{
			printf_s("Check PreHeat Success\r\n");
			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
}

int database::check_depthcalib_by_pn(char *pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s item where pn=%s  and depth_calib = 1", tablename, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		rowcount = mysql_num_rows(result);
		if (rowcount == 0)
		{
			printf_s("Check Depth_Calib Failed\r\n");
			return -1;
		}
		else
		{
			printf_s("Check Depth_Calib Success\r\n");
			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}

}

int database::check_d2ccalib_by_pn(char *pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s item where pn=%s  and d2c_calib = 1", tablename, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		rowcount = mysql_num_rows(result);
		if (rowcount == 0)
		{
			printf_s("Check D2C_Calib Failed\r\n");
			return -1;
		}
		else
		{
			printf_s("Check D2C_Calib Success\r\n");
			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
}

int database::check_depthquality_by_pn(char *pn)
{
	char sql[512];
	int ret = 0;
	MYSQL_RES *result = NULL;
	my_ulonglong rowcount = 0;
	unsigned int fieldcount = 0;
	MYSQL_ROW row = NULL;

	if (sn_is_valid((unsigned char *)pn) == 0)
	{
		sprintf_s(sql, "select * from %s item where pn=%s  and depth_quality = 1", tablename, pn);

		ret = mysql_query(&m_mysql, sql);
		if (ret != 0)
		{
			printf_s("MySQL Error:%s\n", mysql_error(&m_mysql));
			return -1;
		}
		result = mysql_store_result(&m_mysql);
		rowcount = mysql_num_rows(result);
		if (rowcount == 0)
		{
			printf_s("Check Depth_Quality Failed\r\n");
			return -1;
		}
		else
		{
			printf_s("Check Depth_Quality Success\r\n");
			return 0;
		}
	}
	else
	{
		printf_s("PN is illegal\r\n");
		return -1;
	}
}