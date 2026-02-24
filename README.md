#  AI Voice Assistant using ESP32 (STT + LLM + TTS)

A complete AI-powered Voice Assistant built using **ESP32**, **Deepgram Speech-to-Text**, **Groq LLM (LLaMA 3.1)**, and **Deepgram Text-to-Speech**.

The system listens to user voice through a microphone, converts speech to text, processes the query using an AI model, converts the AI response back into speech, and plays it through a speaker using DF Mini Player.

---

#  Introduction

This project demonstrates real-time AI voice interaction using embedded hardware and cloud AI services.  
The ESP32 captures audio from a microphone, sends it to Deepgram STT for transcription, forwards the text to Groq’s LLaMA model for AI response generation, converts the response into speech using Deepgram TTS, and plays the audio through DF Mini Player.

It is a low-cost smart assistant integrating IoT + AI + Embedded Systems.

---

#  Features

-  Real-time voice input via microphone
-  Deepgram Speech-to-Text (STT)
-  Groq LLaMA 3.1 AI processing
-  Deepgram Text-to-Speech (TTS)
-  MP3 stored in DF Mini Player SD card
-  Speaker-based voice output
-  WiFi-based cloud AI communication

---

#  Hardware Components Required

| Component | Quantity |
|-----------|----------|
| ESP32 | 1 |
| DF Mini Player | 1 |
| Micro SD Card (FAT32) | 1 |
| 3W 4Ω Speaker | 1 |
| Microphone Module (I2S Recommended: INMP441) | 1 |
| Jumper Wires | As required |
| 5V Power Supply | 1 |

---

#  Hardware Connections

##  DF Mini Player ↔ ESP32

| DF Mini Player Pin | ESP32 Pin |
|--------------------|------------|
| VCC | 5V |
| GND | GND |
| TX | GPIO16 |
| RX | GPIO17 |
| SPK1 | Speaker + |
| SPK2 | Speaker - |



## I2S Microphone (INMP441) ↔ ESP32

| INMP441 Pin | ESP32 Pin |
|-------------|------------|
| VCC | 3.3V |
| GND | GND |
| WS | GPIO25 |
| SCK | GPIO26 |
| SD | GPIO33 |

---




#  Software Requirements

- VS Code (PlatformIO)
- ESP32 Board Package
- Required Libraries:
  - WiFi.h
  - WiFiClientSecure.h
  - HTTPClient.h
  - ArduinoJson
  - DFRobotDFPlayerMini
  - I2S library (ESP32 built-in)



#  System Workflow

1.  User speaks into microphone.
2. ESP32 records audio using I2S.
3. Audio is sent to Deepgram STT API.
4. STT converts speech → text.
5. Text is sent to Groq LLM.
6. LLM generates intelligent response.
7. Response is sent to Deepgram TTS.
8. MP3 file is saved to SD card inside DF Player.
9. DF Mini Player plays response through speaker.

-

#  APIs Used

- Deepgram Speech-to-Text API
- Groq LLaMA 3.1 8B Instant Model
- Deepgram Text-to-Speech API

---


---



