// -*- C++ -*-
/*!
 * @file  WiimoteRTC.h
 * @brief Controll Component for wiiremote 
 * @date  $Date$
 * This is New BSD License.
 * $Id$
 */

#ifndef WIIMOTERTC_H
#define WIIMOTERTC_H

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

#include "wiimote.h"

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

using namespace RTC;

/*!
 * @class WiimoteRTC
 * @brief Controll Component for wiiremote 
 *
 */
class WiimoteRTC
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  WiimoteRTC(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~WiimoteRTC();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   * Configurationの初期化を行い、PCとBluetooth接続したwiimoteデバ
   * イスと接続する。
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   * wiimoteデバイスの接続を切断する。
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   * インポートにTimedOctet型データがある場合、wiimoteデバイスのLE
   * Dと振動をコントロールする。
   * （LEDと振動のコントロールはコンフィギュレーションから操作も可
   * 能）
   * コンフィギュレーションで指定し接続したwiimoteデバイスのデータ
   * を取得・変換し、
   * 各デバイスのアウトポートからTimedShortSeq型で送信する。
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * コンソール画面に整形データを表示するConfiguration。
   * - Name: debug 
   * - DefaultValue: 0
   */
  bool m_debug;
  /*!
   * 芝浦工大学が開発したWiiRemoteComponentsに互換性のあるデータを
   * 送信するConfiguration。
   * - Name: SIU_compati
   * - DefaultValue: 0
   */
  bool m_SIU_compati;
  /*!
   * wiiClassicControllerの情報を取得・送信するConfiguration。
   * - Name: ClassicController
   * - DefaultValue: 0
   */
  bool m_ClassicController;
  /*!
   * wiiNunchukの情報を取得・送信するConfiguration。
   * - Name: Nunchuk
   * - DefaultValue: 0
   */
  bool m_Nunchuk;
  /*!
   * wiiMotionPlusの情報を取得・送信するConfiguration。
   * - Name: MotionPlus
   * - DefaultValue: 0
   */
  bool m_MotionPlus;
  /*!
   * wiiBalanceBoardの情報を取得・送信するConfiguration。
   * - Name: BalanceBoard
   * - DefaultValue: 0
   */
  bool m_BalanceBoard;
  /*!
   * wiiRemoteリモコンのLEDの点灯を設定するConfiguration。
   * - Name: LED
   * - DefaultValue: ""
   */
  std::string m_LED;
  /*!
   * wiiRemoteリモコンの振動を設定するConfiguration。
   * - Name: Rumble
   * - DefaultValue: 0
   */
  bool m_Rumble;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::TimedOctet m_command;
  /*!
   * WiiコントローラのLEDと振動を制御するコマンドデータを受信する。
   * 上位4ビットでLEDの点灯パターン、最下位1ビットで振動を操作する
   * 。
   * - Type: TimedOctet
   */
  InPort<RTC::TimedOctet> m_commandIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedShortSeq m_WiiRemote;
  /*!
   * WiiRemoteコントローラから取得したデータを送信する。
   * [0] 十字キー・上ボタンの状態(BOOL 値)
   * [1] 十字キー・右ボタンの状態(BOOL 値)
   * [2] 十字キー・下ボタンの状態(BOOL 値)
   * [3] 十字キー・左ボタンの状態(BOOL 値)
   * [4] A ボタンの状態(BOOL 値)
   * [5] B ボタンの状態(BOOL 値)
   * [6] ＋ボタンの状態(BOOL 値)
   * [7] Home ボタンの状態(BOOL 値)
   * [8] －ボタンの状態(BOOL 値)
   * [9] 1 ボタンの状態(BOOL 値)
   * [10] 2 ボタンの状態(BOOL 値)
   * [11] X軸方向の加速度
   * [12] Y軸方向の加速度
   * [13] Z軸方向の加速度
   * [14] pitchのデータ
   * [15] rollのデータ
   * [16] バッテリー残量(%)
   * [17] LEDの状態（BYTE値）
   * ※コンフィギュレーションの"SIU_compti"が”true"の場合、[0]~[1
   * 3]のデータを送信する。
   * - Type: TimedShortSeq
   * - Number: 18(※"SIU_compti"が"true"の場合、14)
   */
  OutPort<RTC::TimedShortSeq> m_WiiRemoteOut;
  
  
  RTC::TimedShortSeq m_WiiNunchuk;
  /*!
   * WiiNunchukコントローラから取得したデータを送信する。
   * 尚、[0]から[13]までがWiiリモコン本体の情報、それ以降はWiiNunc
   * hukの情報となっている。
   * [0] 十字キー・上ボタンの状態(BOOL 値)
   * [1] 十字キー・右ボタンの状態(BOOL 値)
   * [2] 十字キー・下ボタンの状態(BOOL 値)
   * [3] 十字キー・左ボタンの状態(BOOL 値)
   * [4] A ボタンの状態(BOOL 値)
   * [5] B ボタンの状態(BOOL 値)
   * [6] ＋ボタンの状態(BOOL 値)
   * [7] Home ボタンの状態(BOOL 値)
   * [8] －ボタンの状態(BOOL 値)
   * [9] 1 ボタンの状態(BOOL 値)
   * [10] 2 ボタンの状態(BOOL 値)
   * [11] X軸方向の加速度
   * [12] Y軸方向の加速度
   * [13] Z軸方向の加速度
   * [14] C ボタンの状態(BOOL 値)
   * [15] Z ボタンの状態(BOOL 値)
   * [16] アナログスティックのX 方向入力値
   * (解放時0，最大まで左に傾けると-100，同右で100)
   * [17] アナログスティックのY 方向入力値
   * (解放時0，最大まで左に傾けると-100，同右で100)
   * [18] X軸方向の加速度
   * [19] Y軸方向の加速度
   * [20] Z軸方向の加速度
   * [21] バッテリー残量(%)
   * [22] LEDの状態（BYTE値）
   * ※コンフィギュレーションの"SIU_compti"が"true"の場合、[0]~[21
   * ]のデータを送信する。
   * - Type: TimeShortSeq
   * - Number: 23(※"SIU_compti"が"true"の場合、21)
   */
  OutPort<RTC::TimedShortSeq> m_WiiNunchukOut;


  RTC::TimedShortSeq m_WiiClassicC;
  /*!
   * WiiClassic Controllerから取得したデータを送信する。
   * 尚、[0]から[13]までがWiiリモコン本体の情報、それ以降はWiiClas
   * sicControllerの情報となっている。
   * [0] 十字キー・上ボタンの状態(BOOL 値)
   * [1] 十字キー・右ボタンの状態(BOOL 値)
   * [2] 十字キー・下ボタンの状態(BOOL 値)
   * [3] 十字キー・左ボタンの状態(BOOL 値)
   * [4] A ボタンの状態(BOOL 値)
   * [5] B ボタンの状態(BOOL 値)
   * [6] ＋ボタンの状態(BOOL 値)
   * [7] Home ボタンの状態(BOOL 値)
   * [8] －ボタンの状態(BOOL 値)
   * [9] 1 ボタンの状態(BOOL 値)
   * [10] 2 ボタンの状態(BOOL 値)
   * [11] X軸方向の加速度
   * [12] Y軸方向の加速度
   * [13] Z軸方向の加速度
   * [14] A ボタンの状態(BOOL 値)
   * [15] B ボタンの状態(BOOL 値)
   * [16] X ボタンの状態(BOOL 値)
   * [17] Y ボタンの状態(BOOL 値)
   * [18] －ボタンの状態(BOOL 値)
   * [19] Home ボタンの状態(BOOL 値)
   * [20] ＋ボタンの状態(BOOL 値)
   * [21] 十字キー・上ボタンの状態(BOOL 値)
   * [22] 十字キー・下ボタンの状態(BOOL 値)
   * [23] 十字キー・左ボタンの状態(BOOL 値)
   * [24] 十字キー・右ボタンの状態(BOOL 値)
   * [25] L ボタンの状態(BOOL 値)
   * [26] R ボタンの状態(BOOL 値)
   * [27] ZL ボタンの状態(BOOL 値)
   * [28] ZR ボタンの状態(BOOL 値)
   * [29] L ボタンのアナログ入力値
   * 　(最小値0，最大値100，PRO の場合は解放で0，押下で100)
   * [30] R ボタンのアナログ入力値
   * 　(最小値0，最大値100，PRO の場合は解放で0，押下で100)
   * [31] 左アナログスティックのX 方向入力値
   * 　(解放時0，最大まで左に傾けると-100，同右で100)
   * [32] 左アナログスティックのY 方向入力値
   * 　(解放時0，最大まで下に傾けると-100，同上で100)
   * [33] 右アナログスティックのX 方向入力値(-100～100)
   * 　(解放時0，最大まで左に傾けると-100，同右で100)
   * [34] 右アナログスティックのY 方向入力値(-100～100)
   * 　(解放時0，最大まで下に傾けると-100，同上で100)
   * [35] バッテリー残量(%)
   * [36] LEDの状態（BYTE値）
   * ※コンフィギュレーションの"SIU_compti"が"true"の場合、[0]~[36
   * ]のデータを送信する。
   * - Type: TimedShortSeq
   * - Number: 35(※"SIU_compti"が"true"の場合、37)
   */
  OutPort<RTC::TimedShortSeq> m_WiiClassicCOut;


  RTC::TimedShortSeq m_WiiBalanceB;
  /*!
   * WiiBalanceBoardから取得したデータを送信する。
   * [0] 電源ボタンの状態(BOOL 値)
   * [1] X軸の重心座標
   * [2] Y軸の重心座標
   * [3] 左上にかかる重量データ
   * [4] 右上にかかる重量データ
   * [5] 右下にかかる重量データ
   * [6] 左下にかかる重量データ
   * [7] 左上センサの生データ
   * [8] 右上センサの生データ
   * [9] 右下センサの生データ
   * [10] 左下センサの生データ
   * [11] バッテリー残量(%)
   * [12] LEDの状態（BYTE値）
   * ※コンフィギュレーションの"SIU_compti"が"true"の場合、[0]~[10
   * ]のデータを送信する。
   * - Type: TimedShortSeq
   * - Number: 13(※SIU_comptiがtrueの場合、11)
   */
  OutPort<RTC::TimedShortSeq> m_WiiBalanceBOut;
  RTC::TimedShortSeq m_WiiMotionP;
  /*!
   * WiiMotionPlusコントローラから取得したデータを送信する。
   * 尚、[0]から[13]までがWiiリモコン本体の情報、それ以降はWiiMoti
   * onPlusの情報となっている。
   * [0] 十字キー・上ボタンの状態(BOOL 値)
   * [1] 十字キー・右ボタンの状態(BOOL 値)
   * [2] 十字キー・下ボタンの状態(BOOL 値)
   * [3] 十字キー・左ボタンの状態(BOOL 値)
   * [4] A ボタンの状態(BOOL 値)
   * [5] B ボタンの状態(BOOL 値)
   * [6] ＋ボタンの状態(BOOL 値)
   * [7] Home ボタンの状態(BOOL 値)
   * [8] －ボタンの状態(BOOL 値)
   * [9] 1 ボタンの状態(BOOL 値)
   * [10] 2 ボタンの状態(BOOL 値)
   * [11] X軸方向の加速度
   * [12] Y軸方向の加速度
   * [13] Z軸方向の加速度
   * [14] MotionPlus のyaw の生データ
   * [15] MotionPlus のpitch の生データ
   * [16] MotionPlus のroll の生データ
   * [17] MotionPlus のyaw を速度に変換した値
   * [18] MotionPlus のpitch を速度に変換した値
   * [19] MotionPlus のroll を速度に変換した値
   * [20] バッテリー残量(%)
   * [21] LEDの状態
   * ※コンフィギュレーションの"SIU_compti"が"true"の場合、[0]~[19
   * ]のデータを送信する。
   * - Type: TimedShortSeq
   * - Number: 22(※SIU_comptiがtrueの場合、19)
   */
  OutPort<RTC::TimedShortSeq> m_WiiMotionPOut;
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:
	 wiimote wiiremote;
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

};


extern "C"
{
  DLL_EXPORT void WiimoteRTCInit(RTC::Manager* manager);
};

#endif // WIIMOTERTC_H
