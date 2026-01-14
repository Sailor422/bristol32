#pragma once
#include <Arduino.h>

struct LoraCfg { float freq=433.775f; int bw=125; int sf=9; int cr=7; int power=10; };
struct MotionCfg { uint32_t refractory_ms=10000; };
struct EnvCfg { uint32_t period_s=60; };
struct SmtpCfg { String host; uint16_t port=465; String user; String app_pw; String to; };
struct AlarmCfg { bool armed=true; uint32_t grace_period_s=30; uint32_t alert_cooldown_s=300; };
struct AppCfg { LoraCfg lora; MotionCfg motion; EnvCfg env; SmtpCfg smtp; AlarmCfg alarm; bool chime=true; };

extern AppCfg CFG; // defined in each firmware target
