#ifndef myoptions_h
#define myoptions_h

// 1. 显示屏核心宏与引脚定义 (2.8寸 ILI9341 SPI)
#define DSP_MODEL           DSP_ILI9341
#define TFT_ROTATE          1          // 横屏模式 (320x240)
#define TFT_CS              10
#define TFT_DC              46         // 官方硬件标定：GPIO 46
#define TFT_RST             -1         // 与主控复位直连
#define TFT_MOSI            11
#define TFT_SCLK            12

// 2. 屏幕背光与亮度控制 (YoRadio 专用宏：BRIGHTNESS_PIN)
#define BRIGHTNESS_PIN      45         // 官方硬件标定：GPIO 45 高电平点亮背光

// 3. 音频解码芯片 ES8311 (I2C 控制总线)
#define D_I2S
#define I2S_DS_ES8311
#define ES8311_SDA          16
#define ES8311_SCL          15

// 4. I2S 数字音频传输引脚
#define I2S_MCLK            4
#define I2S_BCLK            5
#define I2S_DOUT            6
#define I2S_LRC             7
#define I2S_DIN             8

// 5. 功放使能引脚 (GPIO 1 低电平使能放音)
#define AUDIO_PA_PIN        1
#define AUDIO_PA_ACTIVE     0

// 6. 电容触摸屏配置 (ES3C28P - FT6336G)
#define TS_MODEL            TS_MODEL_FT6336
#define TS_SDA              16         // I2C 数据线
#define TS_SCL              15         // I2C 时钟线
#define TS_INT              17         // 触摸中断脚
#define TS_RST              18         // 触摸复位脚

#endif
