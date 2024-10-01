/***********************************************************************
 * @file 	:	network_proc.cpp
 * @author	:	CH
 * @brief	:
 * @Copyright (C)  2024-07-20  .cdWFVCEL. all right reserved
 ***********************************************************************/

/* include ----------------------------------------------------------------------------------------------------------------- */
#include "include.h"
#include "net_man.hpp"
#include "debug_log.h"
#include "led.h"
#include "param.hpp"
/* macro definition -------------------------------------------------------------------------------------------------------- */

/* type definition --------------------------------------------------------------------------------------------------------- */

/* variable declaration ---------------------------------------------------------------------------------------------------- */
extern LED led;
ESP8266WiFiClass WiFi;
/* function declaration ---------------------------------------------------------------------------------------------------- */

// void WebServerMainHtml();

/* variable definition ----------------------------------------------------------------------------------------------------- */
static const char mqtt_ca_crt[] PROGMEM = R"EOF(
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

// const char html[] PROGMEM = R"(
// <!-- FILEPATH: /d:/workspace/1-project/DWY/DWY421/code/DWY421_ESP8266_V2/other/wifi-config.html -->
// <!DOCTYPE html>
// <html>

// <head>
//   <title>WiFi 配置</title>
//   <style>
//     body {
//       font-family: Arial, sans-serif;
//       background-color: #f2f2f2;
//     }

//     html,
//     body {
//       height: 100vh;
//       margin: 0;
//       padding: 0;
//     }

//     .container {
//       height: 100%;
//       display: flex;
//       flex-direction: column;
//       margin: 0 auto;
//       padding: 20px;
//       background-color: #fff;
//       border-radius: 5px;
//       box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
//     }

//     .form-group {
//       flex: 1;
//       margin-bottom: 20px;
//     }

//     .form-group label {
//       display: block;
//       font-weight: bold;
//       margin-bottom: 5px;
//     }

//     .form-group input {
//       width: 100%;
//       padding: 10px;
//       border: 1px solid #ccc;
//       border-radius: 3px;
//     }

//     .submit-btn {
//       display: block;
//       width: 100%;
//       padding: 10px;
//       background-color: #4CAF50;
//       color: #fff;
//       border: none;
//       border-radius: 3px;
//       cursor: pointer;
//     }

//     .submit-btn:hover {
//       background-color: #45a049;
//     }

//     /* 添加媒体查询以适应手机界面 */
//     @media only screen and (max-width: 600px) {
//       .container {
//         max-width: 100%;
//         height: 100%;
//         margin: 0;
//         border-radius: 0;
//         box-shadow: none;
//         overflow: auto;
//       }

//       .form-group input {
//         padding: 8px;
//       }
//     }
//   </style>
// </head>

// <body>
//   <div class="container">
//     <h2>WiFi 配置</h2>
//     <form method='POST' action='/setap'>
//       <div class="form-group">
//         <label for="ssid">WIFI 账号:</label>
//         <input type="text" id="ssid" name="ssid" placeholder="输入 WiFi 账号">
//       </div>
//       <div class="form-group">
//         <label for="password">WIFI 密码:</label>
//         <input type="password" id="password" name="password" placeholder="输入 WiFi 密码">
//       </div>
//       <button class="submit-btn" type="submit">配置</button>
//     </form>
//   </div>
// </body>

// </html>
//   )";



const char html[] PROGMEM = 
R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <title>WiFi 配置</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
      margin: 0;
      padding: 0;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
    }

    .container {
      width: 90%;
      max-width: 350px;
      padding: 20px;
      background-color: #fff;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
    }

    h2 {
      text-align: center;
      color: #333;
      margin-bottom: 20px;
    }

    .form-group {
      margin-bottom: 15px;
    }

    .form-group label {
      display: block;
      font-weight: bold;
      margin-bottom: 5px;
      color: #555;
    }

    .form-group input {
      width: calc(100% - 20px);
      padding: 8px 10px;
      border: 1px solid #ccc;
      border-radius: 3px;
      font-size: 14px;
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
      font-size: 16px;
    }

    .submit-btn:hover {
      background-color: #45a049;
    }

    @media only screen and (max-width: 400px) {
      .container {
        width: 95%;
        padding: 15px;
      }

      .form-group input {
        font-size: 13px;
      }

      .submit-btn {
        font-size: 14px;
      }
    }
  </style>
</head>

<body>
  <div class="container">
    <h2>WiFi 配置</h2>
    <form id="wifiForm" onsubmit="return submitForm(event)">
      <div class="form-group">
        <label for="ssid">WIFI 账号:</label>
        <input type="text" id="ssid" name="ssid" placeholder="输入 WiFi 账号" required>
      </div>
      <div class="form-group">
        <label for="password">WIFI 密码:</label>
        <input type="password" id="password" name="password" placeholder="输入 WiFi 密码" required>
      </div>
      <button class="submit-btn" type="submit">配置</button>
    </form>
  </div>

  <script>
    function validateForm() {
      var ssid = document.getElementById("ssid").value;
      var password = document.getElementById("password").value;

      if (ssid.trim() === "") {
        alert("请输入 WiFi 账号");
        return false;
      }

      if (password.trim() === "") {
        alert("请输入 WiFi 密码");
        return false;
      }

      if (password.length < 8) {
        alert("WiFi 密码长度应至少为 8 个字符");
        return false;
      }

      return true;
    }

    function submitForm(event) {
      event.preventDefault();
      if (!validateForm()) {
        return;
      }

      var form = document.getElementById("wifiForm");
      var formData = new FormData(form);

      fetch('/setap', {
        method: 'POST',
        body: formData
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'success') {
          alert(data.message || "配置成功！");
          form.reset();
        } else {
          alert(data.message || "配置失败，请重试。");
        }
      })
      .catch(error => {
        alert("配置失败，请重试。");
        console.error('Error:', error);
      });
    }
  </script>
</body>

</html>
)rawliteral";


BearSSL::X509List mqttcert(mqtt_ca_crt);
// ESP8266WebServer NetClient::server = new ESP8266WebServer(80);
ESP8266WebServer NetClient::server(80);
/* function implementation ------------------------------------------------------------------------------------------------- */
void NetClient::Init(Callback_t callback)
{
  this->callback = callback;
}

uint8_t NetClient::ConnWifi(String ssid, String password)
{
  static struct
  {
    uint8_t stat;
    uint32_t last_ms;
    uint16_t conn_run_cnt;
  } fsm;

  WiFiConnResult_et conn_result;

  if (fsm.stat == 0)
  {
    // 设置重连超时时间和尝试次数
    // WiFi.setAutoReconnect(true);        // 启用自动重新连接
    // WiFi.setSleepMode(WIFI_NONE_SLEEP); // 设置睡眠模式（在连接失败时保持唤醒）
    // WiFi.persistent(true);              // 持久化配置（使重启后仍能记住WiFi配置）
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    conn_result = WIFI_CONNECTING, fsm.stat = 1;
  }
  if (fsm.stat == 1) // wait for WiFi connection
  {
    // 等待WiFi连接,成功连接后输出成功信息
    if (millis() - fsm.last_ms >= 500)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        logf(".");
        if (fsm.conn_run_cnt++ > 30)
        {
          tpf("WiFi connection timeout");
          conn_result = WIFI_CONNECT_TIMEOUT;
          fsm.stat = 3;
        }
      }
      else if (WiFi.status() == WL_CONNECTED)
      {
        fsm.stat = 2;
        ln();
      }
      fsm.last_ms = millis();
    }

    // led.ToggleEnd();
  }
  if (fsm.stat == 2)
  {
    tpf("connected to WiFi,IP address:%s ,MAC address:%s", WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());

    // wifi_client.setTrustAnchors(&mqttcert);
    // if no check the CA Certification
    // wifi_client.setInsecure();
    wifi_conn = true;
    conn_result = WIFI_CONNECED;
    fsm.stat = 3;
  }
  if (fsm.stat == 3)
  {
    memset(&fsm, 0, sizeof(fsm)); // clean fsm flags
    // fsm.conn_result = (WiFiConnResult_et)0, fsm.stat = 0, fsm.conn_run_cnt = 0, fsm.last_ms = 0;
  }

  return (uint8_t)conn_result;
}

bool NetClient::ConnServer(char *server, uint16_t port, char *id, char *username, char *password)
{
  // mqtt_client.setClient(wifi_client);
  // mqtt_client.setServer(server, port);
  // mqtt_client.setCallback(callback);
  // mqtt_conn = mqtt_client.connect(id, username, password);

  // TODO temporary modify to test use
  // ip：106.75.224.82
  // 端口：1884
  // 账号：dwy_gaiban
  // 密码：gaibanmenjin
  mqtt_client.setClient(wifi_client);
  IPAddress ip(106, 75, 224, 82);
  mqtt_client.setServer(ip, port);
  mqtt_client.setCallback(callback);
  username = "dwy_gaiban";
  password = "gaibanmenjin";
  mqtt_conn = mqtt_client.connect(id, username, password);
  return mqtt_conn;
}

void NetClient::Loop()
{
  mqtt_client.loop();
  // time_client.update();  // TODO
}

bool NetClient::GetWifiConn()
{
  if (WiFi.status() != WL_CONNECTED)
    wifi_conn = false;
  else
    wifi_conn = true;
  return wifi_conn;
}

bool NetClient::GetMqttConn()
{
  return mqtt_client.connected();
}

bool NetClient::Subscribe(char *topic)
{
  return mqtt_client.subscribe(topic);
}

uint32_t NetClient::GetEpochTime()
{
  return time_client.getEpochTime();
}

bool NetClient::Publish(char *topic, char *payload)
{
  return mqtt_client.publish(topic, payload);
}

void NetClient::Disconnect()
{
  mqtt_client.disconnect();
}

void NetClient::WebServer_pool()
{
  server.handleClient(); // 处理客户端请求
}

void NetClient::WebServer_start(char *ssid, char *password, void (*callback)())
{
  tpf("%d", WiFi.status());
  // if (GetWifiConn() == true) WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  tpf("%d", WiFi.status());
  WiFi.softAP(ssid, password);

  // Set up the server route for the main page
  server.on("/", HTTP_GET, [this]() {
    server.send(200, "text/html", html);
  });
  server.on("/setap", HTTP_POST, callback);
  server.begin(80); // 设置 ESP8266 为 AP 模式并创建 Wi-Fi 网络
}

void NetClient::WebServerMainHtml()
{
  server.send(200, "text/html", html);
}

void NetClient::WebServer_stop()
{
  WiFi.softAPdisconnect(true);
  // delete server;
  // delete WiFi;
  // WiFi = new ESP8266WiFiClass();
}

String NetClient::GetArg(char *arg)
{
  return server.arg(arg);
}
// network_proc.cpp