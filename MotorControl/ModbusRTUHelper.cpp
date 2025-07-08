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

#include "StdAfx.h"
#include "ModbusRTUHelper.h"


EventCallback OnDataSend()
{
	::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_DATASEND_MESSAGE, 0, 0);
	return 0;
}

EventCallback OnDataReceive()
{
	::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_DATARECEIVED_MESSAGE, 0, 0);
	return 0;
}

ModbusRTUHelper::ModbusRTUHelper(void)
{
	m_bWasLoaded = false;

	m_hDll = LoadLibrary(_T("ModbusRTU_x86.dll"));  // for 64-bit windows, please comment this line and uncomment next line
	//m_hDll = LoadLibrary("ModbusRTU_x64.dll"); // for 32-bit windows, please comment this line and uncomment previous line

	if (m_hDll != NULL)
	{
		// Trigger when send data to drive
		m_OnDataSend = (lpOnDataSend)GetProcAddress(m_hDll, "OnDataSend");
		// Trigger when received data from drive
		m_OnDataReceive = (lpOnDataReceive)GetProcAddress(m_hDll, "OnDataReceive");
		// Get the port is open or closed
		m_IsOpen = (lpIsOpen)GetProcAddress(m_hDll, "IsOpen");
		// Open serial port communication
		m_Open = (lpOpen)GetProcAddress(m_hDll, "Open");
		// Close serial port communication
		m_Close = (lpClose)GetProcAddress(m_hDll, "Close");
		// Set endian type
		m_SetEndianType = (lpSetEndianType)GetProcAddress(m_hDll, "SetEndianType");
		// Return it is big endian type or not
		m_IsBigEndian = (lpIsBigEndian)GetProcAddress(m_hDll, "IsBigEndian");
		// Get execute time out
		m_GetExecuteTimeOut = (lpGetExecuteTimeOut)GetProcAddress(m_hDll, "GetExecuteTimeOut");
		// Set execute time out
		m_SetExecuteTimeOut = (lpSetExecuteTimeOut)GetProcAddress(m_hDll, "SetExecuteTimeOut");
		// Set it will trigger send event or not when sending command
		m_SetTriggerSendEvent = (lpSetTriggerSendEvent)GetProcAddress(m_hDll, "SetTriggerSendEvent");
		// Set it will trigger received event or not when receive data
		m_SetTriggerReceiveEvent = (lpSetTriggerReceiveEvent)GetProcAddress(m_hDll, "SetTriggerReceiveEvent");
		// Clear send buffer
		m_ClearSendBuffer = (lpClearSendBuffer)GetProcAddress(m_hDll, "ClearSendBuffer");
		// Clear received buffer
		m_ClearReceivedBuffer = (lpClearReceivedBuffer)GetProcAddress(m_hDll, "ClearReceivedBuffer");
		// Clear send and received Buffer
		m_ClearBuffer = (lpClearBuffer)GetProcAddress(m_hDll, "ClearBuffer");
		// Get last error information, includes code and description
		m_GetLastErrorInfo = (lpGetLastErrorInfo)GetProcAddress(m_hDll, "GetLastErrorInfo");
		// Get the last command that send
		m_GetLastCommandSent = (lpGetLastCommandSent)GetProcAddress(m_hDll, "GetLastCommandSent");
		// Get the last command that received
		m_GetLastCommandReceived = (lpGetLastCommandReceived)GetProcAddress(m_hDll, "GetLastCommandReceived");
		// Read single holding register from the drive
		m_ReadSingleHoldingRegister = (lpReadSingleHoldingRegister)GetProcAddress(m_hDll, "ReadSingleHoldingRegister");
		// Write single holding register value to the drive
		m_WriteSingleHoldingRegister = (lpWriteSingleHoldingRegister)GetProcAddress(m_hDll, "WriteSingleHoldingRegister");
		// Read multiple holding register from the drive
		m_ReadMultiHoldingRegisters = (lpReadMultiHoldingRegisters)GetProcAddress(m_hDll, "ReadMultiHoldingRegisters");
		// Write multiple holding register values to the drive
		m_WriteMultiHoldingRegisters = (lpWriteMultiHoldingRegisters)GetProcAddress(m_hDll, "WriteMultiHoldingRegisters");
		// Read 16-bit int data from the drive
		m_ReadDataInt16 = (lpReadDataInt16)GetProcAddress(m_hDll, "ReadDataInt16");
		// Write 16-bit int data to the drive
		m_WriteDataInt16 = (lpWriteDataInt16)GetProcAddress(m_hDll, "WriteDataInt16");
		// Read 16-bit unsighed int data from the drive
		m_ReadDataUInt16 = (lpReadDataUInt16)GetProcAddress(m_hDll, "ReadDataUInt16");
		// Write 16-bit unsigned int data to the drive
		m_WriteDataUInt16 = (lpWriteDataUInt16)GetProcAddress(m_hDll, "WriteDataUInt16");
		// Read 32-bit int data from the drive
		m_ReadDataInt32 = (lpReadDataInt32)GetProcAddress(m_hDll, "ReadDataInt32");
		// Write 32-bit int data to the drive
		m_WriteDataInt32 = (lpWriteDataInt32)GetProcAddress(m_hDll, "WriteDataInt32");
		// Read 32-bit unsighed int data from the drive
		m_ReadDataUInt32 = (lpReadDataUInt32)GetProcAddress(m_hDll, "ReadDataUInt32");
		// Write 32-bit unsigned int data to the drive
		m_WriteDataUInt32 = (lpWriteDataUInt32)GetProcAddress(m_hDll, "WriteDataUInt32");
		// Execute command with opcode
		m_ExecuteCommandWithOpcode = (lpExecuteCommandWithOpcode)GetProcAddress(m_hDll, "ExecuteCommandWithOpcode");
		// Set P2P profile arguments
		m_SetP2PProfile = (lpSetP2PProfile)GetProcAddress(m_hDll, "SetP2PProfile");
		// Set Jog profile arguments
		m_SetJogProfile = (lpSetJogProfile)GetProcAddress(m_hDll, "SetJogProfile");
		// Set the drive enabled or disabled
		m_DriveEnable = (lpDriveEnable)GetProcAddress(m_hDll, "DriveEnable");
		// Reset drive's alarm
		m_AlarmReset = (lpAlarmReset)GetProcAddress(m_hDll, "AlarmReset");
		// Launch feed to position move
		m_FeedtoPosition = (lpFeedtoPosition)GetProcAddress(m_hDll, "FeedtoPosition");
		// Launch feed to length move
		m_FeedtoLength = (lpFeedtoLength)GetProcAddress(m_hDll, "FeedtoLength");
		// Launch absolute move
		m_AbsMove = (lpAbsMove)GetProcAddress(m_hDll, "AbsMove");
		// Launch relative move
		m_RelMove = (lpRelMove)GetProcAddress(m_hDll, "RelMove");
		// Launch feed to sensor move
		m_FeedtoSensor = (lpFeedtoSensor)GetProcAddress(m_hDll, "FeedtoSensor");
		// Lanuch feed to sensor move with safety distance
		m_FeedtoSensorwithSafetyDistance = (lpFeedtoSensorwithSafetyDistance)GetProcAddress(m_hDll, "FeedtoSensorwithSafetyDistance");
		// Launch feed to double sensor move with mask distance
		m_FeedtoSensorwithMaskDistance = (lpFeedtoSensorwithMaskDistance)GetProcAddress(m_hDll, "FeedtoSensorwithMaskDistance");
		// Launch Point to Point Move and set output
		m_FeedandSetOutput = (lpFeedandSetOutput)GetProcAddress(m_hDll, "FeedandSetOutput");
		// Launch feed to double sensor move
		m_FeedtoDoubleSensor = (lpFeedtoDoubleSensor)GetProcAddress(m_hDll, "FeedtoDoubleSensor");
		// Launch follow encoder move
		m_FollowEncoder = (lpFollowEncoder)GetProcAddress(m_hDll, "FollowEncoder");
		// Commence Jogging
		m_StartJogging = (lpStartJogging)GetProcAddress(m_hDll, "StartJogging");
		// Stop Jogging
		m_StopJogging = (lpStopJogging)GetProcAddress(m_hDll, "StopJogging");
		// Set encoder function to the stepper drives with encoder feedback
		m_SetEncoderFunction = (lpSetEncoderFunction)GetProcAddress(m_hDll, "SetEncoderFunction");
		// Set encoder position
		m_SetEncoderPosition = (lpSetEncoderPosition)GetProcAddress(m_hDll, "SetEncoderPosition");
		// Jog disable
		m_JogDisable = (lpJogDisable)GetProcAddress(m_hDll, "JogDisable");
		// Jog enable
		m_JogEnable = (lpJogEnable)GetProcAddress(m_hDll, "JogEnable");
		// Launch seek home move
		m_SeekHome = (lpSeekHome)GetProcAddress(m_hDll, "SeekHome");
		// Set position
		m_SetPosition = (lpSetPosition)GetProcAddress(m_hDll, "SetPosition");
		// Set filter input
		m_SetFilterInput = (lpSetFilterInput)GetProcAddress(m_hDll, "SetFilterInput");
		// Write analog deadband
		m_WriteAnalogDeadband = (lpWriteAnalogDeadband)GetProcAddress(m_hDll, "WriteAnalogDeadband");
		// Set output of the drive
		m_SetOutput = (lpSetOutput)GetProcAddress(m_hDll, "SetOutput");
		// Write wait for input
		m_WriteWaitforInput = (lpWriteWaitforInput)GetProcAddress(m_hDll, "WriteWaitforInput");
		// Queue Load and Execute
		m_QueueLoadAndExecute = (lpQueueLoadAndExecute)GetProcAddress(m_hDll, "QueueLoadAndExecute");
		// Write aait time
		m_WriteWaitTime = (lpWriteWaitTime)GetProcAddress(m_hDll, "WriteWaitTime");
		// Stop and kill current move
		m_StopAndKill = (lpStopAndKill)GetProcAddress(m_hDll, "StopAndKill");
		// Stop and kill current move with normal deceleration
		m_StopAndKillwithNormalDecel = (lpStopAndKillwithNormalDecel)GetProcAddress(m_hDll, "StopAndKillwithNormalDecel");
		// Set P2P profile arguments
		m_SetP2PProfile_M3 = (lpSetP2PProfile_M3)GetProcAddress(m_hDll, "SetP2PProfile_M3");
		// Set Jog profile arguments
		m_SetJogProfile_M3 = (lpSetJogProfile_M3)GetProcAddress(m_hDll, "SetJogProfile_M3");
		// Set the drive enabled or disabled
		m_DriveEnable_M3 = (lpDriveEnable_M3)GetProcAddress(m_hDll, "DriveEnable_M3");
		// Reset drive's alarm
		m_AlarmReset_M3 = (lpAlarmReset_M3)GetProcAddress(m_hDll, "AlarmReset_M3");
		// Launch feed to position move
		m_FeedtoPosition_M3 = (lpFeedtoPosition_M3)GetProcAddress(m_hDll, "FeedtoPosition_M3");
		// Launch feed to length move
		m_FeedtoLength_M3 = (lpFeedtoLength_M3)GetProcAddress(m_hDll, "FeedtoLength_M3");
		// Launch absolute move
		m_AbsMove_M3 = (lpAbsMove_M3)GetProcAddress(m_hDll, "AbsMove_M3");
		// Launch relative move
		m_RelMove_M3 = (lpRelMove_M3)GetProcAddress(m_hDll, "RelMove_M3");
		// Set Jog profile arguments
		m_SetHomeProfile = (lpSetHomeProfile)GetProcAddress(m_hDll, "SetHomeProfile");
		// Set Jog profile arguments
		m_SetHomeMethod = (lpSetHomeMethod)GetProcAddress(m_hDll, "SetHomeMethod");
		// Launch seek home move
		m_FeedHome = (lpFeedHome)GetProcAddress(m_hDll, "FeedHome");
		// Read alarm code
		m_ReadAlarmCode = (lpReadAlarmCode)GetProcAddress(m_hDll, "ReadAlarmCode");
		// Read status code
		m_ReadStatusCode = (lpReadStatusCode)GetProcAddress(m_hDll, "ReadStatusCode");
		// Read immediate expanded inputs
		m_ReadImmediateExpandedInputs = (lpReadImmediateExpandedInputs)GetProcAddress(m_hDll, "ReadImmediateExpandedInputs");
		// Read driver board inputs
		m_ReadDriverBoardInputs = (lpReadDriverBoardInputs)GetProcAddress(m_hDll, "ReadDriverBoardInputs");
		// Read encoder position
		m_ReadEncoderPosition = (lpReadEncoderPosition)GetProcAddress(m_hDll, "ReadEncoderPosition");
		// Read immediate absolute position
		m_ReadImmediateAbsolutePosition = (lpReadImmediateAbsolutePosition)GetProcAddress(m_hDll, "ReadImmediateAbsolutePosition");
		// Read immediate actual velocity
		m_ReadImmediateActualVelocity = (lpReadImmediateActualVelocity)GetProcAddress(m_hDll, "ReadImmediateActualVelocity");
		// Read immediate target velocity
		m_ReadImmediateTargetVelocity = (lpReadImmediateTargetVelocity)GetProcAddress(m_hDll, "ReadImmediateTargetVelocity");
		// Read immediate drive temperature
		m_ReadImmediateDriveTemperature = (lpReadImmediateDriveTemperature)GetProcAddress(m_hDll, "ReadImmediateDriveTemperature");
		// Read immediate bus voltage
		m_ReadImmediateBusVoltage = (lpReadImmediateBusVoltage)GetProcAddress(m_hDll, "ReadImmediateBusVoltage");
		// Read immediate position error
		m_ReadImmediatePositionError = (lpReadImmediatePositionError)GetProcAddress(m_hDll, "ReadImmediatePositionError");
		// Read immediate analog input value
		m_ReadImmediateAnalogInputValue = (lpReadImmediateAnalogInputValue)GetProcAddress(m_hDll, "ReadImmediateAnalogInputValue");
		// Read immediate analog input1 Value
		m_ReadImmediateAnalogInput1Value = (lpReadImmediateAnalogInput1Value)GetProcAddress(m_hDll, "ReadImmediateAnalogInput1Value");
		// Read immediate analog input2 Value
		m_ReadImmediateAnalogInput2Value = (lpReadImmediateAnalogInput2Value)GetProcAddress(m_hDll, "ReadImmediateAnalogInput2Value");
		// Read Q program line number
		m_ReadQProgramLineNumber = (lpReadQProgramLineNumber)GetProcAddress(m_hDll, "ReadQProgramLineNumber");
		// Read immediate current command
		m_ReadImmediateCurrentCommand = (lpReadImmediateCurrentCommand)GetProcAddress(m_hDll, "ReadImmediateCurrentCommand");
		// Read relative distance
		m_ReadRelativeDistance = (lpReadRelativeDistance)GetProcAddress(m_hDll, "ReadRelativeDistance");
		// Read sensor position
		m_ReadSensorPosition = (lpReadSensorPosition)GetProcAddress(m_hDll, "ReadSensorPosition");
		// Read condition code
		m_ReadConditionCode = (lpReadConditionCode)GetProcAddress(m_hDll, "ReadConditionCode");
		// Read command mode
		m_ReadCommandMode = (lpReadCommandMode)GetProcAddress(m_hDll, "ReadCommandMode");
		// Read distance or position
		m_ReadDistanceOrPosition = (lpReadDistanceOrPosition)GetProcAddress(m_hDll, "ReadDistanceOrPosition");
		// Write distance or position
		m_WriteDistanceOrPosition = (lpWriteDistanceOrPosition)GetProcAddress(m_hDll, "WriteDistanceOrPosition");
		// Read change distance
		m_ReadChangeDistance = (lpReadChangeDistance)GetProcAddress(m_hDll, "ReadChangeDistance");
		// Write change distance
		m_WriteChangeDistance = (lpWriteChangeDistance)GetProcAddress(m_hDll, "WriteChangeDistance");
		// Read change velocity
		m_ReadChangeVelocity = (lpReadChangeVelocity)GetProcAddress(m_hDll, "ReadChangeVelocity");
		// Write change velocity
		m_WriteChangeVelocity = (lpWriteChangeVelocity)GetProcAddress(m_hDll, "WriteChangeVelocity");
		// Read velocity move state
		m_ReadVelocityMoveState = (lpReadVelocityMoveState)GetProcAddress(m_hDll, "ReadVelocityMoveState");
		// Read P2P move state
		m_ReadP2PMoveState = (lpReadP2PMoveState)GetProcAddress(m_hDll, "ReadP2PMoveState");
		// Read Q program segment number
		m_ReadQProgramSegmentNumber = (lpReadQProgramSegmentNumber)GetProcAddress(m_hDll, "ReadQProgramSegmentNumber");
		// Read position offset
		m_ReadPositionOffset = (lpReadPositionOffset)GetProcAddress(m_hDll, "ReadPositionOffset");
		// Write position offset
		m_WritePositionOffset = (lpWritePositionOffset)GetProcAddress(m_hDll, "WritePositionOffset");
		// Read running current
		m_ReadRunningCurrent = (lpReadRunningCurrent)GetProcAddress(m_hDll, "ReadRunningCurrent");
		// Write running current
		m_WriteRunningCurrent = (lpWriteRunningCurrent)GetProcAddress(m_hDll, "WriteRunningCurrent");
		// Read electronic gearing
		m_ReadElectronicGearing = (lpReadElectronicGearing)GetProcAddress(m_hDll, "ReadElectronicGearing");
		// Write electronic gearing
		m_WriteElectronicGearing = (lpWriteElectronicGearing)GetProcAddress(m_hDll, "WriteElectronicGearing");
		// Read pulse counter
		m_ReadPulseCounter = (lpReadPulseCounter)GetProcAddress(m_hDll, "ReadPulseCounter");
		// Write pulse counter
		m_WritePulseCounter = (lpWritePulseCounter)GetProcAddress(m_hDll, "WritePulseCounter");
		// Read analog position gain
		m_ReadAnalogPositionGain = (lpReadAnalogPositionGain)GetProcAddress(m_hDll, "ReadAnalogPositionGain");
		// Write analog position gain
		m_WriteAnalogPositionGain = (lpWriteAnalogPositionGain)GetProcAddress(m_hDll, "WriteAnalogPositionGain");
		// Read analog threshold
		m_ReadAnalogThreshold = (lpReadAnalogThreshold)GetProcAddress(m_hDll, "ReadAnalogThreshold");
		// Write analog threshold
		m_WriteAnalogThreshold = (lpWriteAnalogThreshold)GetProcAddress(m_hDll, "WriteAnalogThreshold");
		// Read analogoffset
		m_ReadAnalogOffset = (lpReadAnalogOffset)GetProcAddress(m_hDll, "ReadAnalogOffset");
		// Write analog offset
		m_WriteAnalogOffset = (lpWriteAnalogOffset)GetProcAddress(m_hDll, "WriteAnalogOffset");
		// Read accumulator
		m_ReadAccumulator = (lpReadAccumulator)GetProcAddress(m_hDll, "ReadAccumulator");
		// Read user defined register
		m_ReadUserDefinedRegister = (lpReadUserDefinedRegister)GetProcAddress(m_hDll, "ReadUserDefinedRegister");
		// Write user defined register
		m_WriteUserDefinedRegister = (lpWriteUserDefinedRegister)GetProcAddress(m_hDll, "WriteUserDefinedRegister");
		// Read brake release delay
		m_ReadBrakeReleaseDelay = (lpReadBrakeReleaseDelay)GetProcAddress(m_hDll, "ReadBrakeReleaseDelay");
		// Write brake release delay
		m_WriteBrakeReleaseDelay = (lpWriteBrakeReleaseDelay)GetProcAddress(m_hDll, "WriteBrakeReleaseDelay");
		// Read brake engage delay
		m_ReadBrakeEngageDelay = (lpReadBrakeEngageDelay)GetProcAddress(m_hDll, "ReadBrakeEngageDelay");
		// Write brake engage delay
		m_WriteBrakeEngageDelay = (lpWriteBrakeEngageDelay)GetProcAddress(m_hDll, "WriteBrakeEngageDelay");
		// Read analog filter gain
		m_ReadAnalogFilterGain = (lpReadAnalogFilterGain)GetProcAddress(m_hDll, "ReadAnalogFilterGain");
		// Write analog filter gain
		m_WriteAnalogFilterGain = (lpWriteAnalogFilterGain)GetProcAddress(m_hDll, "WriteAnalogFilterGain");
		// Read alarm code
		m_ReadAlarmCode_M3 = (lpReadAlarmCode_M3)GetProcAddress(m_hDll, "ReadAlarmCode_M3");
		// Read status code
		m_ReadStatusCode_M3 = (lpReadStatusCode_M3)GetProcAddress(m_hDll, "ReadStatusCode_M3");
		// Read driver inputs
		m_ReadDriverBoardOutputs_M3 = (lpReadDriverBoardOutputs_M3)GetProcAddress(m_hDll, "ReadDriverBoardOutputs_M3");
		// Read driver oututs
		m_ReadDriverBoardInputs_M3 = (lpReadDriverBoardInputs_M3)GetProcAddress(m_hDll, "ReadDriverBoardInputs_M3");
		// Read encoder position
		m_ReadEncoderPosition_M3 = (lpReadEncoderPosition_M3)GetProcAddress(m_hDll, "ReadEncoderPosition_M3");
		// Read encoder position
		m_Read2ndEncoderPosition_M3 = (lpRead2ndEncoderPosition_M3)GetProcAddress(m_hDll, "Read2ndEncoderPosition_M3");
		// Read immediate absolute position
		m_ReadImmediateAbsolutePosition_M3 = (lpReadImmediateAbsolutePosition_M3)GetProcAddress(m_hDll, "ReadImmediateAbsolutePosition_M3");
		// Read immediate actual velocity
		m_ReadImmediateActualVelocity_M3 = (lpReadImmediateActualVelocity_M3)GetProcAddress(m_hDll, "ReadImmediateActualVelocity_M3");
		// Read immediate target velocity
		m_ReadImmediateTargetVelocity_M3 = (lpReadImmediateTargetVelocity_M3)GetProcAddress(m_hDll, "ReadImmediateTargetVelocity_M3");
		// Read immediate drive temperature
		m_ReadImmediateDriveTemperature_M3 = (lpReadImmediateDriveTemperature_M3)GetProcAddress(m_hDll, "ReadImmediateDriveTemperature_M3");
		// Read immediate bus voltage
		m_ReadImmediateBusVoltage_M3 = (lpReadImmediateBusVoltage_M3)GetProcAddress(m_hDll, "ReadImmediateBusVoltage_M3");
		// Read immediate position error
		m_ReadImmediatePositionError_M3 = (lpReadImmediatePositionError_M3)GetProcAddress(m_hDll, "ReadImmediatePositionError_M3");
		// Read immediate analog input1 Value
		m_ReadImmediateAnalogInput1Value_M3 = (lpReadImmediateAnalogInput1Value_M3)GetProcAddress(m_hDll, "ReadImmediateAnalogInput1Value_M3");
		// Read immediate analog input2 Value
		m_ReadImmediateAnalogInput2Value_M3 = (lpReadImmediateAnalogInput2Value_M3)GetProcAddress(m_hDll, "ReadImmediateAnalogInput2Value_M3");
		// Read immediate analog output 1
		m_ReadImmediateAnalogOutput1Value_M3 = (lpReadImmediateAnalogOutput1Value_M3)GetProcAddress(m_hDll, "ReadImmediateAnalogOutput1Value_M3");
		// Write immediate analog output 1
		m_WriteImmediateAnalogOutput1Value_M3 = (lpWriteImmediateAnalogOutput1Value_M3)GetProcAddress(m_hDll, "WriteImmediateAnalogOutput1Value_M3");
		// Read immediate analog output 2
		m_ReadImmediateAnalogOutput2Value_M3 = (lpReadImmediateAnalogOutput2Value_M3)GetProcAddress(m_hDll, "ReadImmediateAnalogOutput2Value_M3");
		// Write immediate analog output 2
		m_WriteImmediateAnalogOutput2Value_M3 = (lpWriteImmediateAnalogOutput2Value_M3)GetProcAddress(m_hDll, "WriteImmediateAnalogOutput2Value_M3");
		// Read Q program line number
		m_ReadQProgramLineNumber_M3 = (lpReadQProgramLineNumber_M3)GetProcAddress(m_hDll, "ReadQProgramLineNumber_M3");
		// Read immediate current command
		m_ReadImmediateCurrentCommand_M3 = (lpReadImmediateCurrentCommand_M3)GetProcAddress(m_hDll, "ReadImmediateCurrentCommand_M3");
		// Read relative distance
		m_ReadRelativeDistance_M3 = (lpReadRelativeDistance_M3)GetProcAddress(m_hDll, "ReadRelativeDistance_M3");
		// Read sensor position
		m_ReadSensorPosition_M3 = (lpReadSensorPosition_M3)GetProcAddress(m_hDll, "ReadSensorPosition_M3");
		// Read condition code
		m_ReadConditionCode_M3 = (lpReadConditionCode_M3)GetProcAddress(m_hDll, "ReadConditionCode_M3");
		// Read command mode
		m_ReadCommandMode_M3 = (lpReadCommandMode_M3)GetProcAddress(m_hDll, "ReadCommandMode_M3");
		// Read distance or position
		m_ReadDistanceOrPosition_M3 = (lpReadDistanceOrPosition_M3)GetProcAddress(m_hDll, "ReadDistanceOrPosition_M3");
		// Write distance or position
		m_WriteDistanceOrPosition_M3 = (lpWriteDistanceOrPosition_M3)GetProcAddress(m_hDll, "WriteDistanceOrPosition_M3");
		// Read change distance
		m_ReadChangeDistance_M3 = (lpReadChangeDistance_M3)GetProcAddress(m_hDll, "ReadChangeDistance_M3");
		// Write change distance
		m_WriteChangeDistance_M3 = (lpWriteChangeDistance_M3)GetProcAddress(m_hDll, "WriteChangeDistance_M3");
		// Read change velocity
		m_ReadChangeVelocity_M3 = (lpReadChangeVelocity_M3)GetProcAddress(m_hDll, "ReadChangeVelocity_M3");
		// Write change velocity
		m_WriteChangeVelocity_M3 = (lpWriteChangeVelocity_M3)GetProcAddress(m_hDll, "WriteChangeVelocity_M3");
		// Read velocity move state
		m_ReadVelocityMoveState_M3 = (lpReadVelocityMoveState_M3)GetProcAddress(m_hDll, "ReadVelocityMoveState_M3");
		// Read P2P move state
		m_ReadP2PMoveState_M3 = (lpReadP2PMoveState_M3)GetProcAddress(m_hDll, "ReadP2PMoveState_M3");
		// Read Q program segment number
		m_ReadQProgramSegmentNumber_M3 = (lpReadQProgramSegmentNumber_M3)GetProcAddress(m_hDll, "ReadQProgramSegmentNumber_M3");
		// Read Command current
		m_ReadCommandCurrent_M3 = (lpReadCommandCurrent_M3)GetProcAddress(m_hDll, "ReadCommandCurrent_M3");
		// Write Command current
		m_WriteCommandCurrent_M3 = (lpWriteCommandCurrent_M3)GetProcAddress(m_hDll, "WriteCommandCurrent_M3");
		// Read Max current
		m_ReadMaximumCurrent_M3 = (lpReadMaximumCurrent_M3)GetProcAddress(m_hDll, "ReadMaximumCurrent_M3");
		// Write Max current
		m_WriteMaximumCurrent_M3 = (lpWriteMaximumCurrent_M3)GetProcAddress(m_hDll, "WriteMaximumCurrent_M3");
		// Read electronic gearing
		m_ReadElectronicGearing_M3 = (lpReadElectronicGearing_M3)GetProcAddress(m_hDll, "ReadElectronicGearing_M3");
		// Write electronic gearing
		m_WriteElectronicGearing_M3 = (lpWriteElectronicGearing_M3)GetProcAddress(m_hDll, "WriteElectronicGearing_M3");
		// Read pulse counter
		m_ReadPulseCounter_M3 = (lpReadPulseCounter_M3)GetProcAddress(m_hDll, "ReadPulseCounter_M3");
		// Write pulse counter
		m_WritePulseCounter_M3 = (lpWritePulseCounter_M3)GetProcAddress(m_hDll, "WritePulseCounter_M3");
		// Read analog position gain
		m_ReadAnalogPositionGain_M3 = (lpReadAnalogPositionGain_M3)GetProcAddress(m_hDll, "ReadAnalogPositionGain_M3");
		// Write analog position gain
		m_WriteAnalogPositionGain_M3 = (lpWriteAnalogPositionGain_M3)GetProcAddress(m_hDll, "WriteAnalogPositionGain_M3");
		// Read analog velocity gain
		m_ReadAnalogVelocityGain_M3 = (lpReadAnalogVelocityGain_M3)GetProcAddress(m_hDll, "ReadAnalogVelocityGain_M3");
		// Write analog velocity gain
		m_WriteAnalogVelocityGain_M3 = (lpWriteAnalogVelocityGain_M3)GetProcAddress(m_hDll, "WriteAnalogVelocityGain_M3");
		// Read analog torque gain
		m_ReadAnalogTorqueGain_M3 = (lpReadAnalogTorqueGain_M3)GetProcAddress(m_hDll, "ReadAnalogTorqueGain_M3");
		// Write analog torque gain
		m_WriteAnalogTorqueGain_M3 = (lpWriteAnalogTorqueGain_M3)GetProcAddress(m_hDll, "WriteAnalogTorqueGain_M3");
		// Read analog threshold 1
		m_ReadAnalogThreshold1_M3 = (lpReadAnalogThreshold1_M3)GetProcAddress(m_hDll, "ReadAnalogThreshold1_M3");
		// Write analog threshold 1
		m_WriteAnalogThreshold1_M3 = (lpWriteAnalogThreshold1_M3)GetProcAddress(m_hDll, "WriteAnalogThreshold1_M3");
		// Read analog threshold 2
		m_ReadAnalogThreshold2_M3 = (lpReadAnalogThreshold2_M3)GetProcAddress(m_hDll, "ReadAnalogThreshold2_M3");
		// Write analog threshold 2
		m_WriteAnalogThreshold2_M3 = (lpWriteAnalogThreshold2_M3)GetProcAddress(m_hDll, "WriteAnalogThreshold2_M3");
		// Read analog offset 1
		m_ReadAnalogOffset1_M3 = (lpReadAnalogOffset1_M3)GetProcAddress(m_hDll, "ReadAnalogOffset1_M3");
		// Write analog offset 1
		m_WriteAnalogOffset1_M3 = (lpWriteAnalogOffset1_M3)GetProcAddress(m_hDll, "WriteAnalogOffset1_M3");
		// Read analog offset 2
		m_ReadAnalogOffset2_M3 = (lpReadAnalogOffset2_M3)GetProcAddress(m_hDll, "ReadAnalogOffset2_M3");
		// Write analog offset 2
		m_WriteAnalogOffset2_M3 = (lpWriteAnalogOffset2_M3)GetProcAddress(m_hDll, "WriteAnalogOffset2_M3");
		// Read accumulator
		m_ReadAccumulator_M3 = (lpReadAccumulator_M3)GetProcAddress(m_hDll, "ReadAccumulator_M3");
		// Read user defined register
		m_ReadUserDefinedRegister_M3 = (lpReadUserDefinedRegister_M3)GetProcAddress(m_hDll, "ReadUserDefinedRegister_M3");
		// Write user defined register
		m_WriteUserDefinedRegister_M3 = (lpWriteUserDefinedRegister_M3)GetProcAddress(m_hDll, "WriteUserDefinedRegister_M3");
		// Read brake release delay
		m_ReadBrakeReleaseDelay_M3 = (lpReadBrakeReleaseDelay_M3)GetProcAddress(m_hDll, "ReadBrakeReleaseDelay_M3");
		// Write brake release delay
		m_WriteBrakeReleaseDelay_M3 = (lpWriteBrakeReleaseDelay_M3)GetProcAddress(m_hDll, "WriteBrakeReleaseDelay_M3");
		// Read brake engage delay
		m_ReadBrakeEngageDelay_M3 = (lpReadBrakeEngageDelay_M3)GetProcAddress(m_hDll, "ReadBrakeEngageDelay_M3");
		// Write brake engage delay
		m_WriteBrakeEngageDelay_M3 = (lpWriteBrakeEngageDelay_M3)GetProcAddress(m_hDll, "WriteBrakeEngageDelay_M3");
		// Read analog filter gain
		m_ReadAnalogFilterGain1_M3 = (lpReadAnalogFilterGain1_M3)GetProcAddress(m_hDll, "ReadAnalogFilterGain1_M3");
		// Write analog filter gain
		m_WriteAnalogFilterGain1_M3 = (lpWriteAnalogFilterGain1_M3)GetProcAddress(m_hDll, "WriteAnalogFilterGain1_M3");
		// Read analog filter gain
		m_ReadAnalogFilterGain2_M3 = (lpReadAnalogFilterGain2_M3)GetProcAddress(m_hDll, "ReadAnalogFilterGain2_M3");
		// Write analog filter gain
		m_WriteAnalogFilterGain2_M3 = (lpWriteAnalogFilterGain2_M3)GetProcAddress(m_hDll, "WriteAnalogFilterGain2_M3");

		// Write broadcast command
		m_WriteBroadcastCommand = (lpWriteBroadcastCommand)GetProcAddress(m_hDll, "WriteBroadcastCommand");
		// Execute multi-axes linear interpolation move
		m_ExecuteLinearInterpolationMove = (lpExecuteLinearInterpolationMove)GetProcAddress(m_hDll, "ExecuteLinearInterpolationMove");
	}
	else
	{
		AfxMessageBox(_T("Fail to load ModbusRTU_x86.dll"));
	}
	m_bWasLoaded = true;
}


ModbusRTUHelper::~ModbusRTUHelper(void)
{
	m_bWasLoaded = false;
	if (m_hDll != NULL)
	{
		Close();
		FreeLibrary(m_hDll);
	}	
	m_hDll = NULL;
}

// Trigger when send data to drive
void ModbusRTUHelper::OnDataSend(EventCallback func)
{
	m_OnDataSend(func);
}

// Trigger when received data from drive
void ModbusRTUHelper::OnDataReceive(EventCallback func)
{
	m_OnDataReceive(func);
}

// Get the port is open or closed
BOOL ModbusRTUHelper::IsOpen()
{
	return m_IsOpen(m_nCOMPort);
}

// Open serial port communication
BOOL ModbusRTUHelper::Open(int nCOMPort, int nBaudRate, BOOL bBigEndian)
{
	m_nCOMPort = nCOMPort;
	return m_Open(m_nCOMPort, nBaudRate, bBigEndian);
}

// Close serial port communication
BOOL ModbusRTUHelper::Close()
{
	return m_Close(m_nCOMPort);
}

// Set endian type
void ModbusRTUHelper::SetEndianType(BOOL bBigEndian)
{
	m_SetEndianType(m_nCOMPort, bBigEndian);
}

// Return it is big endian type or not
BOOL ModbusRTUHelper::IsBigEndian()
{
	return m_IsBigEndian(m_nCOMPort);
}

// Get execute time out
UINT ModbusRTUHelper::GetExecuteTimeOut()
{
	return m_GetExecuteTimeOut(m_nCOMPort);
}

// Set execute time out
void ModbusRTUHelper::SetExecuteTimeOut(UINT nExecuteTimeOut)
{
	m_SetExecuteTimeOut(m_nCOMPort, nExecuteTimeOut);
}

// Set it will trigger send event or not when sending command
void ModbusRTUHelper::SetTriggerSendEvent(BOOL bTriggerSendEvent)
{
	m_SetTriggerSendEvent(bTriggerSendEvent);
}

// Set it will trigger received event or not when receive data
void ModbusRTUHelper::SetTriggerReceiveEvent(BOOL bTriggerReceiveEvent)
{
	m_SetTriggerReceiveEvent(bTriggerReceiveEvent);
}

// Clear send buffer
void ModbusRTUHelper::ClearSendBuffer()
{
	m_ClearSendBuffer(m_nCOMPort);
}

// Clear received buffer
void ModbusRTUHelper::ClearReceivedBuffer()
{
	m_ClearReceivedBuffer(m_nCOMPort);
}

// Clear send and received Buffer
void ModbusRTUHelper::ClearBuffer()
{
	m_ClearBuffer(m_nCOMPort);
}

// Get last error information, includes code and description
void ModbusRTUHelper::GetLastErrorInfo(PERROR_INFO pErrorInfo)
{
	m_GetLastErrorInfo(pErrorInfo);
}

// Get the last command that send
void ModbusRTUHelper::GetLastCommandSent(COMMAND_INFO& command)
{
	m_GetLastCommandSent(m_nCOMPort, command);
}

// Get the last command that received
void ModbusRTUHelper::GetLastCommandReceived(COMMAND_INFO& command)
{
	m_GetLastCommandReceived(m_nCOMPort, command);
}

// Read single holding register from the drive
BOOL ModbusRTUHelper::ReadSingleHoldingRegister(byte nNodeID, int nRegisterNo, int* pValue)
{
	return m_ReadSingleHoldingRegister(m_nCOMPort, nNodeID, nRegisterNo, pValue);
}

// Write single holding register value to the drive
BOOL ModbusRTUHelper::WriteSingleHoldingRegister(byte nNodeID, int nRegisterNo, int nValue)
{
	return m_WriteSingleHoldingRegister(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Read multiple holding register from the drive
int ModbusRTUHelper::ReadMultiHoldingRegisters(byte nNodeID, int nRegisterNo, int nCount, int* valueList)
{
	StructRegisterValue sRegisterValue;

	int ret = m_ReadMultiHoldingRegisters(m_nCOMPort, nNodeID, nRegisterNo, nCount, sRegisterValue);
	if (ret)
	{
		for (int i = 0; i < nCount; i++)
		{
			valueList[i] = sRegisterValue.Values[i];
		}
	}
	return ret;
}

// Write multiple holding register values to the drive
BOOL ModbusRTUHelper::WriteMultiHoldingRegisters(byte nNodeID, int nRegisterNo, int nCount, int* valueList)
{
	return m_WriteMultiHoldingRegisters(m_nCOMPort, nNodeID, nRegisterNo, nCount, valueList);
}

// Read 16-bit int data from the drive
BOOL ModbusRTUHelper::ReadDataInt16(byte nNodeID, int nRegisterNo, SHORT* nValue)
{
	return m_ReadDataInt16(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Write 16-bit int data to the drive
BOOL ModbusRTUHelper::WriteDataInt16(byte nNodeID, int nRegisterNo, SHORT nValue)
{
	return m_WriteDataInt16(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Read 16-bit unsighed int data from the drive
BOOL ModbusRTUHelper::ReadDataUInt16(byte nNodeID, int nRegisterNo, USHORT* nValue)
{
	return m_ReadDataUInt16(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Write 16-bit unsigned int data to the drive
BOOL ModbusRTUHelper::WriteDataUInt16(byte nNodeID, int nRegisterNo, USHORT nValue)
{
	return m_WriteDataUInt16(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Read 32-bit int data from the drive
BOOL ModbusRTUHelper::ReadDataInt32(byte nNodeID, int nRegisterNo, int* nValue)
{
	return m_ReadDataInt32(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Write 32-bit int data to the drive
BOOL ModbusRTUHelper::WriteDataInt32(byte nNodeID, int nRegisterNo, int nValue)
{
	return m_WriteDataInt32(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Read 32-bit unsighed int data from the drive
BOOL ModbusRTUHelper::ReadDataUInt32(byte nNodeID, int nRegisterNo, UINT* nValue)
{
	return m_ReadDataUInt32(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Write 32-bit unsigned int data to the drive
BOOL ModbusRTUHelper::WriteDataUInt32(byte nNodeID, int nRegisterNo, UINT nValue)
{
	return m_WriteDataUInt32(m_nCOMPort, nNodeID, nRegisterNo, nValue);
}

// Execute command with opcode
BOOL ModbusRTUHelper::ExecuteCommandWithOpcode(byte nNodeID, USHORT nOpCode, USHORT nParam1, USHORT nParam2, USHORT nParam3, USHORT nParam4, USHORT nParam5)
{
	return m_ExecuteCommandWithOpcode(m_nCOMPort, nNodeID, nOpCode, nParam1, nParam2, nParam3, nParam4, nParam5);
}

// Set P2P profile arguments
BOOL ModbusRTUHelper::SetP2PProfile(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_SetP2PProfile(m_nCOMPort, nNodeID, dVelocity, dAccel, dDecel);
}

// Set Jog profile arguments
BOOL ModbusRTUHelper::SetJogProfile(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_SetJogProfile(m_nCOMPort, nNodeID, dVelocity, dAccel, dDecel);
}

// Set the drive enabled or disabled
BOOL ModbusRTUHelper::DriveEnable(byte nNodeID, BOOL bEnable)
{
	return m_DriveEnable(m_nCOMPort, nNodeID, bEnable);
}

// Reset drive's alarm
BOOL ModbusRTUHelper::AlarmReset(byte nNodeID)
{
	return m_AlarmReset(m_nCOMPort, nNodeID);
}

// Launch feed to position move
BOOL ModbusRTUHelper::FeedtoPosition(byte nNodeID, int* nDistance)
{
	return m_FeedtoPosition(m_nCOMPort, nNodeID, nDistance);
}

// Launch feed to length move
BOOL ModbusRTUHelper::FeedtoLength(byte nNodeID, int* nDistance)
{
	return m_FeedtoLength(m_nCOMPort, nNodeID, nDistance);
}

// Launch absolute move
BOOL ModbusRTUHelper::AbsMove(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_AbsMove(m_nCOMPort, nNodeID, nDistance, dVelocity, dAccel, dDecel);
}

// Launch relative move
BOOL ModbusRTUHelper::RelMove(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_RelMove(m_nCOMPort, nNodeID, nDistance, dVelocity, dAccel, dDecel);
}

// Launch feed to sensor move
BOOL ModbusRTUHelper::FeedtoSensor(byte nNodeID, byte nInputSensor, char chInputStatus)
{
	return m_FeedtoSensor(m_nCOMPort, nNodeID, nInputSensor, chInputStatus);
}

// Lanuch feed to sensor move with safety distance
BOOL ModbusRTUHelper::FeedtoSensorwithSafetyDistance(byte nNodeID, byte nInputSensor, char chInputStatus)
{
	return m_FeedtoSensorwithSafetyDistance(m_nCOMPort, nNodeID, nInputSensor, chInputStatus);
}

// Launch feed to double sensor move with mask distance
BOOL ModbusRTUHelper::FeedtoSensorwithMaskDistance(byte nNodeID, byte nInputSensor, char chInputStatus)
{
	return m_FeedtoSensorwithMaskDistance(m_nCOMPort, nNodeID, nInputSensor, chInputStatus);
}

// Launch Point to Point Move and set output
BOOL ModbusRTUHelper::FeedandSetOutput(byte nNodeID, byte nOutput, char chOutputStatus)
{
	return m_FeedandSetOutput(m_nCOMPort, nNodeID, nOutput, chOutputStatus);
}

// Launch feed to double sensor move
BOOL ModbusRTUHelper::FeedtoDoubleSensor(byte nNodeID, byte nInputSensor1, char chInputStatus1, byte nInputSensor2, char chInputStatus2)
{
	return m_FeedtoDoubleSensor(m_nCOMPort, nNodeID, nInputSensor1, chInputStatus1, nInputSensor2, chInputStatus2);
}

// Launch follow encoder move
BOOL ModbusRTUHelper::FollowEncoder(byte nNodeID, byte nInputSensor, char chInputStatus)
{
	return m_FollowEncoder(m_nCOMPort, nNodeID, nInputSensor, chInputStatus);
}

// Commence Jogging
BOOL ModbusRTUHelper::StartJogging(byte nNodeID)
{
	return m_StartJogging(m_nCOMPort, nNodeID);
}

// Stop Jogging
BOOL ModbusRTUHelper::StopJogging(byte nNodeID)
{
	return m_StopJogging(m_nCOMPort, nNodeID);
}

// Set encoder function to the stepper drives with encoder feedback
BOOL ModbusRTUHelper::SetEncoderFunction(byte nNodeID, byte nFunc)
{
	return m_SetEncoderFunction(m_nCOMPort, nNodeID, nFunc);
}

// Set encoder position
BOOL ModbusRTUHelper::SetEncoderPosition(byte nNodeID, int nPosition)
{
	return m_SetEncoderPosition(m_nCOMPort, nNodeID, nPosition);
}

// Jog disable
BOOL ModbusRTUHelper::JogDisable(byte nNodeID)
{
	return m_JogDisable(m_nCOMPort, nNodeID);
}

// Jog enable
BOOL ModbusRTUHelper::JogEnable(byte nNodeID)
{
	return m_JogEnable(m_nCOMPort, nNodeID);
}

// Launch seek home move
BOOL ModbusRTUHelper::SeekHome(byte nNodeID, byte nInputSensor, char chInputStatus)
{
	return m_SeekHome(m_nCOMPort, nNodeID, nInputSensor, chInputStatus);
}

// Set position
BOOL ModbusRTUHelper::SetPosition(byte nNodeID, int nPosition)
{
	return m_SetPosition(m_nCOMPort, nNodeID, nPosition);
}

// Set filter input
BOOL ModbusRTUHelper::SetFilterInput(byte nNodeID, byte nInputSensor, int nFilterTime)
{
	return m_SetFilterInput(m_nCOMPort, nNodeID, nInputSensor, nFilterTime);
}

// Write analog deadband
BOOL ModbusRTUHelper::WriteAnalogDeadband(byte nNodeID, byte nDeadband)
{
	return m_WriteAnalogDeadband(m_nCOMPort, nNodeID, nDeadband);
}

// Set output of the drive
BOOL ModbusRTUHelper::SetOutput(byte nNodeID, byte nOutput, char nCondition)
{
	return m_SetOutput(m_nCOMPort, nNodeID, nOutput, nCondition);
}

// Write wait for input
BOOL ModbusRTUHelper::WriteWaitforInput(byte nNodeID, byte nInputSensor, char nCondition)
{
	return m_WriteWaitforInput(m_nCOMPort, nNodeID, nInputSensor, nCondition);
}

// Queue Load and Execute
BOOL ModbusRTUHelper::QueueLoadAndExecute(byte nNodeID, byte nSegment)
{
	return m_QueueLoadAndExecute(m_nCOMPort, nNodeID, nSegment);
}

// Write aait time
BOOL ModbusRTUHelper::WriteWaitTime(byte nNodeID, USHORT nTime)
{
	return m_WriteWaitTime(m_nCOMPort, nNodeID, nTime);
}

// Stop and kill current move
BOOL ModbusRTUHelper::StopAndKill(byte nNodeID)
{
	return m_StopAndKill(m_nCOMPort, nNodeID);
}

// Stop and kill current move with normal deceleration
BOOL ModbusRTUHelper::StopAndKillwithNormalDecel(byte nNodeID)
{
	return m_StopAndKillwithNormalDecel(m_nCOMPort, nNodeID);
}

// Set P2P profile arguments
BOOL ModbusRTUHelper::SetP2PProfile_M3(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_SetP2PProfile_M3(m_nCOMPort, nNodeID, dVelocity, dAccel, dDecel);
}

// Set Jog profile arguments
BOOL ModbusRTUHelper::SetJogProfile_M3(byte nNodeID, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_SetJogProfile_M3(m_nCOMPort, nNodeID, dVelocity, dAccel, dDecel);
}

// Set the drive enabled or disabled
BOOL ModbusRTUHelper::DriveEnable_M3(byte nNodeID, BOOL bEnable)
{
	return m_DriveEnable_M3(m_nCOMPort, nNodeID, bEnable);
}

// Reset drive's alarm
BOOL ModbusRTUHelper::AlarmReset_M3(byte nNodeID)
{
	return m_AlarmReset_M3(m_nCOMPort, nNodeID);
}

// Launch feed to position move
BOOL ModbusRTUHelper::FeedtoPosition_M3(byte nNodeID, int* nDistance)
{
	return m_FeedtoPosition_M3(m_nCOMPort, nNodeID, nDistance);
}

// Launch feed to length move
BOOL ModbusRTUHelper::FeedtoLength_M3(byte nNodeID, int* nDistance)
{
	return m_FeedtoLength_M3(m_nCOMPort, nNodeID, nDistance);
}

// Launch absolute move
BOOL ModbusRTUHelper::AbsMove_M3(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_AbsMove_M3(m_nCOMPort, nNodeID, nDistance, dVelocity, dAccel, dDecel);
}

// Launch relative move
BOOL ModbusRTUHelper::RelMove_M3(byte nNodeID, int nDistance, double* dVelocity, double* dAccel, double* dDecel)
{
	return m_RelMove_M3(m_nCOMPort, nNodeID, nDistance, dVelocity, dAccel, dDecel);
}

// Set Jog profile arguments
BOOL ModbusRTUHelper::SetHomeProfile(byte nNodeID, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset)
{
	return m_SetHomeProfile(m_nCOMPort, nNodeID, dVelocity1, dVelocity2, dAccel, nHomingOffset);
}

// Set Jog profile arguments
BOOL ModbusRTUHelper::SetHomeMethod(byte nNodeID, char nHomeMethod)
{
	return m_SetHomeMethod(m_nCOMPort, nNodeID, nHomeMethod);
}

// Launch seek home move
BOOL ModbusRTUHelper::FeedHome(byte nNodeID, char nHomeMethod, double* dVelocity1, double* dVelocity2, double* dAccel, int* nHomingOffset)
{
	return m_FeedHome(m_nCOMPort, nNodeID, nHomeMethod, dVelocity1, dVelocity2, dAccel, nHomingOffset);
}

// Read alarm code
BOOL ModbusRTUHelper::ReadAlarmCode(byte nNodeID, UINT* nAlarmCode)
{
	return m_ReadAlarmCode(m_nCOMPort, nNodeID, nAlarmCode);
}

// Read status code
BOOL ModbusRTUHelper::ReadStatusCode(byte nNodeID, UINT* nStatusCode)
{
	return m_ReadStatusCode(m_nCOMPort, nNodeID, nStatusCode);
}

// Read immediate expanded inputs
BOOL ModbusRTUHelper::ReadImmediateExpandedInputs(byte nNodeID, USHORT* nInputsStatus)
{
	return m_ReadImmediateExpandedInputs(m_nCOMPort, nNodeID, nInputsStatus);
}

// Read driver board inputs
BOOL ModbusRTUHelper::ReadDriverBoardInputs(byte nNodeID, USHORT* nInputsStatus)
{
	return m_ReadDriverBoardInputs(m_nCOMPort, nNodeID, nInputsStatus);
}

// Read encoder position
BOOL ModbusRTUHelper::ReadEncoderPosition(byte nNodeID, int* nEncoderPosition)
{
	return m_ReadEncoderPosition(m_nCOMPort, nNodeID, nEncoderPosition);
}

// Read immediate absolute position
BOOL ModbusRTUHelper::ReadImmediateAbsolutePosition(byte nNodeID, int* nImmediateAbsolutePosition)
{
	return m_ReadImmediateAbsolutePosition(m_nCOMPort, nNodeID, nImmediateAbsolutePosition);
}

// Read immediate actual velocity
BOOL ModbusRTUHelper::ReadImmediateActualVelocity(byte nNodeID, double* dImmediateActualVelocity)
{
	return m_ReadImmediateActualVelocity(m_nCOMPort, nNodeID, dImmediateActualVelocity);
}

// Read immediate target velocity
BOOL ModbusRTUHelper::ReadImmediateTargetVelocity(byte nNodeID, double* dImmediateTargetVelocity)
{
	return m_ReadImmediateTargetVelocity(m_nCOMPort, nNodeID, dImmediateTargetVelocity);
}

// Read immediate drive temperature
BOOL ModbusRTUHelper::ReadImmediateDriveTemperature(byte nNodeID, double* dImmediateDriveTemperature)
{
	return m_ReadImmediateDriveTemperature(m_nCOMPort, nNodeID, dImmediateDriveTemperature);
}

// Read immediate bus voltage
BOOL ModbusRTUHelper::ReadImmediateBusVoltage(byte nNodeID, double* dImmediateBusVoltage)
{
	return m_ReadImmediateBusVoltage(m_nCOMPort, nNodeID, dImmediateBusVoltage);
}

// Read immediate position error
BOOL ModbusRTUHelper::ReadImmediatePositionError(byte nNodeID, int* nImmediatePositionError)
{
	return m_ReadImmediatePositionError(m_nCOMPort, nNodeID, nImmediatePositionError);
}

// Read immediate analog input value
BOOL ModbusRTUHelper::ReadImmediateAnalogInputValue(byte nNodeID, short* nImmediateAnalogInputValue)
{
	return m_ReadImmediateAnalogInputValue(m_nCOMPort, nNodeID, nImmediateAnalogInputValue);
}

// Read immediate analog input1 Value
BOOL ModbusRTUHelper::ReadImmediateAnalogInput1Value(byte nNodeID, short* nImmediateAnalogInputValue)
{
	return m_ReadImmediateAnalogInput1Value(m_nCOMPort, nNodeID, nImmediateAnalogInputValue);
}

// Read immediate analog input2 Value
BOOL ModbusRTUHelper::ReadImmediateAnalogInput2Value(byte nNodeID, short* nImmediateAnalogInputValue)
{
	return m_ReadImmediateAnalogInput2Value(m_nCOMPort, nNodeID, nImmediateAnalogInputValue);
}

// Read Q program line number
BOOL ModbusRTUHelper::ReadQProgramLineNumber(byte nNodeID, USHORT* nQProgramLineNumber)
{
	return m_ReadQProgramLineNumber(m_nCOMPort, nNodeID, nQProgramLineNumber);
}

// Read immediate current command
BOOL ModbusRTUHelper::ReadImmediateCurrentCommand(byte nNodeID, short* nImmediateCurrentCommand)
{
	return m_ReadImmediateCurrentCommand(m_nCOMPort, nNodeID, nImmediateCurrentCommand);
}

// Read relative distance
BOOL ModbusRTUHelper::ReadRelativeDistance(byte nNodeID, int* nRelativeDistance)
{
	return m_ReadRelativeDistance(m_nCOMPort, nNodeID, nRelativeDistance);
}

// Read sensor position
BOOL ModbusRTUHelper::ReadSensorPosition(byte nNodeID, int* nSensorPosition)
{
	return m_ReadSensorPosition(m_nCOMPort, nNodeID, nSensorPosition);
}

// Read condition code
BOOL ModbusRTUHelper::ReadConditionCode(byte nNodeID, USHORT* nConditionCode)
{
	return m_ReadConditionCode(m_nCOMPort, nNodeID, nConditionCode);
}

// Read command mode
BOOL ModbusRTUHelper::ReadCommandMode(byte nNodeID, USHORT* nCommandMode)
{
	return m_ReadCommandMode(m_nCOMPort, nNodeID, nCommandMode);
}

// Read distance or position
BOOL ModbusRTUHelper::ReadDistanceOrPosition(byte nNodeID, int* nDistanceOrPosition)
{
	return m_ReadDistanceOrPosition(m_nCOMPort, nNodeID, nDistanceOrPosition);
}

// Write distance or position
BOOL ModbusRTUHelper::WriteDistanceOrPosition(byte nNodeID, int nDistanceOrPosition)
{
	return m_WriteDistanceOrPosition(m_nCOMPort, nNodeID, nDistanceOrPosition);
}

// Read change distance
BOOL ModbusRTUHelper::ReadChangeDistance(byte nNodeID, int* nChangeDistance)
{
	return m_ReadChangeDistance(m_nCOMPort, nNodeID, nChangeDistance);
}

// Write change distance
BOOL ModbusRTUHelper::WriteChangeDistance(byte nNodeID, int nChangeDistance)
{
	return m_WriteChangeDistance(m_nCOMPort, nNodeID, nChangeDistance);
}

// Read change velocity
BOOL ModbusRTUHelper::ReadChangeVelocity(byte nNodeID, double* dChangeVelocity)
{
	return m_ReadChangeVelocity(m_nCOMPort, nNodeID, dChangeVelocity);
}

// Write change velocity
BOOL ModbusRTUHelper::WriteChangeVelocity(byte nNodeID, double dChangeVelocity)
{
	return m_WriteChangeVelocity(m_nCOMPort, nNodeID, dChangeVelocity);
}

// Read velocity move state
BOOL ModbusRTUHelper::ReadVelocityMoveState(byte nNodeID, USHORT* nVelocityMoveState)
{
	return m_ReadVelocityMoveState(m_nCOMPort, nNodeID, nVelocityMoveState);
}

// Read P2P move state
BOOL ModbusRTUHelper::ReadP2PMoveState(byte nNodeID, USHORT* nP2PMoveState)
{
	return m_ReadP2PMoveState(m_nCOMPort, nNodeID, nP2PMoveState);
}

// Read Q program segment number
BOOL ModbusRTUHelper::ReadQProgramSegmentNumber(byte nNodeID, USHORT* nQProgramSegmentNumber)
{
	return m_ReadQProgramSegmentNumber(m_nCOMPort, nNodeID, nQProgramSegmentNumber);
}

// Read position offset
BOOL ModbusRTUHelper::ReadPositionOffset(byte nNodeID, int* nPositionOffset)
{
	return m_ReadPositionOffset(m_nCOMPort, nNodeID, nPositionOffset);
}

// Write position offset
BOOL ModbusRTUHelper::WritePositionOffset(byte nNodeID, int nPositionOffset)
{
	return m_WritePositionOffset(m_nCOMPort, nNodeID, nPositionOffset);
}

// Read running current
BOOL ModbusRTUHelper::ReadRunningCurrent(byte nNodeID, double* dRunningCurrent)
{
	return m_ReadRunningCurrent(m_nCOMPort, nNodeID, dRunningCurrent);
}

// Write running current
BOOL ModbusRTUHelper::WriteRunningCurrent(byte nNodeID, double dRunningCurrent)
{
	return m_WriteRunningCurrent(m_nCOMPort, nNodeID, dRunningCurrent);
}

// Read electronic gearing
BOOL ModbusRTUHelper::ReadElectronicGearing(byte nNodeID, USHORT* nElectronicGearing)
{
	return m_ReadElectronicGearing(m_nCOMPort, nNodeID, nElectronicGearing);
}

// Write electronic gearing
BOOL ModbusRTUHelper::WriteElectronicGearing(byte nNodeID, USHORT nElectronicGearing)
{
	return m_WriteElectronicGearing(m_nCOMPort, nNodeID, nElectronicGearing);
}

// Read pulse counter
BOOL ModbusRTUHelper::ReadPulseCounter(byte nNodeID, int* nPulseCounter)
{
	return m_ReadPulseCounter(m_nCOMPort, nNodeID, nPulseCounter);
}

// Write pulse counter
BOOL ModbusRTUHelper::WritePulseCounter(byte nNodeID, int nPulseCounter)
{
	return m_WritePulseCounter(m_nCOMPort, nNodeID, nPulseCounter);
}

// Read analog position gain
BOOL ModbusRTUHelper::ReadAnalogPositionGain(byte nNodeID, USHORT* nAnalogPositionGain)
{
	return m_ReadAnalogPositionGain(m_nCOMPort, nNodeID, nAnalogPositionGain);
}

// Write analog position gain
BOOL ModbusRTUHelper::WriteAnalogPositionGain(byte nNodeID, USHORT nAnalogPositionGain)
{
	return m_WriteAnalogPositionGain(m_nCOMPort, nNodeID, nAnalogPositionGain);
}

// Read analog threshold
BOOL ModbusRTUHelper::ReadAnalogThreshold(byte nNodeID, USHORT* nAnalogThreshold)
{
	return m_ReadAnalogThreshold(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Write analog threshold
BOOL ModbusRTUHelper::WriteAnalogThreshold(byte nNodeID, USHORT nAnalogThreshold)
{
	return m_WriteAnalogThreshold(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Read analogoffset
BOOL ModbusRTUHelper::ReadAnalogOffset(byte nNodeID, USHORT* nAnalogOffset)
{
	return m_ReadAnalogOffset(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Write analog offset
BOOL ModbusRTUHelper::WriteAnalogOffset(byte nNodeID, USHORT nAnalogOffset)
{
	return m_WriteAnalogOffset(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Read accumulator
BOOL ModbusRTUHelper::ReadAccumulator(byte nNodeID, int* nAccumulator)
{
	return m_ReadAccumulator(m_nCOMPort, nNodeID, nAccumulator);
}

// Read user defined register
BOOL ModbusRTUHelper::ReadUserDefinedRegister(byte nNodeID, char chRegister, int* nValue)
{
	return m_ReadUserDefinedRegister(m_nCOMPort, nNodeID, chRegister, nValue);
}

// Write user defined register
BOOL ModbusRTUHelper::WriteUserDefinedRegister(byte nNodeID, byte chRegister, int nValue)
{
	return m_WriteUserDefinedRegister(m_nCOMPort, nNodeID, chRegister, nValue);
}

// Read brake release delay
BOOL ModbusRTUHelper::ReadBrakeReleaseDelay(byte nNodeID, double* dBrakeReleaseDelay)
{
	return m_ReadBrakeReleaseDelay(m_nCOMPort, nNodeID, dBrakeReleaseDelay);
}

// Write brake release delay
BOOL ModbusRTUHelper::WriteBrakeReleaseDelay(byte nNodeID, double dBrakeReleaseDelay)
{
	return m_WriteBrakeReleaseDelay(m_nCOMPort, nNodeID, dBrakeReleaseDelay);
}

// Read brake engage delay
BOOL ModbusRTUHelper::ReadBrakeEngageDelay(byte nNodeID, double* dBrakeEngageDelay)
{
	return m_ReadBrakeEngageDelay(m_nCOMPort, nNodeID, dBrakeEngageDelay);
}

// Write brake engage delay
BOOL ModbusRTUHelper::WriteBrakeEngageDelay(byte nNodeID, double dBrakeEngageDelay)
{
	return m_WriteBrakeEngageDelay(m_nCOMPort, nNodeID, dBrakeEngageDelay);
}

// Read analog filter gain
BOOL ModbusRTUHelper::ReadAnalogFilterGain(byte nNodeID, USHORT* nAnalogFilterGain)
{
	return m_ReadAnalogFilterGain(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Write analog filter gain
BOOL ModbusRTUHelper::WriteAnalogFilterGain(byte nNodeID, USHORT nAnalogFilterGain)
{
	return m_WriteAnalogFilterGain(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Read alarm code
BOOL ModbusRTUHelper::ReadAlarmCode_M3(byte nNodeID, UINT* nAlarmCode)
{
	return m_ReadAlarmCode_M3(m_nCOMPort, nNodeID, nAlarmCode);
}

// Read status code
BOOL ModbusRTUHelper::ReadStatusCode_M3(byte nNodeID, UINT* nStatusCode)
{
	return m_ReadStatusCode_M3(m_nCOMPort, nNodeID, nStatusCode);
}

// Read driver inputs
BOOL ModbusRTUHelper::ReadDriverBoardOutputs_M3(byte nNodeID, USHORT* nInputsStatus)
{
	return m_ReadDriverBoardOutputs_M3(m_nCOMPort, nNodeID, nInputsStatus);
}

// Read driver oututs
BOOL ModbusRTUHelper::ReadDriverBoardInputs_M3(byte nNodeID, USHORT* nInputsStatus)
{
	return m_ReadDriverBoardInputs_M3(m_nCOMPort, nNodeID, nInputsStatus);
}

// Read encoder position
BOOL ModbusRTUHelper::ReadEncoderPosition_M3(byte nNodeID, int* nEncoderPosition)
{
	return m_ReadEncoderPosition_M3(m_nCOMPort, nNodeID, nEncoderPosition);
}

// Read encoder position
BOOL ModbusRTUHelper::Read2ndEncoderPosition_M3(byte nNodeID, int* nEncoderPosition)
{
	return m_Read2ndEncoderPosition_M3(m_nCOMPort, nNodeID, nEncoderPosition);
}

// Read immediate absolute position
BOOL ModbusRTUHelper::ReadImmediateAbsolutePosition_M3(byte nNodeID, int* nImmediateAbsolutePosition)
{
	return m_ReadImmediateAbsolutePosition_M3(m_nCOMPort, nNodeID, nImmediateAbsolutePosition);
}

// Read immediate actual velocity
BOOL ModbusRTUHelper::ReadImmediateActualVelocity_M3(byte nNodeID, double* dImmediateActualVelocity)
{
	return m_ReadImmediateActualVelocity_M3(m_nCOMPort, nNodeID, dImmediateActualVelocity);
}

// Read immediate target velocity
BOOL ModbusRTUHelper::ReadImmediateTargetVelocity_M3(byte nNodeID, double* dImmediateTargetVelocity)
{
	return m_ReadImmediateTargetVelocity_M3(m_nCOMPort, nNodeID, dImmediateTargetVelocity);
}

// Read immediate drive temperature
BOOL ModbusRTUHelper::ReadImmediateDriveTemperature_M3(byte nNodeID, double* dImmediateDriveTemperature)
{
	return m_ReadImmediateDriveTemperature_M3(m_nCOMPort, nNodeID, dImmediateDriveTemperature);
}

// Read immediate bus voltage
BOOL ModbusRTUHelper::ReadImmediateBusVoltage_M3(byte nNodeID, double* dImmediateBusVoltage)
{
	return m_ReadImmediateBusVoltage_M3(m_nCOMPort, nNodeID, dImmediateBusVoltage);
}

// Read immediate position error
BOOL ModbusRTUHelper::ReadImmediatePositionError_M3(byte nNodeID, int* nImmediatePositionError)
{
	return m_ReadImmediatePositionError_M3(m_nCOMPort, nNodeID, nImmediatePositionError);
}

// Read immediate analog input1 Value
BOOL ModbusRTUHelper::ReadImmediateAnalogInput1Value_M3(byte nNodeID, short* dImmediateAnalogInputValue)
{
	return m_ReadImmediateAnalogInput1Value_M3(m_nCOMPort, nNodeID, dImmediateAnalogInputValue);
}

// Read immediate analog input2 Value
BOOL ModbusRTUHelper::ReadImmediateAnalogInput2Value_M3(byte nNodeID, short* dImmediateAnalogInputValue)
{
	return m_ReadImmediateAnalogInput2Value_M3(m_nCOMPort, nNodeID, dImmediateAnalogInputValue);
}

// Read immediate analog output 1
BOOL ModbusRTUHelper::ReadImmediateAnalogOutput1Value_M3(byte nNodeID, short* nImmediateAnalogOutputValue)
{
	return m_ReadImmediateAnalogOutput1Value_M3(m_nCOMPort, nNodeID, nImmediateAnalogOutputValue);
}

// Write immediate analog output 1
BOOL ModbusRTUHelper::WriteImmediateAnalogOutput1Value_M3(byte nNodeID, short nImmediateAnalogOutputValue)
{
	return m_WriteImmediateAnalogOutput1Value_M3(m_nCOMPort, nNodeID, nImmediateAnalogOutputValue);
}

// Read immediate analog output 2
BOOL ModbusRTUHelper::ReadImmediateAnalogOutput2Value_M3(byte nNodeID, short* dImmediateAnalogOutputValue)
{
	return m_ReadImmediateAnalogOutput2Value_M3(m_nCOMPort, nNodeID, dImmediateAnalogOutputValue);
}

// Write immediate analog output 2
BOOL ModbusRTUHelper::WriteImmediateAnalogOutput2Value_M3(byte nNodeID, short nImmediateAnalogOutputValue)
{
	return m_WriteImmediateAnalogOutput2Value_M3(m_nCOMPort, nNodeID, nImmediateAnalogOutputValue);
}

// Read Q program line number
BOOL ModbusRTUHelper::ReadQProgramLineNumber_M3(byte nNodeID, USHORT* nQProgramLineNumber)
{
	return m_ReadQProgramLineNumber_M3(m_nCOMPort, nNodeID, nQProgramLineNumber);
}

// Read immediate current command
BOOL ModbusRTUHelper::ReadImmediateCurrentCommand_M3(byte nNodeID, short* nImmediateCurrentCommand)
{
	return m_ReadImmediateCurrentCommand_M3(m_nCOMPort, nNodeID, nImmediateCurrentCommand);
}

// Read relative distance
BOOL ModbusRTUHelper::ReadRelativeDistance_M3(byte nNodeID, int* nRelativeDistance)
{
	return m_ReadRelativeDistance_M3(m_nCOMPort, nNodeID, nRelativeDistance);
}

// Read sensor position
BOOL ModbusRTUHelper::ReadSensorPosition_M3(byte nNodeID, int* nSensorPosition)
{
	return m_ReadSensorPosition_M3(m_nCOMPort, nNodeID, nSensorPosition);
}

// Read condition code
BOOL ModbusRTUHelper::ReadConditionCode_M3(byte nNodeID, USHORT* nConditionCode)
{
	return m_ReadConditionCode_M3(m_nCOMPort, nNodeID, nConditionCode);
}

// Read command mode
BOOL ModbusRTUHelper::ReadCommandMode_M3(byte nNodeID, USHORT* nCommandMode)
{
	return m_ReadCommandMode_M3(m_nCOMPort, nNodeID, nCommandMode);
}

// Read distance or position
BOOL ModbusRTUHelper::ReadDistanceOrPosition_M3(byte nNodeID, int* nDistanceOrPosition)
{
	return m_ReadDistanceOrPosition_M3(m_nCOMPort, nNodeID, nDistanceOrPosition);
}

// Write distance or position
BOOL ModbusRTUHelper::WriteDistanceOrPosition_M3(byte nNodeID, int nDistanceOrPosition)
{
	return m_WriteDistanceOrPosition_M3(m_nCOMPort, nNodeID, nDistanceOrPosition);
}

// Read change distance
BOOL ModbusRTUHelper::ReadChangeDistance_M3(byte nNodeID, int* nChangeDistance)
{
	return m_ReadChangeDistance_M3(m_nCOMPort, nNodeID, nChangeDistance);
}

// Write change distance
BOOL ModbusRTUHelper::WriteChangeDistance_M3(byte nNodeID, int nChangeDistance)
{
	return m_WriteChangeDistance_M3(m_nCOMPort, nNodeID, nChangeDistance);
}

// Read change velocity
BOOL ModbusRTUHelper::ReadChangeVelocity_M3(byte nNodeID, double* dChangeVelocity)
{
	return m_ReadChangeVelocity_M3(m_nCOMPort, nNodeID, dChangeVelocity);
}

// Write change velocity
BOOL ModbusRTUHelper::WriteChangeVelocity_M3(byte nNodeID, double dChangeVelocity)
{
	return m_WriteChangeVelocity_M3(m_nCOMPort, nNodeID, dChangeVelocity);
}

// Read velocity move state
BOOL ModbusRTUHelper::ReadVelocityMoveState_M3(byte nNodeID, USHORT* nVelocityMoveState)
{
	return m_ReadVelocityMoveState_M3(m_nCOMPort, nNodeID, nVelocityMoveState);
}

// Read P2P move state
BOOL ModbusRTUHelper::ReadP2PMoveState_M3(byte nNodeID, USHORT* nP2PMoveState)
{
	return m_ReadP2PMoveState_M3(m_nCOMPort, nNodeID, nP2PMoveState);
}

// Read Q program segment number
BOOL ModbusRTUHelper::ReadQProgramSegmentNumber_M3(byte nNodeID, USHORT* nQProgramSegmentNumber)
{
	return m_ReadQProgramSegmentNumber_M3(m_nCOMPort, nNodeID, nQProgramSegmentNumber);
}

// Read Command current
BOOL ModbusRTUHelper::ReadCommandCurrent_M3(byte nNodeID, double* dCommandCurrent)
{
	return m_ReadCommandCurrent_M3(m_nCOMPort, nNodeID, dCommandCurrent);
}

// Write Command current
BOOL ModbusRTUHelper::WriteCommandCurrent_M3(byte nNodeID, double dCommandCurrent)
{
	return m_WriteCommandCurrent_M3(m_nCOMPort, nNodeID, dCommandCurrent);
}

// Read Max current
BOOL ModbusRTUHelper::ReadMaximumCurrent_M3(byte nNodeID, double* dMaximumCurrent)
{
	return m_ReadMaximumCurrent_M3(m_nCOMPort, nNodeID, dMaximumCurrent);
}

// Write Max current
BOOL ModbusRTUHelper::WriteMaximumCurrent_M3(byte nNodeID, double dMaximumCurrent)
{
	return m_WriteMaximumCurrent_M3(m_nCOMPort, nNodeID, dMaximumCurrent);
}

// Read electronic gearing
BOOL ModbusRTUHelper::ReadElectronicGearing_M3(byte nNodeID, UINT* nElectronicGearing)
{
	return m_ReadElectronicGearing_M3(m_nCOMPort, nNodeID, nElectronicGearing);
}

// Write electronic gearing
BOOL ModbusRTUHelper::WriteElectronicGearing_M3(byte nNodeID, UINT nElectronicGearing)
{
	return m_WriteElectronicGearing_M3(m_nCOMPort, nNodeID, nElectronicGearing);
}

// Read pulse counter
BOOL ModbusRTUHelper::ReadPulseCounter_M3(byte nNodeID, int* nPulseCounter)
{
	return m_ReadPulseCounter_M3(m_nCOMPort, nNodeID, nPulseCounter);
}

// Write pulse counter
BOOL ModbusRTUHelper::WritePulseCounter_M3(byte nNodeID, int nPulseCounter)
{
	return m_WritePulseCounter_M3(m_nCOMPort, nNodeID, nPulseCounter);
}

// Read analog position gain
BOOL ModbusRTUHelper::ReadAnalogPositionGain_M3(byte nNodeID, int* nAnalogPositionGain)
{
	return m_ReadAnalogPositionGain_M3(m_nCOMPort, nNodeID, nAnalogPositionGain);
}

// Write analog position gain
BOOL ModbusRTUHelper::WriteAnalogPositionGain_M3(byte nNodeID, int nAnalogPositionGain)
{
	return m_WriteAnalogPositionGain_M3(m_nCOMPort, nNodeID, nAnalogPositionGain);
}

// Read analog velocity gain
BOOL ModbusRTUHelper::ReadAnalogVelocityGain_M3(byte nNodeID, double* dAnalogVelocityGain)
{
	return m_ReadAnalogVelocityGain_M3(m_nCOMPort, nNodeID, dAnalogVelocityGain);
}

// Write analog velocity gain
BOOL ModbusRTUHelper::WriteAnalogVelocityGain_M3(byte nNodeID, double dAnalogVelocityGain)
{
	return m_WriteAnalogVelocityGain_M3(m_nCOMPort, nNodeID, dAnalogVelocityGain);
}

// Read analog torque gain
BOOL ModbusRTUHelper::ReadAnalogTorqueGain_M3(byte nNodeID, double* dAnalogTorqueGain)
{
	return m_ReadAnalogTorqueGain_M3(m_nCOMPort, nNodeID, dAnalogTorqueGain);
}

// Write analog torque gain
BOOL ModbusRTUHelper::WriteAnalogTorqueGain_M3(byte nNodeID, double dAnalogTorqueGain)
{
	return m_WriteAnalogTorqueGain_M3(m_nCOMPort, nNodeID, dAnalogTorqueGain);
}

// Read analog threshold 1
BOOL ModbusRTUHelper::ReadAnalogThreshold1_M3(byte nNodeID, int* nAnalogThreshold)
{
	return m_ReadAnalogThreshold1_M3(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Write analog threshold 1
BOOL ModbusRTUHelper::WriteAnalogThreshold1_M3(byte nNodeID, int nAnalogThreshold)
{
	return m_WriteAnalogThreshold1_M3(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Read analog threshold 2
BOOL ModbusRTUHelper::ReadAnalogThreshold2_M3(byte nNodeID, int* nAnalogThreshold)
{
	return m_ReadAnalogThreshold2_M3(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Write analog threshold 2
BOOL ModbusRTUHelper::WriteAnalogThreshold2_M3(byte nNodeID, int nAnalogThreshold)
{
	return m_WriteAnalogThreshold2_M3(m_nCOMPort, nNodeID, nAnalogThreshold);
}

// Read analog offset 1
BOOL ModbusRTUHelper::ReadAnalogOffset1_M3(byte nNodeID, int* nAnalogOffset)
{
	return m_ReadAnalogOffset1_M3(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Write analog offset 1
BOOL ModbusRTUHelper::WriteAnalogOffset1_M3(byte nNodeID, int nAnalogOffset)
{
	return m_WriteAnalogOffset1_M3(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Read analog offset 2
BOOL ModbusRTUHelper::ReadAnalogOffset2_M3(byte nNodeID, int* nAnalogOffset)
{
	return m_ReadAnalogOffset2_M3(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Write analog offset 2
BOOL ModbusRTUHelper::WriteAnalogOffset2_M3(byte nNodeID, int nAnalogOffset)
{
	return m_WriteAnalogOffset2_M3(m_nCOMPort, nNodeID, nAnalogOffset);
}

// Read accumulator
BOOL ModbusRTUHelper::ReadAccumulator_M3(byte nNodeID, int* nAccumulator)
{
	return m_ReadAccumulator_M3(m_nCOMPort, nNodeID, nAccumulator);
}

// Read user defined register
BOOL ModbusRTUHelper::ReadUserDefinedRegister_M3(byte nNodeID, char chRegister, int* nValue)
{
	return m_ReadUserDefinedRegister_M3(m_nCOMPort, nNodeID, chRegister, nValue);
}

// Write user defined register
BOOL ModbusRTUHelper::WriteUserDefinedRegister_M3(byte nNodeID, byte chRegister, int nValue)
{
	return m_WriteUserDefinedRegister_M3(m_nCOMPort, nNodeID, chRegister, nValue);
}

// Read brake release delay
BOOL ModbusRTUHelper::ReadBrakeReleaseDelay_M3(byte nNodeID, double* dBrakeReleaseDelay)
{
	return m_ReadBrakeReleaseDelay_M3(m_nCOMPort, nNodeID, dBrakeReleaseDelay);
}

// Write brake release delay
BOOL ModbusRTUHelper::WriteBrakeReleaseDelay_M3(byte nNodeID, double dBrakeReleaseDelay)
{
	return m_WriteBrakeReleaseDelay_M3(m_nCOMPort, nNodeID, dBrakeReleaseDelay);
}

// Read brake engage delay
BOOL ModbusRTUHelper::ReadBrakeEngageDelay_M3(byte nNodeID, double* dBrakeEngageDelay)
{
	return m_ReadBrakeEngageDelay_M3(m_nCOMPort, nNodeID, dBrakeEngageDelay);
}

// Write brake engage delay
BOOL ModbusRTUHelper::WriteBrakeEngageDelay_M3(byte nNodeID, double dBrakeEngageDelay)
{
	return m_WriteBrakeEngageDelay_M3(m_nCOMPort, nNodeID, dBrakeEngageDelay);
}

// Read analog filter gain
BOOL ModbusRTUHelper::ReadAnalogFilterGain1_M3(byte nNodeID, USHORT* nAnalogFilterGain)
{
	return m_ReadAnalogFilterGain1_M3(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Write analog filter gain
BOOL ModbusRTUHelper::WriteAnalogFilterGain1_M3(byte nNodeID, USHORT nAnalogFilterGain)
{
	return m_WriteAnalogFilterGain1_M3(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Read analog filter gain
BOOL ModbusRTUHelper::ReadAnalogFilterGain2_M3(byte nNodeID, USHORT* nAnalogFilterGain)
{
	return m_ReadAnalogFilterGain2_M3(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Write analog filter gain
BOOL ModbusRTUHelper::WriteAnalogFilterGain2_M3(byte nNodeID, USHORT nAnalogFilterGain)
{
	return m_WriteAnalogFilterGain2_M3(m_nCOMPort, nNodeID, nAnalogFilterGain);
}

// Write broadcast command
BOOL ModbusRTUHelper::WriteBroadcastCommand(byte nNodeIDCount, byte* aNodeIDArr, byte nFunctionCode)
{
	return m_WriteBroadcastCommand(m_nCOMPort, nNodeIDCount, aNodeIDArr, nFunctionCode);
}

// Execute multi-axes linear interpolation move
BOOL ModbusRTUHelper::ExecuteLinearInterpolationMove(byte nNodeIDCount, byte* aNodeIDArr, int* aPosArr, UINT nMaxLineSpeed, UINT nAccelTimeInMs, BOOL bRelMove, BOOL bValidParamLimit)
{
	return m_ExecuteLinearInterpolationMove(m_nCOMPort, nNodeIDCount, aNodeIDArr, aPosArr, nMaxLineSpeed, nAccelTimeInMs, bRelMove, bValidParamLimit);
}

