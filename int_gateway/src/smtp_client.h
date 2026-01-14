#pragma once
#include <Arduino.h>
void smtp_client_start();
void smtp_send_async(const String& subject, const String& body);
