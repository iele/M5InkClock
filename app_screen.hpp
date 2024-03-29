#include <M5Unified.h>
#include <efontEnableCn.h>
#include <efontFontData.h>

#include "twenty_four.hpp"
#include "wifi_scan.hpp"

#define APPS 3
const char *apps[] = {"WiFi扫描器", "24点计算器", "返回"};
int selected_apps = 0;

void appScreen()
{
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.fillScreen(TFT_WHITE);

    while (true)
    {
        if (M5.BtnA.wasClicked())
        {
            if (selected_apps <= 0)
            {
                selected_apps = APPS - 1;
            }
            else
            {
                selected_apps = (--selected_apps) % APPS;
            }
        }
        else if (M5.BtnC.wasClicked())
        {
            if (selected_apps >= APPS - 1)
            {
                selected_apps = 0;
            }
            else
            {
                selected_apps = (++selected_apps) % APPS;
            }
        }
        else if (M5.BtnB.wasClicked())
        {
            switch (selected_apps)
            {
            case 0:
                wifi_scan_app();
                break;
            case 1:
                twenty_four_app();
                break;
            case 2:
            case 3:
            case 4:
                return;
                break;
            }
        }
        else if (M5.BtnPWR.wasHold() || M5.BtnB.wasHold())
        {
            return;
        }

        M5.Display.startWrite();
        M5.Display.fillScreen(TFT_WHITE);
        M5.Display.drawRoundRect(10, 10, M5.Display.width() - 20, M5.Display.width() - 20, 5, TFT_BLACK);

        M5.Display.setFont(&efontCN_24);
        M5.Display.setTextSize(1);
        for (int i = 0; i < APPS; i++)
        {
            M5.Display.setCursor(20, 20 + i * 30);
            if (i == selected_apps)
            {
                M5.Display.fillRect(10, 20 + i * 30 - 2, M5.Display.width() - 20, 24 + 4, TFT_BLACK);
                M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
            }
            else
            {
                M5.Display.fillRect(10 + 1, 20 + i * 30 - 2, M5.Display.width() - 20 - 2, 24 + 4, TFT_WHITE);
                M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
            }
            M5.Display.println(apps[i]);
        }
        M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
        M5.Display.clearClipRect();

        M5.Display.endWrite();
        M5.Display.display();
        M5.Display.waitDisplay();

        M5.update();
    }
}