#include <M5Unified.h>
#include <WiFi.h>

#include "utils.hpp"

void drawAxes()
{
    M5.Display.drawLine(20, M5.Display.height() - 30, M5.Display.width() - 10, M5.Display.height() - 30, TFT_BLACK);
    M5.Display.drawLine(20, 10, 20, M5.Display.height() - 30, TFT_BLACK);

    // 使用Font0绘制坐标轴标签，并确保标签与轴对齐
    int xLabelX = M5.Display.width() / 2 - M5.Display.textWidth("Channels", &fonts::Font0) / 2;
    int xLabelY = M5.Display.height() - M5.Display.fontHeight(&fonts::Font0) - 5;
    M5.Display.drawString("Channels", xLabelX, xLabelY, &fonts::Font0);

    int yLabelX = 5;
    int yLabelY = 0;
    M5.Display.drawString("RSSI", yLabelX, yLabelY, &fonts::Font0);

    for (int i = 1; i <= 13; i++)
    {
        int x = map(i, 1, 13, 30, M5.Display.width() - 20);
        M5.Display.drawLine(x, M5.Display.height() - 35, x, M5.Display.height() - 25, TFT_BLACK);
        String label = String(i);
        M5.Display.drawString(label, x - M5.Display.textWidth(label, &fonts::Font0) / 2, M5.Display.height() - 25, &fonts::Font0);
    }

    for (int i = 0; i >= -100; i -= 20)
    {
        int y = map(i, 0, -100, M5.Display.height() - 30, 10);
        M5.Display.drawLine(15, y, 25, y, TFT_BLACK);
        if (i > -100)
        { // 避免在边界绘制负100的标签
            String label = String(i);
            M5.Display.drawString(label, 25 - M5.Display.textWidth(label, &fonts::Font0) - 5, y - M5.Display.fontHeight(&fonts::Font0) / 2, &fonts::Font0);
        }
    }
}

void drawPoint(int channel, int rssi, const char *name, bool tag)
{
    int x = map(channel, 1, 13, 30, M5.Display.width() - 20);
    int y = map(abs(rssi), 20, 100, M5.Display.height() - 30, 30);
    M5.Display.fillCircle(x, y, 2, TFT_BLACK);
    if (tag) {
        M5.Display.drawCircle(x, y, 5, TFT_BLACK);
        M5.Display.drawString(name, x + 3, y, &Font0);
        char detail[20];
        sprintf(detail, "ch %d r%d", channel, rssi);
        M5.Display.drawString(detail, x + 3, y+10, &Font0);
    }
}

void drawGraph(int total, int current)
{
    M5.Display.startWrite();
    M5.Display.fillScreen(TFT_WHITE);
    drawAxes();
    for (int i = 0; i < total; i++)
    {
        auto ssid = WiFi.SSID(i);
        auto chan = WiFi.channel(i);
        auto rssi = WiFi.RSSI(i);
        if (rssi > -5) rssi = -5;
        if (rssi < -100) rssi = -100;
        drawPoint(chan, rssi, ssid.c_str(), current == i);
    }
    M5.Display.endWrite();
}

void wifi_scan_app()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    while (true)
    {
        int total = -1;
        int current = -1;
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);
        displayText("WiFi扫描中", &efontCN_24, M5.Display.width() / 2, 80, ALIGN_CENTER, 1);
        WiFi.scanNetworks(true);
        while (true)
        {
            total = WiFi.scanComplete();
            if (total <= -1)
            {
                continue;
            }
            else if (total == 0)
            {
                displayText("未发现WiFi", &efontCN_24, M5.Display.width() / 2, 80, ALIGN_CENTER, 1);
                break;
            }
            else
            {
                M5.Display.fillScreen(TFT_BLACK);
                M5.Display.fillScreen(TFT_WHITE);
                drawGraph(total, current);
                break;
            }
        }

        while (true)
        {
            M5.update();
            bool need_update = false;
            if (M5.BtnB.wasHold())
            {
                WiFi.disconnect(true);
                WiFi.mode(WIFI_OFF);
                return;
            }
            if (M5.BtnA.wasClicked())
            {
                if (current == -1)
                    current = total;
                current--;
                need_update = true;
            }
            else if (M5.BtnC.wasClicked())
            {
                current++;
                if (current == total)
                    current = -1;
                need_update = true;
            }
            if (need_update)
            {
                drawGraph(total, current);
            }

            else if (M5.BtnB.wasClicked())
            {
                break;
            }
            delay(100);
        }
    }
}