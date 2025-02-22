# 软件版本: v2.0
## 功能描述

### 一、网络连接

1. 默认 Wi-Fi 凭据：
   - SSID: 88888888
   - 密码: 88888888

2. 设备启动流程：
   - 自动连接 MQTT 服务器
   - 订阅两个主题：
     1. `device_no/device_sub_topic`
     2. `device_global_sub_topic`
   - 成功连接后，向 `device_no/device_pub_topic` 发送设备信息：
     - 设备唯一标识（device_no）
     - 继电器状态等

1. 默认Wi-Fi账号和密码均为：88888888
2. 设备上电后会自动连接MQTT服务器，并订阅两个主题（device_no/device_sub_topic 和 device_global_sub_topic）。成功连接MQTT服务器后，设备会向device_no/device_pub_topic主题发送当前终端信息，包括设备唯一标识（device_no）和继电器状态等。
3. 如遇网络断开，终端会自动尝试重新连接Wi-Fi网络，确保设备始终在线。

3. 网络中断处理：
   - 自动尝试重新连接 Wi-Fi
   - 确保设备持续在线

### 二、设备标识 (device_no)

1. 用途：代表终端的唯一编号
2. 默认值：设备的 MAC 地址

### 三、MQTT 主题说明

| 主题 | 用途 | 通信方向 |
|------|------|----------|
| `device_no/device_sub_topic` | 接收服务器指令和数据 | 服务器 → 设备 |
| `device_no/device_pub_topic` | 发送状态更新和事件通知 | 设备 → 服务器 |
| `device_global_pub_topic` | 广播全局消息 | 设备 → 服务器（所有设备共用） |
| `device_global_sub_topic` | 接收全局指令和通知 | 服务器 → 设备（所有设备共用） |

注：`device_no` 为设备唯一标识符。

### 四、按键控制

1. 短按功能：
   - 正常模式：切换开/关状态
   - 自动关闭模式：按下开启，10 秒后自动关闭

2. 长按功能（6 秒以上）：
   - 进入配网模式
   - 设备变为 Wi-Fi 热点：
     - SSID: ESP8266-AP
     - 密码: 88888888
   - 配网步骤：
     1. 连接到设备热点
     2. 自动打开配网页面
     3. 完成配网后设备重新连接 Wi-Fi

### 五、通信协议

- 控制方式：服务器通过 `device_no/device_sub_topic` 或 `device_global_topic` 发送命令
- 设备响应：对每个命令进行应答（确认、否认或数据回复）

#### 5.1 支持的命令列表

| 命令 | JSON 格式 | 说明 |
|------|-----------|------|
| 打开继电器 | `{"open_relay_cmd":""}` | |
| 关闭继电器 | `{"close_relay_cmd":""}` | |
| 语音播报 | `{"play_voice_cmd":"b4f3bcd2bac3"}` | "b4f3bcd2bac3" 为 GBK 编码的十六进制字符串，如 "大家好" |
| 打开倒计时 | `{"start_countdown_cmd":""}` | 需先设置倒计时时间 |
| 关闭倒计时 | `{"stop_countdown_cmd":""}` | |
| 修改服务器地址 | `{"set_mqtt_server_cmd":"www.example.com"}` | |
| 修改 Wi-Fi SSID | `{"set_wifi_ssid_cmd":"xxxx"}` | |
| 修改 Wi-Fi 密码 | `{"set_wifi_pwd_cmd":"xxxx"}` | |
| 修改 MQTT 用户名 | `{"set_mqtt_username_cmd":"xxxx"}` | |
| 修改 MQTT 密码 | `{"set_mqtt_password_cmd":"xxxx"}` | |
| 设置 MQTT 端口 | `{"set_mqtt_port_cmd":1883}` | |
| 设置倒计时 | `{"set_countdown_cmd":100}` | 单位：分钟 |
| 设置开门模式 | `{"set_open_door_mode_cmd":0}` | 0: 正常模式, 1: 常开模式 |
| 设置设备 ID | `{"set_device_id_cmd":"xxxx"}` | |
| 获取系统信息 | `{"get_system_info_cmd":""}` | 回复发送至 device_no/device_pub_topic |
| 恢复出厂设置 | `{"factory_params_cmd":""}` | |
| 重启设备 | `{"restart_cmd":""}` | |

#### 5.2 消息上报

终端运行过程中，在运行至特殊的状态时，会主动往 device_no/device_pub_topic 和 device_global_pub_topic 上报数据，上报格式为 JSON 格式。

1. 系统信息上报 (上电时,会上报一次)：
   ```json
   {"system_info_event":{"soft_ver":"版本号","hard_ver":"版本号","relay":状态,"wifi_ssid":"SSID","wifi_pwd":"密码","mqtt_server":"服务器地址","mqtt_port":端口号,"mqtt_username":"用户名","mqtt_password":"密码","count_down_minute":倒计时分钟数,"count_down_start_f":倒计时状态,"device_id":"设备ID","open_door_mode":开门模式,"auto_close_door_time_s":自动关门时间,"device_sub_topic":"订阅主题","device_pub_topic":"发布主题"}}
   ```

2. 倒计时完成时，上报：
   ```json
   {"countdown_finish_event":1}
   ```

3. 语音播报成功时，上报：
   ```json
   {"play_voice_result_event":1}
   ```

4. 语音播报失败时，上报：
   ```json
   {"play_voice_result_event":0}
   ```

5. 确认应答时，上报：
   ```json
   {"ask":1}
   ```

6. 否认应答时，上报：
   ```json
   {"ask":0}
   ```

7. 收到未知命令时，上报：
   ```json
   {"unknown_cmd":0}
   ```

8. 继电器状态变化时，上报：
   ```json
   {"relay_state_change_event":${state}} or {"relay_state_change_event":${state}}
   ```
   其中，${state} 为 0 表示关闭，1 表示打开。

注意：所有这些消息都会同时发送到 device_no/device_pub_topic 和 device_global_pub_topic 两个主题。
***



