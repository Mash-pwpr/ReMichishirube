/*
==============================================================
 Name        : sensor.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : センサ関連の関数たちです．

  更新履歴
 2016/1/29		山上	コメント追加、get_wall_info関数内の始めの
 	 	 	 	 	pins_write関数の第3引数を6からLED_NUMに変更
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
*/

//※解説の章番号等は日本語版ユーザーマニュアルRev.00.15に準拠

/*ヘッダファイルの読み込み*/
#include "global.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//get_base
//	制御用の基準値を取得する
// 引数：なし
// 戻り値：理想的な値を取得できたか　1:できた　0:できなかった
//+++++++++++++++++++++++++++++++++++++++++++++++
unsigned char get_base()
{
	unsigned char res = 0;										//理想的な値を取得できたか

	ms_wait(10);
	//----制御用の基準を取得----
	base_l = ad_l - WALL_OFFSET;										//現在の左側のセンサ値で決定
	base_r = ad_r - WALL_OFFSET;										//現在の右側のセンサ値で決定

	//----基準が理想的だとLED点滅----
	if((-50 < (int)(base_l - base_r)) && ((int)(base_l - base_r) < 50)){
		//左右で差が50以下である場合
		pin_write(DISP_LEDS[0], 0x0);					//0番目のLEDを消灯
		pins_write(DISP_LEDS, 0x0f, LED_NUM);			//LEDを全点灯
		pins_write(DISP_LEDS, 0x00, LED_NUM);			//LEDを全消灯
		res = 1;										//resを1に
	}else{
	}
	uart_printf("base:%d, %d\r\n", base_r, base_l);

	return res;											//理想的な値を取得できたかを返す

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_wall_info
//	壁情報取得を取得する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void get_wall_info()
{
	unsigned char tmp = 0;						//点灯させるLEDの場所
	//----壁情報の初期化----
	wall_info = 0x00;									//壁情報を初期化
	pins_write(DISP_LEDS, 0, LED_NUM);					//LEDを全消灯

	//----前壁を見る----
	if(ad_ff > WALL_BASE_F){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x88;								//壁情報を更新
		tmp = 0x06;										//1番目と2番目のLEDを点灯させるよう設定
	}
	//----右壁を見る----
	if(ad_r > wall_base_r){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x44;								//壁情報を更新
		tmp |= 0x01;									//0番目のLEDを点灯させるよう設定
	}
	//----左壁を見る----
	if(ad_l > wall_base_l){
		//AD値が閾値より大きい(=壁があって光が跳ね返ってきている)場合
		wall_info |= 0x11;								//壁情報を更新
		tmp |= 0x08;									//3番目のLEDを点灯させるよう設定
	}
	
	pins_write(DISP_LEDS, tmp, LED_NUM);				//LEDを点灯させる
}

void enc_test(){
	totalR_mm = totalL_mm = 0;
	pulse_flag_r = pulse_flag_l = 0;
	time = 0;
	R_PG_Timer_StartCount_MTU_U0_C1();
	R_PG_Timer_StartCount_MTU_U0_C2();
	R_PG_Timer_StartCount_CMT_U1_C2();
	
	while(1){
/*		
		totalR_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096) / 4096;
		totalL_mm += -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096) / 4096;
		
		pulse_pre_r = pulse_r;
		pulse_pre_l = pulse_l;
*/		uart_printf("totalR_mm:%4lf totalL_mm:%4lf\r\n",totalR_mm, totalL_mm);
		ms_wait(500);
	}

}

void sensor_start(){
	R_PG_Timer_StartCount_MTU_U0_C1();	//エンコーダ左右
	R_PG_Timer_StartCount_MTU_U0_C2();
	
	R_PG_Timer_StartCount_CMT_U0_C1();	//壁センサ用LED起動タイマ
	R_PG_Timer_StartCount_CMT_U1_C2();	//エンコーダ処理，PID計算用タイマ
	
}
void sensor_stop(){
	R_PG_Timer_HaltCount_MTU_U0_C1();
	R_PG_Timer_HaltCount_MTU_U0_C2();

	R_PG_Timer_HaltCount_CMT_U0_C1();
	R_PG_Timer_HaltCount_CMT_U1_C2();
	
	pin_write(PE0,0);								
	pin_write(PE1,0);
	pin_write(PE2,0);
	pin_write(PE3,0);
	pin_write(PE4,0);

}