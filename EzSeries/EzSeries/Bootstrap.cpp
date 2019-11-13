#pragma once
#include "SDK\EventHandler.h"
#include "Champions\EzJinx.h"
#include "Champions\EzTristana.h"
#include "Champions\EzKaisa.h"
#include "Champions\EzVayne.h"

// oh dis...
PLUGIN_API const char PLUGIN_PRINT_NAME[32] = "EzSeries";
PLUGIN_API const char PLUGIN_PRINT_AUTHOR[32] = "Kurisu";
PLUGIN_API ChampionId PLUGIN_TARGET_CHAMP = ChampionId::Unknown;

IMenu * Config = {};

auto on_delete(IGameObject * unit) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Jinx:
            return EzJinx::on_delete_obj(unit);

        default: ;
    }
}

auto on_update() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_update();

        case ChampionId::Jinx:
            return EzJinx::on_update();

        case ChampionId::Kaisa:
            return EzKaisa::on_update();

        default: ;
    }
}

auto on_draw() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_huddraw();

        case ChampionId::Jinx:
            return EzJinx::on_huddraw();

        case ChampionId::Kaisa:
            return EzKaisa::on_huddraw();
    }
}

auto on_hpbar_draw() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_hpbardraw();

        case ChampionId::Jinx:
            return EzJinx::on_hpbar_draw();

        case ChampionId::Kaisa:
            return EzKaisa::on_hpbardraw();
    }
}

auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) { }
}

auto on_cast_spell(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_dash(unit, args);

        case ChampionId::Kaisa:
            return EzKaisa::on_dash(unit, args);

        default: ;
    }
}

auto on_play_animation(IGameObject * unit, OnPlayAnimationEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) { }
}

auto on_before_attack(BeforeAttackOrbwalkerArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_before_attack(args);

        case ChampionId::Jinx:
            return EzJinx::on_before_attack(args);

        case ChampionId::Kaisa:
            return EzKaisa::on_before_attack(args);

        default: ;
    }
}

auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return;
    }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Jinx:
            return EzJinx::on_do_cast(unit, args);

        case ChampionId::Kaisa:
            return EzKaisa::on_do_cast(unit, args);

        default: ;
    }
}

auto build_menu(IMenu * menu) -> IMenu * {

    switch(g_LocalPlayer->ChampionId()) {

        case ChampionId::Tristana:
            return EzTristana::on_load(menu);

        case ChampionId::Jinx:
            return EzJinx::on_boot(menu);

        case ChampionId::Kaisa:
            return EzKaisa::on_load(menu);

        case ChampionId::Vayne:
            return EzVayne::on_load(menu);

        default: {
                g_Common->ChatPrint(R"(<font color="#99FF99"><b>EzSeries:</b></font><b><font color="#FF3366"> No Support!</font>)");
            }
    }
}

PLUGIN_API bool OnLoadSDK(IPluginsSDK * plugin_sdk) {
    DECLARE_GLOBALS(plugin_sdk);
    EventHandler<Events::GameUpdate>::AddEventHandler(on_update);
    EventHandler<Events::OnHudDraw>::AddEventHandler(on_draw);
    EventHandler<Events::OnHPBarDraw>::AddEventHandler(on_hpbar_draw);
    EventHandler<Events::OnBuff>::AddEventHandler(on_buff);
    EventHandler<Events::OnDeleteObject>::AddEventHandler(on_delete);
    EventHandler<Events::OnPlayAnimation>::AddEventHandler(on_play_animation);
    EventHandler<Events::OnProcessSpellCast>::AddEventHandler(on_cast_spell);
    EventHandler<Events::OnBeforeAttackOrbwalker>::AddEventHandler(on_before_attack);
    EventHandler<Events::OnDoCast>::AddEventHandler(on_do_cast);
    Config = build_menu(g_Menu->CreateMenu("EzSeries", "EzSeries"));
    return true;
}

PLUGIN_API void OnUnloadSDK() {
    EventHandler<Events::GameUpdate>::RemoveEventHandler(on_update);
    EventHandler<Events::OnHudDraw>::RemoveEventHandler(on_draw);
    EventHandler<Events::OnHPBarDraw>::RemoveEventHandler(on_hpbar_draw);
    EventHandler<Events::OnBuff>::RemoveEventHandler(on_buff);
    EventHandler<Events::OnDeleteObject>::RemoveEventHandler(on_delete);
    EventHandler<Events::OnPlayAnimation>::RemoveEventHandler(on_play_animation);
    EventHandler<Events::OnProcessSpellCast>::RemoveEventHandler(on_cast_spell);
    EventHandler<Events::OnBeforeAttackOrbwalker>::RemoveEventHandler(on_before_attack);
    EventHandler<Events::OnDoCast>::RemoveEventHandler(on_do_cast);
    Config->Remove();
}
