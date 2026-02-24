#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <driver/i2s.h>
#include <SPIFFS.h>


const char* ssid = "Ankush";
const char* password = "123456789";


const char* groqKey = "YOUR_GROQ_KEY";
const char* deepgramKey = "YOUR_DEEPGRAM_KEY";


HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfplayer;


#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 26
#define I2S_PORT I2S_NUM_0

#define SAMPLE_RATE 16000
#define RECORD_TIME 5   // seconds

WiFiClientSecure client;

void setupI2S();
String speechToText();
String askGroq(String question);
bool saveTTS(String text);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi Connected");

  client.setInsecure();

  // SPIFFS for temporary audio storage
  SPIFFS.begin(true);

  // DF Player
  dfSerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(dfSerial)) {
    Serial.println("DFPlayer Error");
    while (true);
  }
  dfplayer.volume(25);

  setupI2S();

  Serial.println("AI Voice Assistant Ready...");
}

void loop() {

  Serial.println("Speak Now...");
  delay(2000);

  String question = speechToText();

  if (question == "") {
    Serial.println("No speech detected");
    return;
  }

  Serial.println("You Said: " + question);

  String reply = askGroq(question);

  Serial.println("AI: " + reply);

  if (saveTTS(reply)) {
    delay(1000);
    dfplayer.playMp3Folder(1);
  }

  delay(5000);
}


void setupI2S() {

  i2s_config_t config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 1024
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}


String speechToText() {

  File file = SPIFFS.open("/audio.raw", FILE_WRITE);

  int totalSamples = SAMPLE_RATE * RECORD_TIME;
  int16_t sampleBuffer[512];
  size_t bytesRead;

  for (int i = 0; i < totalSamples / 512; i++) {
    i2s_read(I2S_PORT, sampleBuffer, sizeof(sampleBuffer), &bytesRead, portMAX_DELAY);
    file.write((uint8_t*)sampleBuffer, bytesRead);
  }

  file.close();

  HTTPClient https;
  https.begin(client, "https://api.deepgram.com/v1/listen");
  https.addHeader("Authorization", "Token " + String(deepgramKey));
  https.addHeader("Content-Type", "audio/raw;encoding=signed-integer;bits=16;rate=16000;channels=1");

  File audioFile = SPIFFS.open("/audio.raw", FILE_READ);
  int code = https.sendRequest("POST", &audioFile, audioFile.size());

  if (code > 0) {
    String response = https.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, response);
    https.end();
    return doc["results"]["channels"][0]["alternatives"][0]["transcript"].as<String>();
  }

  https.end();
  return "";
}


String askGroq(String question) {

  HTTPClient https;
  https.begin(client, "https://api.groq.com/openai/v1/chat/completions");

  https.addHeader("Content-Type", "application/json");
  https.addHeader("Authorization", "Bearer " + String(groqKey));

  DynamicJsonDocument doc(4096);
  doc["model"] = "llama-3.1-8b-instant";

  JsonArray messages = doc["messages"].to<JsonArray>();
  JsonObject msg = messages.add<JsonObject>();
  msg["role"] = "user";
  msg["content"] = question;

  String body;
  serializeJson(doc, body);

  int code = https.POST(body);

  if (code > 0) {
    String response = https.getString();
    DynamicJsonDocument resDoc(8192);
    deserializeJson(resDoc, response);
    https.end();
    return resDoc["choices"][0]["message"]["content"].as<String>();
  }

  https.end();
  return "Error getting response";
}


bool saveTTS(String text) {

  HTTPClient https;
  https.begin(client, "https://api.deepgram.com/v1/speak?model=aura-asteria-en");

  https.addHeader("Authorization", "Token " + String(deepgramKey));
  https.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(2048);
  doc["text"] = text;

  String body;
  serializeJson(doc, body);

  int code = https.POST(body);

  if (code == 200) {

    File file = SPIFFS.open("/0001.mp3", FILE_WRITE);
    WiFiClient* stream = https.getStreamPtr();

    while (https.connected()) {
      while (stream->available()) {
        file.write(stream->read());
      }
    }

    file.close();
    https.end();

    return true;
  }

  https.end();
  return false;
}
