﻿/*
==============================================================
 Name        : init.h
 Copyright   : Copyright (C) 早稲田大学マイクロマウスクラブ
 Description : 初期化関数たちです．

 更新履歴
 2016/2/6		深山	一部コメント追加
==============================================================
*/


#ifndef TIMER_H_												//対応ファイルで一度も読み込まれていない場合以下を定義
	#define TIMER_H_											//読み込んだことを表す
/*============================================================
		関数プロトタイプ宣言
============================================================*/
		void port_Init();		//各種入出力の設定
		void sensor_Init();		//センサー設定
		void val_Init();		//センサ系，走行系，探索系　の変数初期化
		void timer_Init();		//タイマ初期化
		void batt_Check(uint16_t);

#endif /* TIMER_H_ */
