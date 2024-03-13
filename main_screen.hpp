#include <M5Unified.h>
#include <efontEnableCn.h>
#include <efontFontData.h>
#include "time.h"

#include "env_hat.hpp"
#include "utils.hpp"

typedef struct
{
    float temperature;
    float humidity;
} TempHumData;

#define DATA_POINTS 60

RTC_DATA_ATTR time_t stat_time;
RTC_DATA_ATTR int stat_count = 0;
RTC_DATA_ATTR TempHumData temp_hum_data[DATA_POINTS];

void storeTempHumData(float temperature, float humidity)
{
    time_t now;
    time(&now);
    bool need_shift = false;
    if (now > stat_time + 24 * 60)
    {
        need_shift = true;
        stat_time = now;
        stat_count = 0;
    }
    else
    {
        stat_count++;
    }

    if (need_shift)
    {
        for (int i = 0; i < DATA_POINTS - 1; i++)
        {
            temp_hum_data[i].temperature = temp_hum_data[i + 1].temperature;
            temp_hum_data[i].humidity = temp_hum_data[i + 1].humidity;
        }
        temp_hum_data[DATA_POINTS - 1].temperature = temperature;
        temp_hum_data[DATA_POINTS - 1].humidity = humidity;
    }
    else
    {
        temp_hum_data[DATA_POINTS - 1].temperature = (temp_hum_data[DATA_POINTS - 1].temperature * stat_count + temperature) / (stat_count + 1);
        temp_hum_data[DATA_POINTS - 1].humidity = (temp_hum_data[DATA_POINTS - 1].humidity * stat_count + humidity) / (stat_count + 1);
    }
}

void updateSensorData()
{
    bool temp_avaliable = sht3x.update();
    if (temp_avaliable)
    {
        float temperature = sht3x.cTemp;
        float humidity = sht3x.humidity;
        storeTempHumData(temperature, humidity);
    }
    else
    {
        storeTempHumData(-0XFF, -0XFF);
    }
}

void drawGraph()
{
    // 用于绘图的起始坐标
    int startX = 30;
    int startY = 125;
    int endX = 170;
    int endY = 185;

    // 清除图表区域
    M5.Display.fillRect(startX, startY, endX - startX, endY - startY, TFT_WHITE);

    // 绘制坐标轴
    M5.Display.drawLine(startX, endY, endX, endY, TFT_BLACK);     // X 轴
    M5.Display.drawLine(startX, startY, startX, endY, TFT_BLACK); // Y 轴
    M5.Display.drawLine(endX, startY, endX, endY, TFT_BLACK);     // Y 轴

    // 绘制坐标轴标签
    M5.Display.setFont(&Font0);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setCursor(endX - 10, endY - 10);

    int32_t temp_min = 10;
    int32_t temp_max = 30;
    int32_t hum_min = 0;
    int32_t hum_max = 100;

    for (int i = 0; i < DATA_POINTS; i++)
    {
        float temp = temp_hum_data[i].temperature;
        float hum = temp_hum_data[i].humidity;
        if (temp < temp_min && temp > -60)
            temp_min = temp;
        if (temp > temp_max && temp < 60)
            temp_max = temp;
    }

    // 绘制温度线图
    float ratio = ((float)endX - startX) / DATA_POINTS;
    for (int i = 0; i < DATA_POINTS - 1; i++)
    {
        int x1 = i;
        int x2 = i + 1;
        if (x1 < x2 && temp_hum_data[x1].temperature != -0XFF && temp_hum_data[x2].temperature != -0XFF)
        {
            M5.Display.drawLine(
                (ratio * x1) + startX,
                map(temp_hum_data[x1].temperature, temp_max, temp_min, startY, endY),
                (ratio * x2) + startX,
                map(temp_hum_data[x2].temperature, temp_max, temp_min, startY, endY),
                TFT_BLACK);
        }
    }

    // 绘制湿度线图
    for (int i = 0; i < DATA_POINTS - 1; i += 2)
    {
        int x1 = i;
        int x2 = i + 1;
        if (x1 < x2 && temp_hum_data[x1].humidity != -0XFF && temp_hum_data[x2].humidity != -0XFF)
        {

            M5.Display.drawLine(
                (ratio * x1) + startX,
                map(temp_hum_data[x1].humidity, hum_max, hum_min, startY, endY),
                (ratio * x2) + startX,
                map(temp_hum_data[x2].humidity, hum_max, hum_min, startY, endY), TFT_BLACK);
        }
    }

    int y = map(temp_min, temp_max, temp_min, startY, endY);
    M5.Display.drawLine(startX - 5, y, startX, y, TFT_BLACK);
    displayText(String(int(temp_min)).c_str(), &Font0, startX - 10, y - 3, ALIGN_RIGHT, 1);
    y = map((temp_min + temp_max) / 2, temp_max, temp_min, startY, endY);
    M5.Display.drawLine(startX - 5, y, startX, y, TFT_BLACK);
    displayText(String(int(temp_min + temp_max) / 2).c_str(), &Font0, startX - 10, y - 3, ALIGN_RIGHT, 1);
    y = map(temp_max, temp_max, temp_min, startY, endY);
    M5.Display.drawLine(startX - 5, y, startX, y, TFT_BLACK);
    displayText(String(int(temp_max)).c_str(), &Font0, startX - 10, y - 3, ALIGN_RIGHT, 1);

    y = map(hum_min, hum_max, hum_min, startY, endY);
    M5.Display.drawLine(endX + 5, y, endX, y, TFT_BLACK);
    displayText(String(int(hum_min)).c_str(), &Font0, endX + 10, y - 3, ALIGN_LEFT, 1);
    y = map((hum_min + hum_max) / 2, hum_max, hum_min, startY, endY);
    M5.Display.drawLine(endX + 5, y, endX, y, TFT_BLACK);
    displayText(String(int(hum_min + hum_max) / 2).c_str(), &Font0, endX + 10, y - 3, ALIGN_LEFT, 1);
    y = map(hum_max, hum_max, hum_min, startY, endY);
    M5.Display.drawLine(endX + 5, y, endX, y, TFT_BLACK);
    displayText(String(int(hum_max)).c_str(), &Font0, endX + 10, y - 3, ALIGN_LEFT, 1);
}

const char *months[12] = {"一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月",
                          "十一月", "十二月"};

const char *days[32] = {"", "一日", "二日", "三日", "四日", "五日", "六日", "七日", "八日", "九日", "十日",
                        "十一日", "十二日", "十三日", "十四日", "十五日", "十六日", "十七日", "十八日", "十九日", "二十日",
                        "二十一", "二十二", "二十三", "二十四", "二十五", "二十六", "二十七", "二十八", "二十九", "三十日",
                        "三十一"};

const char *week[7] = {"日", "一", "二", "三", "四", "五", "六"};

void mainScreen()
{
    auto timeinfo = M5.Rtc.getDateTime().get_tm();
    auto tm = mktime(&timeinfo);
    auto ltm = (tm + 60 * 60 * 8);
    timeinfo = *gmtime(&ltm);

    M5.Display.startWrite();
    M5.Display.fillScreen(TFT_WHITE);

    // Power ----------------------------------------------------------
    int32_t level = M5.Power.getBatteryLevel();
    int32_t volt = M5.Power.getBatteryVoltage();
    M5.Display.startWrite();
    char level_str[4];
    sprintf(level_str, "%d%%", level);
    char volt_str[10];
    sprintf(volt_str, "%.1fV", (float)volt/1000);
    M5.Display.drawRect(0, 0, M5.Display.width() - 60, 8, TFT_BLACK);
    M5.Display.fillRect(0, 0, ((float)level / 100 * (M5.Display.width() - 60)), 8, TFT_BLACK);
    displayText(level_str, &Font0, M5.Display.width() - 30, 0, ALIGN_RIGHT, 1);
    displayText(volt_str, &Font0, M5.Display.width(), 0, ALIGN_RIGHT, 1);
    M5.Display.endWrite();

    // Date ----------------------------------------------------------
    M5.Display.startWrite();
    char date_str[30];
    char week_str[30];
    sprintf(date_str, "%s%s ", months[timeinfo.tm_mon], days[timeinfo.tm_mday]);
    sprintf(week_str, "%s", week[timeinfo.tm_wday]);
    M5.Display.setFont(&efontCN_24);
    auto width1 = M5.Display.textWidth(date_str);
    auto width2 = M5.Display.textWidth(week_str);
    auto width = width1 + width2;
    displayText(date_str, &efontCN_24, (M5.Display.width() - width) / 2, 25, ALIGN_LEFT, 1);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.fillRect((M5.Display.width() + width) / 2 - width2 - 3, 25 - 3, 30, 30, TFT_BLACK);
    displayText(week_str, &efontCN_24, (M5.Display.width() + width) / 2, 25, ALIGN_RIGHT, 1);
    M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
    M5.Display.endWrite();

    bool temp_avaliable = sht3x.update();
    bool press_avaliable = qmp.update();
    // Hour ----------------------------------------------------------
    M5.Display.startWrite();
    char hour_str[6];
    sprintf(hour_str, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    displayText(hour_str, &Font4,
                M5.Display.width() / 2 - ((temp_avaliable && press_avaliable) ? 23 : 0), 65,
                ALIGN_CENTER, 2);
    M5.Display.endWrite();

    if (temp_avaliable && press_avaliable)
    {
        if ((tm / 60) % 2)
        {
            // Temp ----------------------------------------------------------
            M5.Display.startWrite();
            char temp_str[5];
            sprintf(temp_str, "%2.1fC", sht3x.cTemp);
            displayText(temp_str, &Font2, M5.Display.width() / 2 + 67, 69, ALIGN_CENTER, 1);
            M5.Display.endWrite();
            // Hum ----------------------------------------------------------
            M5.Display.startWrite();
            char hum_str[5];
            sprintf(hum_str, "%0.1f%%", sht3x.humidity);
            displayText(hum_str, &Font2, M5.Display.width() / 2 + 67, 89, ALIGN_CENTER, 1);
            M5.Display.endWrite();
        }
        else
        {
            // Temp ----------------------------------------------------------
            M5.Display.startWrite();
            char press_str[10];
            sprintf(press_str, "%.0fkPa", qmp.pressure / 1000);
            displayText(press_str, &Font2, M5.Display.width() / 2 + 67, 69, ALIGN_CENTER, 1);
            M5.Display.endWrite();
            // Hum ----------------------------------------------------------
            M5.Display.startWrite();
            char alt_str[10];
            sprintf(alt_str, "%0.0fM", qmp.altitude);
            displayText(alt_str, &Font2, M5.Display.width() / 2 + 67, 89, ALIGN_CENTER, 1);
            M5.Display.endWrite();
        }
    }

    M5.Display.startWrite();
    updateSensorData();
    drawGraph();
    M5.Display.endWrite();

    M5.Display.endWrite();
    M5.Display.display();
    M5.Display.waitDisplay();
}
