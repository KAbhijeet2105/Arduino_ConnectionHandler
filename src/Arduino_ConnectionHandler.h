/*
   This file is part of ArduinoIoTCloud.

   Copyright 2019 ARDUINO SA (http://www.arduino.cc/)

   This software is released under the GNU General Public License version 3,
   which covers the main part of arduino-cli.
   The terms of this license can be found at:
   https://www.gnu.org/licenses/gpl-3.0.en.html

   You can be released from the requirements of the above licenses by purchasing
   a commercial license. Buying such a license is mandatory if you want to modify or
   otherwise use the software for commercial activities involving the Arduino
   software without disclosing the source code of your own applications. To purchase
   a commercial license, send an email to license@arduino.cc.
*/

#ifndef ARDUINO_CONNECTION_HANDLER_H_
#define ARDUINO_CONNECTION_HANDLER_H_

#ifdef ARDUINO_SAMD_MKR1000
  #include <WiFi101.h>
  #include <WiFiUdp.h>

  #define BOARD_HAS_WIFI
  #define NETWORK_HARDWARE_ERROR WL_NO_SHIELD
  #define NETWORK_IDLE_STATUS WL_IDLE_STATUS
  #define NETWORK_CONNECTED WL_CONNECTED
  #define WIFI_FIRMWARE_VERSION_REQUIRED WIFI_FIRMWARE_REQUIRED
#endif

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
  #include <WiFiUdp.h>

  #define BOARD_HAS_WIFI
  #define NETWORK_HARDWARE_ERROR WL_NO_MODULE
  #define NETWORK_IDLE_STATUS WL_IDLE_STATUS
  #define NETWORK_CONNECTED WL_CONNECTED
  #define WIFI_FIRMWARE_VERSION_REQUIRED WIFI_FIRMWARE_LATEST_VERSION
#endif

#ifdef ARDUINO_SAMD_MKRGSM1400
  #include <MKRGSM.h>
  #define BOARD_HAS_GSM
  #define NETWORK_HARDWARE_ERROR GPRS_PING_ERROR
  #define NETWORK_IDLE_STATUS GSM3_NetworkStatus_t::IDLE
  #define NETWORK_CONNECTED GSM3_NetworkStatus_t::GPRS_READY
#endif

#ifdef ARDUINO_SAMD_MKRNB1500
  #include <MKRNB.h>
  #define BOARD_HAS_NB
  #define NETWORK_HARDWARE_ERROR
  #define NETWORK_IDLE_STATUS NB_NetworkStatus_t::IDLE
  #define NETWORK_CONNECTED NB_NetworkStatus_t::GPRS_READY
#endif

#if defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310)
  #include <MKRWAN.h>
  #define BOARD_HAS_LORA
#endif

#if    defined(ARDUINO_ESP8266_ESP12)    \
    || defined(ESP8266)                  \
    || defined(ESP8266_ESP01)            \
    || defined(ESP8266_ESP13)            \
    || defined(ESP8266_GENERIC)          \
    || defined(ESP8266_ESPRESSO_LITE_V1) \
    || defined(ESP8266_ESPRESSO_LITE_V2) \
    || defined(ESP8266_PHOENIX_V1)       \
    || defined(ESP8266_PHOENIX_V2)       \
    || defined(ESP8266_NODEMCU)          \
    || defined(MOD_WIFI_ESP8266)         \
    || defined(ESP8266_THING)            \
    || defined(ESP8266_THING_DEV)        \
    || defined(ESP8266_ESP210)           \
    || defined(ESP8266_WEMOS_D1MINI)     \
    || defined(ESP8266_WEMOS_D1MINIPRO)  \
    || defined(ESP8266_WEMOS_D1MINILITE) \
    || defined(ESP8266_WEMOS_D1R1)       \
    || defined(ESP8266_ESP12)            \
    || defined(WIFINFO)                  \
    || defined(ESP8266_ARDUINO)          \
    || defined(GEN4_IOD)                 \
    || defined(ESP8266_OAK)              \
    || defined(WIFIDUINO_ESP8266)        \
    || defined(AMPERKA_WIFI_SLOT)        \
    || defined(ESP8266_WIO_LINK)         \
    || defined(ESP8266_ESPECTRO_CORE)
  
  #define BOARD_ESP8266
#endif

#if defined(BOARD_ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>

  #define BOARD_HAS_WIFI
  #define NETWORK_HARDWARE_ERROR WL_NO_SHIELD
  #define NETWORK_IDLE_STATUS WL_IDLE_STATUS
  #define NETWORK_CONNECTED WL_CONNECTED
  #define WIFI_FIRMWARE_VERSION_REQUIRED WIFI_FIRMWARE_REQUIRED
#endif

/******************************************************************************
   INCLUDES
 ******************************************************************************/

#include <Arduino_DebugUtils.h>

/******************************************************************************
   TYPEDEFS
 ******************************************************************************/

enum class NetworkConnectionState {
  INIT,
  CONNECTING,
  CONNECTED,
  GETTIME,
  DISCONNECTING,
  DISCONNECTED,
  CLOSED,
  ERROR
};

enum class NetworkConnectionEvent {
  INIT, CONNECTING, CONNECTED, DISCONNECTING, DISCONNECTED, CLOSED, ERROR
};

typedef void (*OnNetworkEventCallback)(void * /* arg */);

/******************************************************************************
   CLASS DECLARATION
 ******************************************************************************/

class ConnectionHandler {
  public:
    virtual void init() = 0;
    virtual void check() = 0;
    virtual void update() __attribute__((deprecated)) = 0; /* use 'check()' instead */

    #if defined(BOARD_HAS_WIFI) || defined(BOARD_HAS_GSM) || defined(BOARD_HAS_NB)
      virtual unsigned long getTime() = 0;
      virtual Client &getClient() = 0;
      virtual UDP &getUDP() = 0;
    #endif

    virtual NetworkConnectionState getStatus() {
      return netConnectionState;
    }
    virtual void connect();
    virtual void disconnect();
    void addCallback(NetworkConnectionEvent const event, OnNetworkEventCallback callback);
    void addConnectCallback(OnNetworkEventCallback callback);
    void addDisconnectCallback(OnNetworkEventCallback callback);
    void addErrorCallback(OnNetworkEventCallback callback);

  protected:
    OnNetworkEventCallback  _on_connect_event_callback = NULL,
                            _on_disconnect_event_callback = NULL,
                            _on_error_event_callback = NULL;

    unsigned long lastValidTimestamp = 0;   /*  UNUSED  */
    NetworkConnectionState netConnectionState = NetworkConnectionState::INIT;

    static void execNetworkEventCallback(OnNetworkEventCallback & callback, void * callback_arg);

};

#if defined(BOARD_HAS_WIFI) || defined(BOARD_HAS_GSM) || defined(BOARD_HAS_NB)
  #include "Arduino_TcpIpConnectionHandler.h"
#endif

#if defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310)
  #include "Arduino_LPWANConnectionHandler.h"
#endif

#endif /* CONNECTION_HANDLER_H_ */
