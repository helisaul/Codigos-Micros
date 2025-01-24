#include <WiFi.h>
#include <WebServer.h>
#include <ESP_Mail_Client.h>
#include <EEPROM.h>

// Configuración de red WiFi
const char *apSSID = "ESP32-Config";
const char *apPassword = "123456789";
WebServer server(80);  // Servidor web en el puerto 80

String wifiSSID;
String wifiPassword;
String recipientEmails;
int humedad;
int sensorPin = 35; // GPIO 35
// Configuración EEPROM
#define EEPROM_SIZE 512  // Tamaño de la EEPROM (en bytes)
#define SSID_ADDR 0
#define PASS_ADDR 100
#define EMAIL_ADDR 200

/////////////
const int boton = 34; // Botón asignado en el pin 4.
int anterior;         // Guardamos el estado anterior.
int estado;           // El estado del botón.
unsigned long temporizador;
unsigned long tiemporebote = 50;
////////////

// Configuración del cliente de correo
#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465
#define sender_email "helivas90@gmail.com"
#define sender_password "yxkmqnwfnbjnagpl"

SMTPSession smtp;  // Sesión SMTP para el envío de correos

// Pin para el LED
const int ledPin = 2;  // Puedes cambiar el número de pin según tu conexión

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  // Inicializar EEPROM
  EEPROM.begin(EEPROM_SIZE);
  pinMode(boton, INPUT_PULLUP);
  estado = HIGH;      // Suponiendo que el botón está no presionado
  anterior = HIGH;    // Estado anterior también no presionado
  // Inicializar el pin del LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Asegurarse de que el LED esté apagado al inicio

  // Leer datos almacenados en la EEPROM
  wifiSSID = readStringFromEEPROM(SSID_ADDR);
  wifiPassword = readStringFromEEPROM(PASS_ADDR);
  recipientEmails = readStringFromEEPROM(EMAIL_ADDR);

  // Conectar automáticamente si hay datos guardados
  if (wifiSSID.length() > 0 && wifiPassword.length() > 0) {
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    Serial.println("Conectando a WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("Conectado a la red WiFi");
    
    // Encender el LED al conectarse exitosamente
    digitalWrite(ledPin, HIGH);  // Enciende el LED
    
  } else {
    // Iniciar punto de acceso si no hay datos de WiFi guardados
    WiFi.softAP(apSSID, apPassword);
    Serial.println("Punto de acceso iniciado");
    Serial.print("IP de AP: ");
    Serial.println(WiFi.softAPIP());
  }

  // Iniciar servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/add-email", HTTP_POST, handleAddEmail); // Maneja agregar nuevos correos
  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();


  /////////////////si se preciona borroa la eprom//////////////////////
    int lectura = digitalRead(boton);

  // Si el estado es igual a lo leído, la entrada no ha cambiado
  if (estado == lectura) {
    temporizador = 0;  // Reiniciar el temporizador
  } 
  else if (temporizador == 0) {
    temporizador = millis();  // Guardar el tiempo de cambio
  } 
  else if (millis() - temporizador > tiemporebote) {
    estado = lectura; // Actualizar el estado del botón
  }

  // Imprimir si el botón fue pulsado
  if (anterior == HIGH && estado == LOW) {
    Serial.println("Botón pulsado");
    Reseteo();
  }

  // Guardar el estado anterior
  anterior = estado;
  //delay(1000);
  // Opción de añadir un pequeño retardo
  delay(10); // Opcional

/////////////////////////////////////////////

  humedad = readhumedad();
  if (humedad > 0 && humedad < 3000) {  // 
    Serial.print("Se ha detactado ");
    Serial.println("humedad  ");
     Serial.print( humedad);
    
    
    // Enviar correo si se detecta un objeto
    sendEmail();
    
    delay(1000);  // Espera 10 segundos para evitar múltiples correos por lecturas continuas
  }
}

int readhumedad() {
  int sensorValue = analogRead(sensorPin);
  // Mostrar el valor crudo por el monitor serial
 
  return sensorValue;
}
///////////////////////funcion para resetrar la eprom////////////////////////////////////////////////
void Reseteo(){

// Borrar EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);  // Escribir 0 en cada dirección
  }
  EEPROM.commit();  // Guardar los cambios

  Serial.println("EEPROM reseteada.");
}


////////////////////////////////////////////////////////////////

// Página de configuración de WiFi
void handleRoot() {
  String html = "<!DOCTYPE html>"
                "<html lang='es'>"
                "<head>"
                "<meta charset='UTF-8'>"
                "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC' crossorigin='anonymous'>"
                "</head>"
                "<body class='bg-light'>"
                "<div class='container min-vh-100 d-flex justify-content-center align-items-center'>"
                "<div class='card'>"
                "<svg version='1.1' id='Layer_1' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' x='0px' y='0px' width='72px' height='72px' viewBox='0 0 108.13 122.88' style='enable-background:new 0 0 108.13 122.88' xml:space='preserve'><style type='text/css'><![CDATA[.st0{fill-rule:evenodd;clip-rule:evenodd;fill:#A1E367;}.st1{fill:#4CA71E;}.st2{fill:#89BF3F;}.st3{fill-rule:evenodd;clip-rule:evenodd;}.st4{fill-rule:evenodd;clip-rule:evenodd;fill:#2394E0;}]]></style><g><path class='st4' d='M55.13,0c28.11,0,50.9,22.79,50.9,50.9c0,0.17,0,0.35-0.01,0.52c-1.27-0.52-2.73-0.61-4.14-0.13 c-0.52,0.17-0.99,0.41-1.42,0.71c-3.41,2.18-7.44,4.2-11.49,6.21c-17.06,8.52-34.26,17.11-28.89,43.35 c-1.63,0.16-3.29,0.24-4.96,0.24c-2.39,0-4.74-0.17-7.04-0.49c5.21-26.05-11.94-34.61-28.94-43.1c-4.04-2.02-8.08-4.03-11.48-6.21 c-0.43-0.29-0.91-0.53-1.42-0.71c-0.66-0.23-1.34-0.32-2-0.31l0-0.07C4.23,22.79,27.03,0,55.13,0L55.13,0L55.13,0z'/><path class='st0' d='M13.97,77.2c-3-4.55-5.25-9.64-6.56-15.1l8.96,4.32l0.05,2.67c0,0.98-1.68,3.07-2.21,3.87L13.97,77.2 L13.97,77.2L13.97,77.2z M69.11,5.38c17.08,5.68,29.9,20.71,32.37,39.02l-1.62-0.17c-0.29,1.24-0.55,1.27-0.55,2.76 c0,1.27,1.55,2.14,1.65,4.7c-0.17,0.09-0.33,0.19-0.49,0.3c-0.72,0.46-1.48,0.92-2.25,1.37l-1.67-1.95v2.9l-0.46,0.25l-0.63-6.43 l-1.45,0.46l-1.72-5.31L86.6,49.2l-0.07,4.34l-1.85,1.24l-1.97-11.13l-1.18,0.86l-2.67-3.6l-3.99,0.12l-1.53-1.75l-1.56,0.43 l-3.08-3.52l-0.59,0.4l1.91,4.87l2.21,0v-1.11l1.11,0c0.8,2.2,1.66,0.9,1.66,2.21c0,4.6-5.68,7.97-9.39,8.84 c0.2,0.83,0.12,1.66,1.1,1.66c2.08,0,1.05-0.36,3.32-0.55c-0.11,4.7-5.38,10.3-7.64,13.8l1.01,7.2c0.27,1.56-3.25,3.22-4.44,4.98 l0.57,2.76l-1.62,0.65c-0.28,2.83-3.03,5.98-6.12,5.98h-3.31c0-3.88-2.76-9.42-2.76-12.16c0-2.33,1.1-2.64,1.1-5.53 c0-2.66-2.76-6.48-2.76-7.18v-4.42h-2.21c-0.33-1.23-0.13-1.66-1.66-1.66h-0.55c-2.41,0-2.01,1.1-4.42,1.1H33 c-1.99,0-5.52-6.4-5.52-7.18v-6.63c0-2.86,2.62-5.98,4.42-7.19v-2.76l2.49-2.53l1.38-0.23c2.96,0,2.61-1.66,4.42-1.66l4.97,0v3.87 l5.47,2.33l0.52-2.36c2.48,0.58,3.12,1.68,6.18,1.68h1.1c2.1,0,2.21-2.78,2.21-4.97l-4.43,0.44l-1.93-4.2l-1.91,0.51 c0.34,1.5,0.53,0.88,0.53,2.14c0,0.75-0.61,0.83-1.11,1.1l-1.92-4.86l-4.12-2.94l-0.55,0.54l3.51,3.69 c-0.47,1.32-0.52,5.14-2.45,2.47l1.81-0.87l-4.51-4.72l-2.7,1.06l-2.66,2.55c-0.28,2.06-0.84,3.09-2.99,3.09 c-1.43,0-0.57-0.37-2.76-0.55v-5.52h4.97l-1.61-3.68l-0.6,0.36v-1.11l8.07-3.72c-0.15-1.16-0.34-0.54-0.34-1.8 c0-0.08,0.54-1.1,0.55-1.11l2.09,1.3l-0.5-2.38l-3.22,0.66l-0.6-2.89c2.55-1.35,8.18-6.08,9.96-6.08l1.66,0 c1.75,0,6.42,1.72,7.18,2.76l-4.43-0.45l3.29,2.71l0.32-1.16l2.46-0.67l0.03-1.54h1.11v1.66L69.11,5.38L69.11,5.38L69.11,5.38z'/><path class='st3' d='M76.14,91.66c-0.12,0.37-0.24,0.73-0.37,1.09h-0.4v-1.09H76.14L76.14,91.66L76.14,91.66z'/><path class='st3' d='M77.71,85.47c-0.13,0.69-0.26,1.37-0.42,2.05l-0.25-1.76L77.71,85.47L77.71,85.47L77.71,85.47z'/><path class='st2' d='M69.05,110.55c-1.49,3.46-2.78,6.36-3.12,9.41c-0.33,2.97-0.43,4.32,1.14,0.93c1.47-3.15,2.82-6.2,5.11-9.32 c0.21,0,0.45-0.01,0.68-0.02c27.37-1.18,43.52-17.55,30.92-54.69c-18.4,11.86-47.56,15.97-35.92,50.92 C68.3,109.08,68.62,109.97,69.05,110.55L69.05,110.55L69.05,110.55L69.05,110.55z M73.99,103.91 c3.24-16.37,19.54-25.68,25.79-37.84C94.83,86.74,87.35,89.52,73.99,103.91L73.99,103.91L73.99,103.91z'/><path class='st1' d='M73.99,103.91c3.24-16.37,19.54-25.68,25.79-37.84C94.83,86.74,87.35,89.52,73.99,103.91L73.99,103.91 L73.99,103.91z'/><path class='st2' d='M39.08,110.55c1.49,3.46,2.78,6.36,3.12,9.41c0.33,2.97,0.43,4.32-1.14,0.93c-1.47-3.15-2.82-6.2-5.11-9.32 c-0.21,0-0.45-0.01-0.68-0.02C7.89,110.38-8.25,94,4.34,56.87c18.4,11.86,47.56,15.97,35.92,50.92 C39.83,109.08,39.5,109.97,39.08,110.55L39.08,110.55L39.08,110.55L39.08,110.55z M34.14,103.91C30.9,87.55,14.6,78.24,8.35,66.07 C13.29,86.74,20.78,89.52,34.14,103.91L34.14,103.91L34.14,103.91z'/><path class='st1' d='M34.14,103.91C30.9,87.55,14.6,78.24,8.35,66.07C13.29,86.74,20.78,89.52,34.14,103.91L34.14,103.91 L34.14,103.91z'/></g></svg>"
                
                "<div class='card-header text-center'>"
                "<h1>Configura tu WiFi y Correo</h1>"
                "</div>"

                 "<div class='card-body'>"
                "<form action='/save' method='POST'>"
                "<div class='mb-3'>"
                "<label for='ssid' class='form-label'>Nombre de la red WiFi (SSID)</label>"
                "<input type='text' class='form-control' id='ssid' name='ssid' required>"
                "</div>"
                "<div class='mb-3'>"
                "<label for='password' class='form-label'>Contraseña de la red WiFi</label>"
                "<input type='password' class='form-control' id='password' name='password' required>"
                "</div>"
                "<div class='mb-3'>"
                "<label for='email' class='form-label'>Correo del destinatario</label>"
                "<input type='text' class='form-control' id='email' name='email' value='" + recipientEmails + "' required>"
                "</div>"
                "<input type='submit' class='btn btn-danger' value='Guardar'>"
                "</form>"
                "<form action='/add-email' method='POST' style='margin-top: 20px;'>"
                "<div class='mb-3'>"
                "<label for='new_email' class='form-label'>Agregar otro correo</label>"
                "<input type='text' class='form-control' id='new_email' name='new_email'>"
                "</div>"
                "<input type='submit' class='btn btn-warning' value='Agregar Correo'>"
                "</form>"
                "</div>"
                "</div>"
                "</div>"
                "</body>"
                "</html>";
  server.send(200, "text/html", html);
}

// Guardar credenciales en EEPROM
void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("email")) {
    wifiSSID = server.arg("ssid");
    wifiPassword = server.arg("password");
    recipientEmails = server.arg("email");
    
    // Guardar datos en EEPROM
    writeStringToEEPROM(SSID_ADDR, wifiSSID);
    writeStringToEEPROM(PASS_ADDR, wifiPassword);
    writeStringToEEPROM(EMAIL_ADDR, recipientEmails);
    EEPROM.commit();
    
    // Conectar a la red WiFi
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    Serial.println("Conectando a WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("Conectado a la red WiFi");
    
    // Encender el LED al conectarse exitosamente
    digitalWrite(ledPin, HIGH);  // Enciende el LED
    
    server.send(200, "text/html", "<html><body><h1>Configuración guardada. Conectado a WiFi.</h1></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: Faltan parámetros.</h1></body></html>");
  }
}

// Manejar agregar un nuevo correo electrónico
void handleAddEmail() {
  if (server.hasArg("new_email")) {
    String newEmail = server.arg("new_email");

    // Validar el formato del correo electrónico
    if (!isValidEmail(newEmail)) {
      server.send(400, "text/html", "<html><body><h1>Error: Correo no válido.</h1></body></html>");
      return;
    }

    // Agregar el nuevo correo a la lista existente
    if (recipientEmails.length() > 0) {
      recipientEmails += ",";  // Agregar una coma si ya hay correos
    }
    recipientEmails += newEmail;

    // Guardar el correo actualizado en la EEPROM
    writeStringToEEPROM(EMAIL_ADDR, recipientEmails);
    EEPROM.commit();

    server.send(200, "text/html", "<html><body><h1>Correo agregado exitosamente.</h1></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: Correo no válido.</h1></body></html>");
  }
}

// Función para validar el formato del correo electrónico
bool isValidEmail(const String &email) {
  int atIndex = email.indexOf('@');
  int dotIndex = email.lastIndexOf('.');
  return (atIndex > 0 && dotIndex > atIndex);
}

// Función para escribir una cadena en la EEPROM
void writeStringToEEPROM(int addr, const String &data) {
  for (int i = 0; i < data.length(); ++i) {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + data.length(), '\0');  // Termina con el carácter nulo
}

// Función para leer una cadena desde la EEPROM
String readStringFromEEPROM(int addr) {
  String data;
  char c;
  for (int i = addr; (c = EEPROM.read(i)) != '\0'; ++i) {
    data += c;
  }
  return data;
}

void sendEmail() {
  // Configurar el servidor SMTP y las credenciales
  ESP_Mail_Session session;
  session.server.host_name = SMTP_server;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  // Declarar el mensaje de correo
  SMTP_Message message;
  message.sender.name = "Compostador";
  message.sender.email = sender_email;
  message.subject = "Alerta Compostaje Liquido";
  
  // Imprimir el correo a enviar para depuración
  Serial.print("Correo a enviar: ");
  Serial.println(recipientEmails);

  // Separar los correos por comas y agregar cada uno como destinatario
  int start = 0;
  int end = recipientEmails.indexOf(',');
  while (end > 0) {
    message.addRecipient("", recipientEmails.substring(start, end).c_str()); // Agregar destinatario
    start = end + 1;
    end = recipientEmails.indexOf(',', start);
  }
  message.addRecipient("", recipientEmails.substring(start).c_str()); // Agregar el último destinatario

  // Contenido del mensaje en HTML
  String htmlMsg = "<div style=\"color:#000000;\"><h1>Compostaje</h1>"
                   "<p>Su fertilizante liquido esta en optimas condiciones para ser usada !! .</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Conectar al servidor SMTP y enviar el correo
  if (!smtp.connect(&session)) {
    Serial.println("Error conectando al servidor SMTP.");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error enviando el correo: " + smtp.errorReason());
  } else {
    Serial.println("Correo enviado correctamente.");
  }

  // Limpiar los datos del correo después de enviar
  smtp.closeSession();
}
