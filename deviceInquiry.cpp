#include "deviceInquiry.h"
#include "cmdDevice.h"
#include <windows.h>

using namespace openni;

deviceInquiry::deviceInquiry(void)
{
	for (int i = 0; i < 6; i++)
	{
		ProgressData.progressValue[i] = 0;
		ProgressData.progressPercent[i] = 0;
	}
}

deviceInquiry::~deviceInquiry(void)
{
	
}

DevArray deviceInquiry::Openni_EnumerateDevs()
{
 	DevArray devListArray;
	Array<DeviceInfo> listOfDevices;
	OpenNI::enumerateDevices(&listOfDevices);
 	devListArray.ListHandle = &listOfDevices;
 	devListArray.ListSize = listOfDevices.getSize();
	for (int i = 0; i < listOfDevices.getSize(); i++)
	{
		DeviceInfo lisDev = listOfDevices[i];
		devListArray.UriLists.push_back(lisDev.getUri());
	}
	
	return devListArray;
}

void deviceInquiry::UpdataDevicesList()
{
	DeviceList.previousPortID = DeviceList.connectedPortID;
	DeviceList.previousUri = DeviceList.connectedUri;
	DevArray enmueraArray = Openni_EnumerateDevs();
	DeviceList.connectedUri.resize(enmueraArray.UriLists.size());
	DeviceList.connectedPortID.resize(enmueraArray.UriLists.size());
	for (int listIndex = 0; listIndex < enmueraArray.UriLists.size(); listIndex++)
	{
		DeviceList.connectedUri[listIndex] = enmueraArray.UriLists[listIndex];
		uriParsing(DeviceList.connectedUri, listIndex, DeviceList.connectedPortID[listIndex]);


	}

}

void deviceInquiry::deviceCommand(vector<string> InputUri, int uriIndex, string SN[])
{
	string StringTobeConv = InputUri[uriIndex];
	SN[uriIndex].clear();
	char PathUri[260];
	copy(StringTobeConv.begin(), StringTobeConv.end(), PathUri);
	PathUri[StringTobeConv.size()] = '\0';
	XnUSBConnectionString deviceUri[260];
	memcpy(deviceUri,PathUri,sizeof(PathUri));

	cmdDev.ob_astrDevicePaths = deviceUri;
	//Enable Laser 
	cmdDev.GetDevHandle(deviceUri);
	cmdDev.Emitter(true);

	//Read SN from device
	cmdDev.Read_flash(0x30000, 6);
	for (int i = 0; i < 12; i++)
	{
		SN[uriIndex] += cmdDev.respone_buffer[i + 10];
	}

	cmdDev.ob_astrDevicePaths = NULL;
}

void deviceInquiry::ReadDeviceSN(string NewUriPlugIn,string &SN)
{
	string StringTobeConv = NewUriPlugIn;
	SN.clear();
	char PathUri[260];
	copy(StringTobeConv.begin(), StringTobeConv.end(), PathUri);
	PathUri[StringTobeConv.size()] = '\0';
	XnUSBConnectionString deviceUri[260];
	memcpy(deviceUri, PathUri, sizeof(PathUri));

	cmdDev.ob_astrDevicePaths = deviceUri;
	//Enable Laser 
	cmdDev.GetDevHandle(deviceUri);
	cmdDev.Emitter(true);

	//Read SN from device
	cmdDev.Read_flash(0x30000, 6);
	cmdDev.respone_buffer[21] = '\0';
	for (int i = 0; i < 12; i++)
	{
		SN += cmdDev.respone_buffer[i + 10];
	}
	cmdDev.ob_astrDevicePaths = NULL;
}

void deviceInquiry::uriParsing(vector<string> InputUri, int uriIndex, string &uniquePortID)
{
	uniquePortID.clear();
	if (InputUri[0].empty())
	{
		uniquePortID.clear();
	}
	string signAnd = "&";
	//get rid of first "&"
	int startIndex = InputUri[uriIndex].find_first_of("&") + signAnd.length();
	string subInputUri = InputUri[uriIndex].substr(startIndex, InputUri[uriIndex].length() - startIndex);
	//get rid of second "&"
	int secondIndex = subInputUri.find_first_of("&") + signAnd.length();
	string theRestUri = subInputUri.substr(secondIndex, subInputUri.length() - secondIndex);
	//now get the id string
	int first = theRestUri.find_first_of("&") + signAnd.length();
	int last = theRestUri.find_last_of("&");

	uniquePortID = theRestUri.substr(first, last - first - 2);
}

void deviceInquiry::DeviceStatusChange(vector<string> previousIDarray, vector<string> currentIDarray, string &theDifferenceArray, 
									   vector<string> DevPreviousUri, vector<string> DevConnectUri, string &theDiffUri, int &newDeviceFlag)
{
	theDifferenceArray;
	newDeviceFlag = 0;
	if (previousIDarray.size() < currentIDarray.size())
	{
		for (int IDindex = 0; IDindex < currentIDarray.size(); IDindex++)
		{
			bool inArray = find(previousIDarray.begin(), previousIDarray.end(), currentIDarray[IDindex]) != previousIDarray.end();
			if (!inArray)
			{
				theDifferenceArray = currentIDarray[IDindex];
				theDiffUri = DevConnectUri[IDindex];
				newDeviceFlag = 1;
			}
		}
	}
	if (previousIDarray.size() > currentIDarray.size())
	{
		for (int IDindex2 = 0; IDindex2 < previousIDarray.size(); IDindex2++)
		{
			bool inArray2 = find(currentIDarray.begin(), currentIDarray.end(), previousIDarray[IDindex2]) != currentIDarray.end();
			if (!inArray2)
			{
				theDifferenceArray = previousIDarray[IDindex2];
				theDiffUri = DevPreviousUri[IDindex2];
				newDeviceFlag = -1;
			}
		}
	}

	if (previousIDarray.size() == currentIDarray.size())
	{
		newDeviceFlag = 0;
	}

}

void deviceInquiry::pushItemInIDqueue(string InputItemID, string NewPN, int DevOnDBflag)
{
	const int PROGRESSCOUNT = 6;
	for (int searchIndex = 0; searchIndex < PROGRESSCOUNT; searchIndex++)
	{
		string disConnectedmark = "&";
		if (DeviceList.IDqueue[searchIndex] == InputItemID)
		{
			break;
		}
		if (DeviceList.IDqueue[searchIndex] == (disConnectedmark + InputItemID))
		{
			DeviceList.IDqueue[searchIndex] = InputItemID;
			DeviceList.PNqueue[searchIndex] = NewPN;
			switch (DevOnDBflag)
			{
			case 0:
				DeviceList.DBstatus[searchIndex] = "正常";
				break;
			case 1:
				DeviceList.DBstatus[searchIndex] = "数据库无数据";
				break;
			case 2:
				DeviceList.DBstatus[searchIndex] = "BIST工位测试失败";
				break;
			}
			break;
		}
		if (DeviceList.IDqueue[searchIndex].empty())
		{
			DeviceList.IDqueue[searchIndex] = InputItemID;
			DeviceList.PNqueue[searchIndex] = NewPN;
			switch (DevOnDBflag)
			{
			case 0:
				DeviceList.DBstatus[searchIndex] = "正常";
				break;
			case 1:
				DeviceList.DBstatus[searchIndex] = "数据库无数据";
				break;
			case 2:
				DeviceList.DBstatus[searchIndex] = "BIST工位测试失败";
				break;
			}
			break;
		}
	}
}

void deviceInquiry::clearValueInIDqueue(string ItemToBeSet)
{
	const int PROGRESSCOUNT = 6;
	for (int searchIndex = 0; searchIndex < PROGRESSCOUNT; searchIndex++)
	{
		string disConnectedmark = "&";
		if (DeviceList.IDqueue[searchIndex] == ItemToBeSet)
		{
			DeviceList.IDqueue[searchIndex] = (disConnectedmark + ItemToBeSet);
			DeviceList.PNqueue[searchIndex] = "";
			DeviceList.DBstatus[searchIndex] = "";
		}
	}
}

void deviceInquiry::renewProgressBarValue()
{
	const int PROGRESSCOUNT = 6;
	const string InvalidPNstring;
	for (int Index = 0; Index < PROGRESSCOUNT; Index++)
	{
		if (DeviceList.IDqueue[Index].empty())
		{
			ProgressData.progressValue[Index] = 0;
			ProgressData.progressPercent[Index] = 0;
			break;
		}
		int indexOfMark = DeviceList.IDqueue[Index].find_first_of("&");
		if (indexOfMark == 0)
		{
			ProgressData.progressValue[Index] = 0;
			ProgressData.progressPercent[Index] = 0;
		}
		if (indexOfMark == string::npos)
		{
			if (DeviceList.DBstatus[Index] == "正常")
			{
				ProgressData.progressValue[Index]++;
			}
		}
		ProgressData.progressPercent[Index] = (int)((float)ProgressData.progressValue[Index] / (float)secondsForExecution * 100);
		
		if (ProgressData.progressPercent[Index] > 100)
		{
			ProgressData.progressPercent[Index] = 100;
		}

	}

}
