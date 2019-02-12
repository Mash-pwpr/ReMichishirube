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
	duty_l = KL * (vpid_L + sen_dl + xpid_G - wpid_G - apid_G);
	
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
	duty_r = KR * (vpid_R + sen_dr + xpid_G + wpid_G + apid_G);
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
		ad_r =  ad_res[3] - ad_r_off;
				
		break;
	case 1:
		pin_write(PE1,0);
		pin_write(PE3,0);
				
		//正面壁センサ
		
		pin_write(PE0,1);								//??????LED??ON
		pin_write(PE2,1);								//??????LED??ON
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
				if(t_cnt_r > minindex) t_cnt_r--; 
				if(t_cnt_l > minindex) t_cnt_l--; 
			}
			//----加速----
			else if(MF.FLAG.ACCL){			
				if(t_cnt_r < maxindex) t_cnt_r++;
				if(t_cnt_l < maxindex) t_cnt_l++;
			}
		}
		if(MF.FLAG.WCTRL){
			if(MF.FLAG.WDECL){															
				if(t_cnt_w > minindex) t_cnt_w--; 
			}
			else if(MF.FLAG.WACCL){
				if(t_cnt_w < maxindex_w) t_cnt_w++; 	
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
				sen_dl = (float)dif_l * cont_l * CONT_FIX ;
				else
				sen_dl = (float)dif_l * cont_l;
			}else{
				sen_dl = 0;	//範囲外なら０に
			}
			
			if((SREF_MIN_R < dif_r) && (dif_r < SREF_MAX_R)){
				if(SREF_HALF_R < dif_r)
				sen_dr = (float)dif_r * cont_r * CONT_FIX;
				else
				sen_dr = (float)dif_r * cont_r;
			}else{
				sen_dr = 0;
			}
		}else{
			//そもそもフラグ無
			sen_dl = sen_dr = 0;
		}
		
		if(sen_dr > 0.05){
			sen_dr = 0.1;
		}
		if(sen_dl > 0.05){
			sen_dl = 0.1;
		}
		break;
	}

	//====タスクポインタで分割処理====
	tp = (tp+1) % 4;
}

void Cmt2IntFunc(){

	time++;
	time2++;
	if(time % 10 == 0){
		//エンコーダの値取得
		R_PG_Timer_GetCounterValue_MTU_U0_C1(&pulse_l);
		R_PG_Timer_GetCounterValue_MTU_U0_C2(&pulse_r);
		
		R_PG_Timer_SetCounterValue_MTU_U0_C1(0);
		R_PG_Timer_SetCounterValue_MTU_U0_C2(0);
		
		dif_pulse_r = pulse_r + (65536 * pulse_sum_r);
		dif_pulse_l = pulse_l + (65536 * pulse_sum_l);
	
		pulse_sum_r = 0;
		pulse_sum_l = 0;
		
/*		//取得値のオーバーフロー，アンダーフローの処理
		if(pulse_sum_r > 0){				//オーバーフロー
			dif_pulse_r = - (65536 * pulse_sum_r  + pulse_pre_r - pulse_r);				
			pulse_sum_r = 0;
		}else if(pulse_sum_r < 0){			//アンダーフロー
			pulse_sum_r = - pulse_sum_r;
			dif_pulse_r = (65536 * pulse_sum_r) + pulse_pre_r - pulse_r;
			pulse_sum_r = 0;
		}else{
			dif_pulse_r = pulse_r - pulse_pre_r;	//通常時の処理
			pulse_sum_r = 0;
		}
	

		if(pulse_sum_l > 0 ){				//オーバーーフロー
			dif_pulse_l = - (65536 * pulse_sum_l + pulse_pre_l - pulse_l);
			pulse_sum_l = 0;
		}else if(pulse_sum_l < 0){			//アンダーフロー
			pulse_sum_l = - pulse_sum_l;
			dif_pulse_l = (65536 * pulse_sum_l) + pulse_pre_l - pulse_l;
			pulse_sum_l = 0;
		}else {
			dif_pulse_l = pulse_l - pulse_pre_l;	//通常処理
			pulse_sum_l = 0;
		}
*/

/*
	xR = -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096)??? / 4096;
	xL = -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096)??? / 4096;
*/

	//物理量計算
	if(dif_pulse_r != 0 ) xR = Kxr * dif_pulse_r * 0.1;	//割込周期10[ms]での変位[mm] →　速度[m/s]
	else xR = 0;
	if(dif_pulse_l != 0 ) xL = Kxr * dif_pulse_l * 0.1;
	else xL = 0;
	
	xG = 0.5 * (xR + xL);
	
	totalR_mm += xR * 10;
	totalL_mm += xL * 10;
	totalG_mm += xG * 10;
	
	vel_R = xR;					//距離[mm]を時間で除してに直す[m/s] 
	vel_L = xL;
	vel_G = xG;
		
	}
		
	
/*	test_valR[time2] = targ_vel[t_cnt_r];//angle_G, sen_dr;
	test_valL[time2] = xG;//dif_angle, sen_dl;
	
	test_valR1[time2] = dif_pulse_r;//angle_G;//ad_r, kvpR
	test_valL1[time2] = dif_pulse_l;//omega_G;//ad_l, kvdR
	
	test_valR2[time2] = totalL_mm;
	test_valL2[time2] = totalR_mm;

*/
	test_valR[time2] = targ_omega[t_cnt_w];
	test_valL[time2] = omega_G_rad;
	
	test_valR1[time2] = angle_G;
	test_valL1[time2] = kwpG;
	
	test_valR2[time2] = kwdG;
	test_valL2[time2] = kwiG;

	//PIDしてみる？
	
	if(MF.FLAG.VCTRL){
		//偏差の計算
		dif_vel_R = (targ_vel[t_cnt_r] * velR0) - vel_R;
		dif_vel_L = (targ_vel[t_cnt_l] * velL0) - vel_L;
		//偏差のP制御
		kvpR = V_KP * dif_vel_R;				
		kvpL = V_KP * dif_vel_L;
		//偏差のD制御
		kvdR = V_KD * (dif_vel_R - dif_pre_vel_R);	
		kvdL = V_KD * (dif_vel_L - dif_pre_vel_L);
		//偏差のI制御 現在速度係数０とコメントアウトで使っていない
		kviR += V_KI * dif_vel_R;
		kviL += V_KI * dif_vel_L;
		
		//PID制御値を統合
		vpid_R = (kvpR + kvdR + kviR);
		vpid_L = (kvpL + kvdL + kviR);
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_vel_R = dif_vel_R;		
		dif_pre_vel_L = dif_vel_L;

	}else{
		vpid_R = 0;
		vpid_L = 0;
	}
	
	if(MF.FLAG.WCTRL){
		//偏差の計算
		dif_omega = (W_dir * targ_omega[t_cnt_w]) - omega_G_rad;
		//偏差のP制御
		kwpG = W_KP * dif_omega;				
		//偏差のD制御
		kwdG = W_KD * (dif_omega - dif_pre_omega);
		//偏差のI制御
		kwiG = W_KI * dif_omega;	
		//PID制御値を統合
		wpid_G = (kwpG + kwdG + kwiG);
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_omega = dif_omega;		
	}else{
		wpid_G = 0;
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
	pulse_sum_r--;
}
void Mtu2OvIntFunc() {
	pulse_sum_r++;
}

void Mtu1UnIntFunc() {
	pulse_sum_l--;
}
void Mtu1OvIntFunc() {
	pulse_sum_l++;
}


//時間計測用割込み関数2
void Cmt0IntFunc(){
	ms_time++;
}

/*void S12ad0IntFunc(void){
	R_PG_ADC_12_GetResult_S12AD0(ad_res);
}*/
