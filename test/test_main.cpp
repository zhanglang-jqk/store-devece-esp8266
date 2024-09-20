#include <Arduino.h>

#include <unity.h>
#include <TTS.h>
#include <SoftwareSerial.h>

#define LED_PIN 13

#if 0

void test_led_state_high(void)
{
  digitalWrite(LED_PIN, HIGH);
  TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_PIN));
}

void test_led_state_low(void)
{
  digitalWrite(LED_PIN, LOW);
  const char *message = "Blink count: ";
  for (int i = 0; message[i] != '\0'; i++)
  {
    UNITY_OUTPUT_CHAR(message[i]);
  }
  printf("hello world\n");
  TEST_ASSERT_EQUAL(LOW, digitalRead(LED_PIN));
}

void test_serial_print(void)
{
  Serial.begin(115200);
  Serial.println("hello world");
}

void test_tts_send_voice()
{
  Serial.begin(115200);
  TTS tts(Serial);

  tts.Init(Serial);

  char *voice_str = "你好";
  tts.SendPlayVoice_cmd((uint8_t *)voice_str, strlen(voice_str));

  delay(1000);
}

#endif

#if 0
void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!

  // delay(2000);

  pinMode(13, OUTPUT);

  Serial.begin(115200);
}

uint8_t i = 0;
uint8_t max_blinks = 20;

void loop()
{
  test_tts_send_voice();
  UNITY_END(); // stop unit testinu
}
#endif