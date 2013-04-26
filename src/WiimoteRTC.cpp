// -*- C++ -*-
/*!
 * @file  WiimoteRTC.cpp
 * @brief Controll Component for wiiremote 
 * @date $Date$
 * This is New BSD License.
 * $Id$
 */

#include "WiimoteRTC.h"

void on_state_change (wiimote			  &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state)
	{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).
	
	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if(changed & CONNECTED)
		{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
			{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
			}
		}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
		{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
			}
		}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
		{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
		}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
		}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(1000, 200);
#endif
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
		}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(200, 300);
#endif
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
}


// Module specification
// <rtc-template block="module_spec">
static const char* wiimotertc_spec[] =
  {
    "implementation_id", "WiimoteRTC",
    "type_name",         "WiimoteRTC",
    "description",       "Controll Component for wiiremote ",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
	"max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.debug", "0",
    "conf.default.SIU_compati", "0",
    "conf.default.ClassicController", "0",
    "conf.default.Nunchuk", "0",
    "conf.default.MotionPlus", "0",
    "conf.default.BalanceBoard", "0",
    "conf.default.LED", "",
    "conf.default.Rumble", "0",
    // Widget
    "conf.__widget__.debug", "radio",
    "conf.__widget__.SIU_compati", "radio",
    "conf.__widget__.ClassicController", "radio",
    "conf.__widget__.Nunchuk", "radio",
    "conf.__widget__.MotionPlus", "radio",
    "conf.__widget__.BalanceBoard", "radio",
    "conf.__widget__.LED", "checkbox",
    "conf.__widget__.Rumble", "radio",
    // Constraints
    "conf.__constraints__.debug", "(0,1)",
    "conf.__constraints__.SIU_compati", "(0,1)",
    "conf.__constraints__.ClassicController", "(0,1)",
    "conf.__constraints__.Nunchuk", "(0,1)",
    "conf.__constraints__.MotionPlus", "(0,1)",
    "conf.__constraints__.BalanceBoard", "(0,1)",
    "conf.__constraints__.LED", "(0,1,2,3)",
    "conf.__constraints__.Rumble", "(0,1)",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WiimoteRTC::WiimoteRTC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_commandIn("command", m_command),
    m_WiiRemoteOut("WiiRemote", m_WiiRemote),
    m_WiiNunchukOut("WiiNunchuk", m_WiiNunchuk),
    m_WiiClassicCOut("WiiClassicC", m_WiiClassicC),
    m_WiiBalanceBOut("WiiBalanceB", m_WiiBalanceB),
    m_WiiMotionPOut("WiiMotionP", m_WiiMotionP)

    // </rtc-template>
{
	m_WiiRemote.data.length(20);
	m_WiiNunchuk.data.length(23);
	m_WiiClassicC.data.length(37);
	m_WiiBalanceB.data.length(37);
	m_WiiMotionP.data.length(25);
}

/*!
 * @brief destructor
 */
WiimoteRTC::~WiimoteRTC()
{
}


/*!
 * Configurationの初期化を行い、PCとBluetooth接続したwiimoteデバイ
 * スと接続する。
 */
RTC::ReturnCode_t WiimoteRTC::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("command", m_commandIn);
  
  // Set OutPort buffer
  addOutPort("WiiRemote", m_WiiRemoteOut);
  addOutPort("WiiNunchuk", m_WiiNunchukOut);
  addOutPort("WiiClassicC", m_WiiClassicCOut);
  addOutPort("WiiBalanceB", m_WiiBalanceBOut);
  addOutPort("WiiMotionP", m_WiiMotionPOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("debug", m_debug, "0");
  bindParameter("SIU_compati", m_SIU_compati, "0");
  bindParameter("ClassicController", m_ClassicController, "0");
  bindParameter("Nunchuk", m_Nunchuk, "0");
  bindParameter("MotionPlus", m_MotionPlus, "0");
  bindParameter("BalanceBoard", m_BalanceBoard, "0");
  bindParameter("LED", m_LED, "");
  bindParameter("Rumble", m_Rumble, "0");
  // </rtc-template>

  wiiremote.ChangedCallback = on_state_change;
		wiiremote.CallbackTriggerFlags = (state_change_flags)(CONNECTED |
												   EXTENSION_CHANGED |
												   MOTIONPLUS_CHANGED);
												   
		if(!wiiremote.Connect(wiimote::FIRST_AVAILABLE)){
		std::cout<< "Connect ERROR..." << std::endl;
		}else{
			std::cout<< "Connect ...";

			if(wiiremote.IsBalanceBoard()) {
				std::cout << " Balance Board."<<std::endl;
			}else if(wiiremote.MotionPlusEnabled()){
				std::cout << " MotionPlus."<<std::endl;
			}else if(wiiremote.NunchukConnected()){
				std::cout << " Nunchuk."<<std::endl;
			}else if(wiiremote.ClassicConnected()){
				std::cout << " ClassicController."<<std::endl;
			}else{
				std::cout << " OK" << std::endl;
			}										   
	}

  
  return RTC::RTC_OK;
}

/*!
 * wiimoteデバイスの接続を切断する。
 */

RTC::ReturnCode_t WiimoteRTC::onFinalize()
{
	wiiremote.Disconnect();
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WiimoteRTC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*!
 * インポートにTimedOctet型データがある場合、wiimoteデバイスのLEDと
 * 振動をコントロールする。
 * （LEDと振動のコントロールはコンフィギュレーションから操作も可能）
 *
 * 各デバイスのアウトポートからコンフィギュレーションで指定し接続した
 * wiimoteデバイスのデータを取得・変換し、TimedShortSeq型で送信する。
 */

RTC::ReturnCode_t WiimoteRTC::onExecute(RTC::UniqueId ec_id)
{
	if(wiiremote.RefreshState())
	if(wiiremote.ConnectionLost()){
		Beep(100,100);
	}else{
		if(wiiremote.IsBalanceBoard()|| wiiremote.MotionPlusEnabled() || wiiremote.NunchukConnected() || wiiremote.ClassicConnected()) {
			//std::cout << " ...connect " <<std::endl;
			Sleep(500);
			}
	}

	int cnt=0;
	TimedDoubleSeq buff;
	buff.data.length(20);
		for(unsigned bit=0; bit<16; bit++)
			{
			WORD mask = (WORD)(1 << bit);
			if((wiimote_state::buttons::ALL & mask) == 0)
				continue;
				bool pressed = ((wiiremote.Button.Bits & mask) != 0);

				buff.data[cnt]=pressed;
				cnt++;
			}

	if(m_Nunchuk){

		if(wiiremote.NunchukConnected()){
			//[14]~[20]setting
			m_WiiNunchuk.data[0]  = (short)buff.data[3];
			m_WiiNunchuk.data[1]  = (short)buff.data[1];
			m_WiiNunchuk.data[2]  = (short)buff.data[2];
			m_WiiNunchuk.data[3]  = (short)buff.data[0];
			m_WiiNunchuk.data[4]  = (short)buff.data[8];
			m_WiiNunchuk.data[5]  = (short)buff.data[7];
			m_WiiNunchuk.data[6]  = (short)buff.data[4];
			m_WiiNunchuk.data[7]  = (short)buff.data[10];
			m_WiiNunchuk.data[8]  = (short)buff.data[9];
			m_WiiNunchuk.data[9]  = (short)buff.data[6];
			m_WiiNunchuk.data[10] = (short)buff.data[5];

			m_WiiNunchuk.data[11] = (short)wiiremote.Acceleration.X * 100;
			m_WiiNunchuk.data[12] = (short)wiiremote.Acceleration.Y * 100;
			m_WiiNunchuk.data[13] = (short)wiiremote.Acceleration.Z * 100; 

			m_WiiNunchuk.data[14] = (short)wiiremote.Nunchuk.C;
			m_WiiNunchuk.data[15] = (short)wiiremote.Nunchuk.Z;

			m_WiiNunchuk.data[16] = (short)wiiremote.Nunchuk.Joystick.X * 200;
			m_WiiNunchuk.data[17] = (short)wiiremote.Nunchuk.Joystick.Y * 200;

			m_WiiNunchuk.data[18] = (int)(((short)wiiremote.Nunchuk.Acceleration.RawX - wiiremote.Nunchuk.CalibrationInfo.X0) / 
		((short)wiiremote.Nunchuk.CalibrationInfo.XG - wiiremote.Nunchuk.CalibrationInfo.X0)*100);
			m_WiiNunchuk.data[19] = (int)(((short)wiiremote.Nunchuk.Acceleration.RawY - wiiremote.Nunchuk.CalibrationInfo.Y0) /
		((short)wiiremote.Nunchuk.CalibrationInfo.YG - wiiremote.Nunchuk.CalibrationInfo.Y0)*100);
				m_WiiNunchuk.data[20] = (int)(((short)wiiremote.Nunchuk.Acceleration.RawZ - wiiremote.Nunchuk.CalibrationInfo.X0) /
		((short)wiiremote.Nunchuk.CalibrationInfo.ZG -  wiiremote.Nunchuk.CalibrationInfo.Z0)*100); 
			
			if(!m_SIU_compati){
				m_WiiNunchuk.data[21] = wiiremote.BatteryPercent;
				m_WiiNunchuk.data[22] = wiiremote.LED.Bits;
			}


			
			if(m_debug == true){
				std::cout << "<<WiiRemoteState>>" << std::endl;
				std::cout << "↑:" << m_WiiNunchuk.data[0] << std::endl;
				std::cout << "→:" << m_WiiNunchuk.data[1] << std::endl;
				std::cout << "↓:" << m_WiiNunchuk.data[2] <<std::endl;
				std::cout << "←:" << m_WiiNunchuk.data[3] << std::endl;
				std::cout << "Ａ:" <<  m_WiiNunchuk.data[4] << std::endl;
				std::cout << "Ｂ:" <<  m_WiiNunchuk.data[5] << std::endl;
				std::cout << "＋:" <<  m_WiiNunchuk.data[6] << std::endl;
				std::cout << "Home:" <<  m_WiiNunchuk.data[7] << std::endl;
				std::cout << "－:" <<  m_WiiNunchuk.data[8] << std::endl;
				std::cout << "１:" <<  m_WiiNunchuk.data[9] << std::endl;
				std::cout << "２:" <<  m_WiiNunchuk.data[10] << std::endl;
				std::cout << "X_Accel:" <<  m_WiiNunchuk.data[11] << std::endl;
				std::cout << "Y_Accel:" <<  m_WiiNunchuk.data[12] << std::endl;
				std::cout << "Z_Accel:" <<  m_WiiNunchuk.data[13] << std::endl;
				std::cout << std::endl;

				std::cout << "<<WiiNunchukState>>" << std::endl;
				std::cout << "Ｃ:" <<  m_WiiNunchuk.data[14] << std::endl;
				std::cout << "Ｚ:" <<  m_WiiNunchuk.data[15] << std::endl;
				std::cout << "X_Joystick:" <<  m_WiiNunchuk.data[16] << std::endl;
				std::cout << "Y_Joystick:" <<  m_WiiNunchuk.data[17] << std::endl;
				std::cout << "X_Accel:" <<  m_WiiNunchuk.data[18] << std::endl;
				std::cout << "Y_Accel:" <<  m_WiiNunchuk.data[19] << std::endl;
				std::cout << "Z_Accel:" <<  m_WiiNunchuk.data[20] << std::endl;
				if(!m_SIU_compati){
				std::cout << "Bettery %:" << m_WiiNunchuk.data[21] << std::endl;
				std::cout << "LED_bits:" << m_WiiNunchuk.data[22] << std::endl;
				}
			}

			m_WiiNunchukOut.write();
		}else{
			return RTC::RTC_ERROR;
	
		}
	}else if(m_ClassicController){
		if(wiiremote.ClassicConnected()){
			//[14]~[34]setting

			int cnt2=0;
			TimedShortSeq buff_cc;
			buff_cc.data.length(20);
				
			for(unsigned bit=0; bit<16; bit++)
					{
					WORD mask = (WORD)(1 << bit);
					// skip unused bits
					if((wiimote_state::classic_controller::buttons::ALL & mask) == 0)
						continue;

					const TCHAR* button_name = wiimote::ClassicButtonNameFromBit[bit];
					bool		 pressed	 = ((wiiremote.ClassicController.Button.Bits & mask) != 0);
					buff_cc.data[bit]=pressed;
					}

		
			m_WiiClassicC.data[0]  = (short)buff.data[3];
			m_WiiClassicC.data[1]  = (short)buff.data[1];
			m_WiiClassicC.data[2]  = (short)buff.data[2];
			m_WiiClassicC.data[3]  = (short)buff.data[0];
			m_WiiClassicC.data[4]  = (short)buff.data[8];
			m_WiiClassicC.data[5]  = (short)buff.data[7];
			m_WiiClassicC.data[6]  = (short)buff.data[4];
			m_WiiClassicC.data[7]  = (short)buff.data[10];
			m_WiiClassicC.data[8]  = (short)buff.data[9];
			m_WiiClassicC.data[9]  = (short)buff.data[6];
			m_WiiClassicC.data[10] = (short)buff.data[5];
			m_WiiClassicC.data[11] = (short)wiiremote.Acceleration.X * 100;
			m_WiiClassicC.data[12] = (short)wiiremote.Acceleration.Y * 100;
			m_WiiClassicC.data[13] = (short)wiiremote.Acceleration.Z * 100;
			
			m_WiiClassicC.data[14] = (short)buff_cc.data[12]; //A_button
			m_WiiClassicC.data[15] = (short)buff_cc.data[14]; //B_button
			m_WiiClassicC.data[16] = (short)buff_cc.data[11]; //X_button
			m_WiiClassicC.data[17] = (short)buff_cc.data[13]; //Y_button
			m_WiiClassicC.data[18] = (short)buff_cc.data[4]; //Minus_button
			m_WiiClassicC.data[19] = (short)buff_cc.data[3]; //Home_button
			m_WiiClassicC.data[20] = (short)buff_cc.data[2]; //Plus_button
			m_WiiClassicC.data[21] = (short)buff_cc.data[8]; //Up_button
			m_WiiClassicC.data[22] = (short)buff_cc.data[6]; //Down_button
			m_WiiClassicC.data[23] = (short)buff_cc.data[9]; //Light_button
			m_WiiClassicC.data[24] = (short)buff_cc.data[7]; //Right_button
			m_WiiClassicC.data[25] = (short)buff_cc.data[5]; //L_button
			m_WiiClassicC.data[26] = (short)buff_cc.data[1]; //R_button
			m_WiiClassicC.data[27] = (short)buff_cc.data[15]; //ZL_button
			m_WiiClassicC.data[28] = (short)buff_cc.data[10]; //ZR_button

			m_WiiClassicC.data[29] = (short)wiiremote.ClassicController.TriggerL; //L_analog_button
			m_WiiClassicC.data[30] = (short)wiiremote.ClassicController.TriggerR; //R_analog_button

			m_WiiClassicC.data[31] = (short)((wiiremote.ClassicController.JoystickL.RawX - 31) * 100 / 32.5);
			m_WiiClassicC.data[32] = (short)((wiiremote.ClassicController.JoystickL.RawY - 16) * 100 / 32.5);
			m_WiiClassicC.data[33] = (short)((wiiremote.ClassicController.JoystickR.RawX - 31) * 100 / 15.5);
			m_WiiClassicC.data[34] = (short)((wiiremote.ClassicController.JoystickR.RawY - 16) * 100 / 15.5);
			if(!m_SIU_compati){
				m_WiiClassicC.data[35] = wiiremote.BatteryPercent;
				m_WiiClassicC.data[36] = wiiremote.LED.Bits;
			}	

			if(m_debug)
			{
			std::cout << "<<WiiRemoteState>>" << std::endl;
			std::cout << "↑:" << m_WiiClassicC.data[0] << std::endl;
			std::cout << "→:" << m_WiiClassicC.data[1] << std::endl;
			std::cout << "↓:" << m_WiiClassicC.data[2] << std::endl;
			std::cout << "←:" << m_WiiClassicC.data[3] << std::endl;
			std::cout << "Ａ:" << m_WiiClassicC.data[4] << std::endl;
			std::cout << "Ｂ:" << m_WiiClassicC.data[5] << std::endl;
			std::cout << "＋:" << m_WiiClassicC.data[6] << std::endl;
			std::cout << "Home:" << m_WiiClassicC.data[7] << std::endl;
			std::cout << "－:" << m_WiiClassicC.data[8] << std::endl;
			std::cout << "１:" << m_WiiClassicC.data[9] << std::endl;
			std::cout << "２:" << m_WiiClassicC.data[10] << std::endl;
			std::cout << "X_Accel:" << m_WiiClassicC.data[11] << std::endl;
			std::cout << "Y_Accel:" << m_WiiClassicC.data[12] << std::endl;
			std::cout << "Z_Accel:" << m_WiiClassicC.data[13] << std::endl;
			std::cout << std::endl;
			
			std::cout << "<<ClassicControllerState>>" << std::endl;
			std::cout << "↑:" << m_WiiClassicC.data[21] << std::endl;
			std::cout << "→:" << m_WiiClassicC.data[24] << std::endl;
			std::cout << "↓:" << m_WiiClassicC.data[22] << std::endl;
			std::cout << "←:" << m_WiiClassicC.data[23] << std::endl;
			std::cout << "Ａ:" << m_WiiClassicC.data[14] << std::endl;
			std::cout << "Ｂ:" << m_WiiClassicC.data[15] << std::endl;
			std::cout << "Ｘ:" << m_WiiClassicC.data[16] << std::endl;
			std::cout << "Ｙ:" << m_WiiClassicC.data[17] << std::endl;
			std::cout << "＋:" << m_WiiClassicC.data[20] << std::endl;
			std::cout << "Home:" << m_WiiClassicC.data[19] << std::endl;
			std::cout << "－:" << m_WiiClassicC.data[18] << std::endl;
			std::cout << "Ｌ:" << m_WiiClassicC.data[25] << std::endl;
			std::cout << "Ｒ:" << m_WiiClassicC.data[26] << std::endl;
			std::cout << "ZL:" << m_WiiClassicC.data[27] << std::endl;
			std::cout << "ZR:" << m_WiiClassicC.data[28] << std::endl;
			std::cout << "L_Trigger:" << m_WiiClassicC.data[29] << std::endl;
			std::cout << "R_Trigger:" << m_WiiClassicC.data[30] << std::endl;
			std::cout << "L_Stick_X:" << m_WiiClassicC.data[31] << std::endl;
			std::cout << "L_Stick_Y:" << m_WiiClassicC.data[32] << std::endl;
			std::cout << "R_Stick_X:" << m_WiiClassicC.data[33] << std::endl;
			std::cout << "R_Stick_Y:" << m_WiiClassicC.data[34] << std::endl;
			if(!m_SIU_compati){
				std::cout << "Bettery %:" << m_WiiClassicC.data[35] << std::endl;
				std::cout << "LED_bits:" << m_WiiClassicC.data[36] << std::endl;
			}	

			}


			m_WiiClassicCOut.write();
		}else{
			return RTC::RTC_ERROR;
	

			}
		

		}else if(m_BalanceBoard){
			if(wiiremote.IsBalanceBoard()){
				//[0]~[10]setting ※他のデバイスと違って[0]からデータ取得必要です。

				short Kx = (short)((wiiremote.BalanceBoard.Kg.TopL + wiiremote.BalanceBoard.Kg.BottomL) / (wiiremote.BalanceBoard.Kg.TopR + wiiremote.BalanceBoard.Kg.BottomR));
				short Ky = (short)((wiiremote.BalanceBoard.Kg.TopL + wiiremote.BalanceBoard.Kg.TopR) / (wiiremote.BalanceBoard.Kg.BottomL + wiiremote.BalanceBoard.Kg.BottomR));


				m_WiiBalanceB.data[0]  = (short)wiiremote.BalanceBoard.ButtonStateA; // On/Off Botton
				m_WiiBalanceB.data[1]  = (short)(((short)(Kx - 1) / (short)(Kx + 1)) * (short)(-43 / 2));; // X
				m_WiiBalanceB.data[2]  = -(short)(((short)(Ky - 1) / (short)(Ky + 1)) * (short)(-24 / 2));; // Y 
				m_WiiBalanceB.data[3]  = (short)wiiremote.BalanceBoard.Kg.TopL; 
				m_WiiBalanceB.data[4]  = (short)wiiremote.BalanceBoard.Kg.TopR; 
				m_WiiBalanceB.data[5]  = (short)wiiremote.BalanceBoard.Kg.BottomL; 
				m_WiiBalanceB.data[6]  = (short)wiiremote.BalanceBoard.Kg.BottomR; 
				m_WiiBalanceB.data[7]  = (short)wiiremote.BalanceBoard.Raw.TopL;
				m_WiiBalanceB.data[8]  = (short)wiiremote.BalanceBoard.Raw.TopR;
				m_WiiBalanceB.data[9]  = (short)wiiremote.BalanceBoard.Raw.BottomL;
				m_WiiBalanceB.data[10] = (short)wiiremote.BalanceBoard.Raw.BottomR;
				if(!m_SIU_compati){
					m_WiiBalanceB.data[11] = (short)wiiremote.BatteryPercent;
					m_WiiBalanceB.data[12] = (short)wiiremote.LED.Bits;				
				}

					if(m_debug == true){
						std::cout << "<<WiiBalanceBoardState>>" << std::endl;
						std::cout << "A :" << m_WiiBalanceB.data[0] << std::endl;
						std::cout << "GX:" << m_WiiBalanceB.data[1] << std::endl;
						std::cout << "GY:" << m_WiiBalanceB.data[2] << std::endl;
						std::cout << "Sensor Value Kg(Top left)     :" << m_WiiBalanceB.data[3] << std::endl;
						std::cout << "Sensor Value Kg(Top right)    :" << m_WiiBalanceB.data[4] << std::endl;
						std::cout << "Sensor Value Kg(Bottom right) :" << m_WiiBalanceB.data[5] << std::endl;
						std::cout << "Sensor Value Kg(Bottom left)  :" << m_WiiBalanceB.data[6] << std::endl;
						std::cout << "Sensor Value Raw(Top left)    :" << m_WiiBalanceB.data[7] << std::endl;
						std::cout << "Sensor Value Raw(Top right)   :" << m_WiiBalanceB.data[8] << std::endl;
						std::cout << "Sensor Value Raw(Bottom right):" << m_WiiBalanceB.data[9] << std::endl;
						std::cout << "Sensor Value Raw(Bottom left) :" << m_WiiBalanceB.data[10] << std::endl;
						if(!m_SIU_compati){
							std::cout << "Bettery %:" << m_WiiBalanceB.data[11] << std::endl;
							std::cout << "LED_bits:" << m_WiiBalanceB.data[12] << std::endl;
						}


					}
				m_WiiBalanceBOut.write();
			}else{
				return RTC::RTC_ERROR;
			}

	}else if(m_MotionPlus){
		//[14]~[19]setting
		if(wiiremote.MotionPlusEnabled()){
			wiiremote.DisableMotionPlus();
		}

		m_WiiMotionP.data[0]  = (short)buff.data[3];   //button~
		m_WiiMotionP.data[1]  = (short)buff.data[1];
		m_WiiMotionP.data[2]  = (short)buff.data[2];
		m_WiiMotionP.data[3]  = (short)buff.data[0];
		m_WiiMotionP.data[4]  = (short)buff.data[8];
		m_WiiMotionP.data[5]  = (short)buff.data[7];
		m_WiiMotionP.data[6]  = (short)buff.data[4];
		m_WiiMotionP.data[7]  = (short)buff.data[10];
		m_WiiMotionP.data[8]  = (short)buff.data[9];
		m_WiiMotionP.data[9]  = (short)buff.data[6];
		m_WiiMotionP.data[10] = (short)buff.data[5];   //~button
		
		m_WiiMotionP.data[11] = (short)wiiremote.Acceleration.X * 100;
		m_WiiMotionP.data[12] = (short)wiiremote.Acceleration.Y * 100;
		m_WiiMotionP.data[13] = (short)wiiremote.Acceleration.Z * 100;
		m_WiiMotionP.data[14] = (short)wiiremote.MotionPlus.Raw.Yaw;
		m_WiiMotionP.data[15] = (short)wiiremote.MotionPlus.Raw.Pitch;
		m_WiiMotionP.data[16] = (short)wiiremote.MotionPlus.Raw.Roll;

		m_WiiMotionP.data[17] = (short)wiiremote.MotionPlus.Speed.Yaw;
		m_WiiMotionP.data[18] = (short)wiiremote.MotionPlus.Speed.Pitch;
		m_WiiMotionP.data[19] = (short)wiiremote.MotionPlus.Speed.Roll;
		
		if(!m_SIU_compati){
			m_WiiMotionP.data[20] = (short)wiiremote.BatteryPercent;
			m_WiiMotionP.data[21] = (short)wiiremote.LED.Bits;
		}

		if(m_debug == true){
			std::cout << "<<WiiMotionPlusState>>" << std::endl;
			std::cout << "Up:" << m_WiiMotionP.data[0] << std::endl;
			std::cout << "Right:" << m_WiiMotionP.data[1] <<std::endl;
			std::cout << "Down:" << m_WiiMotionP.data[2] << std::endl;
			std::cout << "Left:" << m_WiiMotionP.data[3] <<std::endl;
			std::cout << "A:" << m_WiiMotionP.data[4] << std::endl;
			std::cout << "B:" << m_WiiMotionP.data[5] <<std::endl;
			std::cout << "+:" << m_WiiMotionP.data[6] << std::endl;
			std::cout << "Home:" << m_WiiMotionP.data[7] << std::endl;
			std::cout << "-:" << m_WiiMotionP.data[8] <<std::endl;
			std::cout << "1:" << m_WiiMotionP.data[9] << std::endl;
			std::cout << "2:" << m_WiiMotionP.data[10] <<std::endl;
			std::cout << "X_Accel:" << m_WiiMotionP.data[11] << std::endl;
			std::cout << "Y_Accel:" << m_WiiMotionP.data[12] << std::endl;
			std::cout << "Z_Accel:" << m_WiiMotionP.data[13] <<std::endl;
			std::cout << "Yaw:" << m_WiiMotionP.data[14] << std::endl;
			std::cout << "Pitch:" << m_WiiMotionP.data[15] << std::endl;
			std::cout << "Roll:" << m_WiiMotionP.data[16] <<std::endl;
			std::cout << "X_Orient:" << m_WiiMotionP.data[17] << std::endl;
			std::cout << "Y_Orient:" << m_WiiMotionP.data[18] << std::endl;
			std::cout << "Z_Orient:" << m_WiiMotionP.data[19] <<std::endl;
			if(!m_SIU_compati){
				std::cout << "Bettery %:" << m_WiiMotionP.data[20] << std::endl;
				std::cout << "LED_bits:" << m_WiiMotionP.data[21] << std::endl;
			}
			
		}		
						
			m_WiiMotionPOut.write();

	}else{

		// wiiremote controler

		m_WiiRemote.data[0]  = (short)buff.data[3];
		m_WiiRemote.data[1]  = (short)buff.data[1];
		m_WiiRemote.data[2]  = (short)buff.data[2];
		m_WiiRemote.data[3]  = (short)buff.data[0];
		m_WiiRemote.data[4]  = (short)buff.data[8];
		m_WiiRemote.data[5]  = (short)buff.data[7];
		m_WiiRemote.data[6]  = (short)buff.data[4];
		m_WiiRemote.data[7]  = (short)buff.data[10];
		m_WiiRemote.data[8]  = (short)buff.data[9];
		m_WiiRemote.data[9]  = (short)buff.data[6];
		m_WiiRemote.data[10] = (short)buff.data[5];
		m_WiiRemote.data[11] = (short)wiiremote.Acceleration.X * 100; // [cm/s²]
		m_WiiRemote.data[12] = (short)wiiremote.Acceleration.Y * 100;
		m_WiiRemote.data[13] = (short)wiiremote.Acceleration.Z * 100;

		if(!m_SIU_compati){
		m_WiiRemote.data[14] = (short)wiiremote.Acceleration.Orientation.Pitch * 100; // [deg]
		m_WiiRemote.data[15] = (short)wiiremote.Acceleration.Orientation.Roll * 100;
		m_WiiRemote.data[16] = (short)wiiremote.BatteryPercent;
		m_WiiRemote.data[17] = (short)wiiremote.LED.Bits;
		}

	
		if(m_debug == true){
			std::cout << "<<WiiRemoteState>>" << std::endl;
			std::cout << "Up:" << m_WiiRemote.data[0] << std::endl;
			std::cout << "Right:" << m_WiiRemote.data[1] << std::endl;
			std::cout << "Down:" << m_WiiRemote.data[2] << std::endl;
			std::cout << "Left:" << m_WiiRemote.data[3] <<std::endl;
			std::cout << "A:" << m_WiiRemote.data[4] << std::endl;
			std::cout << "B:" << m_WiiRemote.data[5] <<std::endl;
			std::cout << "+:" << m_WiiRemote.data[6] << std::endl;
			std::cout << "Home:" << m_WiiRemote.data[7] << std::endl;
			std::cout << "-:" << m_WiiRemote.data[8] <<std::endl;
			std::cout << "1:" << m_WiiRemote.data[9] << std::endl;
			std::cout << "2:" << m_WiiRemote.data[10] <<std::endl;
			std::cout << "X_Accel:" << m_WiiRemote.data[11] << std::endl;
			std::cout << "Y_Accel:" << m_WiiRemote.data[12] << std::endl;
			std::cout << "Z_Accel:" << m_WiiRemote.data[13] <<std::endl;
			if(!m_SIU_compati){
					std::cout << "Pitch:" << m_WiiRemote.data[14] << std::endl;
					std::cout << "Roll:" << m_WiiRemote.data[15] <<std::endl;
					std::cout << "Bettery %:" << m_WiiRemote.data[16] << std::endl;
					std::cout << "LED_bits:" << m_WiiRemote.data[17] << std::endl;
				}
			}		
		m_WiiRemoteOut.write();

	}


	if (m_LED[0] != 0 || m_Rumble){
		bool m_led0=false,m_led1=false,m_led2=false,m_led3=false;
		wiiremote.SetRumble(m_Rumble);

		int val = m_LED.find("0");
		val==0?  m_led0=true:m_led0=false;
		
		int val1 = m_LED.find("1");
		val1>=0? m_led1=true:m_led1=false;

		int val2 = m_LED.find("2");
		val2>=0? m_led2=true:m_led2=false;

		int val3 = m_LED.find("3");
		val3>=0? m_led3=true:m_led3=false;

		BYTE LED_data = (BYTE)(m_led0 | m_led1<<1 | m_led2<<2 | m_led3<<3);
		wiiremote.SetLEDs((BYTE)LED_data);
	
	}else{
		if (m_commandIn.isNew()){
			m_commandIn.read();
			wiiremote.SetLEDs((BYTE)( m_command.data >>4));
			wiiremote.SetRumble((m_command.data & 00000001));
		}else{
			wiiremote.SetLEDs((BYTE)(0));
			wiiremote.SetRumble(0);
		}
		
	}
	
	Sleep(100);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t WiimoteRTC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t WiimoteRTC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void WiimoteRTCInit(RTC::Manager* manager)
  {
    coil::Properties profile(wiimotertc_spec);
    manager->registerFactory(profile,
                             RTC::Create<WiimoteRTC>,
                             RTC::Delete<WiimoteRTC>);
  }
  
};


