#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "user.h"

volatile uchar sw;				//  押しボタン状態       
volatile uchar sw_flag;			// スイッチ変化を示すフラグ(クリアはユーザ側で実施) 
volatile uchar led[LED_SZ];		//  マトリクスLED      

static volatile uchar period;	//  ブザー音の長さ       
static volatile uchar user_flag;	//  ユーザー処理の開始フラグ 
static volatile uchar delay;	//  待ち時間カウンタ    
static volatile uchar rnd;		//  擬似乱数のカウンタ 

/* マトリクスLEDのダイナミック点灯(2ms毎) */
ISR(TIMER0_COMPA_vect) // 
{
	static uchar sc = 0xFE;
	static uchar scan = 0;		//  LED 走査カウンタ

	// LEDの更新 
	PORTB = 0;	// 残像対策
	sc = (sc << 1) | (sc >> 7);
	PORTD = (PORTD & 0x0F) | (sc & 0xF0);	// 上位4ビット書き換え
	PORTC = (PORTC & 0xF0) | (sc & 0x0F);	// 下位4ビット書き換え
	scan = (scan + 1) & 7;
	PORTB = led[scan];
}

/* スイッチ処理 */
ISR(PCINT1_vect) 
{
	OCR1A = TCNT1 + 500;	// タイマ1に比較値設定(今から64ms後に割り込む)
	TIFR1 = _BV(OCF1A);	    // フラグクリア
	TIMSK1 |= _BV(OCIE1A);	// タイマ1・コンペアマッチA割り込み有効化
	rnd++;	                // 乱数も更新
}
ISR(TIMER1_COMPA_vect) // チャタリング終了後，64ms後に呼び出される
{
	sw = (~PINC >> 4) & 3;	// スイッチ変数の更新
	sw_flag = 1;
	TIMSK1 &= ~_BV(OCIE1A);	// タイマ1・コンペアマッチA割り込み無効化   
}

/* ユーザ処理のための割り込み */
ISR(TIMER1_COMPB_vect) // 100msごとに呼び出される
{	
	OCR1B = TCNT1 + 780;
	TIFR1 = _BV(OCF1B);	// フラグクリア
	if (delay) {	// 待ち
		delay--;
	}
	if (period) {	//  ブザー停止
		period--;
		if (period == 0) {
			TCCR2A = 0;
		}
	}
	user_flag = 1;	// ユーザコードを呼び出す
}

int main(void)
{
	// LED,スイッチ,ブザーのポート設定 
	DDRB = 0xFF;
	DDRC = 0x0F;
	DDRD = 0xFA;
	PORTC = 0x30;	// 入力ピンをプルアップ
	PORTD = 0x00;

	// タイマ0(CTC): ダイナミック点灯用  
	OCR0A = 249;	//  2mS     
	TCCR0A = 2;
	TCCR0B = 3;	//  1/64    
	TIMSK0 |= (1 << OCIE0A); // コンペアマッチA割り込み有効

	// タイマ2(CTC): ブザー用
	TCCR2A = 0;
	TCCR2B = 0x44;	//  1/64  , コンペアマッチ出力B有効（トグル）   

	// タイマ1(ノーマル)：100msの定期割り込み
	TCCR1A = 0x00;
	TCCR1B = 0x05;
	TIMSK1 |= _BV(OCIE1B);	// コンペアマッチB割り込み有効化(100ms)
	TCNT1 = 0xFFFF;

	//  ピン変化割り込み有効
	PCICR = _BV(PCIE1);
	PCMSK1 = 0x30;

	user_init();	// ユーザ処理初期化    
	sei();          // システムとしての割り込みの有効化
	for (user_flag = 0;;) {
		wdt_reset();
		if (user_flag) {	//  ユーザー処理の起動 
			user_main();
			user_flag = 0;
		}
	}
	return 0;
}

/* 時間待ち( n * 100ms) */
void _wait(uchar n)
{
	for (delay = n; delay;) {
		wdt_reset();
	}
}

/* 擬似乱数生成 */
uchar _rand(void)
{
	rnd++; 	// 乱数更新 
	return (uchar)(rnd ^ (rnd << 1)); 	// グレイコードカウンタ 
}

/* ブザー */
void _sound(uchar tone, uchar length)
{
	OCR2A = tone;
	period = length;
	TCCR2A = 0x12;
}
