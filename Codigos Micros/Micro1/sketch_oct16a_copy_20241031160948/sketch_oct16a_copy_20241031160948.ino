#include <IRremote.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pines para los módulos

#define ONE_WIRE_BUS 4 // Pin del bus para el DS18B20


#define LED_PIN 6
// Configuración para el emisor IR
IRsend irsend; // Crear el objeto IRsend

// Configuración para el sensor DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);

  // Inicia el sensor DS18B20
  sensors.begin();

  pinMode(LED_PIN , OUTPUT);
}

void loop() {
  // 1. Enviar un código IR repetidamente
  irsend.sendRC5(0x0, 8); // NEC protocol, 32 bits
  delay(100);
  irsend.sendRC5(0x1, 8); // NEC protocol, 32 bits
  delay(100); // Pausa de 1 segundo entre transmisiones
 
  // 2. Leer la temperatura del sensor DS18B20
  sensors.requestTemperatures(); // Envía comando para leer la temperatura
  float temperatureC = sensors.getTempCByIndex(0); // Lee la primera sonda (index 0)

  // Imprime la temperatura en el monitor serie
 


  if (temperatureC > 70.0){
      digitalWrite(LED_PIN, HIGH);
      //Serial.println(" Temperatura alta! LED ROJO encendido.");

  }else{

   digitalWrite(LED_PIN, LOW);

  }

  // Pausa para dar tiempo al sensor a realizar la siguiente lectura
  //delay(1000); // Pausa de 1 segundo para la siguiente lectura de temperatura
}

