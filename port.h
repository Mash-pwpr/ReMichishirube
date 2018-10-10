/*
==============================================================
 Name        : port.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 各種ピンの入出力およびその設定を行う

 更新履歴
 2015/12/4		山上　コメント追加
==============================================================
*/

/*ヘッダファイルの読み込み*/
#include "macro.h"
#include"stdint.h"

#ifndef PORT_H_											//対応ファイルで一度も読み込まれていない場合以下を定義

	#define PORT_H_										//読み込んだことを表す


	enum {												//GPIOの子番号の列挙型
		P0_0, P0_1, P0_2, P0_3,
		P0_4, P0_5, P0_6, P0_7,
		P0_8, P0_9, P0_10, P0_11,
		P0_12, P0_13, P0_14, P0_15,

		P1_0, P1_1, P1_2, P1_3,
		P1_4, P1_5, P1_6, P1_7,
		P1_8, P1_9, P1_10, P1_11,
		P1_12, P1_13, P1_14, P1_15,

		P2_0, P2_1, P2_2, P2_3,
		P2_4, P2_5, P2_6, P2_7,
		P2_8, P2_9, P2_10, P2_11,
		P2_12, P2_13, P2_14, P2_15,

		P3_0, P3_1, P3_2, P3_3,
		P3_4, P3_5
	};

	enum{												//ピンの入出力の列挙型
		IN,												//入力(=0)
		OUT												//出力(=1)
	};


	/*マクロの定義*/
	#define PIN_H(pin)			pin_write(pin, 1)									//対応ピンの出力をHighにする
	#define PIN_L(pin)			pin_write(pin, 0)									//対応ピンの出力をLowにする
	#define PIN_SET(pin, dat)	pin_write(pin, dat)									//対応ピンの出力を設定

	#define PIN_DIR(pin, dir)	PIN_SET(pin, dir)									//対応ピンの入出力を設定

/*	#define PIN_IS_SET(pin)		bit_is_set(LPC_GPIO[pin>>4]->DATA, pin&0x0f)		//対応ピンの入力がHighか判定
	#define PIN_IS_CLEAR(pin)	bit_is_clear(LPC_GPIO[pin>>4]->DATA, pin&0x0f)		//対応ピンの入力がLowか判定
	#define PIN_STAT(pin)		PIN_IS_SET(pin)										//対応ピンの入力を返す
*/

	/*関数のプロトタイプ宣言*/
	void pin_write(uint16_t pin, unsigned char dat);
	void pins_write(uint16_t pin[], unsigned char dat, unsigned char cnt);
	void pin_dir(uint16_t pin, unsigned char dir);
	void pins_dir(uint16_t pin[], unsigned char dir, unsigned char cnt);

#endif /* PORT_H_ */
