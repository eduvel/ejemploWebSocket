#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncWebSocket.h>
#include <FS.h>


const char* apSsid = "ESP8266_AP"; // Nombre de la red WiFi del Access Point
const char* apPassword = "12345678"; // Contraseña del Access Point

const int serverPort = 80;
AsyncWebServer server(serverPort);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *singleClient;
// Función para manejar eventos del WebSocket
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  char dataReceived[30];
  if (type == WS_EVT_CONNECT) {
    Serial.println("Cliente WebSocket conectado");
    singleClient=client;
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Cliente WebSocket desconectado");
    singleClient=NULL;
  } else if (type == WS_EVT_DATA) {
    // Al recibir datos del cliente WebSocket
    // Enviamos el mensaje recibido de vuelta al cliente
    Serial.println("Mensaje recibido del cliente:");
    for (size_t i = 0; i < len; i++) {
      Serial.print((char)data[i]);
    }
    Serial.println();
    for (int i = 0; i < len; i++) {
        dataReceived[i] = (char)data[i];
    }
    
    Serial.println(dataReceived);
    // Enviar de vuelta el mensaje al cliente
    client->text(data, len);
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar el Access Point WiFi
  WiFi.softAP(apSsid, apPassword);

  Serial.println("Access Point WiFi establecido");
  Serial.print("Dirección IP del Access Point: ");
  Serial.println(WiFi.softAPIP());

  // Montar el sistema de archivos SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Error al montar el sistema de archivos SPIFFS");
    return;
  }

  // Agregar el evento WebSocket al servidor
  ws.onEvent(onWebSocketEvent);

  // Ruta para manejar la página de inicio (index.html)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Leer el archivo index.html desde SPIFFS
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      request->send(404, "text/plain", "Archivo no encontrado");
      return;
    }

    // Enviar el contenido de index.html como respuesta
    request->send(SPIFFS, "/index.html", "text/html");
    file.close();
  });

  // Ruta para manejar la solicitud WebSocket
  server.addHandler(&ws);

  // Iniciar el servidor
  server.begin();
}

void loop() {
  // No es necesario hacer nada en el bucle principal para manejar los eventos del WebSocket
  if(singleClient){
    singleClient->text("envio");
    Serial.println("envio");
  }
  delay(2000);
}
