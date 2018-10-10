/*
==============================================================
 Name        : drive.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 走行関連の関数たちです．

  更新履歴
 2016/1/29		山上	コメント追加、test_drive内ms_waitとWAITを統合
 2016/2/15		山上	パルス関係のマクロを整理
==============================================================
*/
#include "global.h"

//※解説の章番号等は日本語版ユーザーマニュアルRev.00.15に準拠

/*==========================================================
		走行系関数
==========================================================*/
/*
		基本仕様として、
		基幹関数		第一引数:走行パルス数
						第二引数:停止許可フラグ

		マウスフラグ(MF)
			7Bit:ストップフラグ
			6Bit:定速フラグ
			5Bit:減速フラグ
			4Bit:加速フラグ
			3Bit:制御フラグ
			2Bit:スラロームフラグ
			1Bit:二次走行フラグ
			0Bit:予約ビット
		上位4Bitは、上位ほど優先度が高い
*/
/*----------------------------------------------------------
		上位関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA
//	半区画分加速しながら走行する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA()
{
	MF.FLAG.CTRL = 1;
	driveA(HALF_MM);									//半区画のパルス分加速しながら走行。走行後は停止しない
	get_wall_info();										//壁情報を取得
}

void half_sectionA2()
{
	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveA(HALF_MM);									//半区画のパルス分加速しながら走行。走行後は停止しない
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
//	半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD()
{
	MF.FLAG.CTRL = 1;										
	driveD(HALF_MM,1);									//半区画のパルス分減速しながら走行。走行後は停止する													
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//a_section
//	1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void a_section()
{
	half_sectionA();			//半区画分加速走行
	half_sectionD();										
}

void s_section(){
	half_sectionA2();			//半区画分加速走行
	half_sectionA();										
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//a_sectionU
//	等速で1区画分進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void a_sectionU() {
	MF.FLAG.CTRL = 1;
	driveU(DR_SEC_HALF*2, 0);		//1区画のパルス分等速走行。走行後は停止しない
	get_wall_info();			//壁情報を取得
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_R90
//	右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
/*void turn_R90(){
	MF.FLAG.CTRL = 0;									//制御を無効にする
	set_dir(TURN_R);									//右に回転するようモータの回転方向を設定
 	driveC(DR_ROT_R90, 1);									//低速で指定パルス分回転。回転後に停止する
	Wait;													//完全に停止するまで待機
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
}*/
//DC用に改造
void turn_R90(){
	MF.FLAG.CTRL = 0;									//制御を無効にする
	set_dir(TURN_R);									//右に回転するようモータの回転方向を設定
	driveC(ROT_TIME, 1);									//低速で指定パルス分回転。回転後に停止する
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_L90
//	左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_L90()
{
	MF.FLAG.CTRL = 0;										//制御を無効にする
	set_dir(TURN_L);										//左に回転するようモータの回転方向を設定
	//driveAD(DR_ROT_L90, 1);									//定速で指定パルス分回転。回転後に停止する
	driveC(ROT_TIME, 1);									//定速で指定時間回転。回転後に停止する
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_180
//	180度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_180()
{
	MF.FLAG.CTRL = 0;										//制御を無効にする
	set_dir(TURN_R);										//左に回転するようモータの回転方向を設定
	driveC(ROT180_TIME, 1);									//定速で指定パルス分回転。回転後に停止する										//完全に停止するまで待機
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
//	機体の尻を壁に当てて場所を区画中央に合わせる
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position()
{
	MF.FLAG.CTRL = 0;										//制御を無効にする
	set_dir(BACK);											//後退するようモータの回転方向を設定
	driveC(1000, 1);								//尻を当てる程度に後退。回転後に停止する
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
	driveC(CENTER_TIME, 1);									//定速で指定パルス分回転。回転後に停止する
  	Wait;												//完全に停止するまで待機
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
//	指定パルス分加速走行する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
/*void driveA(uint16_t dist, unsigned char rs) {
	//====走行====
	//----走行開始----
	MF.FLAGS = 0x10 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	drive_start();											//走行開始

	//----走行----
	while((pulse_l < dist) && (pulse_r < dist));			//左右のモータが指定パルス以上進むまで待機

	//----停止許可があれば停止----
	drive_stop(rs);
}*/

//DC用に改造しているdriveA,距離はオッケー
void driveA(float dist, unsigned char rs) {					//引数　走行距離　停止の有無（1で停止，０で継続走行）,vel0とtimeは触れていない
	//====走行====
	//----走行開始----
	//MF.FLAGS = 0x00 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	totalG_mm = 0;
	totalR_mm = 0;
	totalL_mm = 0;					//走行距離をリセット
	
	//offsetG_mm = 0.5 * max_vel_G * maxindex * 1000;		//v-tグラフにおける速度減少部の面積　⇒　減少に必要な距離
	drive_start();								//走行開始
	
	//----走行----
	while((totalR_mm + offsetG_mm) < dist || (totalL_mm + offsetG_mm) < dist){
		//uart_printf("%lf, %lf, %d, %d\r\n",totalR_mm,totalL_mm, t_cnt_r,t_cnt_l);		
	}
	
	offsetG_mm = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
//	指定パルス分減速走行して停止する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++


void driveD(uint16_t dist, unsigned char rs) {
	//====走行====
	//----走行開始----
	//MF.FLAGS = 0x00 | (MF.FLAGS & 0x0F);					//加速・減速・定速・ストップフラグを0にする
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.XCTRL = 0;
	
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	drive_start();											//痩躯開始
	offsetG_mm = rs * 0.5 * max_vel_G *maxindex;
	offsetG_mm -= STOP_OFF_MM;
	
	//----走行----
	while((totalR_mm + offsetG_mm) < dist || (totalL_mm + offsetG_mm) < dist){
		//uart_printf("%lf, %lf, %d, %d\r\n",totalR_mm,totalL_mm, t_cnt_r,t_cnt_l);		
	}
	if(rs){
		MF.FLAG.DECL = 0;
		MF.FLAG.DECL = 1;										//減速フラグを1に
		while(totalR_mm < dist || totalL_mm < dist){
			if(time > 500){
				break;
			}
		
		}
		
		
/*		targ_total_mm = (float)dist;
		MF.FLAG.VCTRL = 0;
		MF.FLAG.XCTRL = 1;
		ms_wait(1000);
		MF.FLAG.XCTRL = 0;
*/		
	}
	
	//----停止措置----
	drive_stop(rs);											//走行終了、停止許可があれば停止
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveAD
//	指定パルス分加速(or等速)・減速走行して停止する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
/*void driveAD(uint16_t dist, unsigned char rs)
{
	//====変数宣言====
	signed int ac_pulse;									//等・加速走行のパルス数

	//----加速走行パルス数の算出----
	ac_pulse = dist + minindex - t_cnt_l;					//加速走行のパルス=総距離のパルス-減速しきるのに必要なパルス
															//=総距離のパルス-(現在の速度のパルス-最低のパルス数)
	//====走行====
	//----走行開始----
	MF.FLAGS = 0x10 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	drive_start();											//走行開始

	//----走行----
	while((pulse_l < ac_pulse) && (pulse_r < ac_pulse));	//左右のモータが定速分のパルス以上進むまで待機
	ac_pulse = dist + minindex - t_cnt_l;					//パルスの再計算
	MF.FLAG.DECL = 1;										//減速フラグを1に
	while((pulse_l < dist) && (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//----停止措置----
	drive_stop(rs);											//走行終了、停止許可があれば停止

}*/

//DC用に改造，目的は超信地のみ
void driveAD(float dist, unsigned char rs)
{
	//====走行====
	//----走行開始----
	//MF.FLAGS = 0x10 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	drive_start();								//走行開始
	
	//----走行----
	while((totalL_mm - offsetL_mm) < dist){
			offsetL_mm = rs * 0.5 * max_vel_G * off_dt;		//v-tグラフにおける速度減少部の面積　⇒　減少に必要な距離
	}	
	if(rs){
		MF.FLAG.CTRL = 0;
		velR0 = velL0 = 0;
		ms_wait(100);			//速度がゼロに収束するまで待つ
		t_cnt_r = t_cnt_l = 0;
	}
	//----停止許可があれば停止----
	drive_stop(rs);	
	offsetL_mm = 0;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveU
//	指定パルス分等速走行して停止する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveU(uint16_t dist, unsigned char rs)
{

	//====等速走行開始====
	MF.FLAGS = 0x00 | (MF.FLAGS & 0x0F);					//加速・減速・定速・ストップフラグを0にする
	drive_start();											//走行開始

	//====走行====
	while((pulse_l < dist) && (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	//----停止許可があれば停止----
	drive_stop(rs);											//走行終了、停止許可があれば停止
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
//	指定パルス分定速走行して停止する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t dist, unsigned char rs)			//引数　時間　停止許可　＊時間制限でモータ回せる関数
{
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	t_cnt_r = t_cnt_l = 0;
	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	drive_start();											//走行開始
	
	//====回転====
	while(time < dist);			//左右のモータが定速分のパルス以上進むまで待機

	if(rs){
		velR0 = velL0 = 0;
		ms_wait(100);			//速度がゼロに収束するまで待つ
		t_cnt_r = t_cnt_l = 0;
	}
	//----停止許可があれば停止----
	drive_stop(rs);											//走行終了、停止許可があれば停止
	

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start
//	走行を開始する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start(void){
	time = 0;
	R_PG_Timer_StartCount_MTU_U0_C3();  //左モータ
	R_PG_Timer_StartCount_MTU_U0_C4();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_stop
//	走行終了し、停止許可があれば停止する
// 引数1：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_stop(unsigned char rs){
	R_PG_Timer_HaltCount_MTU_U0_C3();
	R_PG_Timer_HaltCount_MTU_U0_C4();

	pin_write(P54,0);
	pin_write(P55,0);
	pin_write(PA4,0);
	pin_write(PA6,0);
	//====停止処理====
	if(rs) MF.FLAG.STOP = 1;								//停止するのであればストップフラグを1に
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//set_dir
//	進行方向を設定する
// 引数1：d_dir・・・どの方向に進行するか  0桁目で左、1桁目で右の方向設定
// 戻り値：なし
//		DC用に改造完了
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_dir(unsigned char d_dir)
{
	//====左モータ====
	switch(d_dir & 0x0f){									//0~3ビット目を取り出す
		//----正回転----
		case 0x00:									
			MF.FLAG.L_DIR = 1;						//左を前進方向に設定
			velL0 = 1;
			break;
		//----逆回転----
		case 0x01:								 
			MF.FLAG.L_DIR = 0;				//左を後進方向に設定
			velL0 = -1;
			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){									//4~7ビット目を取り出す
		case 0x00:											//0x00の場合
			MF.FLAG.R_DIR = 1;					//右を前進方向に設定
			velR0 = 1;
			break;
		case 0x10:											//0x10の場合
			MF.FLAG.R_DIR = 0;					//右を後進方向に設定
			velR0 = -1;
			break;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//test_drive
//	走行関係のテストをする
// 引数1：mode・・・モード番号を格納する変数のアドレス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void test_drive(char *mode){

	while(1){											
		uart_printf("test_drive:\n");						//UARTで送信
		select_mode(mode);									//モード選択をさせる
		ms_wait(50);
		switch(*mode){										//モード番号により分岐
			//----位置の調整----
			case 0:
				set_position();
				break;

			//----一区画定速走行----
			case 1:
				sensor_start();
				set_dir(FORWARD);
				drive_start();
				
				while(1){
				//uart_printf(" pulse_r: %4d  pulse_l:%4d  totalR_mm:%4lf totalL_mm:%4lf \r\n", pulse_r, pulse_l, totalR_mm, totalL_mm);
				//uart_printf(" time : %4d time4 : %4d totalR_mm:%4lf totalL_mm:%4lf \r\n", time, time % 4, totalR_mm, totalL_mm);
		
				uart_printf("DR: %4lf  DL:%4lf totalR_mm:%4lf totalL_mm:%4lf \r\n",vel_R,vel_L, totalR_mm, totalL_mm);
				ms_wait(1000);
				}
/*				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
				set_dir(FORWARD);
				driveC(DR_SEC_HALF*2, 1);
*/				drive_stop(1);
				R_PG_Timer_HaltCount_MTU_U0_C1();
				R_PG_Timer_HaltCount_MTU_U0_C2();
				R_PG_Timer_HaltCount_MTU_U0_C3();
				R_PG_Timer_HaltCount_MTU_U0_C4();
				R_PG_Timer_HaltCount_CMT_U0_C1();

				uart_printf("BREAK \r\n");
				break;

			//----右90回転----
			case 2:
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				turn_R90();
				break;

			//----左90回転----
			case 3:
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				turn_L90();
				break;

			//----180回転----
			case 4:
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();
				turn_180();

				break;

			//----高速加減速8区画----
			case 7:
				minindex = MINSPEED_H;
				maxindex = MAXSPEED_H;
				stay(DEFWAIT);
				MF.FLAG.CTRL = 1;
				driveAD((DR_SEC_HALF*2) * 6, 1);
				break;

			//----探索用加減速1区画----
			case 8:
				minindex = MINSPEED_S;
				maxindex = MAXSPEED_S;
				stay(DEFWAIT);
				a_section();
				break;

			default:
				
				while(1){
					uart_printf("GoodBy, Drive\r\n");
				}
				return;
		}
		
	}
	uart_printf("drive_Finish: %d\n",*mode);
	
}

