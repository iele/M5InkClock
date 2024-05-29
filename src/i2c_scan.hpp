#pragma once

#include <M5Unified.h>
#include <Wire.h>
#include "utils.hpp"

void i2c_scan_app()
{

    byte error, address;
    int nDevices;

    while (true)
    {
        M5.Display.fillScreen(TFT_WHITE);

        Serial.println("HAT");

        Wire.begin(25, 26, 400000U);
        Wire.setTimeOut(10);
        M5.Display.startWrite();
        displayText("HAT:", &efontCN_24, 20, 20, ALIGN_LEFT, 1);

        auto found = 0;
        for (address = 1; address < 127; address++)
        {
            Serial.println(address);
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0)
            {
                char addr_str[10];
                sprintf(addr_str, "0X%2X", address);
                displayText(addr_str, &Font2, 30 + 40 * found, 50, ALIGN_LEFT, 1);
                found++;
            }
        }
        if (found == 0)
        {
            displayText("Not Found", &Font2, 30, 50, ALIGN_LEFT, 1);
        }
        M5.Display.endWrite();
        Wire.end();

        Serial.println("EXT");

        Wire.begin(32, 33, 400000U);
        Wire.setTimeOut(10);
        M5.Display.startWrite();
        found = 0;
        displayText("Ext.PORT:", &efontCN_24, 20, 75, ALIGN_LEFT, 1);
        for (address = 1; address < 127; address++)
        {
            Serial.println(address);
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0)
            {
                char addr_str[10];
                sprintf(addr_str, "0X%2X", address);
                displayText(addr_str, &Font2, 30 + 40 * found, 105, ALIGN_LEFT, 1);
                found++;
            }
        }
        if (found == 0)
        {
            displayText("Not Found", &Font2, 30, 105, ALIGN_LEFT, 1);
        }
        M5.Display.endWrite();
        Wire.end();

        Serial.println("Int. & Bus");

        Wire.begin(21, 22, 400000U);
        Wire.setTimeOut(10);
        M5.Display.startWrite();
        found = 0;
        displayText("Int./Bus:", &efontCN_24, 20, 130, ALIGN_LEFT, 1);
        for (address = 1; address < 127; address++)
        {
            Serial.println(address);
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0)
            {
                char addr_str[10];
                sprintf(addr_str, "0X%2X", address);
                displayText(addr_str, &Font2, 30 + 40 * found, 160, ALIGN_LEFT, 1);
                found++;
            }
        }
        if (found == 0)
        {
            displayText("Not Found", &Font2, 30, 160, ALIGN_LEFT, 1);
        }
        M5.Display.endWrite();
        Wire.end();

        while (true)
        {
            M5.update();
            bool need_update = false;
            if (M5.BtnB.wasHold())
            {
                return;
            }
            if (M5.BtnA.wasClicked() || M5.BtnA.wasHold())
            {
            }
            else if (M5.BtnC.wasClicked() || M5.BtnC.wasHold())
            {
            }

            else if (M5.BtnB.wasClicked())
            {
                break;
            }
            delay(10);
        }
    }
}
