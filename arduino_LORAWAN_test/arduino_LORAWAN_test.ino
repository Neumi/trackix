#include "LowPower.h"
#include <lmic.h>
#include <hal/hal.h>

static const u1_t NWKSKEY[16] = { };
static const u1_t APPSKEY[16] = { };
static const u4_t DEVADDR = 0x;
const unsigned TX_INTERVAL = 60;


void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;


// tracktick bike tracker 2 pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 6,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 5,
  .dio = {2, 3, LMIC_UNUSED_PIN},
};

void onEvent (ev_t ev) {
  if (ev == EV_TXCOMPLETE) {
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
  }
}

void do_send(osjob_t* j) {
  static uint8_t message[] = "tracktix2 test";
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println("OP_TXRXPEND, not sending");
  } else {
    LMIC_setTxData2(1, message, sizeof(message) - 1, 0);
    Serial.println("Sending uplink packet...");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  Serial.println("os_init");
  os_init();
  Serial.println("LMIC_reset");
  LMIC_reset();
  Serial.println("LMIC_setSession");
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
  Serial.println("LMIC_setLinkCheckMode");
  LMIC_setLinkCheckMode(0);
  Serial.println("DR_SF9");
  LMIC.dn2Dr = DR_SF9;
  Serial.println("LMIC_setDrTxpow");
  LMIC_setDrTxpow(DR_SF12, 14);
  do_send(&sendjob);
  Serial.println("do send");
}

void loop() {
  os_runloop_once();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  asm volatile ("  jmp 0");
}
