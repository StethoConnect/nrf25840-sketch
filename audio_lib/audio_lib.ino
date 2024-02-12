#include <mic.h>

// Settings
#define DEBUG 1                 // Enable pin pulse during ISR  
#define SAMPLES 800

mic_config_t mic_config{
  .channel_cnt = 1,
  .sampling_rate = 16000,
  .buf_size = 1600,
  .debug_pin = LED_BUILTIN                // Toggles each DAC ISR (if DEBUG is set to 1)
};

NRF52840_ADC_Class Mic(&mic_config);

int16_t recording_buf[SAMPLES];
volatile uint8_t recording = 0;
volatile static bool record_ready = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) {delay(10);}

  Mic.set_callback(audio_rec_callback);

  if (!Mic.begin()) {
    Serial.println("Mic initialization failed");
    while (1); // Halt if initialization fails
  }

  Serial.println("Mic initialization done.");
}

void loop() {
  if (record_ready) {
    Serial.println("Finished sampling");
  
    for (int i = 0; i < SAMPLES; i++) {
      int16_t sample = recording_buf[i];
      Serial.println(sample);
    }
  
    record_ready = false; 
  }
}

static void audio_rec_callback(uint16_t *buf, uint32_t buf_len) {
  static uint32_t idx = 0;
  
  for (uint32_t i = 0; i < buf_len; i++) {
    recording_buf[idx++] = buf[i];
    if (idx >= SAMPLES){ 
      idx = 0;
      recording = 0;
      record_ready = true;
      break;
    } 
  }
}


