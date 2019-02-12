/*
==============================================================
 Name        : uart.c
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 通信用関数たちです．

  更新履歴
 2015/12/10		山上	コメント追加、uart_Init内LCR処理を変更
==============================================================
*/

//※解説の章番号等は日本語版ユーザーマニュアルRev.00.15に準拠

/*ヘッダファイルの読み込み*/
#include "global.h"
#include <stdio.h>
#include <stdarg.h>															// 可変引数リストを使用

//+++++++++++++++++++++++++++++++++++++++++++++++
//uart_Init
//	UARTの初期設定を行う
// 引数1：baudrate・・・通信のボーレート
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void uart_Init(){
	R_PG_SCI_Set_C1();		//シリアル通信用準備
	R_PG_SCI_Set_C8();
	pin_write(PC4,1);		//SPI通信のCSピンを初期化しておく
	
	SPI_write_byte(0x6b,0x00);	//PWR_MGMT_1レジスタ
	SPI_write_byte(0x1a,0x00);	//CONFIG レジスタ
	SPI_write_byte(0x1b,0x18);	//GYRO_CONFIG　レジスタ
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//uart_printf
//	UARTで標準出力を行う
//	対応指定子	：%s,%c,%d,%u,%X,%ld,%lu,%lX
//				　数値はリーディングゼロと表示桁数指定可能
// 引数1：format・・・送信する本文
// 引数2以降:・・・可変引数(printf等の本文以降の引数のようなもの)
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++

void uart_printf(const char *format, ...) {
    va_list arg;
    uint8_t buf[64];
    uint8_t *p;

    va_start(arg, format);

    vsprintf((char*)buf, format, arg);
    p = buf;
    while( *p != '\0' ) {
        R_PG_SCI_SendAllData_C1(p, 1);
        p++;
    }
    va_end(arg);
}
    
//+++++++++++++++++++++++++++++++++++++++++++++++
//SPI_read_byte
//	RXの簡易SPI通信でジャイロとお話しするよ！！
// 引数1：受け取りたいデータのアドレス
// 戻り値：受け取りたいデータの中身
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t SPI_read_byte(uint8_t addr){
	uint8_t ret, val;
	uint8_t tx_data[2];
	uint8_t rx_data[2] = {0,0};
	
	ret = addr | 0x80;
	tx_data[0] = ret;
	tx_data[1] = ret;

	pin_write(PC4,0);				//csピンをLOW　にしてSPIをイネーブルに
	R_PG_SCI_SPIMode_Transfer_C8(tx_data,rx_data,2);
	pin_write(PC4,1);				//csピンをHIGHにしてディスネーブルに
	//uart_printf("0x%x,0x%x\r\n",rx_data[0],rx_data[1]);
	val = rx_data[1];
	
	return val;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SPI_write_byte
//	RXの簡易SPI通信でジャイロとお話しするよ！！
// 引数1：書き込みたいデータのアドレス
// 引数2：書き込みたいデータの内容
//+++++++++++++++++++++++++++++++++++++++++++++++

void SPI_write_byte(uint8_t addr, uint8_t data){
	uint8_t ret;
	uint8_t tx_data[2];
	uint8_t rx_data[2] = {0,0};
	
	ret = addr & 0x7f;
	tx_data[0] = ret;
	tx_data[1] = data;
	pin_write(PC4,0);
	R_PG_SCI_SPIMode_Transfer_C8(tx_data,rx_data,2);
	pin_write(PC4,1);
	
}

float GYRO_read(void){
	int16_t gyro_z;
	float omega;
	gyro_z = (int16_t)(SPI_read_byte(0x47) << 8 | SPI_read_byte(0x48));	//0x47が上位，0x48が下位の16bitデータでジャイロ値を取得
	omega = (float)((gyro_z - gyro_base) / GYRO_FIX);
	return omega;
}

void GYRO_OFFSET(uint16_t num){
	float gyro_offset = 0;
	int i;
	
	for(i=0;i<num;i++){
		gyro_offset += GYRO_read();
		ms_wait(1);
	}
	gyro_base = gyro_offset / num;
}