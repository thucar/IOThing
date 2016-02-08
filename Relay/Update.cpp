#include "Config.h"
#include <ESP8266httpUpdate.h>

void checkUpdate( void ){
  
        t_httpUpdate_return ret = ESPhttpUpdate.update(UPDATE_URL,VERSION);
        //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

        switch(ret) {
            case HTTP_UPDATE_FAILED:
//                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
//                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
//                USE_SERIAL.println("HTTP_UPDATE_OK");
                break;
    }
}
