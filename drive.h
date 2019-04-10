/*
==============================================================
 Name        : drive.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 走行関連の関数たちです．

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
	#define FORWARD	0x00			//前進向き
	#define BACK	0x11			//後退
	#define TURN_L	0x01			//回転向き(左)
	#define TURN_R	0x10			//回転向き(右)

	//----方向転換用定数----
	#define DIR_TURN_R90	0x01	//右90度回転
	#define DIR_TURN_L90	0xff	//左90度回転
	#define DIR_TURN_180	0x02	//180度回転


	//====変数====
#ifdef EXTERN								//対応ファイルでEXTERNが定義されている場合
	volatile float vel_R, vel_L,vel_G,xR, xL,xG;			//現在速度，距離変化
	volatile float targ_vel, max_vel_G;				//目標速度
	volatile float targ_total_mm;					//制止するための目標距離
	
	/***速度のPID計算用***/
	volatile float dif_vel_R,dif_vel_L;				//速度偏差
	volatile float dif_pre_vel_R,dif_pre_vel_L;			//速度偏差
	volatile float kvpR,kvdR,kviR,kvpL,kvdL,kviL;			//PID制御量
	volatile int8_t vel_direction_R,vel_direction_L;
	volatile float vpid_R,vpid_L;
	
	/***位置のPID計算***/
	volatile float dif_x_R,dif_x_L,dif_x_G;
	volatile float dif_pre_x_R,dif_pre_x_L,dif_pre_x_G;
	volatile float kxpR,kxdR,kxdL,kxpL,kxpG,kxdG;
	volatile float xpid_R,xpid_L,xpid_G;
	
	/***角速度のPID計算***/
	volatile float targ_omega;
	volatile float dif_omega;
	volatile float vel_omega;
	volatile float dif_pre_omega;
	volatile float kwpG,kwdG,kwiG;
	volatile float wpid_G;
	volatile int8_t omega_direction;
	
	/*** 角度のPID計算 ***/
	volatile float targ_angle;
	volatile float dif_angle,dif_pre_angle;
	volatile float kapG,kadG;
	volatile float apid_G; 
	
	volatile uint16_t time,time2, ms_time;				//
	volatile float minindex, maxindex,maxindex_w;		//時間・加速必要時間・角加速必要時間？

	volatile float duty_fix_gain_R,duty_fix_gain_L;			//旋回時の残留偏差消すための補正係数
	
	volatile float duty_r,duty_l;					//Duty比計算と計算オーバー用のバッファ
	volatile float Kvolt,Kxr;					//加速度計算するための電流定数，距離変換のための定数
	volatile float accel;						//目標加速度？
		
	volatile uint16_t pulse_l, pulse_r;								//左右パルスカウント
	volatile int32_t dif_pulse_r, dif_pulse_l,pulse_flag_l, pulse_flag_r;				//エンコーダ変化値，オバフ用フラグ
	volatile int32_t dif_pulse_counter_r, dif_pulse_counter_l;
	volatile float totalR_mm, totalL_mm,totalG_mm;							//左，右，重心走行距離[mm]
	volatile float offsetR_mm, offsetL_mm, offsetG_mm, offsetA;					//停止に必要なオフセット距離
	volatile float sen_dl, sen_dr;									//壁センサによる比例制御量
	
#else									//対応ファイルでEXTERNが定義されていない場合
	/*グローバル変数の宣言*/			
	extern volatile float dif_pre_vel_R, dif_pre_vel_L;
	extern volatile float vel_R,vel_L,vel_G,xR, xL,xG;		 //両輪速度
	extern volatile float targ_vel,max_vel_G;	       		 //目標速度
	extern volatile float targ_total_mm;
	
	/***速度のPID計算用***/
	extern volatile float dif_vel_R, dif_vel_L;
	extern volatile float kvpR,kvdR,kviR,kvpL,kvdL,kviL;
	extern volatile int8_t vel_direction_R,vel_direction_L;
	extern volatile float vpid_R,vpid_L;

	/***位置のPID計算***/
	extern volatile float dif_x_R,dif_x_L,dif_x_G;
	extern volatile float dif_pre_x_R,dif_pre_x_L,dif_pre_x_G;
	extern volatile float kxpR,kxdR,kxdL,kxpL,kxpG,kxdG;	
	extern volatile float xpid_R,xpid_L,xpid_G;
	
	/***角速度のPID計算***/
	extern volatile float targ_omega;
	extern volatile float dif_omega;
	extern volatile float vel_omega;
	extern volatile float dif_pre_omega;
	extern volatile float kwpG,kwdG,kwiG;
	extern volatile float wpid_G;
	extern volatile int8_t omega_direction;
	
	/*** 角度のPID計算 ***/
	extern volatile float targ_angle;
	extern volatile float dif_angle,dif_pre_angle;
	extern volatile float kapG,kadG;
	extern volatile float apid_G; 
	
	extern volatile uint16_t time,time2, ms_time;
	extern volatile float minindex, maxindex,maxindex_w;		//最低速度・最大速度
	extern volatile float duty_fix_gain_R,duty_fix_gain_L;
	
	extern volatile float duty_r,duty_l;
	extern volatile float Kvolt,Kxr;			
	extern volatile float accel;
	
	extern volatile uint16_t pulse_l, pulse_r;		
	extern volatile int32_t dif_pulse_r, dif_pulse_l,pulse_flag_l, pulse_flag_r;
	extern volatile int32_t dif_pulse_counter_r, dif_pulse_counter_l;
	extern volatile float totalR_mm, totalL_mm,totalG_mm;
	extern volatile float offsetR_mm, offsetL_mm,offsetG_mm,offsetA;
	extern volatile float sen_dl, sen_dr;				
	
	
#endif

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====走行系====
	//----基幹関数----
	void driveA(float);	//加速走行
	void driveD(uint16_t, unsigned char);	//減速走行
	void driveAD(float);	//加減速走行
	void driveU(uint16_t, unsigned char);	//使ってない
	void driveC(uint16_t, unsigned char);	//定時間走行，セッポジぐらいしか使ってない
	void driveX(uint16_t);			//位置・角度制御走行
	void driveW(int16_t);			//角速度制御走行
	
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
	
	void turn_SLA_R90();
	void turn_SLA_L90();

	//----走行試験----
	void test_drive(char *mode);	//走行試験
	
	void start_ready();
	

#endif /* DRIVE_H_ */
