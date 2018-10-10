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
	uint16_t i,j,res_data;
	float ttt;
	//====初期化====
	R_PG_Clock_Set();					//クロック設定

	port_Init();						//portの初期化
	val_Init();							//各種変数の初期化
	timer_Init();						//タイマ系の初期化
	sensor_Init();						//センサ系の初期化
	uart_Init();					//シリアル通信の初期化

/*	while(1){
		ttt = GYRO_read();
		uart_printf("GYRO :%lf \r\n",ttt);	
		ms_wait(100);
	}
*/
/*	S12AD.ADANS0.WORD = 0x40;
	R_PG_ADC_12_StartConversionSW_S12AD0();					
	R_PG_ADC_12_GetResult_S12AD0(ad_res);
	volt_bat = ad_res[6];
	ms_wait(100);

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
*/
/*		
	set_dir(FORWARD);
	sensor_start();
	drive_start();
	ms_wait(100);
		
	targ_vel_L = 0.5;
	targ_vel_R = 0.5;
		
		while(time < 600){
			if(time > 300){
				targ_vel_L = 1.0;
				targ_vel_R = 1.0;
			}
		}
		totalR_mm = 0;
		while(time < 1200)
			if(time > 900){
				targ_vel_L = 0.5;
				targ_vel_R = 0.5;
				offsetR_mm = 0.5 * targ_vel_R * OFFDT;
			}
		while((totalR_mm - offsetR_mm) < 180){
			if(time > 300){
				targ_vel_L = 0.5;
				targ_vel_R = 0.5;
				offsetR_mm = 0.5 * targ_vel_R * OFFDT;
			}
		}	
		targ_vel_L = 0;
		targ_vel_R = 0;
		ms_wait(200);			//速度がゼロに収束するまで待つ
		
			
*/				
	set_dir(FORWARD);
	sensor_start();
	//set_position();
/*	half_sectionA();
	half_sectionD();
	
	half_sectionA();
	half_sectionD();

	half_sectionA();
	half_sectionD();
*/
	sensor_stop();
		
	//a_section();
	melody(1120,1000);

	while(1){ // Main Loop
		uart_printf("Hello, World!\r\n");	
		//====モードセレクト====
		select_mode(&mode);
		ms_wait(100);
		//----選択項目の実行----
		switch(mode){
		case 0:	//----基準値を取る----
			get_base();											//get_base()はsensor.cに関数定義あり 　制御のための壁基準値取得
			//----情報をシリアル送信----
			//uart_printf("base_l = %3d, ", base_l);				//UART_printf()はuart.cに関数定義あり
			//uart_printf("base_r = %3d\r", base_r);
			ms_wait(500);
			uart_printf("START\r\n");
			for(i=0;i<2000;i++){
				uart_printf("%lf, %lf,%lf, %lf\r\n",test_valR1[i],test_valL1[i],test_valR[i],test_valL[i]);
				ms_wait(1);
			}
			uart_printf("ALL\r\n");

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
			turn_dir(DIR_TURN_180);
			break;

			/////////////////////////////////　　↓の二次探索走行とスラローム走行は未実装
			//----二次高速走行----
		case 3:
		
			
			break;

			//----スラローム走行----
		case 4:
			break;
			//////////////////////////////////

			//----走行テスト----
		case 5:
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
		case 6:
			Wait;
			start_wait();
			enc_test();
			ms_wait(100);
			break;
			
			
			//----センサ値, 差を確認----
			//LED点灯は要変更
		default:
			Wait;
			val_Init();
			MF.FLAG.CTRL = 1;	//制御許可
			R_PG_Timer_StartCount_CMT_U0_C1();
			while(1){
				pins_write(DISP_LEDS, 0, LED_NUM);											//pins_write()はport.cに関数定義あり
				uart_printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d ", ad_l, ad_fl, ad_ff, ad_fr, ad_r);
				uart_printf(" | dif_l: %4d dif_r:%4d\r\n", dif_l, dif_r);
				//----LEDが4つの場合----
				if(ad_fr > WALL_BASE_F){
					// ここ、ad_lになってましたよ！！
					pin_write(DISP_LEDS[0], ON);											//pin_write()はport.cに関数定義あり
				}
				if(ad_r > WALL_BASE_R){
					pin_write(DISP_LEDS[1], ON);
				}
				if(ad_l > WALL_BASE_L){
					pin_write(DISP_LEDS[2], ON);
				}
				if(ad_fl > WALL_BASE_F){
					pin_write(DISP_LEDS[3], ON);
				}
				ms_wait(1000);

			}
			MF.FLAG.CTRL = 0;
			ms_wait(100);
			break;
		}
		ms_wait(100);
	}

	return 0 ;
}
