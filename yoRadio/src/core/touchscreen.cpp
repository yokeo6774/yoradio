#include "options.h"
#if (TS_MODEL!=TS_MODEL_UNDEFINED) && (DSP_MODEL!=DSP_DUMMY)
#include "Arduino.h"
#include <Wire.h>
#include "touchscreen.h"
#include "config.h"
#include "controls.h"
#include "display.h"
#include "player.h"

// ESP32-S3 硬件时钟矩阵信号支持
#if CONFIG_IDF_TARGET_ESP32S3
#include "soc/gpio_sig_map.h"
#include "rom/gpio.h"
#include "esp_rom_gpio.h"
#endif

#ifndef TS_X_MIN
  #define TS_X_MIN              400
#endif
#ifndef TS_X_MAX
  #define TS_X_MAX              3800
#endif
#ifndef TS_Y_MIN
  #define TS_Y_MIN              260
#endif
#ifndef TS_Y_MAX
  #define TS_Y_MAX              3800
#endif
#ifndef TS_STEPS
  #define TS_STEPS              40
#endif

#if TS_MODEL==TS_MODEL_XPT2046
  #ifdef TS_SPIPINS
    SPIClass  TSSPI(HSPI);
  #endif
  #include <XPT2046_Touchscreen.h>
  XPT2046_Touchscreen ts(TS_CS);
  typedef TS_Point TSPoint;
#elif TS_MODEL==TS_MODEL_GT911
  #include "../GT911_Touchscreen/TAMC_GT911.h"
  TAMC_GT911 ts = TAMC_GT911(TS_SDA, TS_SCL, TS_INT, TS_RST, 0, 0);
  typedef TP_Point TSPoint;
#elif TS_MODEL==TS_MODEL_FT6336
  struct TSPoint { uint16_t x; uint16_t y; };
#endif

void TouchScreen::init(uint16_t w, uint16_t h){
#if TS_MODEL==TS_MODEL_XPT2046
  #ifdef TS_SPIPINS
    TSSPI.begin(TS_SPIPINS);
    ts.begin(TSSPI);
  #else
    #if TS_HSPI
      ts.begin(SPI2);
    #else
      ts.begin();
    #endif
  #endif
  ts.setRotation(config.store.fliptouch?3:1);
#elif TS_MODEL==TS_MODEL_GT911
  ts.begin();
  ts.setRotation(config.store.fliptouch?0:2);
#elif TS_MODEL==TS_MODEL_FT6336
  // 1. 触摸屏 FT6336G 硬件复位与引脚准备
  if (TS_RST >= 0) {
    pinMode(TS_RST, OUTPUT);
    digitalWrite(TS_RST, LOW);
    delay(10);
    digitalWrite(TS_RST, HIGH);
    delay(50);
  }
  if (TS_INT >= 0) {
    pinMode(TS_INT, INPUT);
  }
  Wire.begin(TS_SDA, TS_SCL);

  // 2. 将 ESP32-S3 硬件 I2S0 MCLK 主时钟信号路由输出到 GPIO 4
  #if CONFIG_IDF_TARGET_ESP32S3
    pinMode(4, OUTPUT);
    gpio_matrix_out(4, I2S0_MCLK_OUT_IDX, false, false);
  #endif

  // 3. 完整初始化 ES3C28P 板载 ES8311 音频解码芯片 (I2C 地址 0x18)
  auto writeCodec = [](uint8_t reg, uint8_t val) {
    Wire.beginTransmission(0x18);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  };

  writeCodec(0x00, 0x80); // 软复位芯片
  delay(20);
  writeCodec(0x00, 0x1F); // 启动 CSM 主控时钟与所有内部逻辑
  writeCodec(0x45, 0x00); // 退出低功耗测试模式
  writeCodec(0x01, 0x30); // 启动时钟管理器并使能时钟
  writeCodec(0x02, 0x10); // 配置系统预分频
  writeCodec(0x03, 0x10); // ADC 时钟设置
  writeCodec(0x04, 0x10); // DAC 时钟设置
  writeCodec(0x05, 0x00); // BCLK 分频
  writeCodec(0x06, 0x00); // LRCK 分频
  writeCodec(0x07, 0x00); // 自动分频模式
  writeCodec(0x08, 0xFF); // 保持所有时钟域使能
  writeCodec(0x09, 0x0C); // 核心：配置 SDP 为标准 Philips I2S 16-Bit 格式
  writeCodec(0x0A, 0x0C); // 配置 SDP 输出为 16-Bit 格式
  writeCodec(0x0B, 0x00); // 标准时钟极性
  writeCodec(0x0C, 0x00); // 禁用混音
  writeCodec(0x0D, 0x01); // 启动模拟偏置基准电源 (Vref / VMID)
  writeCodec(0x0E, 0x02); // 启动模拟内部参考电压生成
  writeCodec(0x0F, 0x44); // 配置模拟偏置电流
  writeCodec(0x10, 0x1F); // 开启低噪声偏置电阻
  writeCodec(0x11, 0x7F); // 开启模拟驱动级电源
  writeCodec(0x12, 0x00); // 启动 DAC 核心电源
  writeCodec(0x13, 0x10); // 使能 DAC 输出通道并送入功放
  writeCodec(0x14, 0x1A); // 设定模拟输出增益为 +3.5dB
  writeCodec(0x15, 0x00); // 禁用反相
  writeCodec(0x31, 0x00); // 解除 DAC 硬件静音
  writeCodec(0x32, 0xC0); // 核心：设定 DAC 数字音量为 0xC0 (标准 0dB 满幅输出)
  writeCodec(0x37, 0x08); // 启用 DAC 内部平滑与低通抗混叠滤波
#endif
  _width  = w;
  _height = h;
#if TS_MODEL==TS_MODEL_GT911
  ts.setResolution(_width, _height);
#endif
}

tsDirection_e TouchScreen::_tsDirection(uint16_t x, uint16_t y) {
  int16_t dX = x - _oldTouchX;
  int16_t dY = y - _oldTouchY;
  if (abs(dX) > 20 || abs(dY) > 20) {
    if (abs(dX) > abs(dY)) {
      if (dX > 0) {
        return TSD_RIGHT;
      } else {
        return TSD_LEFT;
      }
    } else {
      if (dY > 0) {
        return TSD_DOWN;
      } else {
        return TSD_UP;
      }
    }
  } else {
    return TDS_REQUEST;
  }
}

void TouchScreen::flip(){
#if TS_MODEL==TS_MODEL_XPT2046
  ts.setRotation(config.store.fliptouch?3:1);
#elif TS_MODEL==TS_MODEL_GT911
  ts.setRotation(config.store.fliptouch?0:2);
#endif
}

void TouchScreen::loop(){
  uint16_t touchX = 0, touchY = 0;
  static bool wastouched = true;
  static uint32_t touchLongPress;
  static tsDirection_e direct;
  static uint16_t touchVol, touchStation;
  if (!_checklpdelay(20, _touchdelay)) return;
#if TS_MODEL==TS_MODEL_GT911
  ts.read();
#endif
  bool istouched = _istouched();
  if(istouched){
  #if TS_MODEL==TS_MODEL_XPT2046
    TSPoint p = ts.getPoint();
    touchX = map(p.x, TS_X_MIN, TS_X_MAX, 0, _width);
    touchY = map(p.y, TS_Y_MIN, TS_Y_MAX, 0, _height);
  #elif TS_MODEL==TS_MODEL_GT911
    TSPoint p = ts.points[0];
    touchX = p.x;
    touchY = p.y;
  #elif TS_MODEL==TS_MODEL_FT6336
    TSPoint p = {0, 0};
    Wire.beginTransmission(0x38);
    Wire.write(0x03);
    if (Wire.endTransmission(false) == 0 && Wire.requestFrom((uint8_t)0x38, (uint8_t)4) == 4) {
      uint8_t xh = Wire.read();
      uint8_t xl = Wire.read();
      uint8_t yh = Wire.read();
      uint8_t yl = Wire.read();
      uint16_t raw_x = ((xh & 0x0F) << 8) | xl;
      uint16_t raw_y = ((yh & 0x0F) << 8) | yl;
      if (config.store.fliptouch) {
        p.x = 320 - raw_y;
        p.y = raw_x;
      } else {
        p.x = raw_y;
        p.y = 240 - raw_x;
      }
    }
    touchX = p.x;
    touchY = p.y;
  #endif
    if (!wastouched) { /*     START TOUCH     */
      _oldTouchX = touchX;
      _oldTouchY = touchY;
      touchVol = touchX;
      touchStation = touchY;
      direct = TDS_REQUEST;
      touchLongPress=millis();
    } else { /*     SWIPE TOUCH     */
      direct = _tsDirection(touchX, touchY);
      switch (direct) {
        case TSD_LEFT:
        case TSD_RIGHT: {
            touchLongPress=millis();
            if(display.mode()==PLAYER || display.mode()==VOL){
              int16_t xDelta = map(abs(touchVol - touchX), 0, _width, 0, TS_STEPS);
              display.putRequest(NEWMODE, VOL);
              if (xDelta>1) {
                controlsEvent((touchVol - touchX)<0);
                touchVol = touchX;
              }
            }
            break;
          }
        case TSD_UP:
        case TSD_DOWN: {
            touchLongPress=millis();
            if(display.mode()==PLAYER || display.mode()==STATIONS){
              int16_t yDelta = map(abs(touchStation - touchY), 0, _height, 0, TS_STEPS);
              display.putRequest(NEWMODE, STATIONS);
              if (yDelta>1) {
                controlsEvent((touchStation - touchY)<0);
                touchStation = touchY;
              }
            }
            break;
          }
        default:
            break;
      }
    }
    if (config.store.dbgtouch) {
      Serial.print(", x = ");
      Serial.print(p.x);
      Serial.print(", y = ");
      Serial.println(p.y);
    }
  }else{
    if (wastouched) {/*     END TOUCH     */
      if (direct == TDS_REQUEST) {
        uint32_t pressTicks = millis()-touchLongPress;
        if( pressTicks < BTN_PRESS_TICKS*2){
          if(pressTicks > 50) onBtnClick(EVT_BTNCENTER);
        }else{
          display.putRequest(NEWMODE, display.mode() == PLAYER ? STATIONS : PLAYER);
        }
      }
      direct = TSD_STAY;
    }
  }
  wastouched = istouched;
}

bool TouchScreen::_checklpdelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

bool TouchScreen::_istouched(){
#if TS_MODEL==TS_MODEL_XPT2046
  return ts.touched();
#elif TS_MODEL==TS_MODEL_GT911
  return ts.isTouched;
#elif TS_MODEL==TS_MODEL_FT6336
  Wire.beginTransmission(0x38);
  Wire.write(0x02);
  if (Wire.endTransmission() != 0) return false;
  if (Wire.requestFrom((uint8_t)0x38, (uint8_t)1) != 1) return false;
  return (Wire.read() & 0x0F) > 0;
#endif
}

#endif  // TS_MODEL!=TS_MODEL_UNDEFINED
