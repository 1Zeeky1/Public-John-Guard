#pragma once

#include "../manager.h"

#include "deviceSpooferA.h"
#include "deviceSpooferB.h"
#include "deviceSpooferC.h"
#include "hwidSpooferA.h"
#include "botA.h"
#include "botB.h"
#include "botC.h"
#include "botD.h"

void registerAntiCheatChecks(AntiCheatManager& acManager) {
    acManager.registerCheck(std::make_unique<BotA>());
    acManager.registerCheck(std::make_unique<BotB>());
    acManager.registerCheck(std::make_unique<BotC>());
    acManager.registerCheck(std::make_unique<BotD>());
    acManager.registerCheck(std::make_unique<DeviceSpooferA>());
    acManager.registerCheck(std::make_unique<DeviceSpooferB>());
    acManager.registerCheck(std::make_unique<DeviceSpooferC>());
    acManager.registerCheck(std::make_unique<HwidSpooferA>());
}