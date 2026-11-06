#ifndef myoptions_h
#define myoptions_h

// 1. 显示屏定义 (ILI9341 2.8寸 SPI)
#define D_TFT
#define TFT_ILI9341
#define TFT_WIDTH           320
#define TFT_HEIGHT          240
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_CS              10
#define TFT_DC              4
#define TFT_RST             48
#define TFT_BL              45

// 2. 音频解码定义 (ES8311 I2S + I2C 控制)
#define D_I2S
#define I2S_DS_ES8311
#define ES8311_SDA          17
#define ES8311_SCL          18
#define I2S_MCLK            2
#define I2S_BCLK            42
#define I2S_LRC             41
#define I2S_DOUT            40
#define I2S_DIN             39

// 3. 功放使能控制 (GPIO 1 低电平有效)
#define AUDIO_PA_PIN        1
#define AUDIO_PA_ACTIVE     0

#endif
