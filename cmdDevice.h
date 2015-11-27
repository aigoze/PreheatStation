#ifndef CMDDEVICE_H
#define CMDDEVICE_H

#include <XnPlatform.h>
#include <XnOS.h>
#include <XnUSB.h>

enum EPsProtocolOpCodes
{
	OPCODE_GET_VERSION = 0,
	OPCODE_KEEP_ALIVE = 1,
	OPCODE_GET_PARAM = 2,
	OPCODE_SET_PARAM = 3,
	OPCODE_GET_FIXED_PARAMS = 4,
	OPCODE_GET_MODE = 5,
	OPCODE_SET_MODE = 6,
	OPCODE_GET_LOG = 7,
	OPCODE_RESERVED_0 = 8,
	OPCODE_RESERVED_1 = 9,
	OPCODE_I2C_WRITE = 10,
	OPCODE_I2C_READ = 11,
	OPCODE_TAKE_SNAPSHOT = 12,
	OPCODE_INIT_FILE_UPLOAD = 13,
	OPCODE_WRITE_FILE_UPLOAD = 14,
	OPCODE_FINISH_FILE_UPLOAD = 15,
	OPCODE_DOWNLOAD_FILE = 16,
	OPCODE_DELETE_FILE = 17,
	OPCODE_GET_FLASH_MAP = 18,
	OPCODE_GET_FILE_LIST = 19,
	OPCODE_READ_AHB = 20,
	OPCODE_WRITE_AHB = 21,
	OPCODE_ALGORITM_PARAMS = 22,
	OPCODE_SET_FILE_ATTRIBUTES = 23,
	OPCODE_EXECUTE_FILE = 24,
	OPCODE_READ_FLASH = 25,
	OPCODE_SET_GMC_PARAMS = 26,
	OPCODE_GET_CPU_STATS = 27,
	OPCODE_BIST = 28,
	OPCODE_CALIBRATE_TEC = 29,
	OPCODE_GET_TEC_DATA = 30,
	OPCODE_CALIBRATE_EMITTER = 31,
	OPCODE_GET_EMITTER_DATA = 32,
	OPCODE_CALIBRATE_PROJECTOR_FAULT = 33,
	OPCODE_SET_CMOS_BLANKING = 34,
	OPCODE_GET_CMOS_BLANKING = 35,
	OPCODE_GET_CMOS_PRESETS = 36,
	OPCODE_GET_SERIAL_NUMBER = 37,
	OPCODE_GET_FAST_CONVERGENCE_TEC = 38,
	OPCODE_GET_PLATFORM_STRING = 39,
	OPCODE_GET_USB_CORE_TYPE = 40,
	OPCODE_SET_LED_STATE = 41,
	OPCODE_ENABLE_EMITTER = 42,
	OBDEV_ENABLE_EMITTER = 85,//laser
	OBDEV_GET_VERSION = 80,
	OPCODE_KILL = 999,
};

typedef struct
{
	XnUInt16 nMagic;
	XnUInt16 nSize;
	XnUInt16 nOpcode;
	XnUInt16 nId;
} ProtocolHeader;

#define OBDEV_HEADER_MAGIC	(0x4d47)
#define OBDEV_HEADER_LEN		(0x08)

class cmdDevice
{
public:
	cmdDevice();
	~cmdDevice();
public:
	int DevInitial(void);
	int DevGetVersion(void);
	bool CheckConncetion(void);
	int Emitter(bool emitter_status);
	int Read_flash(XnUInt32 offset, XnUInt16 size);
	int GetDevHandle(XnUSBConnectionString* connectionPath);
public:
	XnUInt16 sequence_num;
	XnUInt8 request_buffer[512];
	XnUInt8	respone_buffer[512];
	XnUInt16 ob_vid;
	XnUInt16 ob_pid;
	XN_USB_DEV_HANDLE ob_hUSBDevice;
	XnBool devicePresent;
	const XnUSBConnectionString* ob_astrDevicePaths;

private:
	int initial_header(void *bufffer, XnUInt16 devCommand, XnUInt16 data_len);
	int send(void *command_request, XnUInt16 request_length, void *command_respone, XnUInt16 *respone_length);

};


#endif // !_CMDDEVICE_H_
