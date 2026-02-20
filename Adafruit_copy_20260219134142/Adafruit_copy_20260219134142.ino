/************************** Configuration ***********************************/
#include "config.h"

/************************** Feeds ***********************************/
AdafruitIO_Feed *temperaturaFeed = io.feed("temperatura");
AdafruitIO_Feed *puertaFeed      = io.feed("puerta");
AdafruitIO_Feed *elevadorFeed    = io.feed("elevador");   // acceso
AdafruitIO_Feed *comandoFeed     = io.feed("comando");

/************************** UART ***********************************/
#define RXD2 16
#define TXD2 17

/************************** Timing ***********************************/
#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;

/************************** Variables ***********************************/
int ultimaTemperatura = 0;
int ultimoEstadoPuerta = 0;
int ultimoEstadoElevador = 0;

void setup() {

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  while(!Serial);

  Serial.print("Connecting to Adafruit IO");
  io.connect();

  comandoFeed->onMessage(handleComando);

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  comandoFeed->get();
}

void loop() {

  io.run();

  // -------- Leer datos del Nano --------
  if (Serial2.available()) {

    String dato = Serial2.readStringUntil('\n');
    dato.trim();

    if (dato.startsWith("T:"))
      ultimaTemperatura = dato.substring(2).toInt();

    else if (dato.startsWith("P:"))
      ultimoEstadoPuerta = dato.substring(2).toInt();

    else if (dato.startsWith("A:"))
      ultimoEstadoElevador = dato.substring(2).toInt();
  }

  // -------- Publicar cada 2 segundos --------
  if (millis() - lastUpdate > IO_LOOP_DELAY) {

    temperaturaFeed->save(ultimaTemperatura);
    puertaFeed->save(ultimoEstadoPuerta);
    elevadorFeed->save(ultimoEstadoElevador);

    Serial.println("Datos enviados a Adafruit");

    lastUpdate = millis();
  }
}

/************************** Callback de Comando ***********************************/
void handleComando(AdafruitIO_Data *data) {

  String comando = data->value();

  Serial.print("Comando recibido: ");
  Serial.println(comando);

  if (comando == "RESET")
    Serial2.println("R");

  if (comando == "FORZAR")
    Serial2.println("F");
}