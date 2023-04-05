#include <WiFi.h>
#include "RGBString.h"

class ClientManager
{
    public:
        static RGBString receivedData;
        void setupConnection();
};