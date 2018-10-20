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
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void select_mode(char *mode){
	uint16_t encR,encL;
	uint16_t nowR = 0;
	uint16_t nowL = 0;
	uint16_t preR;
	
	*mode = 0x00;									//変数の初期化
	R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
	R_PG_Timer_SetCounterValue_MTU_U0_C2(0);
	
	//====変数初期化====
	uart_printf(" mode: 0\r");						//モードをUARTで送信
	
	R_PG_Timer_StartCount_MTU_U0_C1();
	R_PG_Timer_StartCount_MTU_U0_C2();

	//====モード選択処理====
	do{	
		preR = nowR;
		//preL = nowL;
		
		R_PG_Timer_GetCounterValue_MTU_U0_C1(&encL);
		R_PG_Timer_GetCounterValue_MTU_U0_C2(&encR);
		
		nowR = (uint16_t)(encR / 4500);
		nowL = (uint16_t)(encL / 30000);
		
		//ms_wait(50);
		 *mode = nowR;
		//LEDで現在の値を表示
		pins_write(DISP_LEDS, *mode, 4);			//LEDがActiveLowの場合
		if(nowR - preR != 0){
			uart_printf(" mode:%2d\r\n", *mode);
			melody(b7,100);
		}
	}while(nowL != 1);
	
	uart_printf("Finish :  This is mode %2d\r\n", *mode);
	melody(d6,500);
	
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
	float count = (float)1000000/Hz;
	
	count = count*54239;
	count = count/1130;
	
	R_PG_Timer_SetTGR_A_MTU_U0_C0((uint16_t)(count/10));
	R_PG_Timer_SetTGR_B_MTU_U0_C0((uint16_t)count);
	
	R_PG_Timer_StartCount_MTU_U0_C0();
	ms_wait(length);
	R_PG_Timer_HaltCount_MTU_U0_C0();
	
}

void start_wait(){
	R_PG_Timer_StartCount_CMT_U0_C1();
	uart_printf("Ready???\r\n");
	
	while(1){
		uart_printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d\r\n ", ad_l, ad_fl, ad_ff, ad_fr, ad_r);
		if(ad_ff > WALL_START){
			melody(e6,300);
			melody(f6,300);
			melody(g6,300);
			ms_wait(1000);
			break;
		}
	}
/*	R_PG_Timer_HaltCount_CMT_U0_C1();
	
	pin_write(PE0,0);
	pin_write(PE1,0);
	pin_write(PE2,0);
	pin_write(PE3,0);
	pin_write(PE4,0);
*/
}

void start_ready(void){
	S12AD.ADANS0.WORD = 0x1f;
	R_PG_ADC_12_StartConversionSW_S12AD0();
	R_PG_ADC_12_GetResult_S12AD0(ad_res);
	
	ad_r_off = ad_res[3];
	ad_fr_off = ad_res[4];
	ad_ff_off = ad_res[2];
	ad_fl_off = ad_res[0];
	ad_l_off = ad_res[1]; 

	sensor_start();
/*	turn_R90();
	ms_wait(100);
	set_position();
	ms_wait(200);
	
	turn_L90();
	ms_wait(100);
*/	
	MF.FLAG.CTRL = 0;										//制御を無効にする
	set_dir(BACK);											//後退するようモータの回転方向を設定
	driveC(1000, 1);								//尻を当てる程度に後退。回転後に停止する
	get_base();
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
	
	melody(c6,1000	);
/*	driveC(CENTER_TIME, 1);									//定速で指定パルス分回転。回転後に停止する
  	Wait;
*/
	driveA(SET_MM);

}