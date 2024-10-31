#include <IRremote.h>

// Pines del sensor ultrasónico HC-SR04
const int trigPin = 2;
const int echoPin = 3;

// Pines de los LEDs
const int ledRojo = 5;  // LED rojo (lleno)
const int ledVerde = 6; // LED verde (disponible)

// Configuración para el receptor IR KY-022
int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;

long duration;
int distance;
bool medirDistancia = false; // Control para activar/desactivar el sensor ultrasónico

void setup() {
  Serial.begin(9600);

  // Inicializar el receptor IR
  irrecv.enableIRIn();

  // Configuración de los pines del sensor ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configurar los pines de los LEDs como salida
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // Asegurarse de que ambos LEDs estén apagados al inicio
  digitalWrite(ledRojo, LOW);
  digitalWrite(ledVerde, LOW);
}

void loop() {
  // Verificar si se ha recibido una señal IR
  if (irrecv.decode(&results)) {
    //Serial.print("Código IR recibido: ");
    //Serial.println(results.value, HEX);

    
    // Ejemplo: Si se recibe el código "0x1FE48B7" (un botón en tu control remoto)
    if (results.value == 1) {
              digitalWrite(trigPin, LOW);
              delayMicroseconds(2);
              digitalWrite(trigPin, HIGH);
              delayMicroseconds(10);
              digitalWrite(trigPin, LOW);

              // Leer la duración del pulso en el pin "echo"
              duration = pulseIn(echoPin, HIGH);

              // Calcular la distancia (duración / 2 * velocidad del sonido (34300 cm/s))
              distance = duration * 0.034 / 2;

              // Imprimir la distancia en el monitor serie
              Serial.print("Distancia: ");
              Serial.print(distance);
              Serial.println(" cm");

              // Encender/apagar LEDs según la distancia
              if (distance <= 10) {
                // Si la distancia es menor o igual a 10 cm, encender el LED rojo y apagar el verde
                digitalWrite(ledRojo, HIGH);
                digitalWrite(ledVerde, LOW);
                Serial.println("Estado: Lleno (LED rojo encendido)");
              } else {
                // Si la distancia es mayor a 10 cm, encender el LED verde y apagar el rojo
                digitalWrite(ledRojo, LOW);
                digitalWrite(ledVerde, HIGH);
                Serial.println("Estado: Disponible (LED verde encendido)");
              }
      
    }
    Serial.println(results.value);
    irrecv.resume(); // Prepararse para recibir el siguiente código IR
  }
  else if (!irrecv.decode(&results)){

                digitalWrite(ledRojo, LOW);
                digitalWrite(ledVerde, LOW);
    
  }
  delay(100);
  // Solo medir la distancia si "medirDistancia" es verdadero

}
