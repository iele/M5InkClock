#pragma once

#include <M5Unified.h>

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_CENTER 2

void displayText(const char *text, const lgfx::v1::IFont *font, int32_t x, int32_t y, uint8_t align, float size)
{
    M5.Display.setFont(font);
    M5.Display.setTextSize(size);
    switch (align)
    {
    case ALIGN_LEFT:
        M5.Display.drawString(text, x, y);
        break;
    case ALIGN_CENTER:
        M5.Display.drawCenterString(text, x, y);
        break;
    case ALIGN_RIGHT:
        M5.Display.drawRightString(text, x, y);
        break;
    }
}