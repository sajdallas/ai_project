/********************************************************************************
* @File name: asr_servo.h
* @Author: hhdd
* @Version: 1.0
* @Date: 2022-6-21
* @Description: 舵机库，使用定时器计时。
********************************************************************************/
#ifndef _ASR_SERVO_H
#define _ASR_SERVO_H

#include "asr.h"
#include "asr_hwtimer.h"

typedef enum {_timer0, _Nbr_16timers} timer16_Sequence_t ;  

#define MIN_PULSE_WIDTH         544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH         2400     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH     1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL        20000     // minumim time to refresh servos in microseconds 

#define SERVOS_PER_TIMER        8     //一个定时器控制的最大舵机数
#define MAX_SERVOS              (_Nbr_16timers  * SERVOS_PER_TIMER)

#define INVALID_SERVO           255     // flag indicating an invalid servo index

typedef struct  
{
    uint8_t nbr        :6;             // a pin number from 0 to 63
    uint8_t isActive   :1;             // true if this channel is enabled, pin not pulsed if false 
} ServoPin_t;  

typedef struct 
{
    ServoPin_t Pin;
    volatile unsigned int ticks;
} servo_t;

class Servo
{
public:
    Servo();
    uint8_t attach(int pin);           // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
    uint8_t attach(int pin, int min, int max); // as above but also sets min and max values for writes. 
    void detach();
    void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
    void writeMicroseconds(int value); // Write pulse width in microseconds 
    int read();                        // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds();            // returns current pulse width in microseconds for this servo (was read_us() in first release)
    bool attached();                   // return true if this servo is attached, otherwise false 
private:
    uint8_t servoIndex;               // index into the channel data for this servo
    int8_t min;                       // minimum is this value times 4 added to MIN_PULSE_WIDTH    
    int8_t max;                       // maximum is this value times 4 added to MAX_PULSE_WIDTH  
    long map(long x, long in_min, long in_max, long out_min, long out_max); 
};

volatile uint32_t _TCNT = 0; //计数值

#define usToTicks(_us)    _us     // converts microseconds to tick (assumes prescale of 8)  // 12 Aug 2009
#define ticksToUs(_ticks) _ticks // converts from ticks back to microseconds

#define TRIM_DURATION       2    // compensation ticks to trim adjust for digitalWrite delays // 12 August 2009

static servo_t _servos[MAX_SERVOS];      //保存每个舵机参数的结构体数组
static volatile int8_t _Channel[_Nbr_16timers];  //通道(如果是刷新间隔则为-1)

uint8_t _ServoCount = 0;   //保存舵机总数

// convenience macros
#define SERVO_INDEX_TO_TIMER(_servo_nbr) ((timer16_Sequence_t)(_servo_nbr / SERVOS_PER_TIMER)) // returns the timer controlling this servo
#define SERVO_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SERVOS_PER_TIMER)       // returns the index of the servo on this timer
#define SERVO_INDEX(_timer,_channel)  ((_timer*SERVOS_PER_TIMER) + _channel)     // macro to access servo index by timer and channel
#define SERVO(_timer,_channel)  (_servos[SERVO_INDEX(_timer,_channel)])            // macro to access servo class by timer and channel

#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min * 4)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max * 4)  // maximum value in uS for this servo 

//========================================================================
// 描述: 定时器中断回调函数.
// 参数: none.
// 返回: none.
//========================================================================
static inline void handle_interrupts(timer16_Sequence_t timer, volatile uint32_t *TCNTn)
{
    if( _Channel[timer] < 0 )   //通道设置为-1表示刷新间隔完成，则重置计时器
    {
        *TCNTn = 0;
    }else{
        if( SERVO_INDEX(timer,_Channel[timer]) < _ServoCount && SERVO(timer,_Channel[timer]).Pin.isActive == true )
        {
            digitalWrite(SERVO(timer,_Channel[timer]).Pin.nbr,0); //如果激活，此通道输出低电平
        }  
    }

    _Channel[timer]++;    //计数值+1

    if( SERVO_INDEX(timer,_Channel[timer]) < _ServoCount && _Channel[timer] < SERVOS_PER_TIMER) 
    {
        *TCNTn=*TCNTn + SERVO(timer,_Channel[timer]).ticks;//同一定时器累加高电平值

        TIM_Duty_Updata(TIMER0, SERVO(timer,_Channel[timer]).ticks);

        if(SERVO(timer,_Channel[timer]).Pin.isActive == true)
        {
            digitalWrite(SERVO(timer,_Channel[timer]).Pin.nbr,1); //its an active channel so pulse it high   
        }
    }  
    else 
    { 
        // finished all channels so wait for the refresh period to expire before starting over 
        if( ((unsigned)*TCNTn) + 4 < usToTicks(REFRESH_INTERVAL) )  // allow a few ticks to ensure the next OCR1A not missed
        {
            TIM_Duty_Updata(TIMER0, 20000-(*TCNTn));
        } else 
        {
            // *OCRnA = *TCNTn + 4;  // at least REFRESH_INTERVAL has elapsed
            TIM_Duty_Updata(TIMER0, 0);
        }
        _Channel[timer] = -1; // this will get incremented at the end of the refresh period to start again at the first channel
    }
}

//========================================================================
// 描述: 定时器中断函数.
// 参数: none.
// 返回: none.
//========================================================================
void TIM_attachInterrupt_0()
{ 
	handle_interrupts(_timer0, &_TCNT); 
}

//========================================================================
// 描述: 定时器初始化函数.
// 参数: timer：使用哪个定时器.
// 返回: none.
//========================================================================
static void initISR(timer16_Sequence_t timer)
{  
    if(timer == _timer0) 
    {
        TIM_attachInterrupt(TIMER0, 2000, TIM_attachInterrupt_0);
        timer_start(TIMER0);
    }
}  

//========================================================================
// 描述: 定时器关闭函数.
// 参数: timer：关闭哪个定时器.
// 返回: none.
//========================================================================
static void finISR(timer16_Sequence_t timer)
{
    if(timer == _timer0) {
        TIM_Disable(TIMER0);
    }
}

//========================================================================
// 描述: 判断定时器是否正在工作.
// 参数: timer：关闭哪个定时器.
// 返回: none.
//========================================================================
static bool isTimerActive(timer16_Sequence_t timer)
{
    // returns true if any servo is active on this timer
    for(uint8_t channel=0; channel < SERVOS_PER_TIMER; channel++) {
        if(SERVO(timer,channel).Pin.isActive == true)
        return true;
    }
    return false;
}


//========================================================================
// 描述: 构造函数，每创建一个对象时，this->servoIndex和_ServoCount就会增加1.
// 参数: none.
// 返回: none.
//========================================================================
Servo::Servo()
{
    if( _ServoCount < MAX_SERVOS) {
        this->servoIndex = _ServoCount++;                    // assign a servo index to this instance
        _servos[this->servoIndex].ticks = usToTicks(DEFAULT_PULSE_WIDTH);   // store default values  - 12 Aug 2009
    }
    else{
        this->servoIndex = INVALID_SERVO;  // too many _servos 
    }     
}

//========================================================================
// 描述: 引脚设置.
// 参数: pin：对应的引脚.
// 返回: none.
//========================================================================
uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

//========================================================================
// 描述: 引脚设置.
// 参数: pin：对应的引脚; min:最小的脉冲宽度; max:最大的脉冲宽度.
// 返回: 该舵机的编号.
//========================================================================
uint8_t Servo::attach(int pin, int min, int max)
{
    if(this->servoIndex < MAX_SERVOS ) {
        pinMode(pin, output);   //设置引脚为输出模式
        #if defined(TW_ASR_PRO)
            set_pin_to_gpio_mode(pin);
        #else
            if((pin<=8) && (pin >=5))
            {
                setPinFun(pin,SECOND_FUNCTION);
            }
            else
            {
                setPinFun(pin,FIRST_FUNCTION);
            }
        #endif
        digitalWrite(pin,0);
        _servos[this->servoIndex].Pin.nbr = pin;  

        // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128 
        this->min  = (MIN_PULSE_WIDTH - min)/4; //最小/最大的分辨率为4uS
        this->max  = (MAX_PULSE_WIDTH - max)/4; 
        //如果计时器尚未初始化，则初始化它
        timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(servoIndex);
        if(isTimerActive(timer) == false)
        {
            initISR(timer);   
        }
        _servos[this->servoIndex].Pin.isActive = true;  //this must be set after the check for isTimerActive
    } 
    return this->servoIndex ;
}

//========================================================================
// 描述: 舵机控制失能.
// 参数: none.
// 返回: none.
//========================================================================
void Servo::detach()  
{
    _servos[this->servoIndex].Pin.isActive = false;  
    timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(servoIndex);
    if(isTimerActive(timer) == false) {
        finISR(timer);
    }
}

//========================================================================
// 描述: 输入角度或者脉冲控制舵机.
// 参数: 角度或者脉冲.
// 返回: none.
//========================================================================
void Servo::write(int value)
{  
    if(value < MIN_PULSE_WIDTH) //如果value的值小于最小脉宽，则当作角度处理
    {
        if(value < 0) value = 0;
        if(value > 180) value = 180;
        value = map(value, 0, 180, SERVO_MIN(),  SERVO_MAX());      
    }
    this->writeMicroseconds(value);
}

//========================================================================
// 描述: 输入脉宽控制舵机.
// 参数: 脉宽.
// 返回: none.
//========================================================================
void Servo::writeMicroseconds(int value)
{
    //计算并存储给定通道的值
    uint8_t channel = this->servoIndex;
    if( (channel < MAX_SERVOS) )   //确保通道有效
    {  
        if( value < SERVO_MIN() ) //确保脉宽有效
        {
            value = SERVO_MIN();
        }
        else if( value > SERVO_MAX() )
        {
            value = SERVO_MAX();   
        }
        _servos[channel].ticks = value;  //写入脉宽
        
    } 
}

//========================================================================
// 描述: 返回舵机的角度.
// 参数: none.
// 返回: 角度.
//========================================================================
int Servo::read()
{
   return  map( this->readMicroseconds()+1, SERVO_MIN(), SERVO_MAX(), 0, 180);
    return 0;     
}

//========================================================================
// 描述: 返回脉宽.
// 参数: none.
// 返回: 角度.
//========================================================================
int Servo::readMicroseconds()
{
    unsigned int pulsewidth;
    if( this->servoIndex != INVALID_SERVO )
    {
        pulsewidth = (_servos[this->servoIndex].ticks)/10;   // 12 aug 2009
    }else{
        pulsewidth  = 0;
    }
    return pulsewidth;   
}

//========================================================================
// 描述: 如果舵机是活动的返回1.
// 参数: none.
// 返回: none.
//========================================================================
bool Servo::attached()
{
  return _servos[this->servoIndex].Pin.isActive ;
}

//========================================================================
// 描述: map映射函数.
// 参数: none.
// 返回: none.
//========================================================================
long Servo::map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif //asr_servo.h