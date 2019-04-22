/*
==============================================================
 Name        : init.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 初期化関数たちです．

 更新履歴
 2015/12/4		山上	一部コメント追加、表示用LEDの設定を追加
 2016/2/6		深山	一部コメント追加
 2016/2/8		山上	port_Init関数内にPIN_SET(MDR_M3, 0);を追加
 2016/2/12		山上	sensor_Init関数内に発光部のIOCON設定を追加
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
 */

#include "global.h"
/*============================================================
		初期化関数
============================================================*/
// 各種入出力の設定
void port_Init(void){
	PORTB.PMR.BIT.B6 = 0;
	PORTB.PMR.BIT.B7 = 0;

	PORTB.PDR.BIT.B6 = 0;
	PORTB.PDR.BIT.B7 = 0;
	
	PORTB.PCR.BIT.B6 = 0;
	PORTB.PCR.BIT.B7 = 0;
	
	PORT.PSRA.BIT.PSEL6 = 0;
	PORT.PSRA.BIT.PSEL7 = 0;
	
	//I/O関係のポート設定
	R_PG_IO_PORT_Set_P5();		//モタドラへの指示用ピン
	R_PG_IO_PORT_Set_PA();		//モタドラへの指示用ピン
	R_PG_IO_PORT_Set_PC();		//SPI用ピン

	PORTB.PDR.BIT.B6 = 1;
	PORTB.PDR.BIT.B7 = 1;
	PORTB.PODR.BIT.B6 = 1;
	PORTB.PODR.BIT.B7 = 1;
				
	set_dir(FORWARD);			//進行方向を前へ

}


// センサー設定
void sensor_Init(void){
	//センサ受光部のポート設定 左から順に
	R_PG_ADC_12_Set_S12AD0();
	//センサ発光部初期設定
	R_PG_IO_PORT_Set_PE();
	
}

// センサ系，走行系，探索系　の変数初期化
void val_Init(void){
	int i;
	float val1 = 0;
	//----センサ系----
	tp = 0;
	wall_l.dif = wall_r.dif = wall_fl.dif = wall_fr.dif = wall_ff.dif = 0;
	wall_l.val = wall_r.val = wall_fl.val = wall_fr.val = wall_ff.val = 0;
	wall_l.base = wall_r.base = wall_fl.base = wall_fr.base = wall_ff.base = 0;
	wall_l.threshold = wall_r.threshold = wall_fl.threshold = wall_fr.threshold = wall_ff.threshold = 0;
	dif_pulse_counter_r = dif_pulse_counter_l = 0;
	pulse_flag_l = pulse_flag_r = 0;
	time = time2 = 0;
	vel_direction_R = vel_direction_L = 1;	//モータの回転方向とりあえず1で旋回にしておく

	//足回り系？
	totalR_mm = totalL_mm = totalG_mm = 0;
	dif_pre_vel_R = dif_pre_vel_L = 0;
	dif_pre_x_R = dif_pre_x_L = 0;
	kvpR = kvdR = kviR = kvpL = kvdL = kviL = 0;
	kxpR = kxdR = kxpL = kxdL = 0;
	vel_omega = 0;
	duty_fix_gain_R = 1.0;
	duty_fix_gain_L = 1.0;
	duty_r = duty_l = 0;
	vpid_R = vpid_L = xpid_R = xpid_L = apid_G = wpid_G = 0;
	
	//パラメータ設定
	params_search1.vel_max = 0.50f;						//単位はm/s, mm/ms
	params_search1.accel = 4.0f;						//単位はm/s/s
	
	val1 = HALF_MM / params_search1.vel_max * 0.001; 			//並進速度で半区画進む時間[s]
	params_search1.omega_max = 1.5 * Pi / 2 / val1;				//最高角速度[rad/s]
	params_search1.omega_accel = 3 * params_search1.omega_max / val1; 	//角加速度[rad/s/s]
	
	params_search1.R90_before = 30;		//35
	params_search1.R90_after = 45;
	params_search1.L90_before = 35;
	params_search1.L90_after = 50;
	
/*	uart_printf("tim is %lf\r\n", val1);
	uart_printf("omega is %lf\r\n", val2);
	uart_printf("omega_accel is %lf\r\n",val3);
	
	while(1);
*/	
	
//	params_search1.omega_max = 6.0f;			//単位はrad/s
//	params_search1.omega_accel = 25.0f;		//単位はrad/s/s
		
	gain_search1.vel_kpR = 14.0f;		//14.0	
	gain_search1.vel_kpL = 14.0f;			
	gain_search1.vel_kiR = 0.05f;		//0.05
	gain_search1.vel_kiL = 0.05f;
	gain_search1.omega_kp = 3.0f;	//1.3
	gain_search1.omega_ki = 0.3f;		//0.11
	gain_search1.wall_kp = 0.015f;
	gain_search1.wall_kd = 0.002f;
	
	setting_params(params_search1);
	setting_gain(gain_search1);
	
	//ジャイロ系
	omega_G = angle_G = 0;
	dif_pre_omega = 0;
	dif_omega = 0;
	kwpG = kwdG = 0;
	gyro_base = 0;
	pre_omega_G = 0;
		
	/* 回転速度，計算処理 */
	angle_G = 0;
	dif_angle = 0;
	kapG = kadG = 0;
	apid_G  = 0;
	dif_pre_angle = 0;

	maxindex_w = val1 / 3;					//回転加速時間計算
	minindex = 0;						//最低速度初期化
	
	/* 並進速度，計算処理  */
	maxindex = params_now.vel_max / params_now.accel;	//並進加速時間計算    
	minindex = 0;						//最低速度初期化
	
	//----走行系----
	minindex = MINSPEED_S;			//最低速度初期化     MINSPEED_Sはglobal.hにマクロ定義あり
	MF.FLAGS = 0x80;			//フラグクリア＆停止状態  0x80=0b10000000

	//----探索系----
	goal_x = GOAL_X;        		//GOAL_Xはglobal.hにマクロ定義あり
	goal_y = GOAL_Y;        		//GOAL_Yはglobal.hにマクロ定義あり
	map_Init();				//マップの初期化
	PRELOC.PLANE = 0x00;			//現在地の初期化
	m_dir = 0;				//マウス方向の初期化

	Kvolt = MASS / 2 * DIA_SQUR_mm / DIA_PINI_mm * DIA_WHEEL_mm / Ktolk * Rmotor; //電源電圧測定に必要な定数
	Kxr =  -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi / 4096;	      //エンコーダ値を物理量に変換するのに必要な定数
	
	//---テスト用配列初期化
	for(i=0;i<2000;i++){
		log.test1[i] = 0;
		log.test2[i] = 0;
		log.test3[i] = 0;
		log.test4[i] = 0;
		log.test5[i] = 0;
		log.test6[i] = 0;
	}	
}

// タイマ初期化
void timer_Init(void){

	R_PG_Timer_Set_CMT_U0_C0();
	R_PG_Timer_Set_CMT_U0_C1();
	R_PG_Timer_Set_CMT_U1_C2();

	R_PG_Timer_Set_MTU_U0_C0();		//ブザー用初期設定
	R_PG_Timer_Set_MTU_U0_C1();		//右エンコーダ
	R_PG_Timer_Set_MTU_U0_C2();		//左エンコーダ
	R_PG_Timer_Set_MTU_U0_C3();		//左モータ
	R_PG_Timer_Set_MTU_U0_C4();		//右モータ

}

void batt_Check(uint16_t num){
	uint16_t i;

	for(i=0;i<num;i++){
		S12AD.ADANS0.WORD = 0x40;
		R_PG_ADC_12_StartConversionSW_S12AD0();					
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ms_wait(1);
		volt_bat += ad_res[6];
	}
	
	volt_bat = volt_bat / num;

	if(volt_bat < 3000 && 1500 < volt_bat){
		melody(1320,500);
		melody(1120,500);
		melody(920,500);
		R_PG_Timer_StopModule_MTU_U0();
		R_PG_Timer_StopModule_CMT_U0();
		uart_printf("Voltage Out! volt is %d\r\n",volt_bat);
		while(1){
		}
	}	
}