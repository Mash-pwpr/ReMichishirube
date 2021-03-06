﻿/*
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

	typedef struct{
		int16_t val;			//LEDがONの時の値
		int16_t base;			//LEDがOFFの時の値
		int16_t dif;			//差分
		int16_t pre;			//前回のやつ
		int16_t diff;			//微分
		
		uint16_t threshold;	//壁判断閾値	
	}wall_sensor;
	
	typedef struct{
		int16_t epsilon;		//壁制御偏差
		int16_t pre_epsilon;		//D制御用のバッファ
		float sen_pid;
	}wall_control;

	//====変数====
	#ifdef EXTERN
		/*グローバル変数の定義*/
		//----LEDポート----
		uint16_t VLED_FL = PE0;								//前向き左側センサ発光部のLEDのポート
		uint16_t VLED_L = PE1;									//左側センサ発光部のLEDのポート
		uint16_t VLED_FF = PE2;
		uint16_t VLED_R = PE3;								//右側センサ発光部のLEDのポート
		uint16_t VLED_FR = PE4;								//前向き右側センサ発光部のLEDのポート
		
		volatile wall_sensor wall_r;
		volatile wall_sensor wall_fr;
		volatile wall_sensor wall_ff;
		volatile wall_sensor wall_fl;
		volatile wall_sensor wall_l;
		
		//----その他----
		unsigned char tp;											//タスクポインタ
		volatile uint16_t ad_res[5],ad_pre_res[5];
		
		volatile uint16_t base_l, base_r;								//基準値を格納
		volatile int16_t dif_l, dif_r;									//AD値と基準との差
		
		volatile float volt_bat;									//電源電圧監視
		volatile float cont_r,cont_l;									//壁制御の比例定数
		
		volatile float dif_total;
		volatile float pre_dif_total;									//壁のPD制御用
		volatile float sen_ctrl;
		
		volatile float gyro_base;
	#else
		//----発光LEDポート----
		extern uint16_t VLED_R;
		extern uint16_t VLED_FR;
		extern uint16_t VLED_FF;
		extern uint16_t VLED_FL;
		extern uint16_t VLED_L;
		
		//----壁センサ構造体----
		extern volatile wall_sensor wall_r;
		extern volatile wall_sensor wall_fr;
		extern volatile wall_sensor wall_ff;
		extern volatile wall_sensor wall_fl;
		extern volatile wall_sensor wall_l;

		
		//----その他----
		extern unsigned char tp;
		extern volatile uint16_t ad_res[5],ad_pre_res[5];
		extern volatile uint16_t base_l, base_r;
		extern volatile int16_t dif_l, dif_r;
		extern volatile float volt_bat;
		
		extern volatile float cont_r,cont_l;
		
		extern volatile float pre_dif_total;
		extern volatile float dif_total;
		extern volatile float sen_ctrl;
		
		
		extern volatile float gyro_base;

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
	void sensor_check();
	
#endif /* SENSOR_H_ */
