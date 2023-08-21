#ifndef RINCPOSTER_H
#define RINCPOSTER_H
#define RINCPOSTER_V 230103

#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

typedef enum
{
  UNDEF,
  GOOGLE_SCRIPT
} RincPosterService;

class RincPoster
{
  public:
    RincPoster(const char *ssid, const char *psw);
    bool begin(RincPosterService service, const char *scriptId);
    bool post(const JsonDocument& doc);

  private:
    bool connect();
    RincPosterService service;
    char scriptId[100];
    char ssid[100];
    char psw[100];
    int httpCode;
};

RincPoster::RincPoster(const char *ssid, const char *psw)
{
  strcpy(this->ssid, ssid);
  strcpy(this->psw, psw);
  service = UNDEF;
}

bool RincPoster::begin(RincPosterService service, const char *scriptId)
{
  this->service = service;
  strcpy(this->scriptId, scriptId);
  return true;
}

bool RincPoster::post(const JsonDocument& doc)
{
  bool success;
  if (WiFi.status() != WL_CONNECTED){
    success = connect();
  } else {
    success = true;
  }
  
  if (!success) return false;

  String url;
  if (service == GOOGLE_SCRIPT)
    url = String("https://script.google.com/macros/s/") + String(scriptId) + String("/exec?");
  String output;
  serializeJson(doc, output);

  auto begin = output.begin();
  auto end = std::remove( output.begin(), output.end(), '\"');
  end = std::remove( output.begin(), end, '{');
  end = std::remove( output.begin(), end, '}');
  std::replace( output.begin(), end, ',', '&');
  std::replace( output.begin(), end, ':', '=');
  for (auto it = begin; it != end; it++){
    url += *it;
  }
  
  // post data
  Serial.printf("sending: %s\n", url.c_str());
  HTTPClient http;
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  httpCode = http.GET();
  Serial.printf("HTTP Status Code: %d\n", httpCode);
  http.end();
  
  return (httpCode >= 200 && httpCode < 204); // range of success codes
}

bool RincPoster::connect()
{
  float wait_time_s = 10;
  float elapsed = 0;
  Serial.printf("Connecting:\n");
  WiFi.begin(ssid, psw);
  while (WiFi.status() != WL_CONNECTED && elapsed < wait_time_s)
  {
    delay(500);
    elapsed += 0.5;
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("\nConnected\n");
    return true;
  } else {
    return false;
  }
}

#endif // RINCPOSTER_H
