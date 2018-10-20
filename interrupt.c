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
	uint16_t paraL1 = 65535;

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
		//duty_oL = duty_l;

/*		//----停止処理----
		if(MF.FLAG.STOP){															
			paraL1 = 0;
				//----定速処理----
		}else if(MF.FLAG.DEF){														
			paraL0 = DEFGRC - sen_dl;
			//----その他----
		}else{
			paraL0 = table[t_cnt_l] - sen_dl;									
		}
*/
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
	uint16_t paraR0 = 40000;
	uint16_t paraR1 = 65535;

	pin_write(PA4,0);
	pin_write(PA6,0);

	//duty_r = Kvolt * accel_r / VOLT_BAT + kpvR + kpdR;
	duty_r = KR * (vpid_R + sen_dr + xpid_G + wpid_G + apid_G);
	if(duty_r < 0){
		MF.FLAG.R_DIR = 0;
		//duty_oR = duty_r;
		duty_r = - duty_r;	
	}else if(duty_r > 0){
		MF.FLAG.R_DIR = 1;					
		//duty_oR = duty_r;
	}
	
	if(duty_r > 1){
		duty_r = 0.70;
	}else if(duty_r <= 0.01){
		duty_r = 0.01;	
	}
	
/*	//----停止処理----
	if(MF.FLAG.STOP){			
		paraR1 = 0;
	
//----定速----
	}else if(MF.FLAG.DEF){														
		paraR0 = DEFGRC - sen_dr;							
		//----?????????----
	}else{
		paraR0 = table[t_cnt_r] - sen_dr;									
	}
*/
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
		angle_G += omega_G * 0.001;
		
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
		}else if(MF.FLAG.WCTRL){
			if(MF.FLAG.DECL){															
				if(t_cnt_w < minindex) t_cnt_w--; 
			}
			else if(MF.FLAG.ACCL){
				if(t_cnt_w > maxindex_w) t_cnt_w++; 	
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
		break;
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
	
	//取得値のオーバーフロー，アンダーフローの処理
	if(pulse_sum_r > 0){				//アンダーフロー
		dif_pulse_r = - (65535 * pulse_sum_r  + pulse_pre_r - pulse_r) ;
		pulse_sum_r = 0;
	}else if(pulse_sum_r < 0){			//オーバーフロー
		pulse_sum_r = - pulse_sum_r;
		dif_pulse_r = 65535 * pulse_sum_r + pulse_pre_r - pulse_r;
		pulse_sum_r = 0;
	}else{
		dif_pulse_r = pulse_r - pulse_pre_r;	//通常時の処理
		pulse_sum_r = 0;
	}
		
	if(pulse_sum_l > 0 ){				//アンダーフロー
		dif_pulse_l = - (65535 * pulse_sum_l + pulse_pre_l - pulse_l);
		pulse_sum_l = 0;
	}else if(pulse_sum_l < 0){			//オーバーフロー
		pulse_sum_l = - pulse_sum_l;
		dif_pulse_l = 65535 * pulse_sum_l + pulse_pre_l - pulse_l;
		pulse_sum_l = 0;
	}else {
		dif_pulse_l = pulse_l - pulse_pre_l;	//通常処理
		pulse_sum_l = 0;
	}
	pulse_pre_r = pulse_r;
	pulse_pre_l = pulse_l;
/*
	xR = -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_r % 4096) / 4096;
	xL = -DIA_WHEEL_mm * (DIA_PINI_mm / DIA_SQUR_mm) * 2 * Pi * (dif_pulse_l % 4096) / 4096;
*/

	//物理量計算
	xR = Kxr * (dif_pulse_r % 4096);	//割込周期1[ms]での変位[mm] →　速度[m/s]
	xL = Kxr * (dif_pulse_l % 4096);
	xG = 0.5 * (xR + xL);

	totalR_mm += xR;
	totalL_mm += xL;
	totalG_mm += xG;
		
	pulse_pre_r = pulse_r;
	pulse_pre_l = pulse_l;
		
	vel_R = xR;					//距離[mm]を時間で除してに直す[m/s] 
	vel_L = xL;
	vel_G = xG;
		
/*	test_valR[time2] = kxpG;//angle_G, sen_dr;
	test_valL[time2] = kxdG;//dif_angle, sen_dl;
	
	test_valR1[time2] = totalG_mm;//ad_r
	test_valL1[time2] = dif_x_G;//ad_l
	
	test_valR2[time2] = apid_G;//dif_r	
	test_valL2[time2] = 0;//dif_l
	
	test_valR[time] = targ_vel[t_cnt_r];
	test_valL[time] = targ_vel[t_cnt_l];
	
	test_valR1[time] = totalR_mm;
	test_valL1[time] = totalL_mm;
	
*/		
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
		//kviR += V_KIR * dif_vel_R;
		//kviL += V_KIL * dif_vel_L;
		
		//PID制御値を統合
		vpid_R = (kvpR - kvdR + kviR);
		vpid_L = (kvpL - kvdL + kviR);
		//現在の偏差をバッファに保存 D制御で使う
		dif_pre_vel_R = dif_vel_R;		
		dif_pre_vel_L = dif_vel_L;

	}else{
		vpid_R = 0;
		vpid_L = 0;
	}
	
	if(MF.FLAG.WCTRL){
		//偏差の計算
		dif_omega = targ_omega[t_cnt_w] - omega_G_rad;
		//偏差のP制御
		kwpG = W_KP * dif_omega;				
		//偏差のD制御
		kwdG = W_KD * (dif_omega - dif_pre_omega);	
			
		//PID制御値を統合
		wpid_G = (kwpG - kwdG);
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
	
/*		dif_x_R = (targ_total_mm - totalR_mm);
		dif_x_L = (targ_total_mm - totalL_mm);	
		kxpR = X_KP * dif_vel_R;
		kxpL = X_KP * dif_vel_L;
		kxdR = X_KD * (dif_x_R - dif_pre_x_R);
		kxdL = X_KD * (dif_x_L - dif_pre_x_L);
		xpid_R = (kxpR - kxdR);
		xpid_L = (kxpL - kxdL);
		//現在の偏差をバッファに保存 D制御で使う	
		dif_pre_x_R = dif_x_R;
		dif_pre_x_L = dif_x_L;
*/	}else{
		xpid_G = 0;
	}

}


void Mtu2UnIntFunc() {
	pulse_sum_r += 1;
}
void Mtu2OvIntFunc() {
	pulse_sum_r -= 1;
}

void Mtu1UnIntFunc() {
	pulse_sum_l += 1;
}
void Mtu1OvIntFunc() {
	pulse_sum_l -= 1;
}

void Cmt0IntFunc(){
	ms_time++;
}

/*void S12ad0IntFunc(void){
	R_PG_ADC_12_GetResult_S12AD0(ad_res);
}*/
