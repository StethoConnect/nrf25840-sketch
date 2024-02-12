#include <ArduinoBLE.h>
#include <mic.h> // Assuming this is your microphone library

// Microphone Settings (Unchanged)
#define DEBUG 1
#define SAMPLES 100

mic_config_t mic_config = {
  .channel_cnt = 1,
  .sampling_rate = 16000,
  .buf_size = 1600,
  .debug_pin = LED_BUILTIN
};

NRF52840_ADC_Class Mic(&mic_config);
int16_t recording_buf[SAMPLES];
volatile static bool record_ready = false;

// BLE UUIDs
#define SERVICE_UUID "12347778-1984-1234-1234-123456789012"
#define CHARACTERISTIC_UUID_AUDIO "87654321-4321-7777-4321-210987654321"

// BLE Service and Characteristic
BLEService audioService(SERVICE_UUID);
BLECharacteristic audioDataCharacteristic(CHARACTERISTIC_UUID_AUDIO, BLERead | BLENotify, 2 * SAMPLES); // 2 * SAMPLES to accommodate the size of the audio buffer

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Microphone initialization
  Mic.set_callback(audio_rec_callback);
  if (!Mic.begin()) {
    Serial.println("Mic initialization failed");
    while (1);
  }
  Serial.println("Mic initialization done.");

  // BLE initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("SCT Audio");
  BLE.addService(audioService);
  audioService.addCharacteristic(audioDataCharacteristic);
  BLE.advertise();
  Serial.println("BLE Peripheral is now advertising");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.println("Connected to central device");
    
    while (central.connected()) {
      if (record_ready) {
        // Plot the audio data in the Serial Plotter
        for (int i = 0; i < SAMPLES; i++) {
          Serial.println(recording_buf[i]);
        }

        // Transmit the audio data
        audioDataCharacteristic.writeValue((uint8_t*)recording_buf, 2 * SAMPLES);
        Serial.println("Audio data transmitted over BLE");
        record_ready = false;
      }
    }

    Serial.println("Disconnected from central device");
  }
}

static void audio_rec_callback(uint16_t *buf, uint32_t buf_len) {
  static uint32_t idx = 0;
  
  for (uint32_t i = 0; i < buf_len; i++) {
    recording_buf[idx++] = buf[i];
    if (idx >= SAMPLES){ 
      idx = 0;
      record_ready = true;
      break;
    } 
  }
}
