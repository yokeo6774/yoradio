#ifndef myoptions_h
#define myoptions_h

// 1. 核心显示屏驱动声明（必须为 DSP_MODEL DSP_ILI9341）
#define DSP_MODEL           DSP_ILI9341
#define TFT_ROTATE          1          // 横屏显示 (320x240)
#define BRIGHTNESS          100        // 背光初始亮度设为最大

// 2. 屏幕硬件引脚定义 (SPI 总线)
#define TFT_CS              10
#define TFT_DC              46         // 若重新编译后背光亮但无字，可改为 4
#define TFT_RST             -1         // 硬件与系统复位脚连通
#define TFT_BL              45         // 背光使能脚
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_MISO            -1

// 3. ES8311 音频芯片 (I2C 控制)
#define D_I2S
#define I2S_DS_ES8311
#define ES8311_SDA          16
#define ES8311_SCL          15

// 4. I2S 数字音频流引脚
#define I2S_MCLK            4
#define I2S_BCLK            5
#define I2S_DOUT            6
#define I2S_LRC             7
#define I2S_DIN             8

// 5. 功放芯片使能 (GPIO 1 低电平开启)
#define AUDIO_PA_PIN        1
#define AUDIO_PA_ACTIVE     0

#endif
