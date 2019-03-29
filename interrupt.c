/*
割込関係の処理
左右モータへのPWM
左右のエンコーダ処理
壁センサ処理
ジャイロ処理
制御値確定などなど

*/
#include "global.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//Mtu3IcCmDIntFunction
//左モータの制御関数
// PWMを１パルス送るごとに呼び出される
// RX631では1msよりは短くなることが多いから計算量に注意
//+++++++++++++++++++++++++++++++++++++++++++++++
void Mtu3IcCmDIntFunc(void){		//パルス一周期でやると呼び出される左モータの制御関数

	uint16_t paraL0;
	uint16_t paraL1 = 1000;

	pin_write(P54,0);
	pin_write(P55,0);

	//duty_l = Kvolt * accel_l / VOLT_BAT + kpvL + kpdL;
	duty_l = duty_fix_gain_L * (vpid_L + sen_dl + xpid_G - apid_G);
	if(duty_l < 0){
		MF.FLAG.L_DIR = 0;					
		duty_l = - duty_l;	
	}else if(duty_l > 0){
		MF.FLAG.L_DIR = 1;					
	}
	
	if(duty_l > 1){
		duty_l = 0.70;
	}else if(duty_l <= 0.01){
		duty_l = 0.01;	
	}
		
		paraL0 = (1 - duty_l) * paraL1;									//デューティ逆計算
		
		R_PG_Timer_SetTGR_C_MTU_U0_C3(paraL0);
		R_PG_Timer_SetTGR_D_MTU_U0_C3(paraL1);

}

void Mtu3IcCmCIntFunc(void){
	
	if(MF.FLAG.L_DIR){
		pin_write(P54,1);
		pin_write(P55,0);
	}else{
		pin_write(P54,0);
		pin_write(P55,1);
	}
	
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//Mtu4IcCmDIntFunc
//右モータの制御関数
// PWMを１パルス送るごとに呼び出される
// RX631では1msよりは短くなることが多いから計算量に注意
//+++++++++++++++++++++++++++++++++++++++++++++++
void Mtu4IcCmDIntFunc(void){			//右モータ制御関数
	uint16_t paraR0;
	uint16_t paraR1 = 1000;

	pin_write(PA4,0);
	pin_write(PA6,0);

	//duty_r = Kvolt * accel_r / VOLT_BAT + kpvR + kpdR;
	duty_r = duty_fix_gain_R * (vpid_R + sen_dr + xpid_G + apid_G);
	if(duty_r < 0){
		MF.FLAG.R_DIR = 0;
		duty_r = - duty_r;	
	}else if(duty_r > 0){
		MF.FLAG.R_DIR = 1;					
	}
	
	if(duty_r > 1){
		duty_r = 0.70;
	}else if(duty_r <= 0.01){
		duty_r = 0.01;	
	}
	
	paraR0 = (1 - duty_r) * paraR1;									
	
	R_PG_Timer_SetTGR_C_MTU_U0_C4(paraR0);
	R_PG_Timer_SetTGR_D_MTU_U0_C4(paraR1);

}

void Mtu4IcCmCIntFunc(void){
	if(MF.FLAG.R_DIR){
		pin_write(PA4,0);
		pin_write(PA6,1);
	}else {
		pin_write(PA4,1);
		pin_write(PA6,0);
	}
	
}
/*------------------------------------------------------------
		センサ系の割り込み
------------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//Cmt1IntFunc
//0.25ms毎に割り込み
// 壁センサの取得と制御値の計算
// ???l?F????
//+++++++++++++++++++++++++++++++++++++++++++++++
void Cmt1IntFunc(void){	
	switch(tp){
	//タイマ内での分割処理
	case 0:	
		//左壁センサ
		pin_write(PE1,1);
		stay(100);
		S12AD.ADANS0.WORD = 0x02;
		R_PG_ADC_12_StartConversionSW_S12AD0();					
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ad_l = ad_res[1] - ad_l_off;
		
		//右壁センサ
		pin_write(PE3,1);
		stay(100);
		S12AD.ADANS0.WORD = 0x08;
		R_PG_ADC_12_StartConversionSW_S12AD0();
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ad_r =  ad_res[3];// - ad_r_off;
		if((ad_r_off - ad_r) > 100){
			MF.FLAG.WALL = 1;	
		}
		ad_r_off = ad_r;
		
		break;
		
	case 1:
		pin_write(PE1,0);
		pin_write(PE3,0);
				
		//正面壁センサ
		pin_write(PE0,1);
		pin_write(PE2,1);
		pin_write(PE4,1);
	
		stay(100);
		S12AD.ADANS0.WORD = 0x01;
		R_PG_ADC_12_StartConversionSW_S12AD0();
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ad_fl = ad_res[0] - ad_fl_off;
		
		S12AD.ADANS0.WORD = 0x04;
		R_PG_ADC_12_StartConversionSW_S12AD0();
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ad_ff = ad_res[2] - ad_ff_off;

		S12AD.ADANS0.WORD = 0x10;
		R_PG_ADC_12_StartConversionSW_S12AD0();
		R_PG_ADC_12_GetResult_S12AD0(ad_res);
		ad_fr = ad_res[4] - ad_fr_off;
		break;
		
	case 2:
		pin_write(PE0,0);								
		pin_write(PE2,0);								
		pin_write(PE4,0);
				
		omega_G = GYRO_read();
		omega_G_rad = omega_G * KW;
		angle_G += (omega_G + pre_omega_G) * 0.5 * 0.001;
		pre_omega_G = omega_G;
		
		break;
		//----制御計算----
	case 3:			
		//====加減速処理====
			//----減速----
		if(MF.FLAG.VCTRL){
			if(MF.FLAG.DECL){
				targ_vel -= params_now.accel * 0.001;
				if(targ_vel < 0){
					targ_vel = 0.0f;	
				}
			}
			//----加速----
			else if(MF.FLAG.ACCL){	
				targ_vel += params_now.accel * 0.001;
				if(targ_vel > params_now.vel_max){
					targ_vel = params_now.vel_max;
				}
			}
		}
		
		if(MF.FLAG.WCTRL){
			if(MF.FLAG.WDECL){															
				targ_omega -= params_now.omega_accel * 0.001;
				if(targ_omega < 0.0f){
					targ_omega = 0.0f;	
				}
			}
			else if(MF.FLAG.WACCL){
				targ_omega += params_now.omega_accel * 0.001;
				if(targ_omega > params_now.omega_max){
					targ_omega = params_now.omega_max;	
				}
			}
		}
		
		//壁制御フラグアリの場合
		if(MF.FLAG.CTRL){
			//差をとる
			dif_l = (int16_t)(ad_l - base_l);
			dif_r = (int16_t)(ad_r - base_r);

			//制御範囲との比較
			if((SREF_MIN_L < dif_l) && (dif_l < SREF_MAX_L)){
				if(SREF_HALF_L < dif_l)
				sen_dl = (float)dif_l * gain_now.wall_kp * CONT_FIX ;
				else
				sen_dl = (float)dif_l * gain_now.wall_kp;
			}else{
				sen_dl = 0;	//範囲外なら０に
			}
			
			if((SREF_MIN_R < dif_r) && (dif_r < SREF_MAX_R)){
				if(SREF_HALF_R < dif_r)
				sen_dr = (float)dif_r * gain_now.wall_kp * CONT_FIX;
				else
				sen_dr = (float)dif_r * gain_now.wall_kp;
			}else{
				sen_dr = 0;
			}
			//sen_dl = sen_dr = 0;
		}else{
			//そもそもフラグ無
			sen_dl = sen_dr = 0;
		}
		
/*		if(sen_dr > 0.05){
			sen_dr = 0.05;
		}
		if(sen_dl > 0.15){
			sen_dl = 0.05;
		}
*/		break;
	}

	//====タスクポインタで分割処理====
	tp = (tp+1) % 4;
}

void Cmt2IntFunc(){

	time++;
	time2++;
	//エンコーダの値取得
	R_PG_Timer_GetCounterValue_MTU_U0_C1(&pulse_l);
	R_PG_Timer_GetCounterValue_MTU_U0_C2(&pulse_r);
	
	R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
	R_PG_Timer_SetCounterValue_MTU_U0_C2(0);
	
	dif_pulse_r = pulse_r + (65536 * pulse_flag_r);
	dif_pulse_l = pulse_l + (65536 * pulse_flag_l);
	
	pulse_flag_r = 0;
	pulse_flag_l = 0;
	
	dif_pulse_counter_r += dif_pulse_r;
	dif_pulse_counter_l += dif_pulse_l;

	vel_R = Kxr * (float)dif_pulse_r;
	vel_L = Kxr * (float)dif_pulse_l;
	
	totalR_mm = Kxr * (float)dif_pulse_counter_r;
	totalL_mm = Kxr * (float)dif_pulse_counter_l;
	
	//物理量計算
/*		if(dif_pulse_r != 0 ) {
			xR = Kxr * dif_pulse_r;	//割込周期10[ms]での変位[mm]
		} else {
			xR = 0;
		}
		if(dif_pulse_l != 0 ){
			xL = Kxr * dif_pulse_l;
		} else {
			xL = 0;
		}
*/	
	//物理量で扱いやすいm/s = mm/msに変換
	vel_G = (vel_R + vel_L) * 0.5;
	totalG_mm = (totalR_mm + totalL_mm) * 0.5;
	
//センサログ用		
/*		log.test1[time2] = totalG_mm;//angle_G, sen_dr;
		log.test2[time2] = ad_l;//dif_angle, sen_dl;
	
		log.test3[time2] = ad_ff;//angle_G;//ad_r, kvpR
		log.test4[time2] = ad_r;//omega_G;//ad_l, kvdR
	
		log.test5[time2] = base_l;
		log.test6[time2] = base_r;		
*/		
//並進速度ログ用		
/*	log.test1[time2] = targ_vel;//angle_G, sen_dr;
	log.test2[time2] = vel_omega;//dif_angle, sen_dl;
	
	log.test3[time2] = vel_R;//angle_G;//ad_r, kvpR
	log.test4[time2] = vel_L;//omega_G;//ad_l, kvdR
	
	log.test5[time2] = vel_G;
	log.test6[time2] = totalG_mm;
*/
//回転速度ログ用
/*		log.test1[time2] = omega_direction * targ_omega;	//angle_G, sen_dr;
		log.test2[time2] = omega_G_rad;				//dif_angle, sen_dl;
	
		log.test3[time2] = vel_omega;				//angle_G;//ad_r, kvpR
		log.test4[time2] = angle_G;				//omega_G;//ad_l, kvdR
	
		log.test5[time2] = vel_R;
		log.test6[time2] = vel_L;
*/
//スラローム
		log.test1[time2] = omega_direction * targ_omega;//angle_G, sen_dr;
		log.test2[time2] = omega_G_rad;//dif_angle, sen_dl;
	
		log.test3[time2] = vel_R;//angle_G;//ad_r, kvpR
		log.test4[time2] = vel_L;//omega_G;//ad_l, kvdR
	
		log.test5[time2] = totalG_mm;
		log.test6[time2] = angle_G;


	//PIDしてみる？
	if(MF.FLAG.WCTRL){
		//偏差角速度の算出
		dif_omega = (omega_direction * targ_omega) - omega_G_rad;
		kwpG = gain_now.omega_kp * dif_omega * TREAD_mm * 0.5 * 0.001;
		kwiG += gain_now.omega_ki * dif_omega * TREAD_mm * 0.5 * 0.001;
		vel_omega = kwpG + kwiG;		
/*		//偏差の計算
		dif_omega = (W_dir * targ_omega[t_cnt_w]) - omega_G_rad;
		//偏差のP制御
		kwpG = W_KP * dif_omega;				
		//偏差のD制御
		kwdG = W_KD * (dif_omega - dif_pre_omega);
		//偏差のI制御
		kwiG = W_KI * dif_omega;
		//PID制御値を統合
		wpid_G = kwpG + kwiG;// + kwdG );
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_omega = dif_omega;		
*/	}else{
		vel_omega = 0;
	}
	
	
	if(MF.FLAG.VCTRL){
		//偏差の計算
		dif_vel_R = (targ_vel * vel_direction_R) + vel_omega - vel_R;
		dif_vel_L = (targ_vel * vel_direction_L) - vel_omega - vel_L;
		//偏差のP制御
		kvpR = gain_now.vel_kpR * dif_vel_R;				
		kvpL = gain_now.vel_kpL * dif_vel_L;
				
		//偏差のI制御
		kviR += gain_now.vel_kiR * dif_vel_R;
		kviL += gain_now.vel_kiL * dif_vel_L;
		
		//PID制御値を統合
		vpid_R = (kvpR + kviR);
		vpid_L = (kvpL + kviL);
		
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_vel_R = dif_vel_R;		
		dif_pre_vel_L = dif_vel_L;

	}else{
		vpid_R = 0;
		vpid_L = 0;
	}
	
	if(MF.FLAG.ACTRL){
		//偏差の計算
		dif_angle = targ_angle - angle_G;
		//偏差のP制御
		kapG = A_KP * dif_angle;				
		//偏差のD制御
		kadG = A_KD * (dif_angle - dif_pre_angle);		
		//PID制御値を統合
		apid_G = (kapG + kadG);
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_angle = dif_angle;		
	}else{
		apid_G = 0;
	}
	
	if(MF.FLAG.XCTRL){
		dif_x_G = (targ_total_mm - totalG_mm);
		kxpG = X_KP * dif_x_G;
		kxdG = X_KD * (dif_pre_x_G - dif_x_G);
		xpid_G = (kxpG - kxdG);
		dif_pre_x_G = dif_x_G;
	}else{
		xpid_G = 0;
	}
	
}

//カウンタ値のオーバーフロー（インクリメント），アンダーフロー(デクリメント)回数をカウント

void Mtu2UnIntFunc() {
	pulse_flag_r--;
}
void Mtu2OvIntFunc() {
	pulse_flag_r++;
}

void Mtu1UnIntFunc() {
	pulse_flag_l--;
}
void Mtu1OvIntFunc() {
	pulse_flag_l++;
}

//時間計測用割込み関数2
void Cmt0IntFunc(){
	ms_time++;
}
