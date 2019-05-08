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
	uint16_t i;
//	float ttt;
	//====������====
	R_PG_Clock_Set();					//�N���b�N�ݒ�

	port_Init();						//port�̏�����
	timer_Init();						//�^�C�}�n�̏�����
	sensor_Init();						//�Z���T�n�̏�����
	uart_Init();						//�V���A���ʐM�̏�����
	val_Init();						//�e��ϐ��̏�����
	map_Init();

	//ttt = GYRO_read();
	//uart_printf("%lf\r\n",ttt);
	batt_Check(1000);
	//GYRO_OFFSET(1000);
	
//	melody(c6h,1000);		

/*	start_wait();
	GYRO_OFFSET(1000);
*/	
	sensor_start();
	start_wait();
	start_ready();
	
	MF.FLAG.CTRL = 0;
	time = time2 = 0;
	set_dir(FORWARD);
	driveA(100);
	driveD(100,1);


/*	time = time2 = 0;
	MF.FLAG.CTRL = 0;
	driveA(90 + SET_MM);
	driveA(180);
	driveA(180);
	MF.FLAG.CTRL = 0;
	driveD(90,1);	
*/
/*	set_dir(TURN_R);
	turn_R90();
*/	

//	MF.FLAG.CTRL = 1;
//	ms_wait(10);
//	auto_Calibration(0.5,0.5);
//	ms_wait(1000);
//	driveA(SET_MM + 90);
//	turn_SLA_R90();
//	turn_SLA_L90();

/*	while(1){
		driveA(180 * 3);
		turn_SLA_L90();			
	}	
*/		
//	driveD(90,1);
//	turn_SLA_L90();
	
/* �T�[�L�b�g��p�v���O����	
	get_base();
	MF.FLAG.CTRL = 1;
	driveA(SET_MM);
	half_sectionA();
	for(i = 0;i<4;i++){
		driveA(14*180 - 90);
		half_sectionD();
		turn_R90();
		driveA(6*180 - 90);
		half_sectionD();
		turn_R90();
	}
	half_sectionA();
	half_sectionD();
*/	
	sensor_stop();

	while(1){ // Main Loop
		uart_printf("Hello, World!\r\n");	
		//====���[�h�Z���N�g====
		select_mode(&mode);
		ms_wait(100);
		//----�I�����ڂ̎��s----
		switch(mode){
		case 0:	//----��l�����----
			
			//----�����V���A�����M----
			ms_wait(500);
			uart_printf("START\r\n");
			
			uart_printf("base:%d, %d\r\n", wall_l.threshold, wall_r.threshold);
			//uart_printf("targ\tvelG\tkvpR\tkvpL\t%lf\r\n",offsetA);
/*			for(i=0;i<2000;i++){
				uart_printf("%lf, %lf,%lf, %lf, %lf, %lf\r\n",log.test1[i],log.test2[i],log.test3[i],log.test4[i],log.test5[i],log.test6[i]);
				ms_wait(1);
			}
*/			uart_printf("ALL\r\n");
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
			sensor_stop();
			turn_dir(DIR_TURN_180);
			break;

			/////////////////////////////////�@�@���̓񎟒T�����s�ƃX�����[�����s�͖�����
			//----�񎟍������s----
		case 3:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSA_ESNW();
			goal_x = goal_y = 0;
			searchSA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180�x��]
			turn_dir(DIR_TURN_180);
			break;

			//----�X�����[�����s----
		case 4:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSLA();
			goal_x = goal_y = 0;
			searchSLA();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180�x��]
			turn_dir(DIR_TURN_180);

			break;
			//////////////////////////////////

		case 5:
			goal_x = GOAL_X;
			goal_y = GOAL_Y;
			
			start_wait();
			start_ready();
			
			searchSLA_ESNW();
			goal_x = goal_y = 0;
			searchSLA_ESNW();
			goal_x = GOAL_X;
			goal_y = GOAL_Y;

			turn_180();									//180�x��]
			turn_dir(DIR_TURN_180);
			break;

			
			//----���s�e�X�g----
		case 11:
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
		case 12:
			Wait;
			start_wait();
			enc_test();
			ms_wait(100);
			break;
		
		case 13:
			Wait;
			start_wait();
			
			/*����|���[�h*/
			MF.FLAG.ACTRL = 0;
			MF.FLAG.VCTRL = 1;
			MF.FLAG.WCTRL = 1;
			MF.FLAG.XCTRL = 0;

			MF.FLAG.ACCL = 0;
			MF.FLAG.DECL = 0;
			MF.FLAG.WACCL = 0;
			MF.FLAG.WDECL = 0;
			
			centor.vel_target = 0;
			omega.target = 0;
			
			drive_start();
			while(1){
//				uart_printf("angle : %lf dif_angle : %lf tpid_G : %lf\r\n");
				ms_wait(100);
			}
			break;
			
			//----�Z���T�l, �����m�F----
			//LED�_���͗v�ύX
		default:
			Wait;
			val_Init();
			start_wait();
			sensor_check();
			ms_wait(100);
			break;
		}
		ms_wait(100);
		sensor_stop();

	}

	return 0 ;
}
