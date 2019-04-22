/*
==============================================================
 Name        : drive.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 走行関連の関数たちです．
==============================================================
*/
#include "global.h"

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
void half_sectionA(int16_t wall, int16_t ctrl)
{
	MF.FLAG.CTRL = ctrl;
	driveA(HALF_MM);									//半区画のパルス分加速しながら走行。走行後は停止しない
	if(wall){
		get_wall_info();		//壁情報を取得
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
//	半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD()
{
	MF.FLAG.CTRL = 0;										
	driveD(HALF_MM,1);									//
	//driveX(0);
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//a_section
//	1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void a_section()
{
	half_sectionA(GET_WALL_ON, CTRL_ON);			//半区画分加速走行
	half_sectionD();										
}

void s_section(){
	half_sectionA(GET_WALL_OFF, CTRL_ON);			//半区画分加速走行
	half_sectionA(GET_WALL_ON, CTRL_ON);									
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//a_sectionU
//	等速で1区画分進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void a_sectionU() {
	MF.FLAG.CTRL = 1;
	//driveU(DR_SEC_HALF*2, 0);		//1区画のパルス分等速走行。走行後は停止しない
	get_wall_info();			//壁情報を取得
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_R90
//	右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
//DC用に改造

void turn_R90(){
	MF.FLAG.CTRL = 0;								//制御を無効にする
	set_dir(TURN_R);								//右に回転するようモータの回転方向を設定
	driveAD(ROT_ANGLE_R90);								//超信地するわよぉ！
	set_dir(FORWARD);								//前進するようにモータの回転方向を設定
}

void turn_SLA_R90(){
	MF.FLAG.CTRL = 0;
	set_dir(FORWARD);								//右に回転するようモータの回転方向を設定
	driveA(params_search1.R90_before);
	
	time = 0;
	time2 = 0;
	MF.FLAG.CTRL = 0;
	driveW(-90);								//低速で指定パルス分回転。回転後に停止する
	
	MF.FLAG.CTRL = 0;
	driveA(params_search1.R90_after);
	
	get_wall_info();
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_L90
//	左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_L90()
{
	MF.FLAG.CTRL = 0;									
	set_dir(TURN_L);									//左に超信地旋回する向きに設定
	driveAD(ROT_ANGLE_L90);									//超信地面するわよぉ！
	set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//turn_SLA_L90	試作中
//	左にslalomで90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_SLA_L90(){
	
	//time2 = 0;
	
	MF.FLAG.CTRL = 0;
	set_dir(FORWARD);							
	driveA(params_search1.L90_before);							//offset　before区間走行
	//time2 = 0;
	
	MF.FLAG.CTRL = 0;
	driveW(90);								//90までスラローム旋回
	
	MF.FLAG.CTRL = 0;
	driveA(params_search1.L90_after);							//offset　after区間
	kwpG = 0;
	kwiG = 0;
	
	get_wall_info();
	
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
	sen_ctrl = 0;
	
	set_dir(TURN_R);										//左に回転するようモータの回転方向を設定driveAD(ROT_ANGLE_R90);
	driveAD(-180);
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
//	機体の尻を壁に当てて場所を区画中央に合わせる
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position(uint8_t flag)
{
	MF.FLAG.CTRL = 0;
	//制御を無効にする
	set_dir(BACK);											//後退するようモータの回転方向を設定
	ms_wait(200);
	driveC(1000,0);								//尻を当てる程度に後退。回転後に停止する
	set_dir(FORWARD);										//前進するようにモータの回転方向を設定
  	Wait;
	MF.FLAG.CTRL =1;
	if(flag == 0){			//スラローム
		driveA(SET_MM);
	}else{
		driveA(SET_MM * 0.5);
		driveD(SET_MM * 0.5,1);
		
	}
	
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
//	指定距離分加速走行する
// 引数1：dist・・・走行する距離mm
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
//DC用に改造しているdriveA,距離はオッケー
void driveA(float dist) {					//引数　走行距離　停止の有無（1で停止，０で継続走行）,vel0とtimeは触れていない
	uint16_t flag = 0;
	float ics1 = totalG_mm;
	
	//====走行====
	//----走行開始----
	//MF.FLAGS = 0x00 | (MF.FLAGS & 0x0F);					//減速・定速・ストップフラグを0に、加速フラグを1にする
	MF.FLAG.ACTRL = 0;
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;
	
	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	
	MF.FLAG.FFCTRL = 0;
	
	//走行距離をリセット
/*	totalR_mm = 0;
	totalL_mm = 0;
	totalG_mm = 0;
	dif_pulse_counter_r = dif_pulse_counter_l = 0;
*/	
	targ_angle = 0;
	drive_start();					//走行開始
	
	time = 0;
	//----走行----
	while(totalG_mm < dist + ics1){
		if(time > 3000){
			break;
		}
		if(MF.FLAG.WALL && flag == 0){
			dif_pulse_counter_r = (dist + ics1 - 40) / Kxr;
			dif_pulse_counter_l = (dist + ics1 - 40) / Kxr;
			MF.FLAG.WALL = 0;
			flag = 1;
		}
	}
	kviR = kviL = 0;
	kwpG = 0;
	kwiG = 0;
	
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
//	指定パルス分減速走行して停止する
// 引数1：dist・・・走行する距離
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++


void driveD(uint16_t dist, unsigned char rs) {
	float ics = totalG_mm;

	//====走行====
	MF.FLAG.CTRL = 0;
	
	//----走行開始----
	MF.FLAG.ACTRL = 0;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.VCTRL = 1;
	
	MF.FLAG.WDECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
		
	drive_start();								//痩躯開始
	offsetG_mm = rs * 0.5 * params_now.vel_max * maxindex * 1000;
	//----走行----
	while((totalG_mm + offsetG_mm) < ((float)dist + ics));	
	
	if(rs){
		MF.FLAG.ACCL = 0;
		MF.FLAG.DECL = 1;
		
		//uart_printf("DEACCL is %lf, %lf, %lf\r\n", totalG_mm, offsetG_mm,maxindex);
		
		while(targ_vel > 0.0f){
			if(targ_vel == 0){
				ms_wait(100);
				break;
			}
		}
		
		MF.FLAG.ACTRL = 0;
		MF.FLAG.WCTRL = 0;
		MF.FLAG.XCTRL = 0;
		MF.FLAG.VCTRL = 0;				
	}
	//uart_printf("Finish is %f\r\n",totalG_mm);
	//----停止措置----
	drive_stop(rs);											//走行終了、停止許可があれば停止

	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;

	kviR = kviL = 0;
	kwpG = 0;
	kwiG = 0;

	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveAD
//	指定パルス分加速(or等速)・減速走行して停止する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
//DC用に改造，目的は超信地のみ 引数1:
void driveAD(float theta)
{
	float angle_offset;
	
	if(theta > 0){				//左旋回
		omega_direction = 1;
		duty_fix_gain_R = 1.0;
		duty_fix_gain_L = 1.0;
	}else if(theta < 0){			//右旋回
		omega_direction = -1;	
		duty_fix_gain_R = 1.0;
		duty_fix_gain_L = 1.0;
	}
	
	//====走行====
	//----走行設定----
	MF.FLAG.ACTRL = 0;
	MF.FLAG.VCTRL = 1;
	MF.FLAG.WCTRL = 1;
	MF.FLAG.XCTRL = 0;
	
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;
	
	angle_G = 0;
	angle_offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;	//減速に必要な角度の絶対値計算
	targ_total_mm = 0;
	targ_vel = 0;
	targ_omega = 0;
	
	drive_start();			//走行開始
	
	if(theta > 0){
		//----走行----
		while(angle_G < theta - angle_offset);				//w-tグラフにおける速度増加部の面積　⇒　現在の回転角度
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		kviR = kviL = 0;

		while(angle_G < theta) {
			if(targ_omega == 0){
				break;
			}
		}
	}else if (theta < 0){
		while(angle_G > theta + angle_offset);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		kviR = kviL = 0;

		while(angle_G > theta) {
			if(targ_omega == 0){
				break;
			}
		}
	}
	
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;
	
	//----停止許可があれば停止----
	drive_stop(1);	
	//time = 0;
	kviR = kviL = 0;
	kwpG = 0;
	kwiG = 0;
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveX
//姿勢制御を加えた減速走行　止まるときに使いたい
// 引数1：dist・・・走行する距離[mm]
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveX(uint16_t dist){
	//====走行====
	//----走行開始----
	MF.FLAG.ACTRL = 1;
	MF.FLAG.VCTRL = 0;
	MF.FLAG.WCTRL = 0;
	MF.FLAG.XCTRL = 1;
	
	MF.FLAG.CTRL = 0;
	
	MF.FLAG.ACCL = 0;
	MF.FLAG.DECL = 0;

	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;

	targ_angle = 0;
	angle_G = 0;
	targ_total_mm = dist;
	
	drive_start();								//走行開始
	
	ms_wait(100);	
	//----走行----
	
	//----停止許可があれば停止----
	drive_stop(1);	
	angle_G = 0;
	targ_vel = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
//	指定時間分加速走行する
// 引数1：dist・・・走行するパルス
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t dist, unsigned char rs)			//引数　時間　停止許可　＊時間制限でモータ回せる関数
{
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;
	
	targ_vel = targ_omega = 0;
	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.WCTRL = 1;
	
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 0;
	MF.FLAG.WDECL = 0;

	drive_start();											//走行開始
	
	time = 0;
	//====回転====
	while(time < dist);			//一定時間経過まで待機

	if(rs){
		vel_direction_R = vel_direction_L = 0;
		ms_wait(100);			//速度がゼロに収束するまで待つ
	}
	//----停止許可があれば停止----
	drive_stop(rs);											//走行終了、停止許可があれば停止
	targ_vel = 0;
	
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;

	
	kviR = kviL = 0;
	kwpG = 0;
	kwiG = 0;

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveW
//	角速度制御走行
// 引数1：theta・・・目標角度
// 引数2：rs・・・走行後停止するか　1:する　それ以外:しない
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveW(int16_t theta)			//引数　時間　停止許可　
{
	float angle_offset = 0;
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;
	
	angle_G = 0;
	targ_omega = 0;
	
	//====回転開始====
	MF.FLAG.VCTRL = 1;
	MF.FLAG.ACTRL = 0;
	MF.FLAG.XCTRL = 0;
	MF.FLAG.WCTRL = 1;
	
	if(theta > 0){				//左旋回
		omega_direction = 1;
		duty_fix_gain_R = 1.0;
		duty_fix_gain_L = 1.0;
	}else if(theta < 0){			//右旋回
		omega_direction = -1;	
		duty_fix_gain_R = 1.0;
		duty_fix_gain_L = 1.0;
	}
	
	MF.FLAG.ACCL = 1;
	MF.FLAG.DECL = 0;
	MF.FLAG.WACCL = 1;
	MF.FLAG.WDECL = 0;
	
	drive_start();

	angle_offset = (0.5 * maxindex_w * params_now.omega_max) * KWP;
	//====回転====
	if(omega_direction == 1){				//左旋回
		
		angle_offset = angle_offset * 1.0f;
		while(angle_G + angle_offset < theta);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		kviR = kviL = 0;
/*		kwpG = 0;
		kwiG = 0;
*/		while(angle_G  < theta){
			if(targ_omega == 0){
				break;
			}
		}
	} else if(omega_direction == -1){
		
		angle_offset = angle_offset * 1.0f;		//右旋回
		while(angle_G - angle_offset > theta);
		MF.FLAG.WACCL = 0;
		MF.FLAG.WDECL = 1;
		kviR = kviL = 0;
/*		kwpG = 0;
		kwiG = 0;
*/
		while(angle_G > theta){
			if(targ_omega == 0){
				break;
			}
		}
	}
	
	omega_direction = 0;
	duty_fix_gain_R = 1.0;
	duty_fix_gain_L = 1.0;
	
	totalG_mm = totalR_mm = totalL_mm = 0;					//走行距離をリセット
	dif_pulse_counter_r = dif_pulse_counter_l = 0;

	//angle_G = 0;
	kviR = kviL = 0;
	kwpG = kwiG = 0;

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start
//	走行を開始する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start(void){
	//time = 0;
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
	
	//time = 0;
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
			MF.FLAG.L_DIR = 1;				//左を前進方向に設定
			vel_direction_L= 1;
			break;
		//----逆回転----
		case 0x01:								 
			MF.FLAG.L_DIR = 0;				//左を後進方向に設定
			vel_direction_L = -1;
			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){									//4~7ビット目を取り出す
		case 0x00:											//0x00の場合
			MF.FLAG.R_DIR = 1;					//右を前進方向に設定
			vel_direction_R = 1;
			break;
		case 0x10:											//0x10の場合
			MF.FLAG.R_DIR = 0;					//右を後進方向に設定
			vel_direction_R = -1;
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
				set_position(1);
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
				
				break;

			//----探索用加減速1区画----
			case 8:
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

