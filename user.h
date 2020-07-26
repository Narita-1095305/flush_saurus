#define	LED_SZ				8

typedef unsigned char uchar;

/*	ブザー用	*/
enum
{
	BEEP_HIGH = 46,
	BEEP_LOW = 168,

	BEEP_C4 = 238,
	BEEP_D4 = 212,
	BEEP_E4 = 189,
	BEEP_F4 = 178,
	BEEP_G4 = 158,
	BEEP_A4 = 141,
	BEEP_B4 = 126,
	BEEP_C5 = 118
};

/*	システム定義		*/
extern void _wait(uchar wait);	//  時間待ち 
extern uchar _rand(void);		//  擬似乱数生成
extern void _sound(uchar tone, uchar length);	//  ブザー           

extern volatile uchar sw_flag;	// スイッチ変化を示すフラグ(クリアは手動)
extern volatile uchar sw;		//  押しボタン状態  
extern volatile uchar led[LED_SZ];	//  マトリクスLED  

/*	ユーザー定義	*/
extern void user_init(void);
extern void user_main(void);
