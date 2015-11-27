#ifndef PREHEATSTATION_H
#define PREHEATSTATION_H

#include <QtWidgets/QMainWindow>
#include "ui_preheatstation.h"

#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/qtconcurrentmap.h>
#include <QFutureWatcher>
#include <QThread>
#include <QDate>

#include "deviceInquiry.h"
#include "INIReader.h"

class ThreadWorker : public QThread
{
	Q_OBJECT

public:
	ThreadWorker(QObject *parent = 0, bool b = false);
	~ThreadWorker();
	void run();
	bool Stop;

signals:
	void procValueChanged(int *progVal);
	void getAndsendSN(string *showSN);
	void TotalDevCount(int DevCount);
	void reportDBstatus(string *dblabel);
	void configTime(int confgTime);
	void DBconnectStatus(int DBconnect);
};



class PreheatStation : public QMainWindow
{
	Q_OBJECT
		
public:
	PreheatStation(QWidget *parent = 0);
	~PreheatStation();
	
	ThreadWorker *workerLoop;

	public slots:
	void ShowSNinLable(string *showSN);
	void progValueChanged(int *progVal);
	void on_StartBtn_clicked();
	void on_StopBtn_clicked();
	void ShowTotalDevs(int DevCount);
	void ShowDBlabel(string *dblabel);
	void ShowConfigTime(int confgTime);
	void ShowDBconnectStatus(int DBconnect);
public:
	Ui::PreheatStationClass ui;
};


//**************************************************************************/

#endif // PREHEATSTATION_H
