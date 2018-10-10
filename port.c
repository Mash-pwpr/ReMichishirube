/*
==============================================================
 Name        : port.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 各種ピンの入出力およびその設定を行う

 更新履歴
 2015/12/4		山上　コメント追加
==============================================================
*/



/*ヘッダファイルの読み込み*/
#include "port.h"
#include "r_pdl_io_port.h"
#include "r_pdl_definitions.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_write
//	指定されたピンをHighまたはLowにする
// 引数1：pin・・・ピン番号(Px_y)
// 引数2：dat・・・HighかLowか(0:Low,それ以外:High)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_write(uint16_t pin, unsigned char dat){

	R_IO_PORT_Write(pin,dat);

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pins_write
//	指定されたピン配列をHighまたはLowにする
// 引数1：pin・・・ピン番号の配列(Px_y)
// 引数2：dat・・・HighかLowかのビット列(0:Low,1:High)
// 引数3：cnt・・・ピンの個数
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pins_write(uint16_t pin[], unsigned char dat, unsigned char cnt){
	//変数の定義
	int i;


	for(i=0; i<cnt; i++) PIN_SET(pin[i], bit_is_set(dat, i));		//各ピンに対応するDATAレジスタの場所を変更
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_dir
//	指定されたピンの入出力を設定する
// 引数1：pin・・・ピン番号(Px_y)
// 引数2：dir・・・入力か出力か(0:入力,それ以外:出力)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_dir(uint16_t pin, unsigned char dir){

	R_IO_PORT_Write(pin,dir);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pins_dir
//	指定されたピン配列の入出力を設定する
// 引数1：pin・・・ピン番号(Px_y)
// 引数2：dir・・・入力か出力かのビット列(0:入力,1:出力)
// 引数3：cnt・・・ピンの個数
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void pins_dir(uint16_t pin[], unsigned char dir, unsigned char cnt){
	//変数の定義
	int i;


	for(i=0; i<cnt; i++) pin_dir(pin[i], dir);						//各ピンでpin_dirを実行

}
