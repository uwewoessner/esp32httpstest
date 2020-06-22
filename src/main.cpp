#include <HTTPClient.h>

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <WiFiClientSecure.h>

#include "../../wifiPasswd.h"
//const char* WIFI_SSID = "yourSSID";
//const char* WIFI_PASS = "yourPassword";

// This is GandiStandardSSLCA2.pem, the root Certificate Authority that signed 
// the server certifcate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Sep 11 23:59:59 2024 GMT
const char *rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDwzCCAqugAwIBAgIBATANBgkqhkiG9w0BAQsFADCBgjELMAkGA1UEBhMCREUx\n" \
"KzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnByaXNlIFNlcnZpY2VzIEdtYkgxHzAd\n" \
"BgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50ZXIxJTAjBgNVBAMMHFQtVGVsZVNl\n" \
"YyBHbG9iYWxSb290IENsYXNzIDIwHhcNMDgxMDAxMTA0MDE0WhcNMzMxMDAxMjM1\n" \
"OTU5WjCBgjELMAkGA1UEBhMCREUxKzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnBy\n" \
"aXNlIFNlcnZpY2VzIEdtYkgxHzAdBgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50\n" \
"ZXIxJTAjBgNVBAMMHFQtVGVsZVNlYyBHbG9iYWxSb290IENsYXNzIDIwggEiMA0G\n" \
"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCqX9obX+hzkeXaXPSi5kfl82hVYAUd\n" \
"AqSzm1nzHoqvNK38DcLZSBnuaY/JIPwhqgcZ7bBcrGXHX+0CfHt8LRvWurmAwhiC\n" \
"FoT6ZrAIxlQjgeTNuUk/9k9uN0goOA/FvudocP05l03Sx5iRUKrERLMjfTlH6VJi\n" \
"1hKTXrcxlkIF+3anHqP1wvzpesVsqXFP6st4vGCvx9702cu+fjOlbpSD8DT6Iavq\n" \
"jnKgP6TeMFvvhk1qlVtDRKgQFRzlAVfFmPHmBiiRqiDFt1MmUUOyCxGVWOHAD3bZ\n" \
"wI18gfNycJ5v/hqO2V81xrJvNHy+SE/iWjnX2J14np+GPgNeGYtEotXHAgMBAAGj\n" \
"QjBAMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgEGMB0GA1UdDgQWBBS/\n" \
"WSA2AHmgoCJrjNXyYdK4LMuCSjANBgkqhkiG9w0BAQsFAAOCAQEAMQOiYQsfdOhy\n" \
"NsZt+U2e+iKo4YFWz827n+qrkRk4r6p8FU3ztqONpfSO9kSpp+ghla0+AGIWiPAC\n" \
"uvxhI+YzmzB6azZie60EI4RYZeLbK4rnJVM3YlNfvNoBYimipidx5joifsFvHZVw\n" \
"IEoHNN/q/xWA5brXethbdXwFeilHfkCoMRN3zUA7tFFHei4R40cR3p1m0IvVVGb6\n" \
"g1XqfMIpiRvpb7PO4gWEyS8+eIVibslfwXhjdFjASBgMmTnrpMwatXlajRWc2BQN\n" \
"9noHV8cigwUtPJslJj0Ys6lDfMjIq2SPDqO/nBudMNva0Bkuqjzx+zOAduTNrRlP\n" \
"BSeOE6Fuwg==\n" \
"-----END CERTIFICATE-----\n";

const char* xrootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIID0zCCArugAwIBAgIUMUUiUhY8h2tekOGdS7uVQHCh2ZEwDQYJKoZIhvcNAQEL\n" \
"BQAweTELMAkGA1UEBhMCREUxCzAJBgNVBAgMAkJXMQ0wCwYDVQQHDARTVEdUMQ0w\n" \
"CwYDVQQKDARITFJTMQwwCgYDVQQLDAN2aXMxEDAOBgNVBAMMB2hscnMuZGUxHzAd\n" \
"BgkqhkiG9w0BCQEWEHdvZXNzbmVyQGhscnMuZGUwHhcNMjAwNDA3MjIzNzUxWhcN\n" \
"MjIwNzExMjIzNzUxWjB5MQswCQYDVQQGEwJERTELMAkGA1UECAwCQlcxDTALBgNV\n" \
"BAcMBFNUR1QxDTALBgNVBAoMBEhMUlMxDDAKBgNVBAsMA3ZpczEQMA4GA1UEAwwH\n" \
"aGxycy5kZTEfMB0GCSqGSIb3DQEJARYQd29lc3NuZXJAaGxycy5kZTCCASIwDQYJ\n" \
"KoZIhvcNAQEBBQADggEPADCCAQoCggEBALUGlJFRbC4PNQFfFUXsg1m5HvFRgbQp\n" \
"fbtUG0BxyJ0fWcaQxF08O6VXDc6cHMZ874vsm+SzK6X0LddBl51vyqHtDAOR3AbD\n" \
"WigP8fjY/0iZNA3I9qsPiqHeSXHAl0K+Q/1wY8VP349i56hBXLdrSORTCbfykgY5\n" \
"heKp1PRRXIukViQ2bYMO+3zxOrO6ygK2/k+fujb/gFORs3tQoqlRwo5nRuDL8XSh\n" \
"5hQfSRoS5GHjehNWmQ4CxvHxKMcI8h2HrvExr5oNBCHwjVKhHVCzBG9DlTJn65Gs\n" \
"Rh3oQ5qFXE6FgRc+Yr2m9K/z9/dqAO9Ys9WFDORNU+P+HVhVYPBMyRUCAwEAAaNT\n" \
"MFEwHQYDVR0OBBYEFEoWvXkBnO6Tb32cNd1apTeniZfgMB8GA1UdIwQYMBaAFEoW\n" \
"vXkBnO6Tb32cNd1apTeniZfgMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\n" \
"BQADggEBAGZ8zRbBCdOSE+HVQyus9bT/lN3uh6cx5/NI72LD8kf/nSl7jX73Lc1O\n" \
"OZuhdbixsqOnc43XOGrmknjCUr3wzjHeSHw40KiXhmvxDb3KJLuYu8LuOLTg6yo2\n" \
"pHS3GFULuzgLZT5kkeLAZzz6Y8u3fW2vNILyuyaxVEBvroJUP6TGLfs9reesx0CR\n" \
"HqO0L3iYqQY2Fy4UNGdvwIQudiq2tD2vAGUvZysBv+N9qyaiQ9dMQIvz7rQy0aQz\n" \
"M9nAQMoRiYZ2Kan5DCCKxvOD9m4ro2oUnL/bK/azK1GMXtsUPzQ3ql2whRVTuGAF\n" \
"LjLasXYh7YfQzG10JMVK30TwWHARaY8=\n" \
"-----END CERTIFICATE-----\n";

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
  Serial.print(F("Waiting for NTP time sync done: "));
}


WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
    
    delay(500);
    //ESP.reset();
  }
  Serial.println(" connected");

  setClock();  
}


void loop() {
      Serial.print("WiFiClientSecure\n");
  WiFiClientSecure *client = new WiFiClientSecure;
      Serial.print("WiFiClientSecure2\n");
  if(client) {
    client -> setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      //char JSONMessage[] = " {\"id\": \"5e8f2f43e7e3b3668ca13151\", \"title\": \"esp32\", \"description\": \"esptest\"}"; 
      
      Serial.print("client ready\n");
char JSONMessage[] = "{\"id\": \"5e8f2f43e7e3b3668ca13151\",\"track\":{\"title\":\"esp32\",\"description\":\"created by an esp32\",\"body\":\"\
06.02.2020;07:01:44;0.000000;0.000000;255;255;0;\
06.02.2020;07:01:44;0.000000;0.000000;255;49;0;\
06.02.2020;07:01:45;0.000000;0.000000;255;49;0;\
06.02.2020;07:01:47;0.000000;0.000000;255;49;0;\
06.02.2020;07:01:48;0.000000;0.000000;255;49;0;\
06.02.2020;07:34:38;48.842174;9.709690;227;255;0;\
06.02.2020;07:34:39;48.842136;9.709634;235;255;0;\
06.02.2020;07:34:40;48.842099;9.709570;255;255;0;\
06.02.2020;07:34:41;48.842070;9.709492;255;255;0;\
06.02.2020;07:34:42;48.842048;9.709409;151;255;0;\
06.02.2020;07:34:43;48.842030;9.709318;255;255;0;\
06.02.2020;07:34:44;48.842013;9.709227;136;255;0;\
06.02.2020;07:34:45;48.841993;9.709136;255;216;1;\
06.02.2020;07:34:46;48.841974;9.709052;255;222;0;\
06.02.2020;07:34:47;48.841957;9.708963;255;255;0;\
06.02.2020;07:34:48;48.841940;9.708875;122;255;0;\
06.02.2020;07:43:38;48.847508;9.720731;255;176;0;\
06.02.2020;07:43:39;48.847507;9.720731;255;255;0;\
06.02.2020;07:43:40;48.847511;9.720733;255;255;0;\
06.02.2020;07:43:41;48.847515;9.720736;255;255;0;\
06.02.2020;07:43:42;48.847517;9.720738;255;255;0;\
06.02.2020;07:43:43;48.847515;9.720738;255;3;1;\
06.02.2020;07:43:44;48.847518;9.720740;255;255;0;\
06.02.2020;07:43:45;48.847517;9.720738;255;255;0;\
06.02.2020;07:43:46;48.847519;9.720740;255;255;0;\
06.02.2020;07:43:47;48.847521;9.720739;255;255;0;\
06.02.2020;07:43:48;48.847509;9.720741;255;40;0;\
06.02.2020;07:43:49;48.847505;9.720743;255;149;0;\
\"}}";
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, "https://openbikesensor.hlrs.de/api/tracks/add")) {  // HTTPS
        Serial.print("[HTTPS] POST...\n");
        https.addHeader("Content-Type", "application/json"); 
      Serial.print("POST2\n");
        int httpCode = https.POST(JSONMessage);
      Serial.print("POST3\n");
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
         /* if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }*/
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  Serial.println("Waiting endlessly ...");
   while(true)
   {
       delay(1000);
   }
}
