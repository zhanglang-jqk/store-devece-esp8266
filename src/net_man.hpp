#ifndef __NETWORK_PROC_H_
#define __NETWORK_PROC_H_
/* include ----------------------------------------------------------------------------------------------------------------- */
#include <WString.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */
#define NETMAN_SERVER_PORT 80
/* type definition --------------------------------------------------------------------------------------------------------- */
typedef std::function<void(char *, uint8_t *, unsigned int)> Callback_t;

class NetClient
{
public:
  typedef enum
  {
    WIFI_CONNECTING,
    WIFI_CONNECED,
    WIFI_CONNECT_TIMEOUT
  } WiFiConnResult_et;

  NetClient() : time_client(NTP_UDP, "pool.ntp.org") {};
  /* wifi & mqtt -------------------- */
  void Init(Callback_t callback);
  uint8_t ConnWifi(String ssid, String password);
  bool ConnServer(char *server, uint16_t port, char *id, char *username, char *password);
  void Loop();
  /**
   * @brief Get the Wifi Conn object
   *
   * @return true wifi is connected
   * @return false wifi is not connected
   */
  bool GetWifiConn();
  bool GetMqttConn();
  bool Subscribe(char *topic);
  uint32_t GetEpochTime();
  bool Publish(char *topic, char *payload);
  void Disconnect();
  /* wifi & mqtt -------------------- */

  /* wifi app -------------------- */
  void WebServer_pool();
  void WebServer_start(char *ap_ssid, char *ap_pwd, void (*pfun)());
  void WebServer_stop();
  String GetArg(char *arg);
  /* wifi app -------------------- */
  
  static ESP8266WebServer server; // 在端口 80 上创建服务器
private:
  // BearSSL::WiFiClientSecure wifi_client;
  WiFiClient wifi_client;
  PubSubClient mqtt_client; /* set SSL/TLS certificate */
  WiFiUDP NTP_UDP;
  NTPClient time_client;

  bool wifi_conn = false;
  bool mqtt_conn = false;
  std::function<void(char *, uint8_t *, unsigned int)> callback;
  void WebServerMainHtml();
};

/* variable declaration ---------------------------------------------------------------------------------------------------- */

/* function declaration ---------------------------------------------------------------------------------------------------- */

#endif // __NETWORK_PROC_H_
