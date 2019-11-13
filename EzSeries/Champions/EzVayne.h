#pragma once
#include "..\SDK\PluginSDK.h"

class EzVayne {
  public:
    static IMenu * on_load(IMenu * menu);
};

inline IMenu * EzVayne::on_load(IMenu * menu) {
    return menu;
}
