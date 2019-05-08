/************************************************************
		Micromouse プログラム
		2007年~2012年	H8/3694f版が運用される
		2013/03/01		LPC向けに移植					長谷川 健人
		2013/09/20		標準化のため色々弄る				高橋 有礼
 ************************************************************/
/*
==============================================================
 Name        : main.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : main definition

 更新履歴
 2016/2/12　深山　一部コメント追加
 2017/6/27 標　連続探索走行の未完成版を実装
==============================================================
 */

#define EXTERN
#include "global.h"

/*===========================================================
		メイン関数
===========================================================*/
int main(void) {

	//====変数宣言====
	char mode = 0;
	uint16_t i;
//	float ttt;
	//====初期化====
	R_PG_Clock_Set();					//クロック設定

	port_Init();						//portの初期化
	timer_Init();						//タイマ系の初期化
	sensor_Init();						//センサ系の初期化
	uart_Init();						//シリアル通信の初期化
	val_Init();						//各種変数の初期化
	map_Init();

	//ttt = GYRO_read();
	//uart_printf("%lf\r\n",ttt);
	batt_Check(1000);
	//GYRO_OFFSET(1000);
	
//	melody(c6h,1000);		

/*	start_wait();
	GYRO_OFFSET(1000);
*/	
	sensor_start();
	start_wait();
	start_ready();
	
	MF.FLAG.CTRL = 0;
	time = time2 = 0;
	set_dir(FORWARD);
	driveA(100);
	driveD(100,1);


/*	time = time2 = 0;
	MF.FLAG.CTRL = 0;
	driveA(90 + SET_MM);
	driveA(180);
	driveA(180);
	MF.FLAG.CTRL = 0;
	driveD(90,1);	
*/
/*	set_dir(TURN_R);
	turn_R90();
*/	

//	MF.FLAG.CTRL = 1;
//	ms_wait(10);
//	auto_Calibration(0.5,0.5);
//	ms_wait(1000);
//	driveA(SET_MM + 90);
//	turn_SLA_R90();
//	turn_SLA_L90();

/*	while(1){
		driveA(180 * 3);
		turn_SLA_L90();			
	}	
*/		
//	driveD(90,1);
//	turn_SLA_L90();
	
/* サーキット専用プログラム	
	get_base();
	MF.FLAG.CTRL = 1;
	driveA(SET_MM);
	half_sectionA();
	for(i = 0;i<4;i++){
		driveA(14*180 - 90);
		half_sectionD();
		turn_R90();
		driveA(6*180 - 90);
		half_sectionD();
		turn_R90();
	}
	half_sectionA();
	half_sectionD();
*/	
	sensor_stop();

	while(1){ // Main Loop
		uart_printf("Hello, World!\r\n");	
		//====モードセレクト====
		select_mode(&mode);
		ms_wait(100);
		//----選択項目の実行----
		switch(mode){
		case 0:	//----基準値を取る----
			
			//----情報をシリアル送信----
			ms_wait(500);
			uart_printf("START\r\n");
			
			uart_printf("base:%d, %d\r\n", wall_l.threshold, wall_r.threshold);
			//uart_printf("targ\tvelG\tkvpR\tkvpL\t%lf\r\n",offsetA);
/*			for(i=0;i<2000;i++){
				uart_printf("%lf, %lf,%lf, %lf, %lf, %lf\r\n",log.test1[i],log.test2[i],log.test3[i],log.test4[i],log.test5[i],log.test6[i]);
				ms_wait(1);
			}
*/			uart_printf("ALL\r\n");
			break;
			
		case 1:	//----一次探索走行----
			goal_x = GOAL_X;									//ゴール座標を設定　　GOAL_Xはglobal.hにマクロ定義あり
			goal_y = GOAL_Y;									//ゴール座標を設定　　GOAL_Yはglobal.hにマクロ定義あり

			start_wait();
			start_ready();
			
			get_wall_info();									//壁情報の初期化     get_wall_info()はsensor.cに関数定義あり
			searchA();										//ゴール区画まで探索，進行する　searchA()はsearch.cに関数定義あり
			goal_x = goal_y = 0;								//ゴール座標をスタート区画に設定
			Wait;												//待機
			searchA();											//戻ってくる

			goal_x = GOAL_X;									//ゴール座標設定
			goal_y = GOAL_Y;									//ゴール座標設定	
			break;

			//----連続探索走行----
		case 2:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSA();
			goal_x = goal_y = 0;
			searchSA();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			sensor_stop();
			turn_dir(DIR_TURN_180);
			break;

			/////////////////////////////////　　↓の二次探索走行とスラローム走行は未実装
			//----二次高速走行----
		case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSA_ESNW();
			goal_x = goal_y = 0;
			searchSA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);
			break;

			//----スラローム走行----
		case 4:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSLA();
			goal_x = goal_y = 0;
			searchSLA();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);

			break;
			//////////////////////////////////

		case 5:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSLA_ESNW();
			goal_x = goal_y = 0;
			searchSLA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180度回転
			turn_dir(DIR_TURN_180);
			break;

			
			//----走行テスト----
		case 11:
			Wait;
			start_wait();
			set_dir(FORWARD);
			drive_start();
			while(1){
				uart_printf("Driving\r\n");
			}
			test_drive(&mode);									//test_drive()はdrive.cに関数定義あり
			ms_wait(100);
			break;
			//----エンコーダテスト----
		case 12:
			Wait;
			start_wait();
			enc_test();
			ms_wait(100);
			break;
		
		case 13:
			Wait;
			start_wait();
			
			/*宴会芸モード*/
			MF.FLAG.ACTRL = 0;
			MF.FLAG.VCTRL = 1;
			MF.FLAG.WCTRL = 1;
			MF.FLAG.XCTRL = 0;

			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 0;
			MF.FLAG.WACCL = 0;
			MF.FLAG.WDECL = 0;
			
			centor.vel_target = 0;
			omega.target = 0;
			
			drive_start();
			while(1){
//				uart_printf("angle : %lf dif_angle : %lf tpid_G : %lf\r\n");
				ms_wait(100);
			}
			break;
			
			//----センサ値, 差を確認----
			//LED点灯は要変更
		default:
			Wait;
			val_Init();
			start_wait();
			sensor_check();
			ms_wait(100);
			break;
		}
		ms_wait(100);
		sensor_stop();

	}

	return 0 ;
}
