#ifndef DEVICEINQUIRY_H
#define DEVICEINQUIRY_H


#include <iostream>
#include <vector>
#include <string>
#include "OpenNI.h"
#include "cmdDevice.h"

using namespace std;

typedef struct _progress_bar_value
{
	int progressValue[6];
	int progressPercent[6];
}ProBarData;

typedef struct _Device_List
{
	vector<string> previousUri;
	vector<string> connectedUri;
	vector<string> previousPortID;
	vector<string> connectedPortID;
	string IDqueue[6];
	string PNqueue[6];
	string DBstatus[6];
}DevList;

typedef struct _Device_Array
{
	int ListSize;
	//void *Listdata;
	void *ListHandle;
	vector<string> UriLists;
}DevArray;

class deviceInquiry
{
public:
	deviceInquiry(void);
	~deviceInquiry(void);

	int secondsForExecution;
	ProBarData ProgressData;
	DevList DeviceList;
	cmdDevice cmdDev;

	DevArray Openni_EnumerateDevs();
	void uriParsing(vector<string> InputUri, int uriIndex, string &uniquePortID);
	void UpdataDevicesList();
	void DeviceStatusChange(vector<string> previousIDarray, vector<string> currentIDarray, 
							string &theDifferenceArray, vector<string> DevPreviousUri, vector<string> DevConnectUri,
							string &theDiffUri, int &newDeviceFlag);
	void pushItemInIDqueue(string InputItemID, string NewSN, int DevOnDBflag);
	void clearValueInIDqueue(string ItemToBeSet);
	void renewProgressBarValue();
	void displayOperation(bool LoopState);
	void deviceCommand(vector<string> InputUri, int uriIndex, string SN[]);
	void ReadDeviceSN(string NewUriPlugIn,string &SN);
};


#endif
