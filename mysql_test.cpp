// mysql_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "database.h"

int print_item(db_item item)
{
	printf("SN:%s\r\nFlash:%s\r\nTEC:%s\r\nIR:%s\r\nRGB:%s\r\nLDP:%s\r\nLD:%s\r\nCodec:%s\r\nWorkstation:%s\r\nTime:%s\r\n",\
		item.sn, item.bist.flash, item.bist.tec, item.bist.ir, item.bist.rgb, item.bist.ldp, item.bist.ld, item.bist.codec, item.bist.worker_id, item.bist.time);
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
    int ret;
    database db;
    db_item item;
	int success, failed;
    ret = db.init_database();
    ret = db.open_database("localhost", "root", "12345", "for_test","test0", 3306);
    ret = db.get_item_by_pn("15071710001");
    ret = db.get_next_valid_pn_by_worker(1);
	ret = db.get_next_valid_sn_by_worker(1);
	ret = db.count_bist(2015, 7, 17, 1,&success, &failed);
	ret = db.count_preheat(2015, 7, 17,2, &success, &failed);
	ret = db.count_depthcalib(2015, 7, 17,1, &success, &failed);
	ret = db.count_d2ccalib(2015, 7, 17,1, &success, &failed);
	ret = db.count_depthquality(2015, 7, 17,1, &success, &failed);
    if(ret!=0)
    {
        printf("Can not get valid sn by worker\r\n");
    }
	db.init_item(&item);
    printf("Next PN: %s\r\n",db.next_pn);
	strcpy_s(item.pn, db.next_pn);
	strcpy_s(item.sn, db.next_sn);
    strcpy_s(item.bist.flash ,"1");
	strcpy_s(item.bist.tec, "1");
	strcpy_s(item.bist.ir, "1");
	strcpy_s(item.bist.rgb, "1");
	strcpy_s(item.bist.ldp, "1");
	strcpy_s(item.bist.ld, "1");
	strcpy_s(item.bist.codec, "1");
	strcpy_s(item.bist.worker_id, "1");

	strcpy_s(item.preheat.worker_id, "1");
	strcpy_s(item.preheat.res, "0");

	strcpy_s(item.depth_calib.worker_id, "1");
	strcpy_s(item.depth_calib.res, "0");

	strcpy_s(item.d2c_calib.worker_id, "1");
	strcpy_s(item.d2c_calib.res, "0");

	strcpy_s(item.depth_quality.worker_id, "1");
	strcpy_s(item.depth_quality.res, "0");

    ret=db.insert_item(&item);
    if(ret!=0)
    {
        printf("Can not insert item by worker\r\n");
    }
    else
    {
        printf("Insert item with PN %s\r\n",item.pn);   
    }


	db.update_preheat_by_pn("15071710001", 2, 1);
	db.update_depthcalib_by_pn("15071710001", 2, 1);
	db.update_d2ccalib_by_pn("15071710001", 2,0);
	db.update_depthquality_by_pn("15071710001", 2, 1);


	ret = db.get_item_by_sn("15071710001");
	if (ret != 0)
	{
		
	}
	else
	{
		print_item(db.cur_item);
	}

    ret = db.close_database();

}
