#ifndef myoptions_h
#define myoptions_h

// 1. 2.8寸 ILI9341 SPI 显示屏引脚定义
#define D_TFT
#define TFT_ILI9341
#define TFT_WIDTH           320
#define TFT_HEIGHT          240
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_CS              10
#define TFT_DC              46     // 官方原理图标定：GPIO 46 (TFT_RS)
#define TFT_RST             -1     // 官方原理图标定：与 ESP32-S3 EN 复位引脚直连
#define TFT_BL              45     // 官方原理图标定：GPIO 45 高电平使能背光

// 2. ES8311 音频芯片 (I2C 控制总线)
#define D_I2S
#define I2S_DS_ES8311
#define ES8311_SDA          16     // 官方原理图标定：GPIO 16
#define ES8311_SCL          15     // 官方原理图标定：GPIO 15

// 3. I2S 数字音频流引脚
#define I2S_MCLK            4      // 官方原理图标定：GPIO 4 (MCK)
#define I2S_BCLK            5      // 官方原理图标定：GPIO 5 (SCK/BCK)
#define I2S_DOUT            6      // 官方原理图标定：GPIO 6 (DO)
#define I2S_LRC             7      // 官方原理图标定：GPIO 7 (WS/LRC)
#define I2S_DIN             8      // 官方原理图标定：GPIO 8 (DI)

// 4. 功放芯片使能控制 (GPIO 1 低电平开启放音)
#define AUDIO_PA_PIN        1
#define AUDIO_PA_ACTIVE     0

#endif
