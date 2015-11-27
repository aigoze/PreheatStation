
#include "cmdDevice.h"
#include <iostream>
#include <iomanip>
#include <initguid.h>

using namespace std;

DEFINE_GUID(GUID_CLASS_OBDRVUSB, 0xc3b5f022, 0x5a42, 0x1980, 0x19, 0x09, 0xea, 0x72, 0x09, 0x56, 0x01, 0xb1);

struct _deviceVidPid
{
	XnUInt16 vendorID;
	XnUInt16 productID;
};

struct _deviceVidPid support_devices_list[]=
{
	{ 0x1D27, 0x0500 },
	{ 0x1D27, 0x0600 },
	{ 0x1D27, 0x0601 },
	{ 0x1D27, 0x0609 },
	{ 0x2BC5, 0x0400 },
	{ 0x2BC5, 0x0401 },
	{ 0x2BC5, 0x0402 },
	{ 0x2BC5, 0x0403 },
	{ 0x2BC5, 0x0404 },
};

cmdDevice::cmdDevice(void)
{
	ob_vid = 0x2BC5;
	ob_pid = 0x0400;
	sequence_num = 0x00;
	devicePresent = FALSE;
}

cmdDevice::~cmdDevice(void)
{

}

int cmdDevice::DevInitial()
{
	XnUInt32 ndevCount = 0;
	XnStatus rc;
	XnUInt actual_length;
	XnUInt8 outbuf[512];
	XnUInt8 inbuf[512];

	XnStatus nRetVal = xnUSBInit();
	if (nRetVal == XN_STATUS_USB_ALREADY_INIT)
		nRetVal = XN_STATUS_OK;

	XnUInt32 OBsupportedProductsCount = 1*sizeof(support_devices_list)/sizeof(struct _deviceVidPid);

	for (XnUInt32 i = 0; i < OBsupportedProductsCount; i++)
	{
		rc = xnUSBEnumerateDevices(support_devices_list[i].vendorID,support_devices_list[i].productID,&ob_astrDevicePaths, &ndevCount);
		if ((rc == XN_STATUS_OK) && (ndevCount != 0))
		{
			ob_vid = support_devices_list[i].vendorID;
			ob_pid = support_devices_list[i].productID;
			//printf_s("Device vid=%x,pid=%x\r\n",ob_vid,ob_pid);
			break;
		}
	}

	if ((rc != XN_STATUS_OK) || (ndevCount == 0))
	{
		//printf_s("No Device\r\n");
		return -1;
	}

	rc = xnUSBOpenDeviceByPath(*ob_astrDevicePaths, &ob_hUSBDevice);

	if (rc != XN_STATUS_OK)
	{
		//cout << "Error: failed to open device" << hex << ob_vid <<hex <<ob_pid << endl;
		return -1;
	}

	return 0;
}

int cmdDevice::GetDevHandle(XnUSBConnectionString* connectionPath)
{
	XnStatus rc;
	XnStatus nRetVal = xnUSBInit();
	rc = xnUSBOpenDeviceByPath(*connectionPath, &ob_hUSBDevice);
	return rc;
}

int cmdDevice::initial_header(void *bufffer, XnUInt16 devCommand, XnUInt16 data_len)
{
	ProtocolHeader *pHeader = (ProtocolHeader*)bufffer;
	pHeader->nMagic = OBDEV_HEADER_MAGIC;
	pHeader->nSize = data_len /2;
	pHeader->nOpcode = devCommand;
	pHeader->nId = sequence_num;
	sequence_num++;

	return 0;
}

int cmdDevice::send(void *command_request, XnUInt16 request_length, void *command_respone, XnUInt16 *respone_length)
{
	XnUInt32 actual_length;

	xnUSBSendControl(ob_hUSBDevice, XN_USB_CONTROL_TYPE_VENDOR, 0x00, 0x0000, 0x0000,(XnUChar*)command_request, request_length, 1000);
	do 
	{
		xnUSBReceiveControl(ob_hUSBDevice, XN_USB_CONTROL_TYPE_VENDOR, 0x00, 0x0000,0x0000, (XnUChar*)command_respone, 0x200, &actual_length, 1000);
	} while ((actual_length == 0) || (actual_length == 0x200));

	*respone_length = actual_length;

	return 0;
}

int cmdDevice::Read_flash(XnUInt32 offset, XnUInt16 size)
{
	int ret;
	XnUInt16 data_len;
	XnUInt16 resp_len;
	XnUInt16 i;
	XnUInt32 *p = NULL;

	data_len = 6;
	ret = initial_header(request_buffer, OPCODE_READ_FLASH, data_len);
	if (ret)
	{
		//cout << "init header of set tec ldp failed" << endl;
		return ret;
	}

	p = (XnUInt32 *)(request_buffer + 8);
	*p = offset;
	request_buffer[12] = size;
	request_buffer[13] = size >> 8;

	ret = send(request_buffer, OBDEV_HEADER_LEN + data_len, respone_buffer, &resp_len);
	if (ret)
	{
		//cout << "send cmd  set tec ldp failed" << endl;
	}

	for (i = 0; i < resp_len - 10; i++)
	{
		if (i > 0 && i % 16 == 0)
		{
			//printf("\n");
		}
		//printf("%02x", *(respone_buffer + 10 + i));
	}
	//printf("\n");

	return ret;

}

int cmdDevice::DevGetVersion(void)
{
	int ret;
	XnUInt16 data_len;
	XnUInt16 resp_len;
	XnUInt16 i;

	data_len = 0;
	ret = initial_header(request_buffer, OBDEV_GET_VERSION, data_len);
	if (ret)
	{
		//cout << "init header of get version failed" << endl;
		return ret;
	}

	ret = send(request_buffer, OBDEV_HEADER_LEN + data_len, respone_buffer, &resp_len);
	if (ret)
	{
		//cout << "send get version command failed" << endl;
	}
	if (resp_len)
	{
// 		cout << "	FW Version:"
// 			<< hex << setfill('0') << setw(2) << int(respone_buffer[10]) <<"."
// 			<< hex << setfill('0') << setw(2) << int(respone_buffer[11]) << endl;
	}

	return ret;
}

bool cmdDevice::CheckConncetion(void)
{
	bool DevStatus;
	GUID extraGUID = GUID_CLASS_OBDRVUSB;
	void* pExtraParam = &extraGUID;
	xnUSBIsDevicePresent(ob_vid, ob_pid, pExtraParam,&devicePresent);

	DevStatus = devicePresent;

	return DevStatus;
}

int cmdDevice::Emitter(bool emitter_status)
{
	int ret;
	XnUInt16 data_len = 2;
	XnUInt16 resp_len;

	ret = initial_header(request_buffer, OBDEV_ENABLE_EMITTER, data_len);
	if (emitter_status == true)
	{
		request_buffer[8] = 0x01;
	}
	if (emitter_status == false)
	{
		request_buffer[8] = 0x00;
	}

	ret = send(request_buffer, OBDEV_HEADER_LEN + data_len, respone_buffer, &resp_len);

	return ret;
}