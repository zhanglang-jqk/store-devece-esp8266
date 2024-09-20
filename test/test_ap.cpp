#if 0
/***********************************************************************
 * @file 	:	test_tm1620.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-09-06  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
// #include "include.h"
#include <Arduino.h>
#include <unity.h>
#include <TTS.h>
#include <SoftwareSerial.h>
#include "param.hpp"
#include "net_man.hpp"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
ESP8266WebServer server(80); // 在端口 80 上创建服务器
static const char _mqtt_ca_crt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";
// BearSSL::WiFiClientSecure _wifi_client;
BearSSL::X509List _mqttcert(_mqtt_ca_crt);

WiFiClient _wifi_client;
/* function declaration ---------------------------------------------------------------------------------------------------- */

/* variable definition ----------------------------------------------------------------------------------------------------- */

/* function implementation ------------------------------------------------------------------------------------------------- */
void handleRoot()
{

  String html = R"(
<!-- FILEPATH: /d:/workspace/1-project/DWY/DWY421/code/DWY421_ESP8266_V2/other/wifi-config.html -->

<!DOCTYPE html>
<html>

<head>
  <title>WiFi 配置</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
    }

    html,
    body {
      height: 100vh;
      margin: 0;
      padding: 0;
    }

    .container {
      height: 100%;
      display: flex;
      flex-direction: column;
      margin: 0 auto;
      padding: 20px;
      background-color: #fff;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
    }

    .form-group {
      flex: 1;
      margin-bottom: 20px;
    }

    .form-group label {
      display: block;
      font-weight: bold;
      margin-bottom: 5px;
    }

    .form-group input {
      width: 100%;
      padding: 10px;
      border: 1px solid #ccc;
      border-radius: 3px;
    }

    .submit-btn {
      display: block;
      width: 100%;
      padding: 10px;
      background-color: #4CAF50;
      color: #fff;
      border: none;
      border-radius: 3px;
      cursor: pointer;
    }

    .submit-btn:hover {
      background-color: #45a049;
    }

    /* 添加媒体查询以适应手机界面 */
    @media only screen and (max-width: 600px) {
      .container {
        max-width: 100%;
        height: 100%;
        margin: 0;
        border-radius: 0;
        box-shadow: none;
        overflow: auto;
      }

      .form-group input {
        padding: 8px;
      }
    }
  </style>
</head>

<body>
  <div class="container">
    <h2>WiFi 配置</h2>
    <form method='POST' action='/setap'>
      <div class="form-group">
        <label for="ssid">WIFI 账号:</label>
        <input type="text" id="ssid" name="ssid" placeholder="输入 WiFi 账号">
      </div>
      <div class="form-group">
        <label for="password">WIFI 密码:</label>
        <input type="password" id="password" name="password" placeholder="输入 WiFi 密码">
      </div>
      <button class="submit-btn" type="submit">配置</button>
    </form>
  </div>
</body>

</html><!-- FILEPATH: /d:/workspace/1-project/DWY/DWY421/code/DWY421_ESP8266_V2/other/wifi-config.html -->

<!DOCTYPE html>
<html>

<head>
  <title>WiFi 配置</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
    }

    html,
    body {
      height: 100vh;
      margin: 0;
      padding: 0;
    }

    .container {
      height: 100%;
      display: flex;
      flex-direction: column;
      margin: 0 auto;
      padding: 20px;
      background-color: #fff;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
    }

    .form-group {
      flex: 1;
      margin-bottom: 20px;
    }

    .form-group label {
      display: block;
      font-weight: bold;
      margin-bottom: 5px;
    }

    .form-group input {
      width: 100%;
      padding: 10px;
      border: 1px solid #ccc;
      border-radius: 3px;
    }

    .submit-btn {
      display: block;
      width: 100%;
      padding: 10px;
      background-color: #4CAF50;
      color: #fff;
      border: none;
      border-radius: 3px;
      cursor: pointer;
    }

    .submit-btn:hover {
      background-color: #45a049;
    }

    /* 添加媒体查询以适应手机界面 */
    @media only screen and (max-width: 600px) {
      .container {
        max-width: 100%;
        height: 100%;
        margin: 0;
        border-radius: 0;
        box-shadow: none;
        overflow: auto;
      }

      .form-group input {
        padding: 8px;
      }
    }

    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
    }

    html,
    body {
      height: 100vh;
    }

    .container {
      /*max-width: 400px;*/
      margin: 0 auto;
      padding: 20px;
      background-color: #fff;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
    }

    .form-group {
      margin-bottom: 20px;
    }

    .form-group label {
      display: block;
      font-weight: bold;
      margin-bottom: 5px;
    }

    .form-group input {
      width: 100%;
      padding: 10px;
      border: 1px solid #ccc;
      border-radius: 3px;
    }

    .submit-btn {
      display: block;
      width: 100%;
      padding: 10px;
      background-color: #4CAF50;
      color: #fff;
      border: none;
      border-radius: 3px;
      cursor: pointer;
    }

    .submit-btn:hover {
      background-color: #45a049;
    }

    /* 添加媒体查询以适应手机界面 */
    @media only screen and (max-width: 600px) {
      .container {
        max-width: 100%;
        height: 100%;
        margin: 0;
        border-radius: 0;
        box-shadow: none;
        overflow: auto;
      }

      .form-group input {
        padding: 8px;
      }
    }
  </style>
</head>

<body>
  <div class="container">
    <h2>WiFi 配置</h2>
    <form method='POST' action='/setap'>
      <div class="form-group">
        <label for="ssid">WIFI 账号:</label>
        <input type="text" id="ssid" name="ssid" placeholder="输入 WiFi 账号">
      </div>
      <div class="form-group">
        <label for="password">WIFI 密码:</label>
        <input type="password" id="password" name="password" placeholder="输入 WiFi 密码">
      </div>
      <button class="submit-btn" type="submit">配置</button>
    </form>
  </div>
</body>

</html>
  )";

  server.send(200, "text/html", html);
}

NetClient _net_client;

struct
{
  bool conn_f = false;
} fsm;

void handleSetAP()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  Serial.println("Received SSID: " + ssid);
  Serial.println("Received Password: " + password);
  server.send(200, "text/plain", "Received SSID and Password");

  if ((ssid.length() != 0) && (password.length() != 0))
  {
    // param.WriteParam(Param::AP_SSID, ssid.c_str(), ssid.length());
    // param.WriteParam(Param::AP_PWD, password.c_str(), password.length());
    // bool isok = _net_client.ConnWifi("88888888", "88888888");
    // if (isok)
    // {
    //   tp();
    // }
    fsm.conn_f = true;
    tp();
    WiFi.softAPdisconnect(true);
  }
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  UNITY_BEGIN(); // IMPORTANT LINE!
  delay(2000);

  Serial.begin(115200);
  tpf("system begin\n");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266-AP", "88888888");
  server.on("/", handleRoot);
  server.on("/setap", HTTP_POST, handleSetAP);
  server.begin(); // 设置 ESP8266 为 AP 模式并创建 Wi-Fi 网络
  // fsm.conn_f = true;
}

void loop()
{

  server.handleClient(); // 处理客户端请求
  // if (fsm.conn_f)
  // {
  //   tp();
  //   _net_client.ConnWifi("88888888", "88888888");
  //   fsm.conn_f = false;
  // }
  // RUN_TEST(test_mqtt_loop);
  // UNITY_END();
  if (fsm.conn_f)
  {
    tp();
    delay(500), ESP.wdtFeed(), delay(500), ESP.wdtFeed();

    WiFi.mode(WIFI_STA);
    WiFi.begin("88888888", "88888888"); // Connect to the network

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
      delay(500);
      Serial.print('.');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

    // _wifi_client.setTrustAnchors(&_mqttcert);
    // if no check the CA Certification
    // _wifi_client.setInsecure();
    if (_wifi_client.connect("ee836835.ala.cn-hangzhou.emqxsl.cn", 8883))
    {

      Serial.println("Internet connection OK");
      _wifi_client.stop();
    }
    else
    {
      Serial.println("Internet connection failed");
    }

    fsm.conn_f = false;
  }
}

#endif
// test_tm1620.cpp