#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "WifiManager.hpp"

#define TOUCH_PIN 15
#define LED_PIN 23
#define BUZZER_PIN 22
#define TOUCH_THRESHOLD 40

int touchValue = 0;
long lastTouch = 0;

void touchWakeCallback() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }

  touch_pad_t touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch (touchPin) {
    case 0:
      Serial.println("Touch detected on GPIO 4");
      break;
    case 1:
      Serial.println("Touch detected on GPIO 0");
      break;
    case 2:
      Serial.println("Touch detected on GPIO 2");
      break;
    case 3:
      Serial.println("Touch detected on GPIO 15");
      break;
    case 4:
      Serial.println("Touch detected on GPIO 13");
      break;
    case 5:
      Serial.println("Touch detected on GPIO 12");
      break;
    case 6:
      Serial.println("Touch detected on GPIO 14");
      break;
    case 7:
      Serial.println("Touch detected on GPIO 27");
      break;
    case 8:
      Serial.println("Touch detected on GPIO 33");
      break;
    case 9:
      Serial.println("Touch detected on GPIO 32");
      break;
    default:
      Serial.println("Wakeup not by touchpad");
      break;
  }
}

void touchTask(void *pv) {
  bool doOnce_ = false;

  touchAttachInterrupt(TOUCH_PIN, [](){}, TOUCH_THRESHOLD);
  esp_sleep_enable_touchpad_wakeup();

  while (1) {
    touchValue = touchRead(TOUCH_PIN);

    if (touchValue < TOUCH_THRESHOLD) {
      if (!doOnce_) {
        Serial.println("RING!!!");
        doOnce_ = true;
      }
      digitalWrite(LED_PIN, 1);
      digitalWrite(BUZZER_PIN, 1);

      lastTouch = millis();
    } else {
      doOnce_ = false;
      digitalWrite(LED_PIN, 0);
      digitalWrite(BUZZER_PIN, 0);
    }

    delay(10);
  }
  vTaskDelete(NULL);
}

// void mqttTask(void* pv) {
//   // connect wifi
//   // connect mqtt
//   while(1) {
//     // reconnect
//     vTaskDelay(10);
//   }
//   vTaskDelete(NULL);
// }

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUILTIN_LED, 1);     // indicator of deepsleep

  touchWakeCallback();

  xTaskCreate(touchTask, "touchTask", 1024 * 2, NULL, 1, NULL);
  // xTaskCreate(mqttTask, "mqttTask", 1024 * 4, NULL, 1, NULL);
}

void loop() {
  // after 10 sec of inactivity, enter deepsleep
  if (millis() - lastTouch > 10000) {
    Serial.println("Entering Deepsleep...");
    esp_deep_sleep_start();
  }
}