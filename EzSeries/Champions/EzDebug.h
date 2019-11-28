#pragma once
#include "EzChampion.h"


class EzDebug : public EzChampion {
  public:

    static std::map<float, IGameObject *> OrnObjs;
    static auto on_create(IGameObject * unit) -> void;
    static auto on_delete(IGameObject * unit);
    static IMenu * on_boot(IMenu * menu);
    static void on_update();
    static auto orn_r() -> IGameObject*;
    static auto orn_q_pillar() -> IGameObject*;
};

// for Biggie
std::map<float, IGameObject *> EzDebug::OrnObjs;

inline auto EzDebug::on_create(IGameObject * unit) -> void {

    // - my method of temporarily storing important objects without OnDelete
    if(unit != nullptr && strstr(unit->Name().c_str(), "OrnnRWave")) {

        // - storing game time into map
        OrnObjs[g_Common->Time()] = unit;
    }

    if(unit != nullptr && strstr(unit->Name().c_str(), "OrnnQPillar")) {

        // - storing game time into map
        OrnObjs[g_Common->Time()] = unit;
    }
}


inline IMenu * EzDebug::on_boot(IMenu * menu) {
    return menu;
}

inline void EzDebug::on_update() {

    for(auto o : OrnObjs) {
        auto key = o.first;
        auto obj = o.second;

        // - delete null maps
        if(obj == nullptr || !obj->IsVisible() || !obj->IsValid()) {
            OrnObjs.erase(key);
            break;
        }

        // - fail safe delete after 8 seconds
        if(g_Common->Time() - key > 7) {
            OrnObjs.erase(key);
            break;
        }
    }

}

inline auto EzDebug::orn_r() -> IGameObject * {
    for(auto o : OrnObjs) {
        auto obj = o.second;

        if(strstr(obj->Name().c_str(), "OrnnRWave")) {
            return obj;
        }
    }

    // - if nothing is found in the map
    return nullptr;
}

inline auto EzDebug::orn_q_pillar() -> IGameObject * {
    for(auto o : OrnObjs) {
        auto obj = o.second;

        if(strstr(obj->Name().c_str(), "OrnnQPillar")) {
            return obj;
        }
    }

    // - if nothing is found in the map
    return nullptr;
}
