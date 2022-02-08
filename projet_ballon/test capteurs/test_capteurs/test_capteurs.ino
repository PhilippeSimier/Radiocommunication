#include <LM75A.h>
#include "Adafruit_MAX31855.h"
#include <HardwareSerial.h>
#include <NeoPixelBus.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_err.h>



#define PIN_RGB     15
#define NUMPIXELS 1 // Popular NeoPixel ring size

//Routing ADC reference voltage to GPIO (esp32 deconnecté du pcb)
#define V_REF 1160  // ADC reference voltage obtenu par mesure https://demo-dijiudu.readthedocs.io/en/latest/api-reference/peripherals/adc.html
#define PIN_BATT ADC1_CHANNEL_3    //gpio 39
#define COEF_BATT 4.7169    //Diviseur de tension ajusté


#define PIN_LED 2
#define PIN_BP 34
#define MAXCS   4

LM75A lm75a_capteur(false,  //A0 8 capteur en tout (000 001...) capteur temperature interieur
                    false,  //A1 LM75A pin state
                    false); //A2 LM75A pin state

NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> strip(NUMPIXELS, PIN_RGB);
RgbColor rouge(8, 8, 0);



Adafruit_MAX31855 thermocouple(MAXCS);
HardwareSerial gps(1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_BATT, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BP, INPUT);
  digitalWrite(PIN_LED, LOW);
  gps.begin(4800, SERIAL_8N1, 16, 17);
  adc1_config_width(ADC_WIDTH_BIT_12);              //12 bits
  adc1_config_channel_atten(PIN_BATT, ADC_ATTEN_DB_11);  //full scale
  strip.Begin();
  strip.SetPixelColor(0, rouge);
  strip.Show();

  //mesure de vref : A utilser avec un esp32 deconnecté du pcb
  //redirige vref sur la sortie DAC pour mesure
/*
  esp_err_t status = adc2_vref_to_gpio(GPIO_NUM_25);
  if (status == ESP_OK) {
    printf("v_ref routed to GPIO\n");
  } else {
    printf("failed to route v_ref\n");
  }
*/

}

void loop() {
  testBatt();
  //testGps()
  //MesureLm75();
  //MesureThermocouple();
  delay(100);
}

void testBatt() {
  esp_adc_cal_characteristics_t characteristics;
  esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);

  // Read ADC and obtain result in mV
  uint32_t voltage = adc1_to_voltage(PIN_BATT, &characteristics);
  printf("%d mV  /  %f V\n", voltage, (float)voltage * COEF_BATT / 1000);
}


void testGps() {
  char car;
  if (gps.available() > 0) {
    car = gps.read();
    Serial.write(car);
  }
}

void testBpLed() {
  int level = digitalRead(PIN_BP);
  if (level == LOW) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("on");
  }
  else {
    digitalWrite(PIN_LED, LOW);
    Serial.println("off");
  }
}


void MesureThermocouple()
{
  double tempExt = (float) thermocouple.readCelsius();
  if (isnan(tempExt)) {
    Serial.println("Il y a un probleme!");
  } else {
    Serial.print("Thermocouple: ");
    Serial.println(tempExt);
  }
}


void MesureLm75()
{
  float tempInt = lm75a_capteur.getTemperatureInDegrees();
  if (tempInt == INVALID_LM75A_TEMPERATURE) {
    Serial.println("erreur pendant la prise de la température");
  } else {
    Serial.print("T int: ");
    Serial.println(tempInt);
  }
}
