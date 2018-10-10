/*
==============================================================
 Name        : auxiliary.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 補助関数たちです．労わりましょう．

 更新履歴
 2015/1/30		山上	一部コメント追加
 2016/2/24		山上	2016年度用にピン設定を変更
==============================================================
*/
#include"pinRX631.h"

#ifndef AUXILIARY_H_									//対応ファイルで一度も読み込まれていない場合以下を定義
	#define AUXILIARY_H_								//読み込んだことを表す

/*============================================================
		各種定数･変数宣言
============================================================*/
	//----スイッチ関連----
	//Custom
	#define	INC		1	//加算ボタン
	#define	RET		1	//決定ボタン
	#define	DEC		1	//減算ボタン

	//----汎用----
	#define ON 1
	#define OFF 0

	//----表示用LED関連----
	#ifdef EXTERN										//対応ファイルでEXTERNが定義されている場合
		/*グローバル変数の定義*/
		uint16_t DISP_LEDS[4] = {PB6, PB7, PC2,PC3};	//表示用LEDのピンの配列
	#else												//対応ファイルでEXTERNが定義されていない場合
		/*グローバル変数の宣言*/
		extern uint16_t DISP_LEDS[4];
	#endif

	//----LEDの数----
	#define LED_NUM 4

	//----その他----
	#define DEFWAIT		10000							//wait()用定数
	#define Wait		ms_wait(200)					//少し待機する

/*============================================================
		関数プロトタイプ宣言
============================================================*/
	//====その他====
	void stay(unsigned int);
	void ms_wait(unsigned int);
	void select_mode(char *mode);						//モードの入力
	void timer_start();
	void melody(uint16_t,uint16_t);
	void start_wait();
	void start_ready();

#endif /* AUXILIARY_H_ */
