/*
==============================================================
 Name        : auxiliary.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 補助関数たちです．労わりましょう．

 更新履歴
 2015/12/4		山上	一部コメント追加、mode_selectのLED表示部分とmodeの処理を変更
 2015/1/30		山上	一部コメント追加、表示用LEDの設定を追加
==============================================================
*/

/*ヘッダファイルの読み込み*/
#include "global.h"

//※解説の章番号等は日本語版ユーザーマニュアルRev.00.15に準拠

//+++++++++++++++++++++++++++++++++++++++++++++++
//wait
//	ms_waitは重複して使用する(以下参照)ことが出来ない
//	(動作が途中で止まる)。そのため、割り込み関数内では
//	正確な時間は測定できないがwhile文ループを用いて待機する
// 引数1：loop・・・待機するループ数
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void stay(unsigned int loop){
	while(loop--);					//loop回while文を回す
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//ms_wait
//	ミリ秒待機する
// 引数1：ms・・・待機時間[ミリ秒]
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void ms_wait(unsigned int ms){

	// PDGで動作クロックを、システムクロック20MHzを32で割って分周してある
	R_PG_Timer_StartCount_CMT_U0_C0(); // カウントスタート
	
	ms_time = 0;
	while(ms_time < ms){
	}
	
	R_PG_Timer_HaltCount_CMT_U0_C0(); // カウント終了

}

// --※ms_waitが本文と割り込み関数で重複した場合の流れ--
//	本文							割り込み関数
//	ms_wait関数を使用
//	↓
//	SysTickタイマを動作開始
//	↓
//	カウンタが一定数たまるまで待機
//	↓
//	(待機中割り込み発生)		→	ms_wait関数を使用
//								↓
//								(SysTickタイマを動作開始)
//								↓
//								カウンタが一定数たまるまで待機
//								↓
//								待機終了
//								↓
//								カウンタを止める
//								↓
//	カウンタが一定数たまるまで待機	←	(割り込み終了)
//	↓
//	(SysTickタイマが動作していないため
//	カウンタがたまらず以降待機のまま動作せず)


/*------------------------------------------------------------
		モードセレクト
------------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//select_mode
//	モードセレクトを行う
// 引数1：mode・・・モード番号を格納する変数のアドレス
// 引数2 :calc・・・カウント時の音をアップorダウンで定める
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void select_mode(char *mode,char calc){
	uint16_t encR,encL;
	uint16_t nowR = 0;
	uint16_t nowL = 0;
	uint16_t preR;
	
	*mode = 0x00;									//変数の初期化
	R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
	R_PG_Timer_SetCounterValue_MTU_U0_C2(0);
	
	//====変数初期化====
	if(calc > 1){
		uart_printf("First Mode Select\r\n");
	}else {
		uart_printf("Second Mode Select\r\n");
	}
	uart_printf(" mode: 0\r");						//モードをUARTで送信
	
	R_PG_Timer_StartCount_MTU_U0_C1();
	R_PG_Timer_StartCount_MTU_U0_C2();

	//====モード選択処理====
	do{	
		preR = nowR;
		//preL = nowL;
		
		R_PG_Timer_GetCounterValue_MTU_U0_C1(&encL);
		R_PG_Timer_GetCounterValue_MTU_U0_C2(&encR);
		
		nowR = (uint16_t)(encR / 4300);
		nowL = (uint16_t)(encL / 30000);
		
		//ms_wait(50);
		 *mode = nowR;
		//LEDで現在の値を表示
		pins_write(DISP_LEDS, *mode, 4);			//LEDがActiveLowの場合
		if(nowR - preR != 0){
			uart_printf(" mode:%2d\r\n", *mode);
			if(calc > 2) { 
				melody(c6+(65 * *mode),100);
			}else{
				melody(b7 + (65 * *mode),100);	
			}
		}
	}while(nowL != 1);
	
	uart_printf("Finish :  This is mode %2d\r\n", *mode);
	if(calc > 0) { 
		melody(c6+(65 * *mode),500);
	}else{
		melody(b7+(65 * *mode),500);	
	}
	
	R_PG_Timer_HaltCount_MTU_U0_C1();
	R_PG_Timer_HaltCount_MTU_U0_C2();

	R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
	R_PG_Timer_SetCounterValue_MTU_U0_C2(0);
}

void timer_start(){
	R_PG_Timer_StartCount_MTU_U0_C1();
	R_PG_Timer_StartCount_MTU_U0_C2();
	R_PG_Timer_StartCount_MTU_U0_C3();
	R_PG_Timer_StartCount_MTU_U0_C4();

	R_PG_Timer_StartCount_CMT_U0_C1();
	R_PG_Timer_StartCount_CMT_U1_C2();

}

void melody(uint16_t Hz, uint16_t length){
	float count = (float)1000000 / Hz;
	
	//周波数をクロック数に変換
	count = count * 54239;
	count = count / 1130;
	
	R_PG_Timer_SetTGR_A_MTU_U0_C0((uint16_t)(count/10));
	R_PG_Timer_SetTGR_B_MTU_U0_C0((uint16_t)count);
	
	R_PG_Timer_StartCount_MTU_U0_C0();
	ms_wait(length);
	
	R_PG_Timer_SetTGR_A_MTU_U0_C0(479);
	R_PG_Timer_SetTGR_B_MTU_U0_C0(479);
	
	R_PG_Timer_HaltCount_MTU_U0_C0();
	
}

void start_wait(){
	S12AD.ADANS0.WORD = 0x1f;
	R_PG_ADC_12_StartConversionSW_S12AD0();
	R_PG_ADC_12_GetResult_S12AD0(ad_res);
	
	R_PG_Timer_StartCount_CMT_U0_C1();
	uart_printf("Ready???\r\n");
	
	while(1){
		uart_printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d\r\n ", wall_l.val,wall_fl.val, wall_ff.val, wall_fr.val, wall_r.val);
		if(wall_ff.val > WALL_START){
			melody(e6,300);
			melody(f6,300);
			melody(g6,300);
			ms_wait(1000);
			break;
		}
	}

}

void start_ready(void){

	sensor_start();
	
	MF.FLAG.CTRL = 0;								//制御を無効にする
	set_dir(BACK);									//後退するようモータの回転方向を設定
	get_base();
	set_dir(FORWARD);								//前進するようにモータの回転方向を設定
	
	//GYRO_OFFSET(1000);
	
	melody(c6,1000);
	auto_Calibration(0.30,0.60);
	time2 = 0;
	driveA(SET_MM);
}

void setting_params(params instance){
	params_now.vel_max = instance.vel_max;
	params_now.accel = instance.accel;
	params_now.omega_max = instance.omega_max;
	params_now.omega_accel = instance.omega_accel;
}

void setting_gain(gain instance){
	gain_now.vel_kpR = instance.vel_kpR;
	gain_now.vel_kiR = instance.vel_kiR;
	gain_now.vel_kpL = instance.vel_kpL;
	gain_now.vel_kiL = instance.vel_kiL;
	gain_now.omega_kp = instance.omega_kp;
	gain_now.omega_ki = instance.omega_ki;
	gain_now.wall_kp = instance.wall_kp;
	gain_now.wall_kd = instance.wall_kd;
}

void auto_Calibration(float constant_l, float constant_r){
	wall_l.threshold = (uint16_t)(wall_l.dif * constant_l);
	wall_fl.threshold = WALL_BASE_F;
	wall_ff.threshold = WALL_BASE_F;
	wall_fr.threshold = WALL_BASE_F;
	wall_r.threshold = (uint16_t)(wall_r.dif * constant_r);
	uart_printf("threshold %d, %d :: dif %d, %d\r\n",wall_l.threshold, wall_r.threshold, wall_l.dif, wall_r.dif);
}

void ctrl_zero(){
	MF.FLAG.CTRL = 0;
	sen_ctrl = 0;
	pre_dif_total = 0;
}

void log_print(){
	char mode = 0;
	uint16_t i = 0;
	select_mode(&mode,2);
	
	switch(mode){
		case 0:	//----テスト情報をシリアル送信----
			ms_wait(500);
			uart_printf("START\r\n");
			
			//uart_printf("base:%d, %d\r\n", wall_l.threshold, wall_r.threshold);
			for(i=0;i<2000;i++){
				uart_printf("%lf, %lf,%lf, %lf, %lf, %lf\r\n",log.test1[i],log.test2[i],log.test3[i],log.test4[i],log.test5[i],log.test6[i]);
				ms_wait(1);
			}
			uart_printf("ALL\r\n");
			break;
			
		case 1:		
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:

		case 11:
			break;
		case 12:
			break;
		
		case 13:		
			break;
		default:
			
			break;
		}
	
}