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
	//----センサ系----
	tp = 0;
	ad_l = ad_r = ad_ff = ad_fr = ad_fl = 0;
	ad_r_off = ad_fr_off = ad_ff_off = ad_fl_off = ad_l_off = 0;
	base_l = base_r = 0;
	pulse_sum_l = pulse_sum_r = 0;
	time = time2 = 0;
	velR0 = velL0 = 1;

	//足回り系？
	totalR_mm = totalL_mm = totalG_mm = 0;
	dif_pre_vel_R = dif_pre_vel_L = 0;
	dif_pre_x_R = dif_pre_x_L = 0;
	t_cnt_r = t_cnt_l = t_cnt_w = 0;
	kvpR = kvdR = kviR = kvpL = kvdL = kviL = 0;
	kxpR = kxdR = kxpL = kxdL = 0;
	section_count = 0;
	
	duty_r = duty_l = 0;
	vpid_R = vpid_L = xpid_R = xpid_L = apid_G = wpid_G = 0;
	
	//ジャイロ系
	omega_G = angle_G;
	dif_pre_omega = 0;
	dif_omega = 0;
	kwpG = kwdG = 0;
	gyro_base = 0;
	pre_omega_G = 0;
	
	Cont_kp[0] = CONT0;
	Cont_kp[1] = CONT1;
	Cont_kp[2] = CONT2;
	Cont_kp[3] = CONT3;
	Cont_kp[4] = CONT4;
	
	cont_r = cont_l = Cont_kp[0];
	/* 回転速度，計算処理 */
	accel_omega = 240;			//rad/s/s
	max_omega_G = 3.88;			//rad/s
	angle_G = 0;
	dif_angle = 0;
	kapG = kadG = 0;
	apid_G  = 0;
	dif_pre_angle = 0;
	
	
	for(i=0;i<2000;i++){
		targ_omega[i] = accel_omega * 0.001 * i;
		//uart_printf("%lf\r\n",targ_omega[i]);
		if(targ_omega[i] > max_omega_G){
			targ_vel[i] = max_omega_G;
			maxindex_w = i;			//最高速度初期化     
			minindex = MINSPEED_S;		//最低速度初期化     MINSPEED_Sはglobal.hにマクロ定義あり
			break;
		}
	}
	
	/* 並進速度，計算処理  */
	accel = 1.0;
	max_vel_G = 0.35;
	
	for(i=0;i<2000;i++){
		targ_vel[i] = accel * 0.001 * i;
		
		if(targ_vel[i] > max_vel_G){
			targ_vel[i] = max_vel_G;
			maxindex = i;			//最高速度初期化     
			minindex = MINSPEED_S;		//最低速度初期化     MINSPEED_Sはglobal.hにマクロ定義あり
			break;
		}
	}
	//----走行系----
/*	maxindex = MAXSPEED_S;			//最高速度初期化     MAXSPEED_Sはglobal.hにマクロ定義あり
*/	minindex = MINSPEED_S;			//最低速度初期化     MINSPEED_Sはglobal.hにマクロ定義あり
	MF.FLAGS = 0x80;			//フラグクリア＆停止状態  0x80=0b10000000

	//----探索系----
	goal_x = GOAL_X;        		//GOAL_Xはglobal.hにマクロ定義あり
	goal_y = GOAL_Y;        		//GOAL_Yはglobal.hにマクロ定義あり
	map_Init();						//マップの初期化
	PRELOC.PLANE = 0x00;			//現在地の初期化
	m_dir = 0;				//マウス方向の初期化

	Kvolt = MASS / 2 * DIA_SQUR_mm / DIA_PINI_mm * DIA_WHEEL_mm / Ktolk * Rmotor;
	Kxr =  -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi / 4096;
	
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
		volt_bat += ad_res[6];
		ms_wait(1);
	}

	if(volt_bat < 3000 && 1000 < volt_bat){
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