软件版本:v1.0

1. 使用mac地址作为设备唯一ID
2. 为每个设备生成不同的topic(根据设备ID生成),确保设备不会重复接收数据
3. 新增公共topic,用于client端进行统一控制
4. 新增,第一次连接到服务器后,自动发布自己的设备编号

***

## 功能说明

1. 按键功能
    1.1. 切换继电器开关,仅在断网时有效
    1.2. 长按3秒以上,设备重启
2. 群组功能
    2.1. 每个终端都允许加入1个群组
    2.2. 群组topic支持动态修改,支持2种方式修改,本地修改(modbus协议),mqtt协议
3. 动态参数配置
    3.1. 可通过2种方式,修改终端配置数据
        3.1.1. wifi 账号与密码
        3.1.2. 群组 topic 名称
        3.1.3. 终端唯一编号

***

## topic 说明

### 说明

deveice_no : 代表设备唯一编号

### 控制指令列表

sw_on_cmd : 打开继电器
sw_off_cmd : 关闭继电器
delay_sw_on_cmd : 延时打开继电器
delay_sw_off_cmd : 延时关闭继电器
get_sw_status_cmd : 获取继电器状态
get_read_cfg_cmd : 获取配置信息
refactory_cmd : 恢复系统参数到出厂状态
restart : 重启

### toppic 列表

group_control
   群组控制,往这个群组发送控制命令,将会控制所有已加入这个群组的所有设备
   继电器状态修改后,会在group_status中上报状态信息
group_status,
   群组状态,节点第一次上电时,将会发送自己的deveice_no到已加入群组中
public_control
   公共控制,玩这个topic发送命令,将会控制所有在线设备
   继电器状态修改后,public_status中上报状态信息
public_status
   公共状态,所有节点在第1次上电时,将会发送自己的状态到此topic
drive_no/control
   节点控制,往此topic发送指令会控制对应的节点
drive_no/status
   节点状态,节点第1次上电时,会往此topic发送自己的状态
   每次继电器状态改变时,也会在此主题上报继电器状态
drive_no/modify_config
   节点参数修改,可以修改对应的参数,修改完成后,修改值断电会保存
drive_no/read_config
   获取配置信息,

## 举个栗子

## 控制继电器打开

topic:deveice_no/control
{"cmd":"sw_on_cmd"}

## 读取保存参数

setup1:订阅topic
drive_no/read_config
setup2:发送数据
topic:drive_no/control
{"cmd":"get_read_cfg_cmd"}

## 获取节点继电器状态

setup1:订阅状态节点
topic:devece_no/status
setup2:发送"获取继电器状态"控制指令
topic:devece_no/control
{"cmd":"get_sw_status_cmd"}

## 群控

topic:group_control
{"cmd":"sw_on_cmd"}

## 读取配置信息

setup1: 订阅 drive_no/read_config
setup2: 发送查询配置信息
topic: drive_no/control
{"cmd":"get_read_cfg_cmd"}

## 修改群控topic名称

topic: drive_no/config_modify
{
  "group_ctrl_topic":"g123"
}

## 修改设备唯一编号

drive_no/config_modify
{
"drive_no":"123"
}

## 修改WIFI账号,密码

drive_no/config_modify
{
"ssid":"123",
"password":"123"
}

## 修改服务器地址,端口号

drive_no/config_modify
{
"mqtt_server":"123",
"mqtt_port":1883
}

***

## 软件版本: v2.0

### 功能描述

#### 联网功能

1. 默认wifi账号与密码都是:88888888
2. 上电会默认连接 MQTT 服务器,订阅2个topic(device_no/device_sub_topic),连接mqtt服务器成功后,会往pub_topic主题发送当前终端的信息(包括终端的device_no,继电器状态等信息)
3. 如果出现断网情况,终端将自动重连wifi

#### TOPIC 说明

1. device_no/device_sub_topic 接收服务端的输入数据
2. device_no/device_pub_topic 终端发送给服务器的数据
3. pub_topic 终端发送给服务器的数据,全局

#### 配网功能

1. 长按按键2s,终端将进入配网状态,终端将作为热点,对外发布一个ESP8266-AP的热点,通过手机端连接到此热点(密码:88888888),在手机端的浏览器输入:<http://192.168.4.1> 进入配网网页,确定后,终端将重新连接WIFI

#### 控制命令

1. 服务器端通过往device_no/device_sub_topic发送"命令数据"(具体参考命令列表),控制终端执行各种动作或配置终端各种参数

#### 继电器控制

1. 短按按键,反转继电器状态,同时会上报到,device_no/device_pub_topic

#### device_no

1. 代表终端的唯一编号
2. 默认将终端的mac地址作为device_no

### 命令列表

1. 修改服务器地址 {"cmd1":"www.baidu.com"}
2. 修改WIFI账号   {"cmd2":"xxxx"}
3. 修改WIFI密码
4. 修改MQTT账号
5. 修改MQTT密码  
6. 打开继电器
7. 关闭继电器
8. 获取继电器状态
9. 获取设备ID
10. 修改MQTT端口号
11. 语音播报 {"cmd11":"b4f3bcd2bac3"} // 控制播报"大家好","b4f3bcd2bac3"这个是"大家好"的gbk编码的16进制字符串
12. 继电器上报
13. 倒计时设置 {"cmd13":100}   // 单位:分钟
14. 获取倒计时 {"cmd14":""}   // 终端将往device_no/device_sub_topic上报,上包的数据格式为 json 格式
15. 打开倒计时 {"cmd15":""}   // 注意,设置了倒计时时间后,需要发送此命令后,才开始进行倒计时
16. 关闭倒计时
