#pragma once

#include <M5UnitENV.h>

SHT3X sht3x;
QMP6988 qmp;

void env_begin()
{
    qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 25, 26, 400000U);
    sht3x.begin(&Wire, SHT3X_I2C_ADDR, 25, 26, 400000U);
}