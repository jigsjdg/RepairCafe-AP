#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Create a web server on port 80
ESP8266WebServer server(80);

// EEPROM addresses for storing credentials
#define EEPROM_SIZE 128
#define SSID_ADDR 0
#define PASS_ADDR 32

// Default AP credentials
String apSSID = "RepairCafe_AP";
String apPassword = "12345678";

// HTML forms
const char* configPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Access Point instellen</title> <!-- Configure Access Point -->
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    input { padding: 10px; margin: 5px; width: 300px; }
    button { padding: 10px; width: 150px; background-color: #4CAF50; color: white; border: none; }
  </style>
</head>
<body>
  <h1>RepairCafe Heemstede</h1> <!-- Give Title of page -->
  <h2>Wi-Fi Instellen</h2>.     <!-- Set up WiFi -->
  <form action="/set" method="GET">
    <input type="text" name="ssid" placeholder="Geef SSID" required><br>         <!-- Set SSID -->
    <input type="password" name="password" placeholder="Geef wachtwoord"><br>.   <!-- Set Password -->
    <button type="submit">Opslaan</button>                                       <!-- Save -->
  </form>
  <br><br>
  <form action="/delete" method="POST">
    <button type="submit" style="background-color: #f44336;">Verwijder Instellingen</button> <!-- Delete Credentials -->
  </form>
</body>
</html>
)rawliteral";

const char* deleteConfirmationPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Instellingen Verwijderd</title> <!-- Credentials Deleted -->
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    button { padding: 10px; width: 150px; background-color: #4CAF50; color: white; border: none; }
  </style>
</head>
<body>
  <h1>Instellingen Verwijderd!</h1> <!-- Credentials Deleted -->
  <p>De ESP8266 zal herstarten met de standaar instellingen.</p> <!-- ESP8266 will restart with default credentials -->
  <button onclick="window.location.href='/'">Terug naar begin</button> <!-- Back to Main -->
</body>
</html>
)rawliteral";

// Function to save credentials to EEPROM
void saveCredentials(const String& ssid, const String& password) {
  Serial.println("Saving credentials to EEPROM...");
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(PASS_ADDR + i, i < password.length() ? password[i] : 0);
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("Credentials saved!");
}

// Function to load credentials from EEPROM
bool loadCredentials(String& ssid, String& password) {
  Serial.println("Loading credentials from EEPROM...");
  EEPROM.begin(EEPROM_SIZE);
  char ssidBuff[32] = {0};
  char passBuff[32] = {0};

  for (int i = 0; i < 32; i++) {
    ssidBuff[i] = EEPROM.read(SSID_ADDR + i);
    passBuff[i] = EEPROM.read(PASS_ADDR + i);
  }
  EEPROM.end();

  ssid = String(ssidBuff);
  password = String(passBuff);

  Serial.print("Loaded SSID: ");
  Serial.println(ssid);
  Serial.print("Loaded Password: ");
  Serial.println(password);

  // Check if SSID is valid (non-empty)
  return ssid.length() > 0;
}

// Function to clear credentials from EEPROM
void clearCredentials() {
  Serial.println("Clearing credentials from EEPROM...");
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 64; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("Credentials cleared!");
}

// Function to handle the root page
void handleRoot() {
  server.send(200, "text/html", configPage);
}

// Function to handle saving credentials
void handleSet() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    apSSID = server.arg("ssid");
    apPassword = server.arg("password");

    saveCredentials(apSSID, apPassword);

    String response = "<h1>Configuratie bijgewerkt!</h1>";                         // Credentials Updated
    response += "<p>Nieuw SSID: " + apSSID + "</p>";                               // New SSID
    response += "<p>Nieuw Wachtwoord: " + apPassword + "</p>";                     // New Password
    response += "<p>Ik start opnieuw op om de wijzigingen door te voeren...</p>";  // Rebooting to apply changes
    server.send(200, "text/html", response);

    delay(3000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "<h1>Fout: Ontbrekend SSID of wachtwoord</h1>"); //Error: Missing SSID or Password
  }
}

// Function to handle deleting credentials from web pae
void handleDelete() {
  clearCredentials();
  server.send(200, "text/html", deleteConfirmationPage);
  delay(3000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);

  // Load credentials from EEPROM
  String savedSSID, savedPassword;
  bool hasCredentials = loadCredentials(savedSSID, savedPassword);

  if (hasCredentials) {
    // Start access point with saved credentials
    apSSID = savedSSID;
    apPassword = savedPassword;
    Serial.println("Starting access point with saved credentials...");
    Serial.println("Clearing saved credentials for next clean boot");
    clearCredentials();
  } else {
    // Start access point with default credentials
    Serial.println("No valid credentials found. Starting default access point...");
  }

  WiFi.softAP(apSSID.c_str(), apPassword.c_str());
  Serial.println("Access Point Started!");
  Serial.print("SSID: ");
  Serial.println(apSSID);
  Serial.print("Password: ");
  Serial.println(apPassword);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Set up server routes
  server.on("/", handleRoot);         // Serve the HTML form
  server.on("/set", handleSet);       // Handle form submission
  server.on("/delete", HTTP_POST, handleDelete); // Handle credential deletion
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}
