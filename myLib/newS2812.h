/**
 * @file asr_ws2812.h
 * @brief 2812 RGB单总线模块驱动的头文件
 * @version 0.2
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 */


#ifndef N_PIXELS
#define N_PIXELS  10 
#endif


#ifndef _NEW_WS2812_H_
#define _NEW_WS2812_H_
#include "asr.h"
#include "asr_math.h"

typedef union {
	struct
	{
		int g;
		int r;
		int b;
	}simple;
	int grb[3];
}color_t;

typedef struct _rgb_strip_t{
 	int pin_num;
 	int num_leds;
 	color_t *leds;
}rgb_strip_t;


class WS2812
{
public:
	WS2812( uint8_t pin,uint16_t leds,uint8_t bri);  //引脚，灯数，亮度//里面调用了初始化部分，不要再用初始化函数
	void pixel_init(uint16_t num_leds);	//RGB初始化
	uint8_t pixel_set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);	//RGB设置颜色
	uint8_t pixel_set_color(uint16_t index, uint32_t c);	//RGB设置颜色
    void pixel_set_all_color(uint8_t red, uint8_t green, uint8_t blue);	//设置所有RGB颜色
	void pixel_show();	//RGB更新显示
	uint8_t pixel_set_color_and_show(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);	//RGB设置颜色并显示
	void pixel_clear();	//RGB清除
	void rainbow(uint16_t wait);//彩虹循环呼吸效果
	void rainbowCycle(uint16_t wait);//流光溢彩效果
	void setBrightness(uint8_t b);//设置亮度值
	//-----自己改的函数====================
	void clear(); //关灯
	void set_hsv(uint32_t leds,uint32_t h,uint32_t s,uint32_t v,uint8_t all);//用HSV的方式显示一种色彩
	uint32_t hsv2rgb(uint32_t h, uint32_t s, uint32_t v);//hsv转rgb
	void demo_2(uint16_t spd); //繁星闪烁
	void demo_3(uint16_t R,uint8_t G,uint8_t B,uint8_t spd);//单色单灯流水
	void demo_4(uint16_t spd); //多彩单灯流水
	void demo_5(uint16_t spd);//多彩单灯固定
	void demo_6(uint16_t spd);//多彩单灯流水1
	void demo_7(uint16_t spd);//多彩单灯流水2
	void demo_8(uint16_t spd,uint8_t pin);//拾音横向
	void demo_9(uint16_t spd,uint8_t pin);//拾音纵向
	void demo_10(uint16_t spd);//左右跑动
	void demo_11(uint16_t spd);//多彩呼吸
	void demo_12(uint16_t spd);//随机跑动
	void demo_13(uint16_t spd);//单色扩散
	//void demo_14(uint16_t spd);

	uint32_t Wheels(uint8_t WheelPos, float opacity); //

	
public:
    void sendOne(uint8_t GPIO_Pin);
	void sendZero(uint8_t GPIO_Pin);
	void sendRGB(uint8_t pin,uint8_t red,uint8_t green,uint8_t blue);
	void rstRGB(uint8_t pin);
	uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
   		return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
 	}
	uint32_t wheel(uint8_t WheelPos);
	rgb_strip_t strip;
	uint8_t _pin;
	uint8_t _brightness;
};

//========================================================================
// 描述: 引脚初始化.
// 参数: pin:引脚.
// 返回: none.
//========================================================================
//初始化全部在这里搞定！

WS2812::WS2812(uint8_t pin,uint16_t leds,uint8_t bri)
{
	_pin = pin;
	// rstRGB(pin);
	pixel_init(leds);//初始化
	_brightness = bri; //设置亮度

}

//关灯
void WS2812::clear(){
	pixel_clear();
	pixel_show();
	delay(50);
	pixel_clear();
	pixel_show();
	delay(50);
	pixel_clear();
	pixel_show();
}

// ====HSV_SHOW==================
void WS2812::set_hsv(uint32_t leds,uint32_t h,uint32_t s,uint32_t v,uint8_t all){
	if(all == 0)
	{
		uint32_t  hsv;
		hsv = hsv2rgb(h,s,v);
		pixel_set_color(leds,hsv);
	}else if(all == 1)
	{
		uint32_t  hsv;
		hsv = hsv2rgb(h,s,v);
		for(uint32_t i=0;i<N_PIXELS;i++)
		{
			pixel_set_color(i,hsv);
		}

	}
	
	
	
	
	
}
// 繁星闪烁----------------------------------

float redStates[N_PIXELS];
float blueStates[N_PIXELS];
float greenStates[N_PIXELS];
float Fade = 0.96;
void WS2812::demo_2(uint16_t spd)
{

  if (random(0,25) == 1) {
      uint16_t i = random(0,N_PIXELS);
      if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1) {
        redStates[i] = random(0,255);
        greenStates[i] = random(0,255);
        blueStates[i] = random(0,255);
      }
    }
    
    for(uint16_t l = 0; l < N_PIXELS; l++) {
      if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1) {
        pixel_set_color(l, redStates[l], greenStates[l], blueStates[l]);
        
        if (redStates[l] > 1) {
          redStates[l] = redStates[l] * Fade;
        } else {
          redStates[l] = 0;
        }
        
        if (greenStates[l] > 1) {
          greenStates[l] = greenStates[l] * Fade;
        } else {
          greenStates[l] = 0;
        }
        
        if (blueStates[l] > 1) {
          blueStates[l] = blueStates[l] * Fade;
        } else {
          blueStates[l] = 0;
        }
        
      } else {
        pixel_set_color(l, 0, 0, 0);
      }
    }
    pixel_show();
    delay(spd);

}
//==========单色单灯流水=====================================================================

void WS2812::demo_3(uint16_t R,uint8_t G,uint8_t B,uint8_t spd)
{
	
	for(int b=0; b<3; b++) { 
			pixel_clear();         
			for(int c=b; c<strip.num_leds; c += 3) {
				pixel_set_color(c, R, G, B); 
			}
			pixel_show(); 
			delay(spd);  
		}


}


//===========多彩单灯流水=====================================================================

void WS2812::demo_4(uint16_t spd)
{
	for(int b=0; b<3; b++) { 
			pixel_clear();         
			for(int c=b; c<strip.num_leds; c += 3) {
				uint8_t r =random(0,255);
				uint8_t g =random(0,255);
				uint8_t b =random(0,255);
				pixel_set_color(c, r, g, b); 
			}
			pixel_show();
			delay(spd); 
		}

}




//===========多彩单灯固定=====================================================================
void WS2812::demo_5(uint16_t spd)
{
	for(int b=0; b<3; b++) { 
		//pixel_clear();         
		for(int c=b; c<strip.num_leds; c += 3) {
			uint8_t r =random(0,255);
			uint8_t g =random(0,255);
			uint8_t b =random(0,255);
			pixel_set_color(c, r, g, b); 
		}
		pixel_show();
		delay(spd); 
	}
}

//======多彩单灯流水 1========================================================================================

void WS2812::demo_6(uint16_t spd)
{

  int firstPixelHue = 0;     
  for(int b=0; b<3; b++) { 
      pixel_clear();     
      for(int c=b; c<strip.num_leds; c += 3) {
        int      hue   = firstPixelHue + c * 65536L / strip.num_leds;
        uint32_t color = wheel(hue);
        pixel_set_color(c, color); 
      }
      pixel_show();                
      delay(spd);                
      firstPixelHue += 65536 / 90; 
    }
}

///=--=====多彩单灯流水 2=====================================------=======================
uint16_t  pixelQueue = 0; 
uint16_t pixelCycle = 0;   
void WS2812::demo_7(uint16_t spd)
{
	                
	for(int i=0; i < strip.num_leds; i+=3) {
		uint32_t color = wheel((i + pixelCycle) % 255);
		pixel_set_color(i + pixelQueue,color); 
	}
	pixel_show(); 
	for(int i=0; i < strip.num_leds; i+=3) {
		pixel_set_color(i + pixelQueue, 0, 0, 0); 
	}      
	pixelQueue++;                          
	pixelCycle++;                           
	if(pixelQueue >= 3)
		pixelQueue = 0;                       
	if(pixelCycle >= 256)
		pixelCycle = 0; 
	delay(spd);

}

///=========横向拾音灯=========================================================

#define TOP       (N_PIXELS + 2) 
#define NOISE     280  
#define SPEED .20 
#define N_PIXELS_HALF (N_PIXELS/2)
#define PEAK_FALL 20      // 20 
#define SAMPLES   60  // 60
// #include "HardwareSerial.h"
uint8_t peak  = 0 , dotCount = 0,volCount  = 0;
int val = 1; //控制比例
uint32_t n; //读adc的值。
int lvl  = 500,//10
minLvlAvg = 0,
vol[SAMPLES],
maxLvlAvg = 512;
float
  greenOffset = 30,
  blueOffset = 150;

void WS2812::demo_8(uint16_t spd,uint8_t pin)
{
  	val= map(val, 0, 1023, -10, 6);
	uint8_t  i;
  	uint16_t minLvl, maxLvl;
  	int  height;
	n = adc_read(pin);  //读adc的值，asrpro只有PC4一路ADC
	n = map(n,0,4096,0,900); 
	// Serial.println(n);                
  	n   = (n <= NOISE) ? 0 : (n - NOISE);            
  
    if(val<0){
        n=n/(val*(-1));
        }
       if(val>0){
        n=n*val;
     }
  
  lvl = ((lvl * 7) + n) >> 3;   
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
  if(height < 0L)       height = 0;      
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; 
  greenOffset += SPEED;
  blueOffset += SPEED;
  if (greenOffset >= 255) greenOffset = 0;
  if (blueOffset >= 255) blueOffset = 0;
  for(i=0; i<N_PIXELS_HALF; i++) {
    if(i >= height) {              
      pixel_set_color(N_PIXELS_HALF-i-1,   0,   0, 0);
      pixel_set_color(N_PIXELS_HALF+i,   0,   0, 0);
    }
    else {
      uint32_t color = wheel(map(i,0,N_PIXELS_HALF-1,(int)greenOffset, (int)blueOffset));
      pixel_set_color(N_PIXELS_HALF-i-1,color);
      pixel_set_color(N_PIXELS_HALF+i,color);
    }
    
  }
  if(peak > 0 && peak <= N_PIXELS_HALF-1) {
    uint32_t color = wheel(map(peak,0,N_PIXELS_HALF-1,30,150));
    pixel_set_color(N_PIXELS_HALF-peak-1,color);
    pixel_set_color(N_PIXELS_HALF+peak,color);
  }
  
   pixel_show(); 
    if(++dotCount >= PEAK_FALL) { 
      
      if(peak > 0) peak--;
      dotCount = 0;
    }
  vol[volCount] = n;                     
  if(++volCount >= SAMPLES) volCount = 0; 
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; 
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; 
  delay(spd);
}

///====================纵向拾音灯========================================

#define DC_OFFSET  0    

void WS2812::demo_9(uint16_t spd,uint8_t pin)
{
	uint8_t i;
	uint16_t minLvl, maxLvl;
	int n, height;
	// val = 300;//;  
	val= map(val, 0, 1023, -10, 6);
	n = adc_read(pin);  //读adc的值。
	n = map(n,0,4096,0,900);           
	n = abs(n - 0 - DC_OFFSET);        
	n = (n <= NOISE) ? 0 : (n - NOISE);  

		if(val<0){
			n=n/(val*(-1));
			}
		if(val>0){
			n=n*val;
			}
			
	lvl = ((lvl * 7) + n) >> 3;    	
	height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

	if (height < 0L)       height = 0;      
	else if (height > TOP) height = TOP;
	if (height > peak)     peak   = height; 

	greenOffset += SPEED;
	blueOffset += SPEED;
	if (greenOffset >= 255) greenOffset = 0;
	if (blueOffset >= 255) blueOffset = 0;

	
	for (i = 0; i < N_PIXELS; i++) {
		if (i >= height) {
		pixel_set_color(i, 0, 0, 0);
		} else {
		pixel_set_color(i, wheel(
			map(i, 0, strip.num_leds - 1, (int)greenOffset, (int)blueOffset)
		));
		} 
	}
	
	if(peak > 0 && peak <= N_PIXELS-1) 
		pixel_set_color(peak,wheel(map(peak,0,strip.num_leds-1,30,150)));
	
	pixel_show();  
	
	
		if(++dotCount >= PEAK_FALL) { 
		
		if(peak > 0) peak--;
		dotCount = 0;
		}
	pixel_show();  

	vol[volCount] = n;
	if (++volCount >= SAMPLES) {
		volCount = 0;
	}
	minLvl = maxLvl = vol[0];
	for (i = 1; i < SAMPLES; i++) {
		if (vol[i] < minLvl) {
		minLvl = vol[i];
		} else if (vol[i] > maxLvl) {
		maxLvl = vol[i];
		}
	}
	if ((maxLvl - minLvl) < TOP) {
		maxLvl = minLvl + TOP;
	}
	minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
	maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}

//========左右跑动=====================================================


void WS2812::demo_10(uint16_t spd){
	uint8_t r = random(0,255);
	uint8_t g = random(0,255);
	uint8_t b =random(0,255);
	for(uint8_t i =0;i<strip.num_leds;i++){
		pixel_set_color(i, r, g, b);
		pixel_show();
		delay(spd);
	}
	r = random(0,255);
	g = random(0,255);
	b =random(0,255);
	for(uint8_t i = strip.num_leds;i>0;i--){
		pixel_set_color(i, r, g, b);
		pixel_show();
		delay(spd);
	}
}
//======多彩呼吸================================================================
void WS2812::demo_11(uint16_t spd){
	uint8_t h = random(0,360);
	uint8_t s = random(60,100);
	uint8_t v = 0;
	uint32_t hsv;
	for(uint8_t i=0;i<100;i+=2){
		v++;
		hsv = hsv2rgb(h,s,v);
		for(uint8_t j=0;j<strip.num_leds;j++){
			pixel_set_color(j,hsv);
		}
		pixel_show();
		delay(spd);
	}
	for(uint8_t i=100;i>1;i-=2){
		v--;
		hsv = hsv2rgb(h,s,v);
		for(uint8_t j=0;j<strip.num_leds;j++){
			pixel_set_color(j,hsv);
		}
		pixel_show();
		delay(spd);
	}
}
//======随机跑动==================================================================
#define BG 0
uint32_t currentBg = random(0,256);
uint32_t nextBg = currentBg;
int color1;
int center = 0;
int step = -1;
int maxSteps = 8;
float fadeRate = 0.80;
int diff;
int wrap(int step) {
  if(step < 0) return N_PIXELS + step;
  if(step > N_PIXELS - 1) return step - N_PIXELS;
  return step;
}

uint32_t WS2812::Wheels(uint8_t WheelPos, float opacity) {
  
  if(WheelPos < 85) {
    return color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
  } 
  else {
    WheelPos -= 170;
    return color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
  }
}
void WS2812::demo_12(uint16_t spd){
	if (BG){
		if (currentBg == nextBg) {
		nextBg = random(0,256);
		} 
		else if (nextBg > currentBg) {
		currentBg++;
		} else {
		currentBg--;
		}
		for(uint16_t l = 0; l < N_PIXELS; l++) {
		pixel_set_color(l, Wheels(currentBg, 0.1));
		}
  	} 
  	else {
		for(uint16_t l = 0; l < N_PIXELS; l++) {
		pixel_set_color(l, 0, 0, 0);
		}
  	}
	if (step == -1) {
		center = random(0,N_PIXELS);
		color1 = random(0,256);
		step = 0;
  	}
 
	if (step == 0) {
		pixel_set_color(center, Wheels(color1, 1));
		step ++;
	} 
	else {
		if (step < maxSteps) {
		pixel_set_color(wrap(center + step), Wheels(color1, pow(fadeRate, step)));
		pixel_set_color(wrap(center - step), Wheels(color1, pow(fadeRate, step)));
		if (step > 3) {
			pixel_set_color(wrap(center + step - 3), Wheels(color1, pow(fadeRate, step - 2)));
			pixel_set_color(wrap(center - step + 3), Wheels(color1, pow(fadeRate, step - 2)));
		}
		step ++;
		} 
		else {
		step = -1;
		}
  	}
  
	pixel_show();
	delay(spd);
}
//=====单色扩散=================================================================================
void WS2812::demo_13(uint16_t spd){
	uint8_t r = random(0,255);
	uint8_t g = random(0,255);
	uint8_t b =random(0,255);
	uint8_t x0 = strip.num_leds/2-1;
	uint8_t x1 = strip.num_leds/2+1;
	pixel_set_color(strip.num_leds/2,r,g,b);
	for(uint8_t i=0;i<strip.num_leds/2+1;i++){
		pixel_set_color(x0,r,g,b);
		pixel_set_color(x1,r,g,b);
		pixel_show();
		delay(spd);
		x0--;
		x1++;
	}
	
}


//=====14=================================================================================

//========================================================================
// 描述: RGB发送1码.
// 参数: GPIOx：端口; GPIO_Pin：引脚号.
// 返回: none.
//========================================================================
void WS2812::sendOne(uint8_t GPIO_Pin)
{
	digitalWrite(GPIO_Pin,1);
	//延时850ns
#if defined(TW_ASR_PRO)
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		           ");
#else
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\n  \
		           ");
#endif

	digitalWrite(GPIO_Pin,0);
	//延时
#if defined(TW_ASR_PRO)
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		           ");
#else
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		           ");
#endif
}

//========================================================================
// 描述: RGB发送0码.
// 参数: GPIOx：端口; GPIO_Pin：引脚号.
// 返回: none.
//========================================================================
void WS2812::sendZero(uint8_t GPIO_Pin)
{
	digitalWrite(GPIO_Pin,1);
	//延时400ns
#if defined(TW_ASR_PRO)
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\n  \
		           ");
#else
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\n	   \
		           ");
#endif
	digitalWrite(GPIO_Pin,0);
	//延时850ns
#if defined(TW_ASR_PRO)
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		           ");
#else
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
		             nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\n  \
		           ");
#endif
}












//========================================================================
// 描述: RGB发送数据.
// 参数: pin：RGB引脚;  red,green,blue:RGB颜色值.
// 返回: none.
//========================================================================
void WS2812::sendRGB(uint8_t pin,uint8_t red,uint8_t green,uint8_t blue)
{
	for(uint8_t i=0;i<8;i++)
	{
		if(green&0x80){
			sendOne(pin);
		}else{
			sendZero(pin);
		}
		 green<<=1;
	}

	for(uint8_t i=0;i<8;i++)
	{
		if(red&0x80){
			sendOne(pin);
		}else{
			sendZero(pin);
		}
		 red<<=1;
	}
	for(uint8_t i=0;i<8;i++)
	{
		if(blue&0x80){
			sendOne(pin);
		}else{
			sendZero(pin);
		}
		 blue<<=1;
	}
}

//========================================================================
// 描述: RGB引脚设置.
// 参数: pin：RGB引脚.
// 返回: none.
//========================================================================
void WS2812::rstRGB(uint8_t pin)
{
	pinMode(_pin,output);
#if defined(TW_ASR_PRO)
	set_pin_to_gpio_mode(_pin);
#else
    if((_pin<=8) && (_pin >=5))
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }else{
        setPinFun(_pin,FIRST_FUNCTION);
    }
#endif
	digitalWrite(_pin,0);
}

//========================================================================
// 描述: RGB初始化.
// 参数: num_leds：RGB个数.
// 返回: none.
//========================================================================
void WS2812::pixel_init(uint16_t num_leds)
{	
	strip.leds = (color_t*) malloc(sizeof(color_t) * num_leds);
	strip.pin_num = _pin;
	strip.num_leds = num_leds;

	rstRGB(strip.pin_num); 

	for (int i = 0; i < num_leds; i++)
	{	
		strip.leds[i].simple.g = 0;
		strip.leds[i].simple.r = 0;
		strip.leds[i].simple.b = 0;
	}
}

//========================================================================
// 描述: RGB显示.
// 参数: none.
// 返回: none.
//========================================================================
void WS2812::pixel_show()
{
	rstRGB(strip.pin_num); 
	delay(1);
	taskENTER_CRITICAL();   //进入临界区
	for (int i = 0; i < strip.num_leds; i++)
	{
		sendRGB(strip.pin_num,strip.leds[i].simple.r,strip.leds[i].simple.g,strip.leds[i].simple.b);
	}
	taskEXIT_CRITICAL();    //退出临界区
}

//========================================================================
// 描述: RGB写入颜色值.
// 参数: index:第几个RGB; red,green,blue:RGB颜色值.
// 返回: 成功 1；失败 0.
//========================================================================
uint8_t WS2812::pixel_set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}

	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = red;
		strip.leds[index].simple.g = green;
		strip.leds[index].simple.b = blue;
	}
	else{
		return 1;
	}
	return 0;
}

//========================================================================
// 描述: RGB写入颜色值.
// 参数: index:第几个RGB; 32位WRGB值.
// 返回: 成功 1；失败 0.
//========================================================================
uint8_t WS2812::pixel_set_color(uint16_t index, uint32_t c)
{
	uint8_t r = (uint8_t)(c >> 16);
  	uint8_t g = (uint8_t)(c >>  8);
  	uint8_t b = (uint8_t)c;

	if(_brightness) 
	{ 
		r = (r * _brightness) >> 8;
		g = (g * _brightness) >> 8;
		b = (b * _brightness) >> 8;
	}

	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = r;
		strip.leds[index].simple.g = g;
		strip.leds[index].simple.b = b;
	}
	else{
		return 1;
	}
	return 0;
}

//========================================================================
// 描述: 全部RGB写入颜色值.
// 参数: red,green,blue:RGB颜色值.
// 返回: none.
//========================================================================
void WS2812::pixel_set_all_color(uint8_t red, uint8_t green, uint8_t blue)
{ 
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}

    for (int i = 0; i < strip.num_leds; i++)
	{
		strip.leds[i].simple.g = green;
		strip.leds[i].simple.r = red;
		strip.leds[i].simple.b = blue;
	}
}

//========================================================================
// 描述: RGB设置颜色并显示.
// 参数: index:第几个RGB;red,green,blue:RGB颜色值.
// 返回: none.
//========================================================================
uint8_t WS2812::pixel_set_color_and_show(uint16_t index, uint8_t red, uint8_t green, uint8_t blue )
{
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}
	
	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = red;
		strip.leds[index].simple.g = green;
		strip.leds[index].simple.b = blue;
		
		pixel_show();
	}
	else
		return 1;
	return 0;
}

//========================================================================
// 描述: 关闭RGB.
// 参数: none
// 返回: none.
//========================================================================
void WS2812::pixel_clear()
{
	for (int i = 0; i < strip.num_leds; i++)
	{
		strip.leds[i].simple.g = 0;
		strip.leds[i].simple.r = 0;
		strip.leds[i].simple.b = 0;
	}
	
	// pixel_show();
}

//========================================================================
// 描述: 颜色转换.
// 参数: 0-255颜色值
// 返回: 32位WRGB值.
//========================================================================
uint32_t WS2812::wheel(uint8_t WheelPos)
{
	WheelPos = 255 - WheelPos;
  	if(WheelPos < 85) {
  	  return color(255 - WheelPos * 3, 0, WheelPos * 3);
  	}
  	if(WheelPos < 170) {
  	  WheelPos -= 85;
  	  return color(0, WheelPos * 3, 255 - WheelPos * 3);
  	}
  	WheelPos -= 170;
  	return color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
 
//========================================================================
// 描述: 彩虹循环呼吸效果.
// 参数: 间隔时间
// 返回: none.
//========================================================================
void WS2812::rainbow(uint16_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.num_leds; i++) {
      pixel_set_color(i, wheel((i+j) & 255));
    }
    pixel_show();
    delay(wait);
	
  }
}

//========================================================================
// 描述: 流光溢彩效果.
// 参数: 间隔时间
// 返回: none.
//========================================================================
void WS2812::rainbowCycle(uint16_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.num_leds; i++) {
      	pixel_set_color(i, wheel(((i * 256 / strip.num_leds) + j) & 255));
    }
    pixel_show();
    delay(wait);
	
  }
}

//========================================================================
// 描述: 设置亮度.
// 参数: 亮度值
// 返回: none.
//========================================================================
void WS2812::setBrightness(uint8_t b){
	_brightness = b;
}




uint32_t WS2812::hsv2rgb(uint32_t h, uint32_t s, uint32_t v)
{
	uint8_t r, g, b;
	h %= 360; 
	uint32_t rgb_max = v * 2.55f;
	uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

	uint32_t i = h / 60;
	uint32_t diff = h % 60;

	uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

	switch (i) {
	case 0:
		r = rgb_max;
		g = rgb_min + rgb_adj;
		b = rgb_min;
		break;
	case 1:
		r = rgb_max - rgb_adj;
		g = rgb_max;
		b = rgb_min;
		break;
	case 2:
		r = rgb_min;
		g = rgb_max;
		b = rgb_min + rgb_adj;
		break;
	case 3:
		r = rgb_min;
		g = rgb_max - rgb_adj;
		b = rgb_max;
		break;
	case 4:
		r = rgb_min + rgb_adj;
		g = rgb_min;
		b = rgb_max;
		break;
	default:
		r = rgb_max;
		g = rgb_min;
		b = rgb_max - rgb_adj;
		break;
	}
	return (uint32_t)(r << 16 | g << 8 | b);
}


# endif  