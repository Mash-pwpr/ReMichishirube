/*
==============================================================
 Name        : uart.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 通信用関数たちです．

  更新履歴
 2015/12/10		山上	コメント追加
==============================================================
*/
#ifndef UART_H_											//対応ファイルで一度も読み込まれていない場合以下を定義
	#define UART_H_										//読み込んだことを表す
/*============================================================
		関数プロトタイプ宣言
============================================================*/
		void uart_Init();
		void uart_printf(const char* format, ...);
		uint8_t SPI_read_byte(uint8_t);
		void SPI_write_byte(uint8_t, uint8_t);
		float GYRO_read(void);
		void GYRO_OFFSET(uint16_t);
		
#endif /* UART_H_ */
