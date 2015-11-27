#include "preheatstation.h"
#include "cmdDevice.h"
#include "database.h"

using namespace openni;

PreheatStation::PreheatStation(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	workerLoop = new ThreadWorker(this);
	connect(workerLoop, SIGNAL(procValueChanged(int*)), this, SLOT(progValueChanged(int*)));
	connect(workerLoop, SIGNAL(getAndsendSN(string*)), this, SLOT(ShowSNinLable(string*)));
	connect(workerLoop, SIGNAL(TotalDevCount(int)), this, SLOT(ShowTotalDevs(int)));
	connect(workerLoop, SIGNAL(reportDBstatus(string*)), this, SLOT(ShowDBlabel(string*)));
	connect(workerLoop, SIGNAL(configTime(int)), this, SLOT(ShowConfigTime(int)));
	connect(workerLoop, SIGNAL(DBconnectStatus(int)), this, SLOT(ShowDBconnectStatus(int)));
}

PreheatStation::~PreheatStation()
{

}

void PreheatStation::progValueChanged(int progVal[])
{
	ui.progressBar1->setValue(progVal[0]);
	ui.progressBar2->setValue(progVal[1]);
	ui.progressBar3->setValue(progVal[2]);
	ui.progressBar4->setValue(progVal[3]);
	ui.progressBar5->setValue(progVal[4]);
	ui.progressBar6->setValue(progVal[5]);
}

void PreheatStation::ShowSNinLable(string *showSN)
{
	ui.label1->setText(QString::fromStdString(showSN[0]));
	ui.label2->setText(QString::fromStdString(showSN[1]));
	ui.label3->setText(QString::fromStdString(showSN[2]));
	ui.label4->setText(QString::fromStdString(showSN[3]));
	ui.label5->setText(QString::fromStdString(showSN[4]));
	ui.label6->setText(QString::fromStdString(showSN[5]));
}

void PreheatStation::ShowTotalDevs(int DevCount)
{
	ui.labelSuccess->setText(QString::number(DevCount));
}

void PreheatStation::ShowDBlabel(string *dblabel)
{
	ui.dblabel1->setText(QString::fromLocal8Bit(dblabel[0].c_str()));
	ui.dblabel2->setText(QString::fromLocal8Bit(dblabel[1].c_str()));
	ui.dblabel3->setText(QString::fromLocal8Bit(dblabel[2].c_str()));
	ui.dblabel4->setText(QString::fromLocal8Bit(dblabel[3].c_str()));
	ui.dblabel5->setText(QString::fromLocal8Bit(dblabel[4].c_str()));
	ui.dblabel6->setText(QString::fromLocal8Bit(dblabel[5].c_str()));
}

void PreheatStation::ShowConfigTime(int confgTime)
{
	ui.labeltime->setText(QString::number(confgTime));
}

void PreheatStation::ShowDBconnectStatus(int DBconnect)
{
	if (DBconnect == 0)
	{
		ui.labeldbStatus->setText(QString::fromLocal8Bit("连接失败"));
	}
	if (DBconnect == 1)
	{
		ui.labeldbStatus->setText(QString::fromLocal8Bit("正常"));
	}
	if (DBconnect == 2)
	{
		ui.labeldbStatus->setText(QString::fromLocal8Bit("离线模式"));
	}
}

void PreheatStation::on_StartBtn_clicked()
{
	workerLoop->start();
}

void PreheatStation::on_StopBtn_clicked()
{
	workerLoop->Stop = true;
}

ThreadWorker::ThreadWorker(QObject *parent, bool b) : QThread(parent), Stop(b)
{

}

ThreadWorker::~ThreadWorker()
{

}

void ThreadWorker::run()
{
	bool loopStatus = true;
	deviceInquiry devINQ;
	database testDatabase;

	INIReader configReader("Config.ini");
	string IniHost = configReader.Get("user", "host", "unknownlocalhost");
	string IniUerName = configReader.Get("user", "name", "unknownorbbec");
	string InipassWd = configReader.Get("user", "password", "unknownorbbec12345");
	string Inidatabase = configReader.Get("user", "database", "unknownmft_database");
	string Initable = configReader.Get("user", "table", "unknowntest");
	int Iniport = configReader.GetInteger("user", "port", 3306);
	int IniworkerId = configReader.GetInteger("user", "workerID", 1);
	int IniExecutionTime = configReader.GetInteger("Time", "PreHeat", 60);
	bool ConnectToDataBase = configReader.GetBoolean("user", "ConnectDataBase", false);

	const char *m_db_host = IniHost.c_str();
	const char *m_db_user = IniUerName.c_str();
	const char *m_db_pw = InipassWd.c_str();
	const char *m_db_database = Inidatabase.c_str();

	const char *m_db_table = Initable.c_str();
	unsigned int m_db_port = (unsigned int)Iniport;
	int m_db_workerId = IniworkerId;

	int successCount = 0;
	int faliedCount = 0;

	devINQ.secondsForExecution = IniExecutionTime;
	emit configTime(devINQ.secondsForExecution);

	string theChangingInputID;
	string theChangingUri;
	string newInputPN;
	int databaseConnectStatus = 0;

	int DeviceStatusFlag;
	int DevOnDatabaseFlag;

	while (loopStatus)
	{
		QMutex mutex;
		int dbretVal;

		Status rc = OpenNI::initialize();
		if (ConnectToDataBase == true)
		{
			dbretVal = testDatabase.init_database();
			//************MUTEX 1******************************/
			mutex.lock();
			dbretVal = testDatabase.open_database(m_db_host, m_db_user, m_db_pw, m_db_database, m_db_table, m_db_port);
			if (dbretVal == -1)
			{
				databaseConnectStatus = 0;
			}
			if (dbretVal == 0)
			{
				databaseConnectStatus = 1;
			}
			mutex.unlock();
			//*************************************************/
		}else
		{
			databaseConnectStatus = 2;
		}
		emit DBconnectStatus(databaseConnectStatus);

		devINQ.UpdataDevicesList();

		devINQ.DeviceStatusChange(devINQ.DeviceList.previousPortID, devINQ.DeviceList.connectedPortID, theChangingInputID, 
								  devINQ.DeviceList.previousUri,devINQ.DeviceList.connectedUri,theChangingUri,DeviceStatusFlag);
		
		//Read changing SN from changing Uri ALSO enable laser
		devINQ.ReadDeviceSN(theChangingUri, newInputPN);
		//**********CHECK BIST***************************************/
		if (ConnectToDataBase == true)
		{
			char pn[12];
			if (theChangingUri.size() != 0)
			{
				copy(newInputPN.begin(), newInputPN.end(), pn);
				dbretVal = testDatabase.get_item_by_pn(pn);
				if (dbretVal == -1)
				{
					DevOnDatabaseFlag = 1;//No dababase Info
				}
				if (dbretVal == 0)
				{
					dbretVal = testDatabase.check_bist_by_pn(pn);
					if (dbretVal == -1)
					{
						DevOnDatabaseFlag = 2;//BIST test failed
					}
					if (dbretVal == 0)
					{
						DevOnDatabaseFlag = 0;
					}
				}
			}
		}
		else
		{
			DevOnDatabaseFlag = 0;
		}
		//***********************************************************/

		if (DeviceStatusFlag == 1)//New Device connected
		{
			devINQ.pushItemInIDqueue(theChangingInputID, newInputPN, DevOnDatabaseFlag);
		}
		if (DeviceStatusFlag == -1)//Device connection Lost
		{
			devINQ.clearValueInIDqueue(theChangingInputID);
		}

		devINQ.renewProgressBarValue();
		//Renew database info, update preheat
		if (ConnectToDataBase == true)
		{
			for (int progBarMax = 0; progBarMax < 6; progBarMax++)
			{
				if (devINQ.ProgressData.progressPercent[progBarMax] == 100)
				{
					char updatePN[12];
					string PNinList;
					PNinList = devINQ.DeviceList.PNqueue[progBarMax];
					copy(PNinList.begin(), PNinList.end(), updatePN);

					dbretVal = testDatabase.update_preheat_by_pn(updatePN, m_db_workerId, 1);

				}
			}
			//*********Read Total Devs Form Database*****************/
			//Get system time
			QDate tDtime;
			QString Yformat = "yyyy";
			QString Mformat = "M";
			QString Dformat = "d";
			QString Ytime = tDtime.currentDate().toString(Yformat);
			QString Mtime = tDtime.currentDate().toString(Mformat);
			QString Dtime = tDtime.currentDate().toString(Dformat);
			int CurYear = Ytime.toInt();
			int CurMonth = Mtime.toInt();
			int CurDay = Dtime.toInt();
			testDatabase.count_preheat(CurYear, CurMonth, CurDay, m_db_workerId, &successCount, &faliedCount);

			//*******************************************************/
			testDatabase.close_database();
		}
		OpenNI::shutdown();


		emit procValueChanged(devINQ.ProgressData.progressPercent);
		emit getAndsendSN(devINQ.DeviceList.PNqueue);
		emit TotalDevCount(successCount);
		emit reportDBstatus(devINQ.DeviceList.DBstatus);

		Sleep(1000);//optional 920
		//this->msleep(1000);
	}

}

//*************************************************************************************************/
