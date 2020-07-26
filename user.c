/* 2020 年度 専門実験 A マイコン演習 自由課題 （3EP3-78 成田 力哉 ）
* Title： フラッシュザウルス
* Last update: 2020/７/26
* Copyright(c) RIKIYA NARITA @ Kanazawa Institute of Technology, All rights reserved.
*/

#include <string.h>
#include "user.h"

static void TimerCount(void);
static void PushCount(void);
static void UpdateLED1(void);
static int waitTimer(void);
static void UpdateLED2(void);
static int dotFall(void);
static void PrintTitle(void);

#define pushMax 40
/*ローカル変数*/
static uchar CountTimer[LED_SZ];
static uchar push[LED_SZ];
static uchar timer = 0;
static uchar pushCount = 0;
static uchar speed[5] = {4,3,2,1,0};
static uchar speed_sel = 0;
/*タイトル画面*/
static uchar title[LED_SZ]
={0b11110000,
  0b10000000,
  0b11110000,
  0b10000000,
  0b00001111,
  0b00000100,
  0b00000010,
  0b00001111
};
/*止める位置を保持するための配列*/
static uchar st_point[LED_SZ]
={0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b11000011,
  0b00000000,
  0b00000000};
static uchar fall_dot[LED_SZ]={0,0,0,0,0,0,0,0};;

void user_init(void)
{
    uchar i;
	uchar j;
	for(i = 0; i < LED_SZ; i++){
		push[i] = 0;
		CountTimer[i] = 0;
	}
}
void user_main(void)
{	
	static uchar stat = 0;
	switch(stat){
	case 0:
		PrintTitle();
		switch(sw){
			case 1:
			case 2:
				stat = 1;
				sw_flag = 0;
			break;		
		}
	break;
	case 1:
		PushCount();
	    TimerCount();
		UpdateLED1();
		if(pushCount == 40){
		stat = 2;
		}
	break;
	case 2:
		if(waitTimer() == 1){
			stat = 3;
			
		}
	break;
	case 3:
		if(dotFall() == 1){
			stat = 4;		
		}
		UpdateLED2();
		switch(sw){
		case 3:
			stat = 4;
			break;		
		}
	break;
	case 4:
		//ゲームクリアはLEDの要素を見て判定している．
		if(led[5] == 0b11011011){
			_sound(BEEP_HIGH,10);
		}else{
			_sound(BEEP_LOW,10);		
		}
		stat = 5;
	break;
	//ブザーを切るための処理
	case 5:
	break;
	}
}

static void TimerCount(void){
	static uchar x = 0;
	static uchar counter10 = 0;
	//LEDの点灯を1秒に調整するための変数
	counter10++;
	if(counter10 == 10){
		counter10 = 0;
		if(timer < 33){
			timer++;
		}
		
		if(timer <= 8){
			CountTimer[7 - x] |= 0b00001000;
			if(x < 7){
				x++;
				_sound(BEEP_LOW,1);		
			}else{
				x = 0;
				_sound(BEEP_HIGH,1);
				speed_sel = 1;		
			}
		}else if(timer <= 16){
			CountTimer[7 - x] |= 0b00000100; 
			if(x < 7){
				x++;
				_sound(BEEP_LOW,1);		
			}else{
				x = 0;
				_sound(BEEP_HIGH,1);
				speed_sel = 2;		
			}
		}else if(timer <= 24){
			CountTimer[7 - x] |= 0b00000010;
			if(x < 7){
				x++;
				_sound(BEEP_LOW,1);		
			}else{
				x = 0;
				_sound(BEEP_HIGH,1);
				speed_sel = 3;		
			} 
		}else if(timer <= 32){
			CountTimer[7 - x] |= 0b00000001; 
			if(x < 7){
				x++;
				_sound(BEEP_LOW,1);		
			}else{
				x = 0;
				_sound(BEEP_HIGH,1);
				speed_sel = 4;		
			}
		}
	}
}

static void PushCount(void){
	switch(sw){
		case 3:
			pushCount++;
			sw_flag = 0;
	}
	if(pushCount == 5){
		push[7] |= 0b11110000; 
	}else if(pushCount == 10){
		push[6] |= 0b11110000;
	}else if(pushCount == 15){
		push[5] |= 0b11110000;
	}else if(pushCount == 20){
		push[4] |= 0b11110000;
	}else if(pushCount == 25){
		push[3] |= 0b11110000;
	}else if(pushCount == 30){
		push[2] |= 0b11110000;
	}else if(pushCount == 35){
		push[1] |= 0b11110000;
	}else if(pushCount == 40){
		push[0] |= 0b11110000;
	}
	
}

int waitTimer(void){
	static uchar counter = 0;
	static uchar timer = 0;
	//counterで10を数えさせることにより1秒ずつの処理にしている.
	if(++counter == 10){
		counter = 0;
		timer++;
		_sound(BEEP_HIGH,1);
	}
	if(timer == 4){
		return 1;	
	}else{
		return 0;
	}
	
}

int dotFall(void){
	static uchar timer = 0;
	static uchar counter = 0;
	/*流す速さをcounterという変数を用いて調整している*/
	if(counter == speed[speed_sel]){
		counter = 0;
		if(timer > 0){
			fall_dot[timer - 1] = 0;	
		}
		fall_dot[timer] = 0b00011000;
		timer++;
		//ビープ音を区切って流している．
		if(timer % 2 == 0)
			_sound(BEEP_HIGH,1);
		//LEDが一番下に来たときはゲームオーバー
		if(timer == 8)
			return 1;
		else
			return 0;
	}else{
		counter++;	
		return 0;
	}
}

void PrintTitle(void){
	uchar i;
	for(i = 0; i < LED_SZ; i++){
		led[i] = title[i];
	}
}


void UpdateLED1(void){
	uchar i;
	for(i = 0; i < LED_SZ; i++){
		led[i] = CountTimer[i] | push[i];
	}
}

void UpdateLED2(void){
	uchar i;
	for(i = 0; i < LED_SZ; i++){
		led[i] = st_point[i] | fall_dot[i];
	}
}
