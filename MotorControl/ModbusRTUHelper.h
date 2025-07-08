// ***********************************************************************
// Module           : ModbusRTUHelper for C++
// Author           : Lei Youbing
// Created          : 12-27-2017
//
// Last Modified By : Lei Youbing
// Last Modified On : 2020-08-18
// ***********************************************************************
//     Copyright (c) Shanghai AMP & MOONS' Automation Co., Ltd.. All rights reserved.
// ***********************************************************************

#pragma once

#define MAX_REGISTERCOUNT 100
#define MAX_BYTES_COUNT 1024

#define MBERROR_ILLEGAL_FUNCTION 0x01;
#define MBERROR_ILLEGAL_DATA_ADDRESS 0x02;
#define MBERROR_ILLEGAL_DATA_VALUE  0x03;
#define MBERROR_SLAVE_DEVICE_FAILURE   0x04;
#define MBERROR_ACKNOWLEDGE  0x05;
#define MBERROR_SLAVE_DEVICE_BUSY   0x06;
#define MBERROR_NEGATIVE_ACKNOWLEDGE  0x07;
#define MBERROR_MEMORY_PARITY_ERROR   0x08;
#define MBERROR_GATEWAY_PATH_UNAVAILABLE   0x0A;
#define MBERROR_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND  0x0B;

//manufacture specific
#define MBERROR_CAN_NOT_READ 0x11;
#define MBERROR_CAN_NOT_WRITE 0x12;
#define MBERROR_DATA_RANG 0x13;

//other
#define MBERROR_FAIL_TO_OPEN_PORT 0x100;
#define MBERROR_PORT_IS_CLOSED 0x101;
#define MBERROR_SEND_FAILED 0x102;
#define MBERROR_THREAD_ERROR 0x103;
#define MBERROR_NO_RESPONSE 0x104;
#define MBERROR_DATA_NOT_ENOUGH 0x105;
#define MBERROR_CRC_ERROR 0x106;
#define MBERROR_SCLREGISTER_NOTFOUND 0x107;
#define MBERROR_UNKNOWN_EXCEPTION 0xFFFF;

typedef   void   (CALLBACK   *EventCallback)();

struct StructRegisterValue
{
	int Count;
	int Values[MAX_REGISTERCOUNT];
};

typedef struct _COMMAND_INFO
{
	int Count;
	byte Values[MAX_BYTES_COUNT];
} COMMAND_INFO, *PCOMMAND_INFO;

typedef struct _ERROR_INFO
{
	int ErrorCode;
	char* Command;
	char* ErrorMessage;
} ERROR_INFO, *PERROR_INFO;

// Trigger when send data to drive
typedef void (__stdcall *lpOnDataSend)(EventCallback func);

// Trigger when received data from drive
typedef void (__stdcall *lpOnDataReceive)(EventCallback func);

// Get the port is open or closed
typedef BOOL (__stdcall *lpIsOpen)(byte nCOMPort);

// Open serial port communication
typedef BOOL (__stdcall *lpOpen)(byte nCOMPort, int nBaudRate, BOOL bBigEndian);

// Close serial port communication
typedef BOOL (__stdcall *lpClose)(byte nCOMPort);

// Set endian type
typedef void (__stdcall *lpSetEndianType)(byte nCOMPort, BOOL bBigEndian);

// Return it is big endian type or not
typedef BOOL (__stdcall *lpIsBigEndian)(byte nCOMPort);

// Get execute time out
typedef UINT (__stdcall *lpGetExecuteTimeOut)(byte nCOMPort);

// Set execute time out
typedef void (__stdcall *lpSetExecuteTimeOut)(byte nCOMPort, UINT nExecuteTimeOut);

// Set it will trigger send event or not when sending command
typedef void (__stdcall *lpSetTriggerSendEvent)(BOOL bTriggerSendEvent);

// Set it will trigger received event or not when receive data
typedef void (__stdcall *lpSetTriggerReceiveEvent)(BOOL bTriggerReceiveEvent);

// Clear send buffer
typedef void (__stdcall *lpClearSendBuffer)(byte nCOMPort);

// Clear received buffer
typedef void (__stdcall *lpClearReceivedBuffer)(byte nCOMPort);

// Clear send and received Buffer
typedef void (__stdcall *lpClearBuffer)(byte nCOMPort);

// Get last error information, includes code and description
typedef void (__stdcall *lpGetLastErrorInfo)(PERROR_INFO pErrorInfo);

// Get the last command that send
typedef void (__stdcall *lpGetLastCommandSent)(byte nCOMPort, COMMAND_INFO& command);

// Get the last command that received
typedef void (__stdcall *lpGetLastCommandReceived)(byte nCOMPort, COMMAND_INFO& command);

// Read single holding register from the drive
typedef BOOL (__stdcall *lpReadSingleHoldingRegister)(byte nCOMPort, byte nNodeID, int nRegisterNo, int* pValue);

// Write single holding register value to the drive
typedef BOOL (__stdcall *lpWriteSingleHoldingRegister)(byte nCOMPort, byte nNodeID, int nRegisterNo, int nValue);

// Read multiple holding register from the drive
typedef BOOL (__stdcall *lpReadMultiHoldingRegisters)(byte nCOMPort, byte nNodeID, int nRegisterNo, int nCount, StructRegisterValue& holdingRegister);

// Write multiple holding register values to the drive
typedef BOOL (__stdcall *lpWriteMultiHoldingRegisters)(byte nCOMPort, byte nNodeID, int nRegisterNo, int nCount, int* valueList);

// Read 16-bit int data from the drive
typedef BOOL (__stdcall *lpReadDataInt16)(byte nCOMPort, byte nNodeID, int nRegisterNo, SHORT* nValue);

// Write 16-bit int data to the drive
typedef BOOL (__stdcall *lpWriteDataInt16)(byte nCOMPort, byte nNodeID, int nRegisterNo, SHORT nValue);

// Read 16-bit unsighed int data from the drive
typedef BOOL (__stdcall *lpReadDataUInt16)(byte nCOMPort, byte nNodeID, int nRegisterNo, USHORT* nValue);

// Write 16-bit unsigned int data to the drive
typedef BOOL (__stdcall *lpWriteDataUInt16)(byte nCOMPort, byte nNodeID, int nRegisterNo, USHORT nValue);

// Read 32-bit int data from the drive
typedef BOOL (__stdcall *lpReadDataInt32)(byte nCOMPort, byte nNodeID, int nRegisterNo, int* nValue);

// Write 32-bit int data to the drive
typedef BOOL (__stdcall *lpWriteDataInt32)(byte nCOMPort, byte nNodeID, int nRegisterNo, int nValue);

// Read 32-bit unsighed int data from the drive
typedef BOOL (__stdcall *lpReadDataUInt32)(byte nCOMPort, byte nNodeID, int nRegisterNo, UINT* nValue);

// Write 32-bit unsigned int data to the drive
typedef BOOL (__stdcall *lpWriteDataUInt32)(byte nCOMPort, byte nNodeID, int nRegisterNo, UINT nValue);

// Execute command with opcode
typedef BOOL (__stdcall *lpExecuteCommandWithOpcode)(byte nCOMPort, byte nNodeID, USHORT nOpCode, USHORT nParam1, USHORT nParam2, USHORT nParam3, USHORT nParam4, USHORT nParam5);

// Set P2P profile arguments
typedef BOOL (__stdcall *lpSetP2PProfile)(byte nCOMPort, byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

// Set Jog profile arguments
typedef BOOL (__stdcall *lpSetJogProfile)(byte nCOMPort, byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

// Set the drive enabled or disabled
typedef BOOL (__stdcall *lpDriveEnable)(byte nCOMPort, byte nNodeID, BOOL bEnable);

// Reset drive's alarm
typedef BOOL (__stdcall *lpAlarmReset)(byte nCOMPort, byte nNodeID);

// Launch feed to position move
typedef BOOL (__stdcall *lpFeedtoPosition)(byte nCOMPort, byte nNodeID, int* nDistance);

// Launch feed to length move
typedef BOOL (__stdcall *lpFeedtoLength)(byte nCOMPort, byte nNodeID, int* nDistance);

// Launch absolute move
typedef BOOL (__stdcall *lpAbsMove)(byte nCOMPort, byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

// Launch relative move
typedef BOOL (__stdcall *lpRelMove)(byte nCOMPort, byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

// Launch feed to sensor move
typedef BOOL (__stdcall *lpFeedtoSensor)(byte nCOMPort, byte nNodeID, byte nInputSensor, char chInputStatus);

// Lanuch feed to sensor move with safety distance
typedef BOOL (__stdcall *lpFeedtoSensorwithSafetyDistance)(byte nCOMPort, byte nNodeID, byte nInputSensor, char chInputStatus);

// Launch feed to double sensor move with mask distance
typedef BOOL (__stdcall *lpFeedtoSensorwithMaskDistance)(byte nCOMPort, byte nNodeID, byte nInputSensor, char chInputStatus);

// Launch Point to Point Move and set output
typedef BOOL (__stdcall *lpFeedandSetOutput)(byte nCOMPort, byte nNodeID, byte nOutput, char chOutputStatus);

// Launch feed to double sensor move
typedef BOOL (__stdcall *lpFeedtoDoubleSensor)(byte nCOMPort, byte nNodeID, byte nInputSensor1, char chInputStatus1, byte nInputSensor2, char chInputStatus2);

// Launch follow encoder move
typedef BOOL (__stdcall *lpFollowEncoder)(byte nCOMPort, byte nNodeID, byte nInputSensor, char chInputStatus);

// Commence Jogging
typedef BOOL (__stdcall *lpStartJogging)(byte nCOMPort, byte nNodeID);

// Stop Jogging
typedef BOOL (__stdcall *lpStopJogging)(byte nCOMPort, byte nNodeID);

// Set encoder function to the stepper drives with encoder feedback
typedef BOOL (__stdcall *lpSetEncoderFunction)(byte nCOMPort, byte nNodeID, byte nFunc);

// Set encoder position
typedef BOOL (__stdcall *lpSetEncoderPosition)(byte nCOMPort, byte nNodeID, int nPosition);

// Jog disable
typedef BOOL (__stdcall *lpJogDisable)(byte nCOMPort, byte nNodeID);

// Jog enable
typedef BOOL (__stdcall *lpJogEnable)(byte nCOMPort, byte nNodeID);

// Launch seek home move
typedef BOOL (__stdcall *lpSeekHome)(byte nCOMPort, byte nNodeID, byte nInputSensor, char chInputStatus);

// Set position
typedef BOOL (__stdcall *lpSetPosition)(byte nCOMPort, byte nNodeID, int nPosition);

// Set filter input
typedef BOOL (__stdcall *lpSetFilterInput)(byte nCOMPort, byte nNodeID, byte nInputSensor, int nFilterTime);

// Write analog deadband
typedef BOOL (__stdcall *lpWriteAnalogDeadband)(byte nCOMPort, byte nNodeID, byte nDeadband);

// Set output of the drive
typedef BOOL (__stdcall *lpSetOutput)(byte nCOMPort, byte nNodeID, byte nOutput, char nCondition);

// Write wait for input
typedef BOOL (__stdcall *lpWriteWaitforInput)(byte nCOMPort, byte nNodeID, byte nInputSensor, char nCondition);

// Queue Load and Execute
typedef BOOL (__stdcall *lpQueueLoadAndExecute)(byte nCOMPort, byte nNodeID, byte nSegment);

// Write aait time
typedef BOOL (__stdcall *lpWriteWaitTime)(byte nCOMPort, byte nNodeID, USHORT nTime);

// Stop and kill current move
typedef BOOL (__stdcall *lpStopAndKill)(byte nCOMPort, byte nNodeID);

// Stop and kill current move with normal deceleration
typedef BOOL (__stdcall *lpStopAndKillwithNormalDecel)(byte nCOMPort, byte nNodeID);

// Set P2P profile arguments
typedef BOOL (__stdcall *lpSetP2PProfile_M3)(byte nCOMPort, byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

// Set Jog profile arguments
typedef BOOL (__stdcall *lpSetJogProfile_M3)(byte nCOMPort, byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

// Set the drive enabled or disabled
typedef BOOL (__stdcall *lpDriveEnable_M3)(byte nCOMPort, byte nNodeID, BOOL bEnable);

// Reset drive's alarm
typedef BOOL (__stdcall *lpAlarmReset_M3)(byte nCOMPort, byte nNodeID);

// Launch feed to position move
typedef BOOL (__stdcall *lpFeedtoPosition_M3)(byte nCOMPort, byte nNodeID, int* nDistance);

// Launch feed to length move
typedef BOOL (__stdcall *lpFeedtoLength_M3)(byte nCOMPort, byte nNodeID, int* nDistance);

// Launch absolute move
typedef BOOL (__stdcall *lpAbsMove_M3)(byte nCOMPort, byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

// Launch relative move
typedef BOOL (__stdcall *lpRelMove_M3)(byte nCOMPort, byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

// Set Jog profile arguments
typedef BOOL (__stdcall *lpSetHomeProfile)(byte nCOMPort, byte nNodeID, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset);

// Set Jog profile arguments
typedef BOOL (__stdcall *lpSetHomeMethod)(byte nCOMPort, byte nNodeID, char nHomeMethod);

// Launch seek home move
typedef BOOL (__stdcall *lpFeedHome)(byte nCOMPort, byte nNodeID, char nHomeMethod, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset);

// Read alarm code
typedef BOOL (__stdcall *lpReadAlarmCode)(byte nCOMPort, byte nNodeID, UINT* nAlarmCode);

// Read status code
typedef BOOL (__stdcall *lpReadStatusCode)(byte nCOMPort, byte nNodeID, UINT* nStatusCode);

// Read immediate expanded inputs
typedef BOOL (__stdcall *lpReadImmediateExpandedInputs)(byte nCOMPort, byte nNodeID, USHORT* nInputsStatus);

// Read driver board inputs
typedef BOOL (__stdcall *lpReadDriverBoardInputs)(byte nCOMPort, byte nNodeID, USHORT* nInputsStatus);

// Read encoder position
typedef BOOL (__stdcall *lpReadEncoderPosition)(byte nCOMPort, byte nNodeID, int* nEncoderPosition);

// Read immediate absolute position
typedef BOOL (__stdcall *lpReadImmediateAbsolutePosition)(byte nCOMPort, byte nNodeID, int* nImmediateAbsolutePosition);

// Read immediate actual velocity
typedef BOOL (__stdcall *lpReadImmediateActualVelocity)(byte nCOMPort, byte nNodeID, double* dImmediateActualVelocity);

// Read immediate target velocity
typedef BOOL (__stdcall *lpReadImmediateTargetVelocity)(byte nCOMPort, byte nNodeID, double* dImmediateTargetVelocity);

// Read immediate drive temperature
typedef BOOL (__stdcall *lpReadImmediateDriveTemperature)(byte nCOMPort, byte nNodeID, double* dImmediateDriveTemperature);

// Read immediate bus voltage
typedef BOOL (__stdcall *lpReadImmediateBusVoltage)(byte nCOMPort, byte nNodeID, double* dImmediateBusVoltage);

// Read immediate position error
typedef BOOL (__stdcall *lpReadImmediatePositionError)(byte nCOMPort, byte nNodeID, int* nImmediatePositionError);

// Read immediate analog input value
typedef BOOL (__stdcall *lpReadImmediateAnalogInputValue)(byte nCOMPort, byte nNodeID, short* nImmediateAnalogInputValue);

// Read immediate analog input1 Value
typedef BOOL (__stdcall *lpReadImmediateAnalogInput1Value)(byte nCOMPort, byte nNodeID, short* nImmediateAnalogInputValue);

// Read immediate analog input2 Value
typedef BOOL (__stdcall *lpReadImmediateAnalogInput2Value)(byte nCOMPort, byte nNodeID, short* nImmediateAnalogInputValue);

// Read Q program line number
typedef BOOL (__stdcall *lpReadQProgramLineNumber)(byte nCOMPort, byte nNodeID, USHORT* nQProgramLineNumber);

// Read immediate current command
typedef BOOL (__stdcall *lpReadImmediateCurrentCommand)(byte nCOMPort, byte nNodeID, short* nImmediateCurrentCommand);

// Read relative distance
typedef BOOL (__stdcall *lpReadRelativeDistance)(byte nCOMPort, byte nNodeID, int* nRelativeDistance);

// Read sensor position
typedef BOOL (__stdcall *lpReadSensorPosition)(byte nCOMPort, byte nNodeID, int* nSensorPosition);

// Read condition code
typedef BOOL (__stdcall *lpReadConditionCode)(byte nCOMPort, byte nNodeID, USHORT* nConditionCode);

// Read command mode
typedef BOOL (__stdcall *lpReadCommandMode)(byte nCOMPort, byte nNodeID, USHORT* nCommandMode);

// Read distance or position
typedef BOOL (__stdcall *lpReadDistanceOrPosition)(byte nCOMPort, byte nNodeID, int* nDistanceOrPosition);

// Write distance or position
typedef BOOL (__stdcall *lpWriteDistanceOrPosition)(byte nCOMPort, byte nNodeID, int nDistanceOrPosition);

// Read change distance
typedef BOOL (__stdcall *lpReadChangeDistance)(byte nCOMPort, byte nNodeID, int* nChangeDistance);

// Write change distance
typedef BOOL (__stdcall *lpWriteChangeDistance)(byte nCOMPort, byte nNodeID, int nChangeDistance);

// Read change velocity
typedef BOOL (__stdcall *lpReadChangeVelocity)(byte nCOMPort, byte nNodeID, double* dChangeVelocity);

// Write change velocity
typedef BOOL (__stdcall *lpWriteChangeVelocity)(byte nCOMPort, byte nNodeID, double dChangeVelocity);

// Read velocity move state
typedef BOOL (__stdcall *lpReadVelocityMoveState)(byte nCOMPort, byte nNodeID, USHORT* nVelocityMoveState);

// Read P2P move state
typedef BOOL (__stdcall *lpReadP2PMoveState)(byte nCOMPort, byte nNodeID, USHORT* nP2PMoveState);

// Read Q program segment number
typedef BOOL (__stdcall *lpReadQProgramSegmentNumber)(byte nCOMPort, byte nNodeID, USHORT* nQProgramSegmentNumber);

// Read position offset
typedef BOOL (__stdcall *lpReadPositionOffset)(byte nCOMPort, byte nNodeID, int* nPositionOffset);

// Write position offset
typedef BOOL (__stdcall *lpWritePositionOffset)(byte nCOMPort, byte nNodeID, int nPositionOffset);

// Read running current
typedef BOOL (__stdcall *lpReadRunningCurrent)(byte nCOMPort, byte nNodeID, double* dRunningCurrent);

// Write running current
typedef BOOL (__stdcall *lpWriteRunningCurrent)(byte nCOMPort, byte nNodeID, double dRunningCurrent);

// Read electronic gearing
typedef BOOL (__stdcall *lpReadElectronicGearing)(byte nCOMPort, byte nNodeID, USHORT* nElectronicGearing);

// Write electronic gearing
typedef BOOL (__stdcall *lpWriteElectronicGearing)(byte nCOMPort, byte nNodeID, USHORT nElectronicGearing);

// Read pulse counter
typedef BOOL (__stdcall *lpReadPulseCounter)(byte nCOMPort, byte nNodeID, int* nPulseCounter);

// Write pulse counter
typedef BOOL (__stdcall *lpWritePulseCounter)(byte nCOMPort, byte nNodeID, int nPulseCounter);

// Read analog position gain
typedef BOOL (__stdcall *lpReadAnalogPositionGain)(byte nCOMPort, byte nNodeID, USHORT* nAnalogPositionGain);

// Write analog position gain
typedef BOOL (__stdcall *lpWriteAnalogPositionGain)(byte nCOMPort, byte nNodeID, USHORT nAnalogPositionGain);

// Read analog threshold
typedef BOOL (__stdcall *lpReadAnalogThreshold)(byte nCOMPort, byte nNodeID, USHORT* nAnalogThreshold);

// Write analog threshold
typedef BOOL (__stdcall *lpWriteAnalogThreshold)(byte nCOMPort, byte nNodeID, USHORT nAnalogThreshold);

// Read analogoffset
typedef BOOL (__stdcall *lpReadAnalogOffset)(byte nCOMPort, byte nNodeID, USHORT* nAnalogOffset);

// Write analog offset
typedef BOOL (__stdcall *lpWriteAnalogOffset)(byte nCOMPort, byte nNodeID, USHORT nAnalogOffset);

// Read accumulator
typedef BOOL (__stdcall *lpReadAccumulator)(byte nCOMPort, byte nNodeID, int* nAccumulator);

// Read user defined register
typedef BOOL (__stdcall *lpReadUserDefinedRegister)(byte nCOMPort, byte nNodeID, char chRegister, int* nValue);

// Write user defined register
typedef BOOL (__stdcall *lpWriteUserDefinedRegister)(byte nCOMPort, byte nNodeID, byte chRegister, int nValue);

// Read brake release delay
typedef BOOL (__stdcall *lpReadBrakeReleaseDelay)(byte nCOMPort, byte nNodeID, double* dBrakeReleaseDelay);

// Write brake release delay
typedef BOOL (__stdcall *lpWriteBrakeReleaseDelay)(byte nCOMPort, byte nNodeID, double dBrakeReleaseDelay);

// Read brake engage delay
typedef BOOL (__stdcall *lpReadBrakeEngageDelay)(byte nCOMPort, byte nNodeID, double* dBrakeEngageDelay);

// Write brake engage delay
typedef BOOL (__stdcall *lpWriteBrakeEngageDelay)(byte nCOMPort, byte nNodeID, double dBrakeEngageDelay);

// Read analog filter gain
typedef BOOL (__stdcall *lpReadAnalogFilterGain)(byte nCOMPort, byte nNodeID, USHORT* nAnalogFilterGain);

// Write analog filter gain
typedef BOOL (__stdcall *lpWriteAnalogFilterGain)(byte nCOMPort, byte nNodeID, USHORT nAnalogFilterGain);

// Read alarm code
typedef BOOL (__stdcall *lpReadAlarmCode_M3)(byte nCOMPort, byte nNodeID, UINT* nAlarmCode);

// Read status code
typedef BOOL (__stdcall *lpReadStatusCode_M3)(byte nCOMPort, byte nNodeID, UINT* nStatusCode);

// Read driver inputs
typedef BOOL (__stdcall *lpReadDriverBoardOutputs_M3)(byte nCOMPort, byte nNodeID, USHORT* nInputsStatus);

// Read driver oututs
typedef BOOL (__stdcall *lpReadDriverBoardInputs_M3)(byte nCOMPort, byte nNodeID, USHORT* nInputsStatus);

// Read encoder position
typedef BOOL (__stdcall *lpReadEncoderPosition_M3)(byte nCOMPort, byte nNodeID, int* nEncoderPosition);

// Read encoder position
typedef BOOL (__stdcall *lpRead2ndEncoderPosition_M3)(byte nCOMPort, byte nNodeID, int* nEncoderPosition);

// Read immediate absolute position
typedef BOOL (__stdcall *lpReadImmediateAbsolutePosition_M3)(byte nCOMPort, byte nNodeID, int* nImmediateAbsolutePosition);

// Read immediate actual velocity
typedef BOOL (__stdcall *lpReadImmediateActualVelocity_M3)(byte nCOMPort, byte nNodeID, double* dImmediateActualVelocity);

// Read immediate target velocity
typedef BOOL (__stdcall *lpReadImmediateTargetVelocity_M3)(byte nCOMPort, byte nNodeID, double* dImmediateTargetVelocity);

// Read immediate drive temperature
typedef BOOL (__stdcall *lpReadImmediateDriveTemperature_M3)(byte nCOMPort, byte nNodeID, double* dImmediateDriveTemperature);

// Read immediate bus voltage
typedef BOOL (__stdcall *lpReadImmediateBusVoltage_M3)(byte nCOMPort, byte nNodeID, double* dImmediateBusVoltage);

// Read immediate position error
typedef BOOL (__stdcall *lpReadImmediatePositionError_M3)(byte nCOMPort, byte nNodeID, int* nImmediatePositionError);

// Read immediate analog input1 Value
typedef BOOL (__stdcall *lpReadImmediateAnalogInput1Value_M3)(byte nCOMPort, byte nNodeID, short* dImmediateAnalogInputValue);

// Read immediate analog input2 Value
typedef BOOL (__stdcall *lpReadImmediateAnalogInput2Value_M3)(byte nCOMPort, byte nNodeID, short* dImmediateAnalogInputValue);

// Read immediate analog output 1
typedef BOOL (__stdcall *lpReadImmediateAnalogOutput1Value_M3)(byte nCOMPort, byte nNodeID, short* nImmediateAnalogOutputValue);

// Write immediate analog output 1
typedef BOOL (__stdcall *lpWriteImmediateAnalogOutput1Value_M3)(byte nCOMPort, byte nNodeID, short nImmediateAnalogOutputValue);

// Read immediate analog output 2
typedef BOOL (__stdcall *lpReadImmediateAnalogOutput2Value_M3)(byte nCOMPort, byte nNodeID, short* dImmediateAnalogOutputValue);

// Write immediate analog output 2
typedef BOOL (__stdcall *lpWriteImmediateAnalogOutput2Value_M3)(byte nCOMPort, byte nNodeID, short nImmediateAnalogOutputValue);

// Read Q program line number
typedef BOOL (__stdcall *lpReadQProgramLineNumber_M3)(byte nCOMPort, byte nNodeID, USHORT* nQProgramLineNumber);

// Read immediate current command
typedef BOOL (__stdcall *lpReadImmediateCurrentCommand_M3)(byte nCOMPort, byte nNodeID, short* nImmediateCurrentCommand);

// Read relative distance
typedef BOOL (__stdcall *lpReadRelativeDistance_M3)(byte nCOMPort, byte nNodeID, int* nRelativeDistance);

// Read sensor position
typedef BOOL (__stdcall *lpReadSensorPosition_M3)(byte nCOMPort, byte nNodeID, int* nSensorPosition);

// Read condition code
typedef BOOL (__stdcall *lpReadConditionCode_M3)(byte nCOMPort, byte nNodeID, USHORT* nConditionCode);

// Read command mode
typedef BOOL (__stdcall *lpReadCommandMode_M3)(byte nCOMPort, byte nNodeID, USHORT* nCommandMode);

// Read distance or position
typedef BOOL (__stdcall *lpReadDistanceOrPosition_M3)(byte nCOMPort, byte nNodeID, int* nDistanceOrPosition);

// Write distance or position
typedef BOOL (__stdcall *lpWriteDistanceOrPosition_M3)(byte nCOMPort, byte nNodeID, int nDistanceOrPosition);

// Read change distance
typedef BOOL (__stdcall *lpReadChangeDistance_M3)(byte nCOMPort, byte nNodeID, int* nChangeDistance);

// Write change distance
typedef BOOL (__stdcall *lpWriteChangeDistance_M3)(byte nCOMPort, byte nNodeID, int nChangeDistance);

// Read change velocity
typedef BOOL (__stdcall *lpReadChangeVelocity_M3)(byte nCOMPort, byte nNodeID, double* dChangeVelocity);

// Write change velocity
typedef BOOL (__stdcall *lpWriteChangeVelocity_M3)(byte nCOMPort, byte nNodeID, double dChangeVelocity);

// Read velocity move state
typedef BOOL (__stdcall *lpReadVelocityMoveState_M3)(byte nCOMPort, byte nNodeID, USHORT* nVelocityMoveState);

// Read P2P move state
typedef BOOL (__stdcall *lpReadP2PMoveState_M3)(byte nCOMPort, byte nNodeID, USHORT* nP2PMoveState);

// Read Q program segment number
typedef BOOL (__stdcall *lpReadQProgramSegmentNumber_M3)(byte nCOMPort, byte nNodeID, USHORT* nQProgramSegmentNumber);

// Read Command current
typedef BOOL (__stdcall *lpReadCommandCurrent_M3)(byte nCOMPort, byte nNodeID, double* dCommandCurrent);

// Write Command current
typedef BOOL (__stdcall *lpWriteCommandCurrent_M3)(byte nCOMPort, byte nNodeID, double dCommandCurrent);

// Read Max current
typedef BOOL (__stdcall *lpReadMaximumCurrent_M3)(byte nCOMPort, byte nNodeID, double* dMaximumCurrent);

// Write Max current
typedef BOOL (__stdcall *lpWriteMaximumCurrent_M3)(byte nCOMPort, byte nNodeID, double dMaximumCurrent);

// Read electronic gearing
typedef BOOL (__stdcall *lpReadElectronicGearing_M3)(byte nCOMPort, byte nNodeID, UINT* nElectronicGearing);

// Write electronic gearing
typedef BOOL (__stdcall *lpWriteElectronicGearing_M3)(byte nCOMPort, byte nNodeID, UINT nElectronicGearing);

// Read pulse counter
typedef BOOL (__stdcall *lpReadPulseCounter_M3)(byte nCOMPort, byte nNodeID, int* nPulseCounter);

// Write pulse counter
typedef BOOL (__stdcall *lpWritePulseCounter_M3)(byte nCOMPort, byte nNodeID, int nPulseCounter);

// Read analog position gain
typedef BOOL (__stdcall *lpReadAnalogPositionGain_M3)(byte nCOMPort, byte nNodeID, int* nAnalogPositionGain);

// Write analog position gain
typedef BOOL (__stdcall *lpWriteAnalogPositionGain_M3)(byte nCOMPort, byte nNodeID, int nAnalogPositionGain);

// Read analog velocity gain
typedef BOOL (__stdcall *lpReadAnalogVelocityGain_M3)(byte nCOMPort, byte nNodeID, double* dAnalogVelocityGain);

// Write analog velocity gain
typedef BOOL (__stdcall *lpWriteAnalogVelocityGain_M3)(byte nCOMPort, byte nNodeID, double dAnalogVelocityGain);

// Read analog torque gain
typedef BOOL (__stdcall *lpReadAnalogTorqueGain_M3)(byte nCOMPort, byte nNodeID, double* dAnalogTorqueGain);

// Write analog torque gain
typedef BOOL (__stdcall *lpWriteAnalogTorqueGain_M3)(byte nCOMPort, byte nNodeID, double dAnalogTorqueGain);

// Read analog threshold 1
typedef BOOL (__stdcall *lpReadAnalogThreshold1_M3)(byte nCOMPort, byte nNodeID, int* nAnalogThreshold);

// Write analog threshold 1
typedef BOOL (__stdcall *lpWriteAnalogThreshold1_M3)(byte nCOMPort, byte nNodeID, int nAnalogThreshold);

// Read analog threshold 2
typedef BOOL (__stdcall *lpReadAnalogThreshold2_M3)(byte nCOMPort, byte nNodeID, int* nAnalogThreshold);

// Write analog threshold 2
typedef BOOL (__stdcall *lpWriteAnalogThreshold2_M3)(byte nCOMPort, byte nNodeID, int nAnalogThreshold);

// Read analog offset 1
typedef BOOL (__stdcall *lpReadAnalogOffset1_M3)(byte nCOMPort, byte nNodeID, int* nAnalogOffset);

// Write analog offset 1
typedef BOOL (__stdcall *lpWriteAnalogOffset1_M3)(byte nCOMPort, byte nNodeID, int nAnalogOffset);

// Read analog offset 2
typedef BOOL (__stdcall *lpReadAnalogOffset2_M3)(byte nCOMPort, byte nNodeID, int* nAnalogOffset);

// Write analog offset 2
typedef BOOL (__stdcall *lpWriteAnalogOffset2_M3)(byte nCOMPort, byte nNodeID, int nAnalogOffset);

// Read accumulator
typedef BOOL (__stdcall *lpReadAccumulator_M3)(byte nCOMPort, byte nNodeID, int* nAccumulator);

// Read user defined register
typedef BOOL (__stdcall *lpReadUserDefinedRegister_M3)(byte nCOMPort, byte nNodeID, char chRegister, int* nValue);

// Write user defined register
typedef BOOL (__stdcall *lpWriteUserDefinedRegister_M3)(byte nCOMPort, byte nNodeID, byte chRegister, int nValue);

// Read brake release delay
typedef BOOL (__stdcall *lpReadBrakeReleaseDelay_M3)(byte nCOMPort, byte nNodeID, double* dBrakeReleaseDelay);

// Write brake release delay
typedef BOOL (__stdcall *lpWriteBrakeReleaseDelay_M3)(byte nCOMPort, byte nNodeID, double dBrakeReleaseDelay);

// Read brake engage delay
typedef BOOL (__stdcall *lpReadBrakeEngageDelay_M3)(byte nCOMPort, byte nNodeID, double* dBrakeEngageDelay);

// Write brake engage delay
typedef BOOL (__stdcall *lpWriteBrakeEngageDelay_M3)(byte nCOMPort, byte nNodeID, double dBrakeEngageDelay);

// Read analog filter gain
typedef BOOL (__stdcall *lpReadAnalogFilterGain1_M3)(byte nCOMPort, byte nNodeID, USHORT* nAnalogFilterGain);

// Write analog filter gain
typedef BOOL (__stdcall *lpWriteAnalogFilterGain1_M3)(byte nCOMPort, byte nNodeID, USHORT nAnalogFilterGain);

// Read analog filter gain
typedef BOOL (__stdcall *lpReadAnalogFilterGain2_M3)(byte nCOMPort, byte nNodeID, USHORT* nAnalogFilterGain);

// Write analog filter gain
typedef BOOL (__stdcall *lpWriteAnalogFilterGain2_M3)(byte nCOMPort, byte nNodeID, USHORT nAnalogFilterGain);

// Write broadcast command
typedef BOOL (__stdcall *lpWriteBroadcastCommand)(byte nCOMPort, byte nNodeIDCount, byte* aNodeIDArr, byte nFunctionCode);

// Execute multi-axes linear interpolation move
typedef BOOL (__stdcall *lpExecuteLinearInterpolationMove)(byte nCOMPort, byte nNodeIDCount, byte* aNodeIDArr, int* aPosArr, UINT nMaxLineSpeed, UINT nAccelTimeInMs, BOOL bRelMove, BOOL bValidParamLimit);

class ModbusRTUHelper
{
private:
	HINSTANCE m_hDll; //DLL Handle
	bool m_bWasLoaded;
	byte m_nCOMPort;
protected:
	lpOnDataSend m_OnDataSend;
	lpOnDataReceive m_OnDataReceive;
	lpIsOpen m_IsOpen;
	lpOpen m_Open;
	lpClose m_Close;
	lpSetEndianType m_SetEndianType;
	lpIsBigEndian m_IsBigEndian;
	lpGetExecuteTimeOut m_GetExecuteTimeOut;
	lpSetExecuteTimeOut m_SetExecuteTimeOut;
	lpSetTriggerSendEvent m_SetTriggerSendEvent;
	lpSetTriggerReceiveEvent m_SetTriggerReceiveEvent;
	lpClearSendBuffer m_ClearSendBuffer;
	lpClearReceivedBuffer m_ClearReceivedBuffer;
	lpClearBuffer m_ClearBuffer;
	lpGetLastErrorInfo m_GetLastErrorInfo;
	lpGetLastCommandSent m_GetLastCommandSent;
	lpGetLastCommandReceived m_GetLastCommandReceived;
	lpReadSingleHoldingRegister m_ReadSingleHoldingRegister;
	lpWriteSingleHoldingRegister m_WriteSingleHoldingRegister;
	lpReadMultiHoldingRegisters m_ReadMultiHoldingRegisters;
	lpWriteMultiHoldingRegisters m_WriteMultiHoldingRegisters;
	lpReadDataInt16 m_ReadDataInt16;
	lpWriteDataInt16 m_WriteDataInt16;
	lpReadDataUInt16 m_ReadDataUInt16;
	lpWriteDataUInt16 m_WriteDataUInt16;
	lpReadDataInt32 m_ReadDataInt32;
	lpWriteDataInt32 m_WriteDataInt32;
	lpReadDataUInt32 m_ReadDataUInt32;
	lpWriteDataUInt32 m_WriteDataUInt32;
	lpExecuteCommandWithOpcode m_ExecuteCommandWithOpcode;
	lpSetP2PProfile m_SetP2PProfile;
	lpSetJogProfile m_SetJogProfile;
	lpDriveEnable m_DriveEnable;
	lpAlarmReset m_AlarmReset;
	lpFeedtoPosition m_FeedtoPosition;
	lpFeedtoLength m_FeedtoLength;
	lpAbsMove m_AbsMove;
	lpRelMove m_RelMove;
	lpFeedtoSensor m_FeedtoSensor;
	lpFeedtoSensorwithSafetyDistance m_FeedtoSensorwithSafetyDistance;
	lpFeedtoSensorwithMaskDistance m_FeedtoSensorwithMaskDistance;
	lpFeedandSetOutput m_FeedandSetOutput;
	lpFeedtoDoubleSensor m_FeedtoDoubleSensor;
	lpFollowEncoder m_FollowEncoder;
	lpStartJogging m_StartJogging;
	lpStopJogging m_StopJogging;
	lpSetEncoderFunction m_SetEncoderFunction;
	lpSetEncoderPosition m_SetEncoderPosition;
	lpJogDisable m_JogDisable;
	lpJogEnable m_JogEnable;
	lpSeekHome m_SeekHome;
	lpSetPosition m_SetPosition;
	lpSetFilterInput m_SetFilterInput;
	lpWriteAnalogDeadband m_WriteAnalogDeadband;
	lpSetOutput m_SetOutput;
	lpWriteWaitforInput m_WriteWaitforInput;
	lpQueueLoadAndExecute m_QueueLoadAndExecute;
	lpWriteWaitTime m_WriteWaitTime;
	lpStopAndKill m_StopAndKill;
	lpStopAndKillwithNormalDecel m_StopAndKillwithNormalDecel;
	lpSetP2PProfile_M3 m_SetP2PProfile_M3;
	lpSetJogProfile_M3 m_SetJogProfile_M3;
	lpDriveEnable_M3 m_DriveEnable_M3;
	lpAlarmReset_M3 m_AlarmReset_M3;
	lpFeedtoPosition_M3 m_FeedtoPosition_M3;
	lpFeedtoLength_M3 m_FeedtoLength_M3;
	lpAbsMove_M3 m_AbsMove_M3;
	lpRelMove_M3 m_RelMove_M3;
	lpSetHomeProfile m_SetHomeProfile;
	lpSetHomeMethod m_SetHomeMethod;
	lpFeedHome m_FeedHome;
	lpReadAlarmCode m_ReadAlarmCode;
	lpReadStatusCode m_ReadStatusCode;
	lpReadImmediateExpandedInputs m_ReadImmediateExpandedInputs;
	lpReadDriverBoardInputs m_ReadDriverBoardInputs;
	lpReadEncoderPosition m_ReadEncoderPosition;
	lpReadImmediateAbsolutePosition m_ReadImmediateAbsolutePosition;
	lpReadImmediateActualVelocity m_ReadImmediateActualVelocity;
	lpReadImmediateTargetVelocity m_ReadImmediateTargetVelocity;
	lpReadImmediateDriveTemperature m_ReadImmediateDriveTemperature;
	lpReadImmediateBusVoltage m_ReadImmediateBusVoltage;
	lpReadImmediatePositionError m_ReadImmediatePositionError;
	lpReadImmediateAnalogInputValue m_ReadImmediateAnalogInputValue;
	lpReadImmediateAnalogInput1Value m_ReadImmediateAnalogInput1Value;
	lpReadImmediateAnalogInput2Value m_ReadImmediateAnalogInput2Value;
	lpReadQProgramLineNumber m_ReadQProgramLineNumber;
	lpReadImmediateCurrentCommand m_ReadImmediateCurrentCommand;
	lpReadRelativeDistance m_ReadRelativeDistance;
	lpReadSensorPosition m_ReadSensorPosition;
	lpReadConditionCode m_ReadConditionCode;
	lpReadCommandMode m_ReadCommandMode;
	lpReadDistanceOrPosition m_ReadDistanceOrPosition;
	lpWriteDistanceOrPosition m_WriteDistanceOrPosition;
	lpReadChangeDistance m_ReadChangeDistance;
	lpWriteChangeDistance m_WriteChangeDistance;
	lpReadChangeVelocity m_ReadChangeVelocity;
	lpWriteChangeVelocity m_WriteChangeVelocity;
	lpReadVelocityMoveState m_ReadVelocityMoveState;
	lpReadP2PMoveState m_ReadP2PMoveState;
	lpReadQProgramSegmentNumber m_ReadQProgramSegmentNumber;
	lpReadPositionOffset m_ReadPositionOffset;
	lpWritePositionOffset m_WritePositionOffset;
	lpReadRunningCurrent m_ReadRunningCurrent;
	lpWriteRunningCurrent m_WriteRunningCurrent;
	lpReadElectronicGearing m_ReadElectronicGearing;
	lpWriteElectronicGearing m_WriteElectronicGearing;
	lpReadPulseCounter m_ReadPulseCounter;
	lpWritePulseCounter m_WritePulseCounter;
	lpReadAnalogPositionGain m_ReadAnalogPositionGain;
	lpWriteAnalogPositionGain m_WriteAnalogPositionGain;
	lpReadAnalogThreshold m_ReadAnalogThreshold;
	lpWriteAnalogThreshold m_WriteAnalogThreshold;
	lpReadAnalogOffset m_ReadAnalogOffset;
	lpWriteAnalogOffset m_WriteAnalogOffset;
	lpReadAccumulator m_ReadAccumulator;
	lpReadUserDefinedRegister m_ReadUserDefinedRegister;
	lpWriteUserDefinedRegister m_WriteUserDefinedRegister;
	lpReadBrakeReleaseDelay m_ReadBrakeReleaseDelay;
	lpWriteBrakeReleaseDelay m_WriteBrakeReleaseDelay;
	lpReadBrakeEngageDelay m_ReadBrakeEngageDelay;
	lpWriteBrakeEngageDelay m_WriteBrakeEngageDelay;
	lpReadAnalogFilterGain m_ReadAnalogFilterGain;
	lpWriteAnalogFilterGain m_WriteAnalogFilterGain;
	lpReadAlarmCode_M3 m_ReadAlarmCode_M3;
	lpReadStatusCode_M3 m_ReadStatusCode_M3;
	lpReadDriverBoardOutputs_M3 m_ReadDriverBoardOutputs_M3;
	lpReadDriverBoardInputs_M3 m_ReadDriverBoardInputs_M3;
	lpReadEncoderPosition_M3 m_ReadEncoderPosition_M3;
	lpRead2ndEncoderPosition_M3 m_Read2ndEncoderPosition_M3;
	lpReadImmediateAbsolutePosition_M3 m_ReadImmediateAbsolutePosition_M3;
	lpReadImmediateActualVelocity_M3 m_ReadImmediateActualVelocity_M3;
	lpReadImmediateTargetVelocity_M3 m_ReadImmediateTargetVelocity_M3;
	lpReadImmediateDriveTemperature_M3 m_ReadImmediateDriveTemperature_M3;
	lpReadImmediateBusVoltage_M3 m_ReadImmediateBusVoltage_M3;
	lpReadImmediatePositionError_M3 m_ReadImmediatePositionError_M3;
	lpReadImmediateAnalogInput1Value_M3 m_ReadImmediateAnalogInput1Value_M3;
	lpReadImmediateAnalogInput2Value_M3 m_ReadImmediateAnalogInput2Value_M3;
	lpReadImmediateAnalogOutput1Value_M3 m_ReadImmediateAnalogOutput1Value_M3;
	lpWriteImmediateAnalogOutput1Value_M3 m_WriteImmediateAnalogOutput1Value_M3;
	lpReadImmediateAnalogOutput2Value_M3 m_ReadImmediateAnalogOutput2Value_M3;
	lpWriteImmediateAnalogOutput2Value_M3 m_WriteImmediateAnalogOutput2Value_M3;
	lpReadQProgramLineNumber_M3 m_ReadQProgramLineNumber_M3;
	lpReadImmediateCurrentCommand_M3 m_ReadImmediateCurrentCommand_M3;
	lpReadRelativeDistance_M3 m_ReadRelativeDistance_M3;
	lpReadSensorPosition_M3 m_ReadSensorPosition_M3;
	lpReadConditionCode_M3 m_ReadConditionCode_M3;
	lpReadCommandMode_M3 m_ReadCommandMode_M3;
	lpReadDistanceOrPosition_M3 m_ReadDistanceOrPosition_M3;
	lpWriteDistanceOrPosition_M3 m_WriteDistanceOrPosition_M3;
	lpReadChangeDistance_M3 m_ReadChangeDistance_M3;
	lpWriteChangeDistance_M3 m_WriteChangeDistance_M3;
	lpReadChangeVelocity_M3 m_ReadChangeVelocity_M3;
	lpWriteChangeVelocity_M3 m_WriteChangeVelocity_M3;
	lpReadVelocityMoveState_M3 m_ReadVelocityMoveState_M3;
	lpReadP2PMoveState_M3 m_ReadP2PMoveState_M3;
	lpReadQProgramSegmentNumber_M3 m_ReadQProgramSegmentNumber_M3;
	lpReadCommandCurrent_M3 m_ReadCommandCurrent_M3;
	lpWriteCommandCurrent_M3 m_WriteCommandCurrent_M3;
	lpReadMaximumCurrent_M3 m_ReadMaximumCurrent_M3;
	lpWriteMaximumCurrent_M3 m_WriteMaximumCurrent_M3;
	lpReadElectronicGearing_M3 m_ReadElectronicGearing_M3;
	lpWriteElectronicGearing_M3 m_WriteElectronicGearing_M3;
	lpReadPulseCounter_M3 m_ReadPulseCounter_M3;
	lpWritePulseCounter_M3 m_WritePulseCounter_M3;
	lpReadAnalogPositionGain_M3 m_ReadAnalogPositionGain_M3;
	lpWriteAnalogPositionGain_M3 m_WriteAnalogPositionGain_M3;
	lpReadAnalogVelocityGain_M3 m_ReadAnalogVelocityGain_M3;
	lpWriteAnalogVelocityGain_M3 m_WriteAnalogVelocityGain_M3;
	lpReadAnalogTorqueGain_M3 m_ReadAnalogTorqueGain_M3;
	lpWriteAnalogTorqueGain_M3 m_WriteAnalogTorqueGain_M3;
	lpReadAnalogThreshold1_M3 m_ReadAnalogThreshold1_M3;
	lpWriteAnalogThreshold1_M3 m_WriteAnalogThreshold1_M3;
	lpReadAnalogThreshold2_M3 m_ReadAnalogThreshold2_M3;
	lpWriteAnalogThreshold2_M3 m_WriteAnalogThreshold2_M3;
	lpReadAnalogOffset1_M3 m_ReadAnalogOffset1_M3;
	lpWriteAnalogOffset1_M3 m_WriteAnalogOffset1_M3;
	lpReadAnalogOffset2_M3 m_ReadAnalogOffset2_M3;
	lpWriteAnalogOffset2_M3 m_WriteAnalogOffset2_M3;
	lpReadAccumulator_M3 m_ReadAccumulator_M3;
	lpReadUserDefinedRegister_M3 m_ReadUserDefinedRegister_M3;
	lpWriteUserDefinedRegister_M3 m_WriteUserDefinedRegister_M3;
	lpReadBrakeReleaseDelay_M3 m_ReadBrakeReleaseDelay_M3;
	lpWriteBrakeReleaseDelay_M3 m_WriteBrakeReleaseDelay_M3;
	lpReadBrakeEngageDelay_M3 m_ReadBrakeEngageDelay_M3;
	lpWriteBrakeEngageDelay_M3 m_WriteBrakeEngageDelay_M3;
	lpReadAnalogFilterGain1_M3 m_ReadAnalogFilterGain1_M3;
	lpWriteAnalogFilterGain1_M3 m_WriteAnalogFilterGain1_M3;
	lpReadAnalogFilterGain2_M3 m_ReadAnalogFilterGain2_M3;
	lpWriteAnalogFilterGain2_M3 m_WriteAnalogFilterGain2_M3;

	lpWriteBroadcastCommand m_WriteBroadcastCommand;
	lpExecuteLinearInterpolationMove m_ExecuteLinearInterpolationMove;
public:
	ModbusRTUHelper(void);
	virtual ~ModbusRTUHelper(void);

	// Trigger when send data to drive
	void OnDataSend(EventCallback func);

	// Trigger when received data from drive
	void OnDataReceive(EventCallback func);

	// Get the port is open or closed
	BOOL IsOpen();

	// Open serial port communication
	BOOL Open(int nCOMPort, int nBaudRate, BOOL bBigEndian);

	// Close serial port communication
	BOOL Close();

	// Set endian type
	void SetEndianType(BOOL bBigEndian);

	// Return it is big endian type or not
	BOOL IsBigEndian();

	// Get execute time out
	UINT GetExecuteTimeOut();

	// Set execute time out
	void SetExecuteTimeOut(UINT nExecuteTimeOut);

	// Set it will trigger send event or not when sending command
	void SetTriggerSendEvent(BOOL bTriggerSendEvent);

	// Set it will trigger received event or not when receive data
	void SetTriggerReceiveEvent(BOOL bTriggerReceiveEvent);

	// Clear send buffer
	void ClearSendBuffer();

	// Clear received buffer
	void ClearReceivedBuffer();

	// Clear send and received Buffer
	void ClearBuffer();

	// Get last error information, includes code and description
	void GetLastErrorInfo(PERROR_INFO pErrorInfo);

	// Get the last command that send
	void GetLastCommandSent(COMMAND_INFO& command);

	// Get the last command that received
	void GetLastCommandReceived(COMMAND_INFO& command);

	// Read single holding register from the drive
	BOOL ReadSingleHoldingRegister(byte nNodeID, int nRegisterNo, int* pValue);

	// Write single holding register value to the drive
	BOOL WriteSingleHoldingRegister(byte nNodeID, int nRegisterNo, int nValue);

	// Read multiple holding register from the drive
	BOOL ReadMultiHoldingRegisters(byte nNodeID, int nRegisterNo, int nCount, int* valueList);

	// Write multiple holding register values to the drive
	BOOL WriteMultiHoldingRegisters(byte nNodeID, int nRegisterNo, int nCount, int* valueList);

	// Read 16-bit int data from the drive
	BOOL ReadDataInt16(byte nNodeID, int nRegisterNo, SHORT* nValue);

	// Write 16-bit int data to the drive
	BOOL WriteDataInt16(byte nNodeID, int nRegisterNo, SHORT nValue);

	// Read 16-bit unsighed int data from the drive
	BOOL ReadDataUInt16(byte nNodeID, int nRegisterNo, USHORT* nValue);

	// Write 16-bit unsigned int data to the drive
	BOOL WriteDataUInt16(byte nNodeID, int nRegisterNo, USHORT nValue);

	// Read 32-bit int data from the drive
	BOOL ReadDataInt32(byte nNodeID, int nRegisterNo, int* nValue);

	// Write 32-bit int data to the drive
	BOOL WriteDataInt32(byte nNodeID, int nRegisterNo, int nValue);

	// Read 32-bit unsighed int data from the drive
	BOOL ReadDataUInt32(byte nNodeID, int nRegisterNo, UINT* nValue);

	// Write 32-bit unsigned int data to the drive
	BOOL WriteDataUInt32(byte nNodeID, int nRegisterNo, UINT nValue);

	// Execute command with opcode
	BOOL ExecuteCommandWithOpcode(byte nNodeID, USHORT nOpCode, USHORT nParam1, USHORT nParam2, USHORT nParam3, USHORT nParam4, USHORT nParam5);

	// Set P2P profile arguments
	BOOL SetP2PProfile(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

	// Set Jog profile arguments
	BOOL SetJogProfile(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

	// Set the drive enabled or disabled
	BOOL DriveEnable(byte nNodeID, BOOL bEnable);

	// Reset drive's alarm
	BOOL AlarmReset(byte nNodeID);

	// Launch feed to position move
	BOOL FeedtoPosition(byte nNodeID, int* nDistance);

	// Launch feed to length move
	BOOL FeedtoLength(byte nNodeID, int* nDistance);

	// Launch absolute move
	BOOL AbsMove(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

	// Launch relative move
	BOOL RelMove(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

	// Launch feed to sensor move
	BOOL FeedtoSensor(byte nNodeID, byte nInputSensor, char chInputStatus);

	// Lanuch feed to sensor move with safety distance
	BOOL FeedtoSensorwithSafetyDistance(byte nNodeID, byte nInputSensor, char chInputStatus);

	// Launch feed to double sensor move with mask distance
	BOOL FeedtoSensorwithMaskDistance(byte nNodeID, byte nInputSensor, char chInputStatus);

	// Launch Point to Point Move and set output
	BOOL FeedandSetOutput(byte nNodeID, byte nOutput, char chOutputStatus);

	// Launch feed to double sensor move
	BOOL FeedtoDoubleSensor(byte nNodeID, byte nInputSensor1, char chInputStatus1, byte nInputSensor2, char chInputStatus2);

	// Launch follow encoder move
	BOOL FollowEncoder(byte nNodeID, byte nInputSensor, char chInputStatus);

	// Commence Jogging
	BOOL StartJogging(byte nNodeID);

	// Stop Jogging
	BOOL StopJogging(byte nNodeID);

	// Set encoder function to the stepper drives with encoder feedback
	BOOL SetEncoderFunction(byte nNodeID, byte nFunc);

	// Set encoder position
	BOOL SetEncoderPosition(byte nNodeID, int nPosition);

	// Jog disable
	BOOL JogDisable(byte nNodeID);

	// Jog enable
	BOOL JogEnable(byte nNodeID);

	// Launch seek home move
	BOOL SeekHome(byte nNodeID, byte nInputSensor, char chInputStatus);

	// Set position
	BOOL SetPosition(byte nNodeID, int nPosition);

	// Set filter input
	BOOL SetFilterInput(byte nNodeID, byte nInputSensor, int nFilterTime);

	// Write analog deadband
	BOOL WriteAnalogDeadband(byte nNodeID, byte nDeadband);

	// Set output of the drive
	BOOL SetOutput(byte nNodeID, byte nOutput, char nCondition);

	// Write wait for input
	BOOL WriteWaitforInput(byte nNodeID, byte nInputSensor, char nCondition);

	// Queue Load and Execute
	BOOL QueueLoadAndExecute(byte nNodeID, byte nSegment);

	// Write aait time
	BOOL WriteWaitTime(byte nNodeID, USHORT nTime);

	// Stop and kill current move
	BOOL StopAndKill(byte nNodeID);

	// Stop and kill current move with normal deceleration
	BOOL StopAndKillwithNormalDecel(byte nNodeID);

	// Set P2P profile arguments
	BOOL SetP2PProfile_M3(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

	// Set Jog profile arguments
	BOOL SetJogProfile_M3(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel);

	// Set the drive enabled or disabled
	BOOL DriveEnable_M3(byte nNodeID, BOOL bEnable);

	// Reset drive's alarm
	BOOL AlarmReset_M3(byte nNodeID);

	// Launch feed to position move
	BOOL FeedtoPosition_M3(byte nNodeID, int* nDistance);

	// Launch feed to length move
	BOOL FeedtoLength_M3(byte nNodeID, int* nDistance);

	// Launch absolute move
	BOOL AbsMove_M3(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

	// Launch relative move
	BOOL RelMove_M3(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel);

	// Set Jog profile arguments
	BOOL SetHomeProfile(byte nNodeID, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset);

	// Set Jog profile arguments
	BOOL SetHomeMethod(byte nNodeID, char nHomeMethod);

	// Launch seek home move
	BOOL FeedHome(byte nNodeID, char nHomeMethod, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset);

	// Read alarm code
	BOOL ReadAlarmCode(byte nNodeID, UINT* nAlarmCode);

	// Read status code
	BOOL ReadStatusCode(byte nNodeID, UINT* nStatusCode);

	// Read immediate expanded inputs
	BOOL ReadImmediateExpandedInputs(byte nNodeID, USHORT* nInputsStatus);

	// Read driver board inputs
	BOOL ReadDriverBoardInputs(byte nNodeID, USHORT* nInputsStatus);

	// Read encoder position
	BOOL ReadEncoderPosition(byte nNodeID, int* nEncoderPosition);

	// Read immediate absolute position
	BOOL ReadImmediateAbsolutePosition(byte nNodeID, int* nImmediateAbsolutePosition);

	// Read immediate actual velocity
	BOOL ReadImmediateActualVelocity(byte nNodeID, double* dImmediateActualVelocity);

	// Read immediate target velocity
	BOOL ReadImmediateTargetVelocity(byte nNodeID, double* dImmediateTargetVelocity);

	// Read immediate drive temperature
	BOOL ReadImmediateDriveTemperature(byte nNodeID, double* dImmediateDriveTemperature);

	// Read immediate bus voltage
	BOOL ReadImmediateBusVoltage(byte nNodeID, double* dImmediateBusVoltage);

	// Read immediate position error
	BOOL ReadImmediatePositionError(byte nNodeID, int* nImmediatePositionError);

	// Read immediate analog input value
	BOOL ReadImmediateAnalogInputValue(byte nNodeID, short* nImmediateAnalogInputValue);

	// Read immediate analog input1 Value
	BOOL ReadImmediateAnalogInput1Value(byte nNodeID, short* nImmediateAnalogInputValue);

	// Read immediate analog input2 Value
	BOOL ReadImmediateAnalogInput2Value(byte nNodeID, short* nImmediateAnalogInputValue);

	// Read Q program line number
	BOOL ReadQProgramLineNumber(byte nNodeID, USHORT* nQProgramLineNumber);

	// Read immediate current command
	BOOL ReadImmediateCurrentCommand(byte nNodeID, short* nImmediateCurrentCommand);

	// Read relative distance
	BOOL ReadRelativeDistance(byte nNodeID, int* nRelativeDistance);

	// Read sensor position
	BOOL ReadSensorPosition(byte nNodeID, int* nSensorPosition);

	// Read condition code
	BOOL ReadConditionCode(byte nNodeID, USHORT* nConditionCode);

	// Read command mode
	BOOL ReadCommandMode(byte nNodeID, USHORT* nCommandMode);

	// Read distance or position
	BOOL ReadDistanceOrPosition(byte nNodeID, int* nDistanceOrPosition);

	// Write distance or position
	BOOL WriteDistanceOrPosition(byte nNodeID, int nDistanceOrPosition);

	// Read change distance
	BOOL ReadChangeDistance(byte nNodeID, int* nChangeDistance);

	// Write change distance
	BOOL WriteChangeDistance(byte nNodeID, int nChangeDistance);

	// Read change velocity
	BOOL ReadChangeVelocity(byte nNodeID, double* dChangeVelocity);

	// Write change velocity
	BOOL WriteChangeVelocity(byte nNodeID, double dChangeVelocity);

	// Read velocity move state
	BOOL ReadVelocityMoveState(byte nNodeID, USHORT* nVelocityMoveState);

	// Read P2P move state
	BOOL ReadP2PMoveState(byte nNodeID, USHORT* nP2PMoveState);

	// Read Q program segment number
	BOOL ReadQProgramSegmentNumber(byte nNodeID, USHORT* nQProgramSegmentNumber);

	// Read position offset
	BOOL ReadPositionOffset(byte nNodeID, int* nPositionOffset);

	// Write position offset
	BOOL WritePositionOffset(byte nNodeID, int nPositionOffset);

	// Read running current
	BOOL ReadRunningCurrent(byte nNodeID, double* dRunningCurrent);

	// Write running current
	BOOL WriteRunningCurrent(byte nNodeID, double dRunningCurrent);

	// Read electronic gearing
	BOOL ReadElectronicGearing(byte nNodeID, USHORT* nElectronicGearing);

	// Write electronic gearing
	BOOL WriteElectronicGearing(byte nNodeID, USHORT nElectronicGearing);

	// Read pulse counter
	BOOL ReadPulseCounter(byte nNodeID, int* nPulseCounter);

	// Write pulse counter
	BOOL WritePulseCounter(byte nNodeID, int nPulseCounter);

	// Read analog position gain
	BOOL ReadAnalogPositionGain(byte nNodeID, USHORT* nAnalogPositionGain);

	// Write analog position gain
	BOOL WriteAnalogPositionGain(byte nNodeID, USHORT nAnalogPositionGain);

	// Read analog threshold
	BOOL ReadAnalogThreshold(byte nNodeID, USHORT* nAnalogThreshold);

	// Write analog threshold
	BOOL WriteAnalogThreshold(byte nNodeID, USHORT nAnalogThreshold);

	// Read analogoffset
	BOOL ReadAnalogOffset(byte nNodeID, USHORT* nAnalogOffset);

	// Write analog offset
	BOOL WriteAnalogOffset(byte nNodeID, USHORT nAnalogOffset);

	// Read accumulator
	BOOL ReadAccumulator(byte nNodeID, int* nAccumulator);

	// Read user defined register
	BOOL ReadUserDefinedRegister(byte nNodeID, char chRegister, int* nValue);

	// Write user defined register
	BOOL WriteUserDefinedRegister(byte nNodeID, byte chRegister, int nValue);

	// Read brake release delay
	BOOL ReadBrakeReleaseDelay(byte nNodeID, double* dBrakeReleaseDelay);

	// Write brake release delay
	BOOL WriteBrakeReleaseDelay(byte nNodeID, double dBrakeReleaseDelay);

	// Read brake engage delay
	BOOL ReadBrakeEngageDelay(byte nNodeID, double* dBrakeEngageDelay);

	// Write brake engage delay
	BOOL WriteBrakeEngageDelay(byte nNodeID, double dBrakeEngageDelay);

	// Read analog filter gain
	BOOL ReadAnalogFilterGain(byte nNodeID, USHORT* nAnalogFilterGain);

	// Write analog filter gain
	BOOL WriteAnalogFilterGain(byte nNodeID, USHORT nAnalogFilterGain);

	// Read alarm code
	BOOL ReadAlarmCode_M3(byte nNodeID, UINT* nAlarmCode);

	// Read status code
	BOOL ReadStatusCode_M3(byte nNodeID, UINT* nStatusCode);

	// Read driver inputs
	BOOL ReadDriverBoardOutputs_M3(byte nNodeID, USHORT* nInputsStatus);

	// Read driver oututs
	BOOL ReadDriverBoardInputs_M3(byte nNodeID, USHORT* nInputsStatus);

	// Read encoder position
	BOOL ReadEncoderPosition_M3(byte nNodeID, int* nEncoderPosition);

	// Read encoder position
	BOOL Read2ndEncoderPosition_M3(byte nNodeID, int* nEncoderPosition);

	// Read immediate absolute position
	BOOL ReadImmediateAbsolutePosition_M3(byte nNodeID, int* nImmediateAbsolutePosition);

	// Read immediate actual velocity
	BOOL ReadImmediateActualVelocity_M3(byte nNodeID, double* dImmediateActualVelocity);

	// Read immediate target velocity
	BOOL ReadImmediateTargetVelocity_M3(byte nNodeID, double* dImmediateTargetVelocity);

	// Read immediate drive temperature
	BOOL ReadImmediateDriveTemperature_M3(byte nNodeID, double* dImmediateDriveTemperature);

	// Read immediate bus voltage
	BOOL ReadImmediateBusVoltage_M3(byte nNodeID, double* dImmediateBusVoltage);

	// Read immediate position error
	BOOL ReadImmediatePositionError_M3(byte nNodeID, int* nImmediatePositionError);

	// Read immediate analog input1 Value
	BOOL ReadImmediateAnalogInput1Value_M3(byte nNodeID, short* dImmediateAnalogInputValue);

	// Read immediate analog input2 Value
	BOOL ReadImmediateAnalogInput2Value_M3(byte nNodeID, short* dImmediateAnalogInputValue);

	// Read immediate analog output 1
	BOOL ReadImmediateAnalogOutput1Value_M3(byte nNodeID, short* nImmediateAnalogOutputValue);

	// Write immediate analog output 1
	BOOL WriteImmediateAnalogOutput1Value_M3(byte nNodeID, short nImmediateAnalogOutputValue);

	// Read immediate analog output 2
	BOOL ReadImmediateAnalogOutput2Value_M3(byte nNodeID, short* dImmediateAnalogOutputValue);

	// Write immediate analog output 2
	BOOL WriteImmediateAnalogOutput2Value_M3(byte nNodeID, short nImmediateAnalogOutputValue);

	// Read Q program line number
	BOOL ReadQProgramLineNumber_M3(byte nNodeID, USHORT* nQProgramLineNumber);

	// Read immediate current command
	BOOL ReadImmediateCurrentCommand_M3(byte nNodeID, short* nImmediateCurrentCommand);

	// Read relative distance
	BOOL ReadRelativeDistance_M3(byte nNodeID, int* nRelativeDistance);

	// Read sensor position
	BOOL ReadSensorPosition_M3(byte nNodeID, int* nSensorPosition);

	// Read condition code
	BOOL ReadConditionCode_M3(byte nNodeID, USHORT* nConditionCode);

	// Read command mode
	BOOL ReadCommandMode_M3(byte nNodeID, USHORT* nCommandMode);

	// Read distance or position
	BOOL ReadDistanceOrPosition_M3(byte nNodeID, int* nDistanceOrPosition);

	// Write distance or position
	BOOL WriteDistanceOrPosition_M3(byte nNodeID, int nDistanceOrPosition);

	// Read change distance
	BOOL ReadChangeDistance_M3(byte nNodeID, int* nChangeDistance);

	// Write change distance
	BOOL WriteChangeDistance_M3(byte nNodeID, int nChangeDistance);

	// Read change velocity
	BOOL ReadChangeVelocity_M3(byte nNodeID, double* dChangeVelocity);

	// Write change velocity
	BOOL WriteChangeVelocity_M3(byte nNodeID, double dChangeVelocity);

	// Read velocity move state
	BOOL ReadVelocityMoveState_M3(byte nNodeID, USHORT* nVelocityMoveState);

	// Read P2P move state
	BOOL ReadP2PMoveState_M3(byte nNodeID, USHORT* nP2PMoveState);

	// Read Q program segment number
	BOOL ReadQProgramSegmentNumber_M3(byte nNodeID, USHORT* nQProgramSegmentNumber);

	// Read Command current
	BOOL ReadCommandCurrent_M3(byte nNodeID, double* dCommandCurrent);

	// Write Command current
	BOOL WriteCommandCurrent_M3(byte nNodeID, double dCommandCurrent);

	// Read Max current
	BOOL ReadMaximumCurrent_M3(byte nNodeID, double* dMaximumCurrent);

	// Write Max current
	BOOL WriteMaximumCurrent_M3(byte nNodeID, double dMaximumCurrent);

	// Read electronic gearing
	BOOL ReadElectronicGearing_M3(byte nNodeID, UINT* nElectronicGearing);

	// Write electronic gearing
	BOOL WriteElectronicGearing_M3(byte nNodeID, UINT nElectronicGearing);

	// Read pulse counter
	BOOL ReadPulseCounter_M3(byte nNodeID, int* nPulseCounter);

	// Write pulse counter
	BOOL WritePulseCounter_M3(byte nNodeID, int nPulseCounter);

	// Read analog position gain
	BOOL ReadAnalogPositionGain_M3(byte nNodeID, int* nAnalogPositionGain);

	// Write analog position gain
	BOOL WriteAnalogPositionGain_M3(byte nNodeID, int nAnalogPositionGain);

	// Read analog velocity gain
	BOOL ReadAnalogVelocityGain_M3(byte nNodeID, double* dAnalogVelocityGain);

	// Write analog velocity gain
	BOOL WriteAnalogVelocityGain_M3(byte nNodeID, double dAnalogVelocityGain);

	// Read analog torque gain
	BOOL ReadAnalogTorqueGain_M3(byte nNodeID, double* dAnalogTorqueGain);

	// Write analog torque gain
	BOOL WriteAnalogTorqueGain_M3(byte nNodeID, double dAnalogTorqueGain);

	// Read analog threshold 1
	BOOL ReadAnalogThreshold1_M3(byte nNodeID, int* nAnalogThreshold);

	// Write analog threshold 1
	BOOL WriteAnalogThreshold1_M3(byte nNodeID, int nAnalogThreshold);

	// Read analog threshold 2
	BOOL ReadAnalogThreshold2_M3(byte nNodeID, int* nAnalogThreshold);

	// Write analog threshold 2
	BOOL WriteAnalogThreshold2_M3(byte nNodeID, int nAnalogThreshold);

	// Read analog offset 1
	BOOL ReadAnalogOffset1_M3(byte nNodeID, int* nAnalogOffset);

	// Write analog offset 1
	BOOL WriteAnalogOffset1_M3(byte nNodeID, int nAnalogOffset);

	// Read analog offset 2
	BOOL ReadAnalogOffset2_M3(byte nNodeID, int* nAnalogOffset);

	// Write analog offset 2
	BOOL WriteAnalogOffset2_M3(byte nNodeID, int nAnalogOffset);

	// Read accumulator
	BOOL ReadAccumulator_M3(byte nNodeID, int* nAccumulator);

	// Read user defined register
	BOOL ReadUserDefinedRegister_M3(byte nNodeID, char chRegister, int* nValue);

	// Write user defined register
	BOOL WriteUserDefinedRegister_M3(byte nNodeID, byte chRegister, int nValue);

	// Read brake release delay
	BOOL ReadBrakeReleaseDelay_M3(byte nNodeID, double* dBrakeReleaseDelay);

	// Write brake release delay
	BOOL WriteBrakeReleaseDelay_M3(byte nNodeID, double dBrakeReleaseDelay);

	// Read brake engage delay
	BOOL ReadBrakeEngageDelay_M3(byte nNodeID, double* dBrakeEngageDelay);

	// Write brake engage delay
	BOOL WriteBrakeEngageDelay_M3(byte nNodeID, double dBrakeEngageDelay);

	// Read analog filter gain
	BOOL ReadAnalogFilterGain1_M3(byte nNodeID, USHORT* nAnalogFilterGain);

	// Write analog filter gain
	BOOL WriteAnalogFilterGain1_M3(byte nNodeID, USHORT nAnalogFilterGain);

	// Read analog filter gain
	BOOL ReadAnalogFilterGain2_M3(byte nNodeID, USHORT* nAnalogFilterGain);

	// Write analog filter gain
	BOOL WriteAnalogFilterGain2_M3(byte nNodeID, USHORT nAnalogFilterGain);

	// Write broadcast command
	BOOL WriteBroadcastCommand(byte nNodeIDCount, byte* aNodeIDArr, byte nFunctionCode);

	// Execute multi-axes linear interpolation move
	BOOL ExecuteLinearInterpolationMove(byte nNodeIDCount, byte* aNodeIDArr, int* aPosArr, UINT nMaxLineSpeed, UINT nAccelTimeInMs, BOOL bRelMove, BOOL bValidParamLimit);
};

