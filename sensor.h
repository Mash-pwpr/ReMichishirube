/*
==============================================================
 Name        : sensor.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : センサ関連の関数たちです．崇め奉りましょう．

  更新履歴
 2016/1/29		山上	コメント追加
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
*/


#include "pinRX631.h"
#include <stdint.h>
#ifndef SENSOR_H_											//対応ファイルで一度も読み込まれていない場合以下を定義
	#define SENSOR_H_										//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----LEDのON/OFF出力----
	#define VLED_ON		1
	#define VLED_OFF	0

	#define SHIFT		6									//A-D変換シフト値

	//====変数====
	#ifdef EXTERN
		/*グローバル変数の定義*/
		//----LEDポート----
		uint16_t VLED_FL = PE0;								//前向き左側センサ発光部のLEDのポート
		uint16_t VLED_L = PE1;									//左側センサ発光部のLEDのポート
		uint16_t VLED_FF = PE2;
		uint16_t VLED_R = PE3;								//右側センサ発光部のLEDのポート
		uint16_t VLED_FR = PE4;								//前向き右側センサ発光部のLEDのポート
		//----その他----
		unsigned char tp;											//タスクポインタ
		volatile int16_t ad_r, ad_fr, ad_ff, ad_fl, ad_l;						//A-D値格納
		volatile int16_t ad_r_off, ad_fr_off, ad_ff_off, ad_fl_off,ad_l_off;
		volatile uint16_t ad_res[5],ad_pre_res[5];
		volatile uint16_t base_l, base_r;								//基準値を格納
		volatile int16_t dif_l, dif_r;									//AD値と基準との差
		volatile uint16_t volt_bat;									//電源電圧監視
		volatile float cont_r,cont_l;										//壁制御の比例定数
		volatile float Cont_kp[5];									//壁制御の場合分け
		volatile float omega_G, angle_G,omega_G_rad;
	#else
		//----発光LEDポート----
		extern uint16_t VLED_R;
		extern uint16_t VLED_FR;
		extern uint16_t VLED_FF;
		extern uint16_t VLED_FL;
		extern uint16_t VLED_L;
		//----その他----
		extern unsigned char tp;
		extern volatile int16_t ad_r, ad_fr, ad_ff, ad_fl, ad_l;
		extern volatile int16_t ad_r_off, ad_fr_off, ad_ff_off, ad_fl_off,ad_l_off;
		extern volatile uint16_t ad_res[5],ad_pre_res[5];
		extern volatile uint16_t base_l, base_r;
		extern volatile int16_t dif_l, dif_r;
		extern volatile uint16_t volt_bat;
		extern volatile float omega_G, angle_G,omega_G_rad;
		
		extern volatile float cont_r,cont_l;										
		extern volatile float Cont_kp[5];

	#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====センサ系====
	unsigned char get_base();					//センサ基準値を取得
	void get_wall_info();				//壁情報を読む
	void enc_test();
	void sensor_start();
	void sensor_stop();
	
#endif /* SENSOR_H_ */
