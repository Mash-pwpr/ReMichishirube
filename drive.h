/*
==============================================================
 Name        : drive.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 走行関連の関数たちです．

  更新履歴
 2015/12/11		山上	コメントとSLA7078用のマクロを追加
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
*/

/*ヘッダファイルの読み込み*/
#include "port.h"

#ifndef DRIVE_H_

	#define DRIVE_H_									//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----動作方向関連----
//	#define MT_FWD_L	1			//CW/CCWで前に進む出力(左)
	#define MT_BACK_L	0			//CW/CCWで後ろに進む出力(左)
	#define MT_FWD_R	1			//CW/CCWで前に進む出力(右)
	#define MT_BACK_R	0		//CW/CCWで後ろに進む出力(右)

	#define FORWARD	0x00			//前進向き
	#define BACK	0x11			//後退
	#define TURN_L	0x01			//回転向き(左)
	#define TURN_R	0x10			//回転向き(右)

	//----方向転換用定数----
	#define DIR_TURN_R90	0x01	//右90度回転
	#define DIR_TURN_L90	0xff	//左90度回転
	#define DIR_TURN_180	0x02	//180度回転


	//====変数====
#ifdef EXTERN										//対応ファイルでEXTERNが定義されている場合
	/*グローバル変数の定義*/
	const uint16_t table[] = {
		#include "table.h"
	};								//table.hに貼り付けた値を保持する配列
	volatile float vel_R, vel_L,vel_G,xR, xL,xG;			//現在速度，距離変化
	volatile float targ_vel[2000],max_vel_G;			//目標速度
	volatile float targ_total_mm;					//制止するための目標距離
	
	/***速度のPID計算用***/
	volatile float dif_vel_R,dif_vel_L;				//速度偏差
	volatile float dif_pre_vel_R,dif_pre_vel_L;			//速度偏差
	volatile float kvpR,kvdR,kviR,kvpL,kvdL,kviL;			//PID制御量
	volatile int16_t velR0,velL0;
	volatile float vpid_R,vpid_L;
	
	/***位置のPID計算***/
	volatile float dif_x_R,dif_x_L;
	volatile float dif_pre_x_R,dif_pre_x_L;
	volatile float kxpR,kxdR,kxdL,kxpL;
	volatile float xpid_R,xpid_L;
	
	/***角速度のPID計算***/
	volatile float targ_omega[2000],max_omega_G,accel_omega;
	volatile float dif_omega;
	volatile float dif_pre_omega;
	volatile float kwpG,kwdG;
	volatile float wpid_G;
	
	/*** 角度のPID計算 ***/
	volatile float targ_angle;
	volatile float dif_angle,dif_pre_angle;
	volatile float kapG,kadG;
	volatile float apid_G; 
	
	
	volatile uint16_t t_cnt_l, t_cnt_r,t_cnt_w;
	volatile uint16_t time, ms_time;		//テーブルカウンタ
	volatile uint16_t minindex, maxindex,maxindex_w;				//最低速度・最大速度
	volatile uint16_t interval_l, interval_r;			//左右インターバル
	
	volatile float duty_r,duty_l,duty_oR,duty_oL;			//Duty比計算と計算オーバー用のバッファ
	volatile float Kvolt,Kxr;					//加速度計算するための電流定数，距離変換のための定数
	volatile float accel;					//目標加速度？
	
	volatile float test_valR[2000],test_valL[2000],test_valR1[2000],test_valL1[2000];			//速度ログなどの保存用配列
	volatile uint16_t test_valR2[1000],test_valL2[1000]; 	//エンコーダ獲得値などの保存用配列
	
	
	volatile uint16_t pulse_l, pulse_r, pulse_pre_r, pulse_pre_l;					//左右パルスカウント
	volatile int32_t dif_pulse_r, dif_pulse_l,pulse_sum_l, pulse_sum_r;				//エンコーダ変化値，オバフ用フラグ
	volatile float totalR_mm, totalL_mm,totalG_mm;							//左，右，重心走行距離[mm]
	volatile float offsetR_mm, offsetL_mm,offsetG_mm;								//停止に必要なオフセット距離
	volatile float sen_dl, sen_dr;										//壁センサによる比例制御量
	
	
#else									//対応ファイルでEXTERNが定義されていない場合
	/*グローバル変数の宣言*/
	extern const uint16_t table[];				
	extern volatile float dif_pre_vel_R, dif_pre_vel_L;
	extern volatile float vel_R,vel_L,vel_G,xR, xL,xG;		//両輪速度
	extern volatile float targ_vel[2000],max_vel_G;	        //目標速度
	extern volatile float targ_total_mm;
	
	/***速度のPID計算用***/
	extern volatile float dif_vel_R, dif_vel_L;
	extern volatile float kvpR,kvdR,kviR,kvpL,kvdL,kviL;
	extern volatile int16_t velR0,velL0;
	extern volatile float vpid_R,vpid_L;

	/***位置のPID計算***/
	extern volatile float dif_x_R,dif_x_L;
	extern volatile float dif_pre_x_R,dif_pre_x_L;
	extern volatile float kxpR,kxdR,kxdL,kxpL;	
	extern volatile float xpid_R,xpid_L;
	
	/***角速度のPID計算***/
	extern volatile float targ_omega[2000],max_omega_G,accel_omega;
	extern volatile float dif_omega;
	extern volatile float dif_pre_omega;
	extern volatile float kwpG,kwdG;
	extern volatile float wpid_G;
	
	/*** 角度のPID計算 ***/
	extern volatile float targ_angle;
	extern volatile float dif_angle,dif_pre_angle;
	extern volatile float kapG,kadG;
	extern volatile float apid_G; 

	
	extern volatile uint16_t t_cnt_l, t_cnt_r,t_cnt_w;
	extern volatile uint16_t time, ms_time;
	extern volatile uint16_t minindex, maxindex,maxindex_w;		//最低速度・最大速度
	extern volatile uint16_t interval_l, interval_r;	//左右インターバル
	
	extern volatile float duty_r,duty_l,duty_oR,duty_oL;
	extern volatile float Kvolt,Kxr;			
	extern volatile float accel;
	
	extern volatile float test_valR[2000],test_valL[2000],test_valR1[2000],test_valL1[2000];
	extern volatile uint16_t test_valR2[1000],test_valL2[1000]; 
	
	extern volatile uint16_t pulse_l, pulse_r,pulse_pre_r, pulse_pre_l;		
	extern volatile int32_t dif_pulse_r, dif_pulse_l,pulse_sum_l, pulse_sum_r;
	extern volatile float totalR_mm, totalL_mm,totalG_mm;
	extern volatile float offsetR_mm, offsetL_mm,offsetG_mm;
	extern volatile float sen_dl, sen_dr;					
	
	
#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====走行系====
	//----基幹関数----
	void driveA(float);	//加速走行
	void driveD(uint16_t, unsigned char);	//減速走行
	void driveAD(float, unsigned char);	//加減速走行
	void driveU(uint16_t, unsigned char);	//等速走行(前の速度を維持)
	void driveC(uint16_t, unsigned char);	//定速走行(あらかじめ決まった速度)
	void set_dir(unsigned char);		//進む方向の設定

	void drive_start();
	void drive_stop(unsigned char);

	//----簡易上位関数----
	void half_sectionA();		//加速半区画
	void half_sectionA2();
	void half_sectionD();		//減速半区画
	void a_section();		//加減速一区画
	void a_sectionU();		//等速一区画
	void s_section();		//連続区画直線走行
	void turn_R90();		//右90回転
	void turn_L90();		//左90回転
	void turn_180();		//180度回転
	void set_position();		//位置合わせ

	//----走行試験----
	void test_drive(char *mode);	//走行試験
	
	void start_ready();
	

#endif /* DRIVE_H_ */
