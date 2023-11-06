#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

#ifndef SSID
#define SSID "Find KingBain; Get Sticker"
#endif

const char *softAP_ssid = SSID;

// The access points IP address and net mask
// It uses the default Google DNS IP address 8.8.8.8 to capture all 
// Android dns requests
IPAddress apIP(8, 8, 8, 8);
IPAddress netMsk(255, 255, 255, 0);

// DNS server 
const byte DNS_PORT = 53; 
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

// check if this string is an IP address
boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

// checks if the request is for the controllers IP, if not we redirect automatically to the
// captive portal 
boolean captivePortal() {
  if (!isIp(server.hostHeader())) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   
    server.client().stop(); 
    return true;
  }
  return false;
}

void handleRoot() {
  if (captivePortal()) { 
    return;
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

String page = F(
  "<!DOCTYPE html>"
  "<html lang=\"en\">"
  "<head>"
  "    <meta charset=\"UTF-8\">"
  "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
  "    <title>Kingbain's Beacon</title>"
  "    <style>"
  "        body {"
  "            font-family: 'Courier New', monospace;"
  "            text-align: left;"
  "            padding: 5% 10%;"
  "            background-color: #000;"
  "            color: #33ff33;"
  "        }"
  "        h1 {"
  "            color: #33ff33;"
  "        }"
  "        p {"
  "            color: #33ff33;"
  "            line-height: 1.6;"
  "        }"
  "        a {"
  "            color: #33ff99;"
  "            text-decoration: none;"
  "        }"
  "        a:hover {"
  "            text-decoration: underline;"
  "        }"
  "        .tweet-button {"
  "            display: inline-block;"
  "            padding: 10px 20px;"
  "            background-color: #33ff33;"
  "            color: black;"
  "            border-radius: 5px;"
  "            font-weight: bold;"
  "            margin-top: 20px;"
  "        }"
  "        ul {"
  "            list-style-type: disc;"
  "            padding-left: 20px;"
  "        }"
  "        pre {"
  "            font-size: 3em; /* Adjust this value as needed */"
  "        }"
  "        #langButtons {"
  "            margin-bottom: 20px;"
  "        }"
  "        .langButton {"
  "            padding: 10px 15px;"
  "            margin-right: 10px;"
  "            cursor: pointer;"
  "            border: none;"
  "            background-color: #33ff33;"
  "            color: black;"
  "            border-radius: 5px;"
  "            font-weight: bold;"
  "        }"
  "        .langButton:focus {"
  "            outline: none;"
  "        }"
  "        /* Keyframes for fading effect */"
  "        @keyframes fadeInOut {"
  "            0% { opacity: 1; }"
  "            50% { opacity: 0; }"
  "            100% { opacity: 1; }"
  "        }"
  "        /* Apply animation to the period */"
  "        .fadePeriod {"
  "            animation: fadeInOut 2s infinite;"
  "        }"
  "    </style>"
  "</head>"
  "<body>"
  "    <script>"
  "window.setLanguage = function(lang) {"
  "    var elements = document.querySelectorAll('[data-french]');"
  "    elements.forEach(function(element) {"
  "        if(lang === 'fr') {"
  "            if (!element.dataset.original) {"
  "                element.dataset.original = element.innerHTML;"
  "            }"
  "            element.innerHTML = element.getAttribute('data-french');"
  "        } else {"
  "            if (element.dataset.original) {"
  "                element.innerHTML = element.dataset.original;"
  "            }"
  "        }"
  "    });"
  "};"
  ""
  "document.addEventListener('DOMContentLoaded', function() {"
  "    // This ensures that setLanguage function is called after the DOM is fully loaded."
  "    // Any initial setup that requires DOM to be ready can be done here."
  "});"
  "    </script>"
  "    <div id=\"langButtons\">"
  "        <button class=\"langButton\" onclick=\"setLanguage('en')\">English</button>"
  "        <button class=\"langButton\" onclick=\"setLanguage('fr')\">Français</button>"
  "    </div>"
  "    <h1 data-french=\"Bienvenue au phare de Kingbain<span class='fadePeriod'>.</span>\">Welcome to Kingbain's Beacon<span class='fadePeriod'>.</span></h1>"
  "    <pre>¯\\_(ツ)_/¯</pre>"
  "    <p data-french=\"Vous avez trouvé mon phare avec succès\">You've successfully found my beacon</p>"
  "    <p data-french=\"Vous voulez un autocollant ? Trouvez <strong>John Bain (alias Kingbain)</strong> à la conférence. Je suis ici quelque part.\">Want a sticker? Find <strong>John Bain (aka Kingbain)</strong> in the conference. I'm around here somewhere.</p>"
  "    <p data-french=\"Restez informé et suivez Kingbain sur différentes plateformes :\">Stay updated and follow Kingbain on various platforms:</p>"
  "    <ul>"
  "        <li><a href=\"https://twitter.com/JohnKingBain\" target=\"_blank\">@JohnKingBain on Twitter</a></li>"
  "        <li><a href=\"https://mastodon.social/@kingbain\" target=\"_blank\">Kingbain on Mastodon</a></li>"
  "        <li><a href=\"https://github.com/KingBain/ConferenceBeacon\" target=\"_blank\">Conference Beacon on GitHub</a></li>"

  "    </ul>"
  "    <a href=\"https://twitter.com/intent/tweet?text=Just%20connected%20to%20Kingbain's%20beacon%20at%20the%20%23fwd50%20conference!%20%F0%9F%8E%89%20%23FindKingbainGetSticker&url=https://twitter.com/JohnKingBain\" target=\"_blank\" class=\"tweet-button\" data-french=\"Tweetez votre connexion ! 🐦\">Tweet Your Connection! 🐦</a>"
  "    <p data-french=\"Tout cela est pour le plaisir et ne fournit pas d'accès internet. Déconnectez-vous après la visualisation pour économiser la batterie. 🙏🔋\">All this is for fun and does not provide internet access. Disconnect after viewing to save battery. 🙏🔋</p>"
  "</body>"
  "</html>"
  "<!--"
  "    Ah, curious soul, you've delved beneath the surface,"
  "    Where few choose to wander, you seek the purpose."
  "    In this digital realm, as in life, layers unfold,"
  "    What's reality? What stories remain untold?"
  ""
  "    Take a moment, reflect on the world you perceive,"
  "    Is it but a shadow, or something you truly believe?"
  "    As you journey onward, with questions anew,"
  "    Here's a beacon, a community, waiting for you:"
  ""
  "    Join the conversation: https://gcdevopsleague.slack.com/"
  "-->"
);




  server.send(200, "text/html", page);
}

void handleNotFound() {
  if (captivePortal()) { 
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.softAPConfig(apIP, apIP, netMsk);
  // its an open WLAN access point without a password parameter
  WiFi.softAP(softAP_ssid);
  delay(1000);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup the web server */
  server.on("/", handleRoot);
  server.on("/generate_204", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin(); // Web server start
  Serial.println("HTTP server started");

}

void loop() {
  // put your main code here, to run repeatedly:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
}
