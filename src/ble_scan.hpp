#pragma once

#include <M5Unified.h>
#include <BLEDevice.h>
#include "utils.hpp"

void ble_scan_app()
{
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.fillScreen(TFT_WHITE);

    M5.Display.setFont(&AsciiFont8x16);
    M5.Display.setTextColor(TFT_BLACK);

    BLEDevice::init("");

    while (true)
    {
        BLEScan *pBLEScan = BLEDevice::getScan();
        auto result = pBLEScan->start(1);
        auto total = result.getCount();
        auto base = 0;
        bool refresh = true;
        M5.Display.startWrite();
        M5.Display.fillScreen(TFT_WHITE);
        displayText("蓝牙扫描中...", &efontCN_24, M5.Display.width() / 2, 80, ALIGN_CENTER, 1);
        M5.Display.endWrite();
        while (true)
        {
            if (refresh)
            {
                if (total != 0)
                {
                    M5.Display.startWrite();
                    M5.Display.fillScreen(TFT_WHITE);
                    char page[10];
                    sprintf(page, "%d/%d", base + 1, total);
                    displayText(page, &Font0, 5, 5, ALIGN_LEFT, 1);

                    M5.Display.drawRect(M5.Display.width() - 15, 0, 15, M5.Display.height(), TFT_BLACK);
                    M5.Display.fillRect(M5.Display.width() - 15, M5.Display.height() * base / total, 15, M5.Display.height() * 2 / total, TFT_BLACK);
                    auto y = 20;
                    for (auto i = base; i < base + 2 || i < total; i++)
                    {
                        if (result.getDevice(i).haveName())
                        {
                            displayText(result.getDevice(i).getName().c_str(), &efontCN_24, M5.Display.width() / 2, y, ALIGN_CENTER, 1);
                        }
                        else
                        {
                            displayText("未知设备", &efontCN_24, M5.Display.width() / 2, y, ALIGN_CENTER, 1);
                        }
                        y += 30;
                        displayText(result.getDevice(i).getAddress().toString().c_str(), &AsciiFont8x16, M5.Display.width() / 2, y, ALIGN_CENTER, 1);
                        y += 20;
                        char info[50] = "";
                        if (result.getDevice(i).haveManufacturerData())
                        {
                            sprintf(info, "%sM:%s ", info, result.getDevice(i).getManufacturerData());
                        }
                        if (result.getDevice(i).haveRSSI())
                        {
                            sprintf(info, "%sR:%d ", info, result.getDevice(i).getRSSI());
                        }
                        if (result.getDevice(i).haveTXPower())
                        {
                            sprintf(info, "%sT:%d ", info, result.getDevice(i).getTXPower());
                        }
                        displayText(info, &AsciiFont8x16, M5.Display.width() / 2, y, ALIGN_CENTER, 1);
                        y += 40;
                    }
                    M5.Display.endWrite();
                    refresh = false;
                }
                else
                {
                    M5.Display.startWrite();
                    M5.Display.fillScreen(TFT_WHITE);
                    displayText("未发现蓝牙", &efontCN_24, M5.Display.width() / 2, 80, ALIGN_CENTER, 1);
                    M5.Display.endWrite();
                }
            }

            M5.update();
            if (M5.BtnB.wasHold())
            {
                return;
            }
            if (M5.BtnA.wasClicked() || M5.BtnA.wasHold())
            {
                base -= 2;
                if (base < 0)
                    base = 0;
                refresh = true;
            }
            if (M5.BtnC.wasClicked() || M5.BtnC.wasHold())
            {
                base += 2;
                if (base >= total)
                    base = total - 2;
                refresh = true;
            }
            delay(10);
        }
    }
}