#include "mbed.h"
#include "controller.hpp"
// #include "canSend.hpp"
#include "firstpenguin.hpp"

extern int leftJoystickX;
extern int leftJoystickY;
extern int rightJoystickX;
extern int lefts; 
extern int rigts;

extern FirstPenguin penguin; // FirstPenguinクラスのインスタンス
int16_t output;

CANMessage msg0;
CANMessage msg1;

BufferedSerial pc(USBTX, USBRX, 250000); // Serial communication with the PC
CAN can(PA_11, PA_12, (int)1e6); 
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
uint8_t DATA[8] = {};
constexpr uint32_t can_id = 30;

int maxspeed = 10000; // maxspeedをグローバル変数として宣言

void readUntilPipe(char *output_buf, int output_buf_size)
{
    char buf[20];
    int output_buf_index = 0;
    while (1)
    {
        if (pc.readable())
        {
            ssize_t num = pc.read(buf, sizeof(buf) - 1); // -1 to leave space for null terminator
            buf[num] = '\0';
            for (int i = 0; i < num; i++)
            {
                if (buf[i] == '|')
                {
                    output_buf[output_buf_index] = '\0';
                    return;
                }
                else if (buf[i] != '\n' && output_buf_index < output_buf_size - 1)
                {
                    output_buf[output_buf_index++] = buf[i];
                }
            }
        }
    }
}

void CANSend()
{
    while (1)
    {
        output = lefts;
        DATA[0] = output >> 8; // Big-endian conversion
        DATA[1] = output;
        DATA[2] = output >> 8; // Big-endian conversion
        DATA[3] = output;
        DATA[4] = -output >> 8; // Big-endian conversion
        DATA[5] = -output;
        DATA[6] = -output >> 8; // Big-endian conversion
        DATA[7] = -output;
        CANMessage msg0(0x200, DATA, 8);

        output = rigts;
        DATA[0] = output >> 8; // Big-endian conversion
        DATA[1] = output;
        DATA[2] = output >> 8; // Big-endian conversion
        DATA[3] = output;
        DATA[4] = -output >> 8; // Big-endian conversion
        DATA[5] = -output;
        DATA[6] = -output >> 8; // Big-endian conversion
        DATA[7] = -output;
        CANMessage msg1(0x1FF, DATA, 8);

        penguin.pwm[0] = -leftJoystickX*0.2 - leftJoystickY*0.2 - rightJoystickX;
        penguin.pwm[1] = leftJoystickX*0.2 - leftJoystickY*0.2 - rightJoystickX;
        penguin.pwm[2] = leftJoystickX*0.2 + leftJoystickY*0.2 - rightJoystickX;
        penguin.pwm[3] = leftJoystickY*0.2 - leftJoystickX*0.2 - rightJoystickX;

        if (penguin.pwm[0] > maxspeed) penguin.pwm[0] = maxspeed;
        if (penguin.pwm[1] > maxspeed) penguin.pwm[1] = maxspeed;
        if (penguin.pwm[2] > maxspeed) penguin.pwm[2] = maxspeed;
        if (penguin.pwm[3] > maxspeed) penguin.pwm[3] = maxspeed;
        printf("penguin.pwm[0] = %d, penguin.pwm[1] = %d, penguin.pwm[2] = %d, penguin.pwm[3] = %d\n", penguin.pwm[0], penguin.pwm[1], penguin.pwm[2], penguin.pwm[3]);

        can1.write(msg0);
        can2.write(msg1);
        penguin.send();
        ThisThread::sleep_for(10ms);
    }
}

int main()
{
    Thread thread1; // Threadインスタンスの宣言
    thread1.start(CANSend); // thread1をCANSend関数で開始
    while (1)
    {
        char output_buf[20];                           // 出力用のバッファを作成します
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);
    }
}

// ##asimawari*4
// 1 2
// 4 3
// hennsuu:maxspeed

// ##kaisyuu*1


// ##hassya1*4
// hensuu:lefts
// ##hassya2*4 
// hensuu:rigts

// 逆回転プログラムなし

//    else if (strncmp(output_buf, "cross", 5) == 0)
//     {
//         lefts = 16380;
//     }
//     else if (strncmp(output_buf, "triangle", 8) == 0)
//     {
//         lefts = 0;
//     }
//     else if (strncmp(output_buf, "circle", 6) == 0)
//     {
//         rigts = 16380;
//     }
//     else if (strncmp(output_buf, "square", 6) == 0)
//     {
//         rigts = 0;
//     }
