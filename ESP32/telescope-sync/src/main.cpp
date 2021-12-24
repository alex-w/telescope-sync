#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

/**
 * A file called 'secrets.h' must be placed in lib/secrets and contain the following content:
 * 
 * #ifndef SECRETS_H
 * #define SECRETS_H
 * struct Secrets
 * {
 *     const char *wifiSsid = "test";   // WiFi AP-Name
 *     const char *wifiPassword = "1234";
 * } secrets;
 * #endif
 * 
 **/
#include <secrets.h>


#define BNO055_PIN_I2C_SCL (22)
#define BNO055_PIN_I2C_SDA (21)
#define BNO055_BUS_ID (0)
#define BNO055_PIN_VCC (4)

TwoWire I2CBME = TwoWire(BNO055_BUS_ID); // set up a new Wire-Instance for BNO055 Intelligent Absolute Orientation Sensor
Adafruit_BNO055 bno = Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS_A, &I2CBME);

bool orientationSensorAvailable = false;
bool gnssModuleAvailable = false;

// Flow control, basic task scheduler
#define SCHEDULER_MAIN_LOOP_MS (10) // ms
uint32_t counterBase = 0;
uint32_t counter2s = 0;
uint32_t counter300s = 0;
uint32_t counter1h = 0;
uint32_t initStage = 0;

void setup() {
  // LED output
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  initStage++;  

  // Setup serial connection for debugging
  Serial.begin(115200U);
  delay(500);
  Serial.println();
  Serial.println("[  INIT  ] Begin");
  initStage++;  

  Serial.printf("[  INIT  ] ChipRevision: 0x%02X    CpuFreq: %uMHz   FlashChipSize: %uKiB   HeapSize: %uKiB   MAC: %s   SdkVersion: %s\n",
                ESP.getChipRevision(),
                ESP.getCpuFreqMHz(),
                ESP.getFlashChipSize() / 1024,
                ESP.getHeapSize() / 1024,
                WiFi.macAddress().c_str(),
                ESP.getSdkVersion());
  initStage++;


  //connect to your local wi-fi network
  Serial.printf("[  INIT  ] Connecting to Wifi '%s'", secrets.wifiSsid);
  WiFi.begin(secrets.wifiSsid, secrets.wifiPassword);

  //check wi-fi is connected to wi-fi network
  int retries = 5;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    retries--;
    if (retries <= 0)
    {
      ESP.restart();
    }
  }
  Serial.print(" connected!");
  Serial.print(" (IP=");
  Serial.print(WiFi.localIP());
  Serial.println(")");
  initStage++;  


  // Power-On Motion Sensor
  pinMode(BNO055_PIN_VCC, OUTPUT);
  digitalWrite(BNO055_PIN_VCC, HIGH);
  delay(500); // wait for power up. Takes around 400ms (T_Sup).
  initStage++;  

  // Initialize Environment Sensor
  if ( I2CBME.begin(BNO055_PIN_I2C_SDA, BNO055_PIN_I2C_SCL, 250000U) ) // set I2C-Clock to 250kHz
  {
    initStage++;
    if (bno.begin(bno.OPERATION_MODE_NDOF)) // use custom Wire-Instance to avoid interference with other libraries.
    {
      initStage++;
      orientationSensorAvailable = true;
      Serial.println("[  INIT  ] found BNO055 Intelligent Absolute Orientation Sensor");
    }
    else
    {
      Serial.println("[ ERROR  ] Could not find a BNO055 sensor. Check wiring!");
    }
  }
  else
  {
    Serial.println("[ ERROR  ] Could not setup I2C Interface!");
  }


  if(orientationSensorAvailable)  
  {
    initStage++;

    sensor_t sensor;
    bno.getSensor(&sensor);
    Serial.print  ("           Sensor:        "); Serial.println(sensor.name);
    Serial.print  ("           Driver Ver:    "); Serial.println(sensor.version);
    Serial.print  ("           Unique ID:     "); Serial.println(sensor.sensor_id);

    /* Get the system status values (mostly for debugging purposes) */
    uint8_t system_status, self_test_results, system_error;
    system_status = self_test_results = system_error = 0;
    bno.getSystemStatus(&system_status, &self_test_results, &system_error);

    Serial.print("           System Status: 0x"); Serial.println(system_status, HEX);
    Serial.print("           Self Test:     0x"); Serial.println(self_test_results, HEX);
    Serial.print("           System Error:  0x"); Serial.println(system_error, HEX);
  }

  Serial.printf("[  INIT  ] Completed at stage %u\n\n", initStage);  
}

void loop() {
  // 100ms Tasks
  if (!(counterBase % (100L / SCHEDULER_MAIN_LOOP_MS)))
  {
    digitalWrite(LED_BUILTIN, HIGH); // regularly turn on LED
  }

  // 500ms Tasks
  if (!(counterBase % (500L / SCHEDULER_MAIN_LOOP_MS)))
  {
  }

  // 2s Tasks
  if (!(counterBase % (2000L / SCHEDULER_MAIN_LOOP_MS)))
  {
    // indicate alive
    digitalWrite(LED_BUILTIN, LOW);

    counter2s++;
  }

  // 30s Tasks
  if (!(counterBase % (30000L / SCHEDULER_MAIN_LOOP_MS)))
  {
    if (orientationSensorAvailable)
    {
    }

    // memory state
    Serial.printf("[ STATUS ] Free: %u KiB (%u KiB)  RSSI:%i dBm  Uptime: %" PRIi64 "s\n",
                  ESP.getFreeHeap() / 1024,
                  ESP.getMaxAllocHeap() / 1024,
                  WiFi.RSSI(),
                  (esp_timer_get_time() / 1000000LL));
  }

  // 300s Tasks
  if (!(counterBase % (300000L / SCHEDULER_MAIN_LOOP_MS)))
  {
    counter300s++;
  }

  // 1h Tasks
  if (!(counterBase % (3600000L / SCHEDULER_MAIN_LOOP_MS)))
  {
    counter1h++;
  }

  delay(SCHEDULER_MAIN_LOOP_MS);
  counterBase++;  
}