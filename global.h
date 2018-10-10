﻿/*
==============================================================
 Name        : global.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : グローバル変数です．パルス調整はまめにしましょう．


  更新履歴
 2016/1/30	山上		コメント追加
 2016/2/15	山上		DR_CENT_BACKを追加、使っていないマクロをコメント化
==============================================================
*/

/*ヘッダファイルの読み込み*/
//各ヘッダファイルを一括して読み込む
#include <stdint.h>

#include "macro.h"
#include "port.h"
#include "auxiliary.h"
#include "drive.h"
#include "sensor.h"
#include "search.h"
#include "uart.h"
#include "init.h"
#include "iodefine.h"

#include "R_PG_IntFuncsExtern.h"
#include "R_PG_RPDL_headers.h"
#include "R_PG_Michishirube.h"
#include "pinRX631.h"

#ifndef GLOBAL_H_			//対応ファイルで一度も読み込まれていない場合以下を定義
#define GLOBAL_H_			//読み込んだことを表す
/*============================================================
		各種定数・変数宣言
============================================================*/
/*------------------------------------------------------------
		走行系
------------------------------------------------------------*/
//----走行パルス関連----
//#define DR_SEC_AD 430		//加減速1区画
//#define DR_SEC_U 430		//等速1区画
//#define DR_SEC_ADL 360	//長距離加減速時1区画
#define DR_SEC_HALF 208		//半区画走行用パルス。2倍すると1区画分に 205
#define DR_ROT_R90 3.14*R_BODY*0.5	//右90度距離mm 145
#define	DR_ROT_L90 3.14*R_BODY*0.5  //左90度距離mm 145
#define DR_ROT_180 294		//180度回転用パルス数 289
#define DR_CENT_BACK 300	//後ろ壁に当てるために下がるパルス数
#define DR_CENT_SET 95		//後ろ壁から中央までのパルス数
#define CENTER_TIME 350

#define CONT0 1 //0.0001				//壁Pゲイン
#define CONT1 0.000025				//壁Pゲイン
#define CONT2 0.000025				//壁Pゲイン
#define CONT3 0.0001				//壁Pゲイン
#define CONT4 0.0001				//壁Pゲイン

#define ROT_TIME 355
#define ROT180_TIME 500

#define GYRO_FIX 16.4
#define KW 0.01744				//Pi/180

//----DC走行関連----
#define HALF_MM 90
#define ONE_MM 180
#define STOP_OFF_MM 20

//----テーブル速度関連----
#define MAXSPEED_S 1000		//探索走行時の速度の最大の要素数
#define MINSPEED_S 0		//探索走行時の速度の最小の要素数
#define MAXSPEED_H 287		//高速走行時の速度の最大の要素数
#define MINSPEED_H 100		//高速走行時の速度の最小の要素数

//----エンコーダ・DCモータ関連----
#define DIA_WHEEL_mm 13
#define DIA_PINI_mm 4
#define DIA_SQUR_mm 17.5
#define Pi 3.1415
#define Ktolk 1.98
#define Rmotor 1.07
#define R_BODY 35
#define MASS 0.1
#define VOLT_BAT 7.4
#define KR 1.1
#define KL 1

//---速度PIDゲイン---
#define V_KP 1.3
#define X_KP 1.3
#define V_KD 0.2
#define X_KD 0.1
#define V_KI 0

#define W_KP 10
#define W_KD 1

#define A_KP 0.05
#define A_KD 0

//----タイマ関連----
#define DEFGRC 22000		//デフォルトのインターバル

/*------------------------------------------------------------
		センサ系t
------------------------------------------------------------*/
//----壁判断基準----
#define WALL_BASE_F 3700		//前壁
#define WALL_BASE_L 3200		//左壁
#define WALL_BASE_R 3200		//右壁

#define WALL_START 3500

//----制御基準値----
#define SREF_MIN_L 10	//左制御基準下限-20
#define SREF_MAX_L 3000	//左制御基準上限280
#define SREF_MIN_R 10	//右制御基準下限-20
#define SREF_MAX_R 3000 //右制御基準上限280

/*------------------------------------------------------------
		探索系
------------------------------------------------------------*/
//----ゴール座標----
#define GOAL_X 7	//7
#define GOAL_Y 7	//7

/*------------------------------------------------------------
		共用・構造体の定義
------------------------------------------------------------*/
//共用・構造体とは、共用体と構造体を組み合わせたもので、
//内部の一括操作も、メンバ単位での操作も可能なものである。
//例えば、以下のmouse_flags共用・構造体のMFでは、
//MF.FLAGS = 0x10;と全体を一括変更できるほか、
//MF.FLAG.DECL = 1;とメンバを指定して単体で変更することも出来る。
//参考：
//https://www.grapecity.com/tools/support/powernews/column/clang/033/page02.htm

//----フラグ共用・構造体----
//順番は間違ってません
#ifndef __MOUSE_FLAGS__					//対応ファイルで一度も読み込まれていない場合以下を定義
	#define __MOUSE_FLAGS__				//読み込んだことを表す
	typedef union {						//共用体の宣言
		uint16_t FLAGS;
		struct ms_flags{				//構造体の宣言
			unsigned char RSV:1;		//予備ビット(B0)		(:1は1ビット分の意味、ビットフィールド)
			unsigned char SCND:1;		//二次フラグ(B1)
			unsigned char SLAL:1;		//旋回フラグ(B2)
			unsigned char CTRL:1;		//制御フラグ(B3)
			unsigned char ACCL:1;		//加速フラグ(B4)
			unsigned char DECL:1;		//減速フラグ(B5)
			unsigned char DEF:1;		//定速フラグ(B6)
			unsigned char STOP:1;		//停止フラグ(B7)
			unsigned char R_DIR:1;		//予備ビット(B8)		
			unsigned char L_DIR:1;		//予備フラグ(B9)
			unsigned char VCTRL:1;		//予備フラグ(B10)
			unsigned char XCTRL:1;		//予備フラグ(B11)
			unsigned char WCTRL:1;		//予備フラグ(B12)
			unsigned char ACTRL:1;		//予備フラグ(B13)
			unsigned char S7:1;		//予備フラグ(B14)
			unsigned char S8:1;		//予備フラグ(B15)
		}FLAG;
	} mouse_flags;
#endif

#ifdef EXTERN							//対応ファイルでEXTERNが定義されている場合
	/*グローバル変数の定義*/
	volatile mouse_flags MF;			//モータステータスをまとめた共用・構造体
#else									//対応ファイルでEXTERNが定義されていない場合
	/*グローバル変数の宣言*/
	extern volatile mouse_flags MF;
#endif



//----現在地格納共用・構造体----
#ifndef __MAP_COOR__					//対応ファイルで一度も読み込まれていない場合以下を定義
	#define __MAP_COOR__				//読み込んだことを表す
	union map_coor{						//共用体の宣言
		unsigned char PLANE;					//YX座標
		struct coor_axis{				//構造体の宣言
			unsigned char X:4;					//X座標
			unsigned char Y:4;					//Y座標
		}AXIS;
	};
#endif

#ifdef EXTERN							//対応ファイルでEXTERNが定義されている場合
	/*グローバル変数の定義*/
	volatile union map_coor PRELOC;		//現在地の座標を格納する共用・構造体
#else									//対応ファイルでEXTERNが定義されていない場合
	/*グローバル変数の宣言*/
	extern union map_coor PRELOC;
#endif

#endif /* GLOBAL_H_ */
