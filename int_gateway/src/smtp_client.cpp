#include <Arduino.h>
#include <ESP_Mail_Client.h>
#include "config.h"
#include "smtp_client.h"

// Simple producer/consumer queue for outbound emails.
struct MailItem { String subject; String body; };
static QueueHandle_t q_mail;
static SMTPSession smtp;
static Session_Config cfg;
static bool ready = false;

static void smtp_task(void*);

void smtp_client_start(){
  if(q_mail) return;
  q_mail = xQueueCreate(10, sizeof(MailItem));
  // Configure server & login from CFG.smtp
  cfg.server.host_name = CFG.smtp.host.c_str();
  cfg.server.port = CFG.smtp.port; // 465 for SSL
  cfg.login.email = CFG.smtp.user.c_str(); // full email address
  cfg.login.password = CFG.smtp.app_pw.c_str(); // app password
  cfg.login.user_domain = F("local");
  smtp.callback(NULL); // no verbose callback
  xTaskCreatePinnedToCore(smtp_task, "smtp", 6144, nullptr, 1, nullptr, 0);
}

void smtp_send_async(const String& subject, const String& body) {
  if(!q_mail) return; // not started yet
  MailItem it{subject, body};
  xQueueSend(q_mail, &it, 0);
}

static bool ensure_login(){
  if(ready && smtp.connected()) return true;
  if(!MailClient.smtpConnect(&smtp, &cfg)) return false;
  ready = true; return true;
}

static void smtp_task(void*){
  uint32_t backoff_ms = 10000; // 10s -> 20s ... cap 10min
  for(;;){
    MailItem it;
    if(xQueueReceive(q_mail, &it, portMAX_DELAY) == pdTRUE) {
      // drain bursts quickly
      for(;;){
        if(!ensure_login()){
          vTaskDelay(pdMS_TO_TICKS(backoff_ms));
          backoff_ms = min<uint32_t>(backoff_ms * 2, 600000); // cap 10 min
          continue;
        }
        SMTP_Message msg;
        msg.sender.name = F("Bristol32");
        msg.sender.email = CFG.smtp.user.c_str();
        msg.subject = it.subject.c_str();
        msg.addRecipient(F("Skipper"), CFG.smtp.to.c_str());
        msg.addMessage(it.body.c_str(), false /*plain text*/);

        if(MailClient.sendMail(&smtp, &msg)){
          backoff_ms = 10000; // reset on success
          break; // sent OK
        } else {
          // push back to queue head after delay (simple retry)
          vTaskDelay(pdMS_TO_TICKS(backoff_ms));
          backoff_ms = min<uint32_t>(backoff_ms * 2, 600000);
          // requeue (best-effort; drop if full to avoid infinite growth)
          xQueueSendToFront(q_mail, &it, 0);
        }
      }
    }
  }
}
