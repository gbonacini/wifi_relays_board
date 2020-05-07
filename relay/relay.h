// -----------------------------------------------------------------
// Relay Controller - A simple controller for 8 relays board for
//                    NodeMcu 8266
// Copyright (C) 2020  Gabriele Bonacini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------

#pragma once

#include<ESP8266WiFi.h>
#include<cstdint> 
#include<cstdlib> 
#include<array> 
#include<string> 
#include<tuple>

namespace relay {

  using Pin=std::tuple<bool, uint8_t>;
  const size_t ON_OFF       = 0;
  const size_t PIN_ADDRESS  = 1;
  const int    SERIAL_SPEED = 115200;

  class RelayBoard {

    public:
                                 RelayBoard(std::string _ssid, 
                                            std::string _pwd, 
                                            int _port=80)                      noexcept;

       void                      init(void)                                    noexcept; 
                                           
       void                      printStatus(void)                       const noexcept;
       
       std::uint8_t              getStatusNum(void)                      const noexcept;
       std::string               getStatusString(void)                   const noexcept;
       const std::array<Pin,8>&  getStatusArray(void)                    const noexcept;
       bool                      getPinStatus(size_t pin)                const noexcept;
       
       void                      setStatus(const std::array<Pin,8>& newStatus)  noexcept;
       void                      setStatus(uint8_t newStatus)                   noexcept;
       void                      setStatus(const std::string& newStatus)        noexcept;
       void                      setPinStatus(bool newstatus, size_t pin)       noexcept;

       void                      responder(void)                                noexcept;
        
    private:
       std::string          ssid,
                            password,
                            request;

       const std::string    cmdSep,
                            infoSep;
                           
       WiFiServer           server;
       std::array<Pin, 8>   status;
       std::array<char, 64> buff;

       void                      connect(void)                            const noexcept;
       void                      startService(void)                             noexcept;
       void                      setter(std::string& data)                      noexcept;     
  };

  RelayBoard::RelayBoard(std::string _ssid, std::string _pwd, int _port) 
    :  ssid(_ssid), password(_pwd), cmdSep("set/"), infoSep("status"), server(_port),
       status{  //             STATUS, ADDRESS
                std::make_tuple(false, 5),
                std::make_tuple(false, 4),
                std::make_tuple(false, 0),
                std::make_tuple(false, 2),
                std::make_tuple(false, 14),
                std::make_tuple(false, 12),
                std::make_tuple(false, 13),
                std::make_tuple(false, 16)
       }
  {  
     Serial.begin(SERIAL_SPEED);
       
     for(auto it = status.cbegin(); it != status.cend(); ++it){
         pinMode(std::get<PIN_ADDRESS>(*it), OUTPUT); 
         digitalWrite(std::get<PIN_ADDRESS>(*it), HIGH); 
     }
  }

  void RelayBoard::init(void) noexcept {
     connect();
     startService();
  }

  void RelayBoard::startService(void) noexcept{
     server.begin();
     Serial.print("Server started.\nURL: \nhttp://");
     Serial.print(WiFi.localIP());
     Serial.println("/"); 
  }

  void RelayBoard::connect(void) const noexcept{
     Serial.print("\n\nConnecting to: ");
     Serial.println(ssid.c_str()); 
     WiFi.begin(ssid.c_str(), password.c_str());
     while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
     }
     Serial.println("\nWiFi connected"); 
}

  void RelayBoard::printStatus(void) const noexcept{
     std::string  pinStat("status: ");
          pinStat.append("1:").append(std::get<ON_OFF>(status[0])  ? "ON" : "OFF")
                 .append("2:").append(std::get<ON_OFF>(status[1])  ? "ON" : "OFF")
                 .append("3:").append(std::get<ON_OFF>(status[2])  ? "ON" : "OFF")
                 .append("4:").append(std::get<ON_OFF>(status[3])  ? "ON" : "OFF")
                 .append("5:").append(std::get<ON_OFF>(status[4])  ? "ON" : "OFF")
                 .append("6:").append(std::get<ON_OFF>(status[5])  ? "ON" : "OFF")
                 .append("7:").append(std::get<ON_OFF>(status[6])  ? "ON" : "OFF")
                 .append("8:").append(std::get<ON_OFF>(status[7])  ? "ON" : "OFF");
     Serial.println(pinStat.c_str());
  } 

  uint8_t RelayBoard::getStatusNum(void) const noexcept{
     return (std::get<ON_OFF>(status[7])  ? 0x01 : 0x0) +
            (std::get<ON_OFF>(status[6])  ? 0x02 : 0x0) +
            (std::get<ON_OFF>(status[5])  ? 0x04 : 0x0) +
            (std::get<ON_OFF>(status[4])  ? 0x08 : 0x0) +
            (std::get<ON_OFF>(status[3])  ? 0x10 : 0x0) +
            (std::get<ON_OFF>(status[2])  ? 0x20 : 0x0) +
            (std::get<ON_OFF>(status[1])  ? 0x40 : 0x0) +
            (std::get<ON_OFF>(status[0])  ? 0x80 : 0x0);
  }

  const std::array<Pin,8>&  RelayBoard::getStatusArray(void) const noexcept{
      return status;
  }

  bool RelayBoard::getPinStatus(size_t pin)  const noexcept{
    return std::get<ON_OFF>(status[pin]);
  }

  std::string RelayBoard::getStatusString(void) const noexcept{
      return std::string(std::get<ON_OFF>(status[0])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[1])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[2])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[3])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[4])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[5])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[6])  ? "1:" : "0:")
                 .append(std::get<ON_OFF>(status[7])  ? "1:" : "0:");
  }

  void RelayBoard::setStatus(const std::array<Pin,8>& newStatus){
     status = newStatus;

     for(auto it = status.cbegin(); it != status.cend(); ++it)
         digitalWrite(std::get<PIN_ADDRESS>(*it), std::get<ON_OFF>(*it) ? LOW : HIGH); 
  } 

  void RelayBoard::setStatus(uint8_t newStatus){
      std::get<ON_OFF>(status[7]) = newStatus & 0x01 ? true : false;
      std::get<ON_OFF>(status[6]) = newStatus & 0x02 ? true : false;
      std::get<ON_OFF>(status[5]) = newStatus & 0x04 ? true : false;
      std::get<ON_OFF>(status[4]) = newStatus & 0x08 ? true : false;
      std::get<ON_OFF>(status[3]) = newStatus & 0x10 ? true : false;
      std::get<ON_OFF>(status[2]) = newStatus & 0x20 ? true : false;
      std::get<ON_OFF>(status[1]) = newStatus & 0x40 ? true : false;
      std::get<ON_OFF>(status[0]) = newStatus & 0x80 ? true : false;

     for(auto it = status.cbegin(); it != status.cend(); ++it)
         digitalWrite(std::get<PIN_ADDRESS>(*it), std::get<ON_OFF>(*it) ? LOW : HIGH); 
  } 

  void RelayBoard::setStatus(const std::string& stringStatus){
     //TODO: use std::stoi , when available
     uint8_t newStatus = static_cast<uint8_t>(std::strtoul(stringStatus.c_str(), nullptr, 2));

     setStatus(newStatus);
  }  

  void RelayBoard::setPinStatus(bool newstatus, size_t pin) noexcept{
     std::get<ON_OFF>(status[pin-1]) = newstatus;
     digitalWrite(std::get<PIN_ADDRESS>(status[pin-1]), newstatus ? LOW : HIGH);
  }

  void RelayBoard::setter(std::string& data) noexcept{
     const size_t singlePinCmdLen = 2;
     
     auto checkAll = [](const std::string& cmd) -> bool { 
                for(auto it = cmd.begin(); it != cmd.end(); ++it ) 
                      if(*it != '0' && *it != '1' )
                           return false;
                return true;                                
     };
     
     auto checkSingle = [](const std::string& cmd) -> bool {
                 if(cmd[0] == '0' || cmd[0] == '1')
                       return cmd[1] >= '1' && cmd[1] <= '8' ? true : false;
                 else
                       return false;               
     };
     
     size_t found = request.find(cmdSep);
     if (found != std::string::npos){
        std::string cmd = request.substr(found)
                                 .erase(0, cmdSep.size());
        cmd.erase(cmd.find(" "));

        Serial.print("Cmd: ");
        Serial.println(cmd.c_str());
        
        if(cmd.size() == status.size()){
                 if(checkAll(cmd)){
                    setStatus(cmd);
                    printStatus();
                    data.append("Valid Request: applied");
                 }else{
                    data.append("Invalid Request: wrong status");
                 }
        } else if (cmd.size() == singlePinCmdLen){
                 if(checkSingle(cmd)){
                     // TODO: stoul, when available
                    setPinStatus(cmd[0] == '1' ? true : false,
                                 static_cast<size_t>(std::atoi(cmd.c_str()+1)));
                    printStatus();
                    data.append("Valid Request: applied");
                 }else{
                    data.append("Invalid Request: wrong status");
                 }
        } else {
                 if( request.find(infoSep) != std::string::npos )
                     data.append(getStatusString()); 
                 else 
                     data.append("Invalid Request: wrong size");
        }
     }else{
        data.append("Invalid Request: wrong syntax");
     }
  }

  void RelayBoard::responder(void) noexcept{
     WiFiClient client = server.available();
     if (!client) 
       return;
 
     Serial.println("New client");
     while(!client.available())
        delay(1);

     client.readStringUntil('\r').toCharArray(buff.data(), buff.size());
     request = buff.data();
  
     std::string data,
                 response("HTTP/1.1 200 OK\nAccept-Ranges: bytes\nContent-Length: ");
                 
     setter(data);

     // TODO: use to_string() or stringstream when available
     snprintf(buff.data(), buff.size(), "%i", data.size());
     std::string nFreq(buff.data());
     response.append(nFreq.c_str()).append("\nConnection: close\nContent-Type: text/plain\n\n")
             .append(data.c_str()).append("\n");

     std::string info("Request: ");
     info.append(request).append("\nResponse: ").append(response);
  
     Serial.println(info.c_str());
     client.print(response.c_str());
     client.flush();

     delay(1);
     Serial.print("Client disonnected.\n");
  }
   
} // End Namespace
