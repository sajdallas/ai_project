#include "asr.h"
enum midi{ C0 = 262,D0 =  294, E0 = 330, F0 = 349, G0 = 392, 
A0 = 440, B0 = 494,C = 523,D = 587,E = 659,F = 698,G = 784,
A = 880,B = 988,C1 = 1047,D1 = 1175,E1 = 1319,F1 = 1397,
G1 = 1568,A1 = 1760,B1 = 1976,N = 0,L1 = 4000,L10=6000,
L2 = 2000,L20 = 3000,L4 = 1000,L40 =1500,L8 = 500,
L80 = 750,L16 = 250,L160 = 375,L32 = 125,L320 = 188};


//沧海一声笑
int demo_0_midi[] = {A,L80,G,L16,E,L8,D,L8,C,L40,N,L8,E,L80,D,L16,C,L8,A0,L8,G0,L40,N,L8,G0,L80,A0,L16,G0,L80,A0,L16,C,L80,D,L16,E,L8,G,L8,A,L80,G,L16,E,L8,D,L16,C,L16,D,L40,N,L8,
                     A,L8,A,L16,G,L16,E,L8,D,L8,C,L40,N,L8,E,L80,D,L16,C,L8,A0,L8,G0,L40,N,L8,G0,L80,A0,L16,G0,L80,A0,L16,C,L80,D,L16,E,L8,G,L8,A,L80,G,L16,E,L16,D,L16,C,L8,D,L40,N,L8,
                     A,L8,A,L16,G,L16,E,L8,D,L8,C,L40,N,L8,E,L80,D,L16,C,L8,A0,L8,G0,L40,N,L8,G0,L80,A0,L16,G0,L80,A0,L16,C,L80,D,L16,E,L8,G,L8,A,L80,G,L16,E,L16,D,L16,C,L8,D,L40,N,L8,
                     A,L8,A,L16,G,L16,E,L8,D,L8,C,L40,N,L8,E,L80,D,L16,C,L8,A0,L8,G0,L40,N,L8,G0,L80,A0,L16,G0,L80,A0,L16,C,L80,D,L16,E,L8,G,L8,A,L80,G,L16,E,L8,D,L8,C,L40,N,L8};


//别知己
int demo_1_midi[] = {E,L80,A0,L16,A0,L8,C,L16,E,L16,D,L4,B0,L16,A0,L16,G0,L16,A0,L16,A0,L40,N,L8,A0,L16,A0,L16,A,L16,A,L8,A,L16,A,L8,A,L16,G,L16,G,L80,G,L16,E,L16,D,L16,D,L16,E,L16,E,L40,N,L8,E,L16,G,L16,
                    A,L80,A,L16,A,L8,A,L16,G,L16,G,L80,G,L16,E,L16,D,L16,D,L8,E,L80,G,L16,E,L8,D,L8,C,L4,N,L8,E,L16,G,L16,D,L16,D,L16,D,L16,D,L16,D,L8,E,L16,G,L16,D,L80,E,L16,B0,L16,A0,L16,G0,L16,A0,L16,A0,L40,N,L8,
                    E,L80,A0,L16,A0,L8,C,L16,E,L16,D,L80,D,L16,B0,L16,A0,L16,G0,L16,A0,L16,A0,L40,N,L8,A0,L16,A0,L16,A,L80,A,L16,A,L8,A,L16,G,L16,G,L80,G,L16,E,L8,D,L16,E,L16,E,L40,N,L8,E,L16,G,L16,
                    A,L80,A,L16,A,L8,A,L16,G,L16,G,L16,G,L8,G,L16,E,L8,D,L16,D,L16,E,L80,G,L16,E,L8,D,L8,C,L4,N,L8,E,L16,G,L16,D,L16,D,L16,D,L16,D,L16,D,L8,E,L16,G,L16,D,L80,E,L16,B0,L16,A0,L16,G0,L16,A0,L16,
                    A0,L40,N,L8,C,L16,D,L16,E,L16,G,L16,A,L40,A,L8,G,L80,G,L16,E,L8,D,L16,E,L16,E,L40,N,L8,C,L16,D,L16,E,L16,G,L16,A,L40,A,L8,B,L80,B,L16,C1,L8,B,L16,A,L16,A,L40,N,L8,E,L16,G,L16,
                    A,L80,A,L16,A,L8,A,L16,G,L16,G,L16,G,L8,G,L16,E,L8,D,L16,D,L16,E,L80,G,L16,E,L8,D,L8,C,L4,N,L8,E,L16,G,L16,D,L16,D,L16,D,L16,D,L16,D,L8,E,L16,G,L16,D,L80,E,L16,B0,L16,A0,L16,G0,L16,A0,L16,A0,L40};



//孤勇者
int demo_2_midi[] = {E,L40,N,L8,C,L16,D,L16,C,L16,E,L40,N,L8,C,L16,D,L16,C,L16,D,L16,E,L16,A0,L80,C,L16,A0,L80,C,L16,A0,L80,C,L16,D,L8,C,L8,
                      B0,L40,N,L8,E,L40,N,L8,C,L16,D,L16,C,L16,E,L40,N,L8,C,L16,D,L16,C,L16,D,L16,E,L16,A0,L80,C,L16,A0,L80,C,L16,A0,L80,C,L16,E,L8,D,L8,B0,L40,N,L8,
                      A0,L16,C,L16,A,L80,A,L16,A,L16,G,L16,A,L8,A,L16,G,L16,A,L16,G,L16,A,L16,G,L16,E,L16,E,L40,N,L8,A0,L16,C,L16,A,L80,A,L16,A,L16,G,L16,A,L16,G,L16,B,L80,B,L16,B,L16,A,L16,B,L80,
                      A,L8,E,L4,N,L8,E,L16,G,L16,E,L16,D,L80,E,L16,D,L80,E,L16,D,L80,E,L16,G,L16,E,L16,G,L16,E,L16,D,L80,E,L16,D,L80,E,L16,D,L4,N,L8,C,L16,D,L16,
                      E,L8,A0,L8,C,L8,E,L8,D,L8,E,L16,D,L16,C,L16,C,L8,A0,L40,N,L8,A,L16,B,L16,C1,L16,D1,L16,B,L16,C1,L16,C1,L8,C1,L16,B,L16,C1,L16,D1,L16,B,L16,C1,L16,C1,L8,C1,L16,D1,L16,
                      E1,L16,D1,L16,E1,L16,D1,L16,E1,L8,E1,L16,D1,L16,E1,L8,G1,L8,E1,L8,A,L16,B,L16,C1,L16,D1,L16,B,L16,C1,L16,C1,L8,C1,L16,B,L16,C1,L16,D1,L16,B,L16,C1,L16,C1,L8,C1,L16,D1,L16,
                      E1,L16,D1,L16,E1,L16,D1,L16,E1,L8,E1,L16,D1,L16,E1,L8,G1,L8,E1,L8,G1,L8,E1,L80,G1,L16,E1,L80,G1,L16,E1,L16,G1,L16,A1,L16,E1,L16,G1,L8,G1,L8,E1,L8,G1,L16,E1,L8,G1,L16,E1,L16,G1,L16,A1,L16,E1,L16,G1,L8,G1,L16,G1,L16,
                      E1,L16,D1,L16,D1,L8,D1,L8,C1,L16,E1,L8,D1,L16,D1,L8,D1,L8,C1,L16,C1,L16,A,L40,N,L8,G1,L16,G1,L16,E1,L16,D1,L16,D1,L8,D1,L8,C1,L16,E1,L8,D1,L16,D1,L8,D1,L8,C1,L16,C1,L16,A,L4};


//赛马
int demo_3_midi[] = {A,L40,E,L16,G,L16,A,L40,E,L16,G,L16,A,L40,E,L16,G,L16,A,L40,E,L16,G,L16,A,L16,G,L16,E,L16,G,L16,A,L16,G,L16,E,L16,G,L16,A,L16,G,L16,E,L16,G,L16,A,L16,G,L16,E,L16,G,L16,
                    A,L8,G,L16,A,L8,G,L8,A,L16,A,L8,G,L8,A,L16,A,L8,G,L8,A,L16,A,L8,A0,L8,E,L8,C,L8,A0,L8,E,L8,A,L8,G,L8,E,L8,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,
                    A0,L8,E,L8,C,L8,A0,L8,E,L8,A,L8,G,L8,E,L8,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L40,A0,L16,C,L16,D,L40,A0,L16,C,L16,
                    D,L40,A0,L16,C,L16,D,L40,A0,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L16,E,L16,D,L16,C,L16,D,L8,C,L16,D,L8,D,L8,C,L16,D,L8,D,L8,C,L16,D,L8,D,L8,C,L16,D,L8,
                    A0,L4,A,L4,G,L4,E,L4,D,L4,G,L4,E,L4,C,L4,A0,L4,A,L8,B,L8,G,L4,E,L4,D,L4,G,L4,E,L4,D,L4,A0,L4,C,L16,D,L16,A0,L40,C,L16,D,L16,
                    A0,L40,C,L16,D,L16,A0,L40,C,L16,D,L16,A0,L16,D,L16,C,L16,D,L16,A0,L16,D,L16,C,L16,D,L16,A0,L16,D,L16,C,L16,D,L16,A0,L16,D,L16,C,L16,D,L16,A0,L4,A0,L8,A0,L8,A0,L4,N,L8,
                    E,L4,A,L8,C1,L16,G,L40,E,L8,G,L8,A,L8,C1,L8,D1,L16,C1,L16,A,L40,N,L8,E,L4,A,L80,C1,L16,G,L4,G,L8,E,L8,D,L8,E,L8,A,L8,G,L8,E,L40,N,L8,
                    G,L4,A,L80,C1,L16,C,L40,A0,L8,D,L8,E,L8,A,L8,G,L8,E,L4,E,L8,D,L8,C,L80,D,L16,E,L8,G,L8,A,L4,A0,L4,D,L8,E,L8,C,L4,A0,L40,N,L8,
                    E,L8,E,L16,E,L16,A,L8,C1,L16,C1,L16,G,L8,G,L16,G,L16,G,L8,E,L16,E,L16,G,L8,G,L16,A,L16,C1,L8,D1,L16,C1,L16,A,L8,A,L16,A,L16,A,L8,A,L16,A,L16,E,L8,E,L16,E,L16,A,L8,C1,L16,C1,L16,
                    A,L8,A,L16,A,L16,A,L8,E,L16,E,L16,D,L16,D,L16,E,L8,A,L16,G,L16,G,L8,E,L16,E,L16,G,L8,E,L8,A0,L8,G,L8,G,L16,G,L16,A,L8,C1,L16,C1,L16,C,L8,C,L16,C,L16,C,L8,A0,L8,D,L8,D,L16,E,L16,A,L8,G,L16,G,L16,
                    E,L8,E,L16,G,L16,E,L8,D,L8,C,L16,A0,L16,C,L16,D,L16,E,L16,D,L16,E,L16,G,L16,A,L16,G,L16,A,L16,C1,L16,G,L16,A,L16,G,L16,E,L16,D,L16,E,L16,D,L16,C,L16,D,L16,C,L16,A0,L16,C,L16,A0,L4,A,L4};


//早安隆回
int demo_4_midi[] = {E,L8,A,L8,A,L4,A,L8,A,L4,A,L8,B,L8,B,L8,C1,L8,B,L8,A,L8,A,L40,N,L8,
                    B,L8,B,L4,B,L8,B,L8,C1,L8,B,L8,A,L40,N,L8,E,L8,A,L8,A,L4,A,L8,A,L4,A,L8,B,L8,B,L8,C1,L8,B,L8,A,L40,N,L8,
                    B,L8,B,L4,C1,L8,D1,L8,D1,L8,C1,L8,B,L8,A,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L8,E1,L4,C1,L8,D1,L4,E1,L8,D1,L40,N,L8,
                    F1,L8,F1,L4,F1,L8,F1,L8,G1,L8,F1,L8,E1,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L8,E1,L4,C1,L8,D1,L4,E1,L8,D1,L40,N,L8,
                    D1,L8,D1,L4,D1,L8,D1,L8,E1,L8,C1,L8,A,L40,N,L8,E,L8,A,L8,A,L4,A,L8,A,L4,A,L8,B,L8,B,L8,C1,L8,B,L8,A,L40,N,L8,
                    B,L8,B,L4,B,L8,B,L8,C1,L8,B,L8,A,L8,A,L4,N,L8,E,L8,A,L8,A,L4,A,L8,A,L4,A,L8,B,L8,B,L8,C1,L8,B,L8,A,L40,N,L8,
                    B,L8,B,L4,C1,L8,D1,L8,D1,L8,C1,L8,B,L8,A,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L8,E1,L4,C1,L8,C1,L8,D1,L8,E1,L8,D1,L40,N,L8,
                    F1,L8,F1,L4,F1,L8,F1,L8,G1,L8,F1,L8,E1,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L8,E1,L4,C1,L8,D1,L4,E1,L8,D1,L40,N,L8,
                    D1,L8,D1,L40,D1,L8,E1,L8,C1,L8,B,L8,A,L40,N,L8,E,L8,A,L8,A,L8,A,L8,A,L8,A,L4,N,L8,C1,L8,E1,L8,E1,L8,D1,L8,E1,L40,N,L8,
                    D1,L8,D1,L4,D1,L8,G1,L8,G1,L8,G1,L8,A1,L8,D1,L16,E1,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L40,C1,L8,D1,L8,D1,L8,E1,L8,D1,L40,N,L8,
                    D1,L8,D1,L8,D1,L40,E1,L8,C1,L8,A,L40,N,L8,E,L8,A,L8,A,L8,A,L8,A,L8,A,L4,N,L8,C1,L8,E1,L8,E1,L8,D1,L8,E1,L40,N,L8,
                    D1,L8,D1,L4,D1,L8,G1,L8,G1,L8,G1,L8,A1,L8,D1,L16,E1,L40,N,L8,D1,L8,E1,L8,E1,L8,E1,L8,E1,L40,C1,L8,D1,L8,D1,L8,E1,L8,D1,L40,N,L8,
                    D1,L8,D1,L4,D1,L8,D1,L8,E1,L8,C1,L8,A,L40};

class MIDI
{
public:

  IOResue_FUNCTION func = FIFTH_FUNCTION; // 第几功能
  pwm_base_t pwm; //第几pwm
  int IO = 0;

  // 初始化
  MIDI(uint8_t io)
  {
    pinMode(io,output);
    IO = io;
    switch (io)
    {
      case 2:  
        pwm = PWM0;
        break;
      case 20:
        pwm = PWM0;
        func = FORTH_FUNCTION;
        break;
      case 3:
        pwm = PWM1;
        break;
      case 13:
        pwm = PWM1;
        func = FORTH_FUNCTION;
        break;
      case 4:
        pwm = PWM2;
        break;
      case 14:
        pwm = PWM2;
        func = FORTH_FUNCTION;
        break;
      case 5:
        pwm = PWM3;
        break;
      case 6:
        pwm = PWM4;
        break;
      case 0:
        pwm = PWM5;
        func =SECOND_FUNCTION;
        break;
    };
  }
  // 演奏一个音符。
  void play(int val,float num)
  {
    setPinFun(IO,func);
    PWM_enble(pwm,val,1000,250);
    delay(num);
    setPinFun(IO,FIRST_FUNCTION);
    digitalWrite(IO,1);

  };

  // 沧海一声笑
  void player_0(int t){
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = (0); j < (sizeof(demo_0_midi)/sizeof(demo_0_midi[0])); j = j + 1) {
        play(demo_0_midi[(int)(j)],(((float)demo_0_midi[(int)(j + 1)] / 1000) * T));
        j++;
      }
  }

  // 别知己
  void player_1(int t){
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = (0); j < (sizeof(demo_1_midi)/sizeof(demo_1_midi[0])); j = j + 1) {
        play(demo_1_midi[(int)(j)],(((float)demo_1_midi[(int)(j + 1)] / 1000) * T));
        j++;
    }

  }


   // 孤勇者
  void player_2(int t){
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = (0); j < (sizeof(demo_2_midi)/sizeof(demo_2_midi[0])); j = j + 1) {
        play(demo_2_midi[(int)(j)],(((float)demo_2_midi[(int)(j + 1)] / 1000) * T));
        j++;
    }
  }


   // 赛马
  void player_3(int t){
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = (0); j < (sizeof(demo_3_midi)/sizeof(demo_3_midi[0])); j = j + 1) {
        play(demo_3_midi[(int)(j)],(((float)demo_3_midi[(int)(j + 1)] / 1000) * T));
        j++;
    }

  }

    // 早安隆回
  void player_4(int t){
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = (0); j < (sizeof(demo_4_midi)/sizeof(demo_4_midi[0])); j = j + 1) {
        play(demo_4_midi[(int)(j)],(((float)demo_4_midi[(int)(j + 1)] / 1000) * T));
        j++;
    }

  }


  // 播放一个midi
  void play_demo(int val,int T){
    switch (val)
    {
    case 0:
      //沧海一声笑
      player_0(T);
      break;
    case 1:
      //别知己
      player_1(T);
      break;
    case 2:
      //孤勇者
      player_2(T);
      break;
    case 3:
      //赛马
      player_3(T);
      break;
    case 4:
      //早安隆回
      player_4(T);
      break;
    }
  }

  //节拍器 0 
  void beat0(int t)
  {
    float T = (float)60/t*1000;
    T = (T-200)/2;
    play(200,100);
    delay(T);
    play(300,100);
    delay(T);
  }

  //节拍器 1 
  void beat1(int t)
  {
    float T = (float)60/t*1000;
    T = (T-100);
    play(400,100);
    delay(T);
  }
  //播放自定义midi 
  void play_diy(int* midi,int c,int t)
  { 
    float T = (float)60/t*1000;
    setPinFun(IO,func);
    for (int j = 0; j < C; j ++) {
        play(midi[(int)(j)],(((float)midi[(int)(j + 1)] / 1000) * T));
        j++;
    }
  }


};
