#pragma once
#include "../SDK/PluginSDK.h"

class EzVayne : public EzChampion {
    public:
        static IMenu * on_load(IMenu * menu);
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline IMenu * EzVayne::on_load(IMenu * menu) {
    ezspells["vayne.q"] = g_Common->AddSpell(SpellSlot::Q);
    ezspells["vayne.e"] = g_Common->AddSpell(SpellSlot::E, 550);
    ezspells["vayne.r"] = g_Common->AddSpell(SpellSlot::R);
    return menu; }




inline void EzVayne::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(unit->IsMe()) {
        if(args->IsAutoAttack) {
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
                // AA -> E
                if(ezspells["vayne.e"]->IsReady() && args->Target->IsAIHero()) {
                    if(args->Target->Distance(unit) <= ezspells["vayne.e"]->Range()) {
                        ezspells["vayne.e"]->Cast(args->Target); } }

                // AA -> Q
                if(ezspells["vayne.q"]->IsReady()) {
                    if(ezspells["vayne.q"]->Cast(g_Common->CursorPosition())) {
                        g_Orbwalker->ResetAA(); } } } } } }
