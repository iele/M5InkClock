#include <M5Unified.h>

using namespace std;

const double PRECISION = 1E-6;
const int COUNT_OF_NUMBER = 4;
const int NUMBER_TO_BE_CAL = 24;
double number[COUNT_OF_NUMBER];
string expression[COUNT_OF_NUMBER];
bool Search(int n)
{
    if (n == 1)
    {
        if (fabs(number[0] - NUMBER_TO_BE_CAL) < PRECISION)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double a, b;
            string expa, expb;
            a = number[i];
            b = number[j];
            number[j] = number[n - 1];
            expa = expression[i];
            expb = expression[j];
            expression[j] = expression[n - 1];
            expression[i] = '(' + expa + '+' + expb + ')';
            number[i] = a + b;
            if (Search(n - 1))
                return true;
            expression[i] = '(' + expa + '-' + expb + ')';
            number[i] = a - b;
            if (Search(n - 1))
                return true;
            expression[i] = '(' + expb + '-' + expa + ')';
            number[i] = b - a;
            if (Search(n - 1))
                return true;
            expression[i] = '(' + expa + '*' + expb + ')';
            number[i] = a * b;
            if (Search(n - 1))
                return true;
            if (b != 0)
            {
                expression[i] = '(' + expa + '/' + expb + ')';
                number[i] = a / b;
                if (Search(n - 1))
                    return true;
            }
            if (a != 0)
            {
                expression[i] = '(' + expb + '/' + expa + ')';
                number[i] = b / a;
                if (Search(n - 1))
                    return true;
            }
            number[i] = a;
            number[j] = b;
            expression[i] = expa;
            expression[j] = expb;
        }
    }
    return false;
}

void twenty_four_app()
{
    bool answered = false;

    while (true)
    {
        M5.update();
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.fillScreen(TFT_WHITE);
        for (int i = 0; i < 4; i++)
        {
            number[i] = random(1, 11); // 生成1到12之间的随机数
        }
        // 显示生成的四个随机数
        M5.Display.startWrite();
        M5.Display.drawRoundRect(10, 10, M5.Display.width() - 20, M5.Display.height() - 60, 10, TFT_BLACK);
        M5.Display.drawFastHLine(10, (M5.Display.height() - 60 + 10) / 2, M5.Display.width() - 20, TFT_BLACK);
        M5.Display.drawFastVLine(M5.Display.width() / 2, 10, M5.Display.height() - 60, TFT_BLACK);
        displayText(String((int)number[0]).c_str(), &Font7, M5.Display.width() / 2 - 45, 20, ALIGN_CENTER, 1);
        displayText(String((int)number[1]).c_str(), &Font7, M5.Display.width() / 2 + 45, 20, ALIGN_CENTER, 1);
        displayText(String((int)number[2]).c_str(), &Font7, M5.Display.width() / 2 - 45, 90, ALIGN_CENTER, 1);
        displayText(String((int)number[3]).c_str(), &Font7, M5.Display.width() / 2 + 45, 90, ALIGN_CENTER, 1);
        M5.Display.drawRoundRect(10, M5.Display.height() - 45, M5.Display.width() - 20, 35, 10, TFT_BLACK);
        displayText("答案？", &efontCN_24, 20, M5.Display.height() - 41, ALIGN_LEFT, 1);
        M5.Display.endWrite();
        M5.Display.display();
        M5.Display.waitDisplay();

        while (true)
        {
            M5.update();
            if (M5.BtnB.wasClicked())
            {
                if (!answered)
                {
                    answered = true;
                    M5.Display.setFont(&Font0);

                    for (int i = 0; i < COUNT_OF_NUMBER; i++)
                    {
                        char buffer[20];
                        itoa(number[i], buffer, 10);
                        expression[i] = buffer;
                    }

                    if (Search(COUNT_OF_NUMBER))
                    {
                        displayText("          ", &efontCN_24, 20, M5.Display.height() - 41, ALIGN_LEFT, 1);
                        displayText(expression[0].c_str(), &Font2, 20, M5.Display.height() - 37, ALIGN_LEFT, 1);
                    }
                    else
                    {
                        displayText("无法计算", &efontCN_24, 20, M5.Display.height() - 41, ALIGN_LEFT, 1);
                    }
                }
                else
                {
                    answered = false;
                    break;
                }
            }
            if (M5.BtnB.wasHold())
            {
                return;
            }
            delay(100);
        }
    }
}