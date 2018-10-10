/************************************************************
		Micromouse �v���O����
		2007�N~2012�N	H8/3694f�ł��^�p�����
		2013/03/01		LPC�����ɈڐA					���J�� ���l
		2013/09/20		�W�����̂��ߐF�X�M��				���� �L��
 ************************************************************/
/*
==============================================================
 Name        : main.c
 Copyright   : Copyright (C) ����c��w�}�C�N���}�E�X�N���u
 Description : main definition

 �X�V����
 2016/2/12�@�[�R�@�ꕔ�R�����g�ǉ�
 2017/6/27 �W�@�A���T�����s�̖������ł�����
==============================================================
 */

#define EXTERN
#include "global.h"

/*===========================================================
		���C���֐�
===========================================================*/
int main(void) {

	//====�ϐ��錾====
	char mode = 0;
	uint16_t i,j,res_data;
	float ttt;
	//====������====
	R_PG_Clock_Set();					//�N���b�N�ݒ�

	port_Init();						//port�̏�����
	val_Init();							//�e��ϐ��̏�����
	timer_Init();						//�^�C�}�n�̏�����
	sensor_Init();						//�Z���T�n�̏�����
	uart_Init();					//�V���A���ʐM�̏�����

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
		ms_wait(200);			//���x���[���Ɏ�������܂ő҂�
		
			
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
		//====���[�h�Z���N�g====
		select_mode(&mode);
		ms_wait(100);
		//----�I�����ڂ̎��s----
		switch(mode){
		case 0:	//----��l�����----
			get_base();											//get_base()��sensor.c�Ɋ֐���`���� �@����̂��߂̕Ǌ�l�擾
			//----�����V���A�����M----
			//uart_printf("base_l = %3d, ", base_l);				//UART_printf()��uart.c�Ɋ֐���`����
			//uart_printf("base_r = %3d\r", base_r);
			ms_wait(500);
			uart_printf("START\r\n");
			for(i=0;i<2000;i++){
				uart_printf("%lf, %lf,%lf, %lf\r\n",test_valR1[i],test_valL1[i],test_valR[i],test_valL[i]);
				ms_wait(1);
			}
			uart_printf("ALL\r\n");

			break;
		case 1:	//----�ꎟ�T�����s----
			goal_x = GOAL_X;									//�S�[�����W��ݒ�@�@GOAL_X��global.h�Ƀ}�N����`����
			goal_y = GOAL_Y;									//�S�[�����W��ݒ�@�@GOAL_Y��global.h�Ƀ}�N����`����

			start_wait();
			start_ready();
			
			get_wall_info();									//�Ǐ��̏�����     get_wall_info()��sensor.c�Ɋ֐���`����
			searchA();										//�S�[�����܂ŒT���C�i�s����@searchA()��search.c�Ɋ֐���`����
			goal_x = goal_y = 0;								//�S�[�����W���X�^�[�g���ɐݒ�
			Wait;												//�ҋ@
			searchA();											//�߂��Ă���

			goal_x = GOAL_X;									//�S�[�����W�ݒ�
			goal_y = GOAL_Y;									//�S�[�����W�ݒ�	
			break;

			//----�A���T�����s----
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

			turn_180();									//180�x��]
			turn_dir(DIR_TURN_180);
			break;

			/////////////////////////////////�@�@���̓񎟒T�����s�ƃX�����[�����s�͖�����
			//----�񎟍������s----
		case 3:
		
			
			break;

			//----�X�����[�����s----
		case 4:
			break;
			//////////////////////////////////

			//----���s�e�X�g----
		case 5:
			Wait;
			start_wait();
			set_dir(FORWARD);
			drive_start();
			while(1){
				uart_printf("Driving\r\n");
			}
			test_drive(&mode);									//test_drive()��drive.c�Ɋ֐���`����
			ms_wait(100);
			break;
			//----�G���R�[�_�e�X�g----
		case 6:
			Wait;
			start_wait();
			enc_test();
			ms_wait(100);
			break;
			
			
			//----�Z���T�l, �����m�F----
			//LED�_���͗v�ύX
		default:
			Wait;
			val_Init();
			MF.FLAG.CTRL = 1;	//���䋖��
			R_PG_Timer_StartCount_CMT_U0_C1();
			while(1){
				pins_write(DISP_LEDS, 0, LED_NUM);											//pins_write()��port.c�Ɋ֐���`����
				uart_printf("ad_l: %4d ad_fl:%4d ad_ff:%4d  ad_fr:%4d ad_r:%4d ", ad_l, ad_fl, ad_ff, ad_fr, ad_r);
				uart_printf(" | dif_l: %4d dif_r:%4d\r\n", dif_l, dif_r);
				//----LED��4�̏ꍇ----
				if(ad_fr > WALL_BASE_F){
					// �����Aad_l�ɂȂ��Ă܂�����I�I
					pin_write(DISP_LEDS[0], ON);											//pin_write()��port.c�Ɋ֐���`����
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
