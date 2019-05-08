/*
==============================================================
 Name        : auxiliary.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 補助関数たちです．労わりましょう．

 更新履歴
 2015/1/30		山上	一部コメント追加
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
*/
#include"pinRX631.h"

#ifndef AUXILIARY_H_									//対応ファイルで一度も読み込まれていない場合以下を定義
	#define AUXILIARY_H_								//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----スイッチ関連----
	//Custom
	#define	INC		1	//加算ボタン
	#define	RET		1	//決定ボタン
	#define	DEC		1	//減算ボタン

	//----汎用----
	#define ON 1
	#define OFF 0

	typedef struct{
		//物理量
		float vel_max;
		float accel;
		float omega_max;
		float omega_accel;
		
		//オフセット長さ
		uint8_t R90_before;
		uint8_t R90_after;
		uint8_t L90_before;
		uint8_t L90_after;

	} params;
	
	typedef struct{
		float vel_kpR;
		float vel_kpL;
		float vel_kiR;
		float vel_kiL;
		float omega_kp;
		float omega_ki;
		float wall_kp;
		float wall_kd;
	} gain;
	
	typedef struct{
		float test1[2000];
		float test2[2000];
		float test3[2000];
		float test4[2000];
		float test5[2000];
		float test6[2000];
	} data_array;
		
	//----表示用LED関連----
	#ifdef EXTERN										//対応ファイルでEXTERNが定義されている場合
		/*グローバル変数の定義*/
		uint16_t DISP_LEDS[4] = {PB6, PB7, PC2,PC3};	//表示用LEDのピンの配列
		volatile params params_now;
		volatile gain gain_now;
		volatile params params_search1;
		volatile gain gain_search1;
//		volatile data_array log; 
	#else											//対応ファイルでEXTERNが定義されていない場合
		/*グローバル変数の宣言*/
		extern uint16_t DISP_LEDS[4];
		extern volatile params params_now;
		extern volatile gain gain_now;
		extern volatile params params_search1;
		extern volatile gain gain_search1;
//		extern volatile data_array log; 

	#endif

	//----LEDの数----
	#define LED_NUM 4

	//----その他----
	#define DEFWAIT		10000						//wait()用定数
	#define Wait		ms_wait(200)					//少し待機する

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====その他====
	void stay(unsigned int);
	void ms_wait(unsigned int);
	void select_mode(char *mode);						//モードの入力
	void timer_start();
	void melody(uint16_t,uint16_t);
	void start_wait();
	void start_ready();
	
	void auto_Calibration(float,float);
	void setting_params(params);
	void setting_gain(gain);
	
	void ctrl_zero();
	void reset_distance();

#endif /* AUXILIARY_H_ */
