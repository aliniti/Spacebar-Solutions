#pragma once
#include "EzChampion.h"


class EzVayne : public EzChampion {
    public:
        static auto on_boot(IMenu * menu) -> IMenu*;
        static auto on_update() -> void;
        static auto on_draw() -> void;
        static auto hpbarfill_render() -> void;
        static auto condemn_target(IGameObject * unit) -> void;

        static auto on_create(IGameObject * unit) -> void;
        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void;
        static auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void; };

inline IMenu * EzVayne::on_boot(IMenu * menu) {

    Spells["vayne.q"] = g_Common->AddSpell(SpellSlot::Q);
    Spells["vayne.e"] = g_Common->AddSpell(SpellSlot::E, 550);
    Spells["vayne.r"] = g_Common->AddSpell(SpellSlot::R);

    auto e_settings = menu->AddSubMenu("Condemn Settings", "vayne.condemn");
    Menu["use.vayne.e.pred"] = e_settings->AddCheckBox("Use Movement Prediction", "use.vayne.e.pred", false);
    Menu["use.vayne.e.auto"] = e_settings->AddCheckBox("Condemn Only Combo", "use.vayne.e.auto", false);
    Menu["use.vayne.e.dist"] = e_settings->AddSlider("Condemn Distance: ", "use.vayne.e.dist", 420, 100, 475);

    Menu["use.vayne.q"] = menu->AddCheckBox("Use Tumble", "use.vayne.q", true);
    Menu["use.vayne.e"] = menu->AddCheckBox("Use Condemn", "use.vayne.e",  true);

    return menu; }

inline void EzVayne::on_update() {

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo) ||
        g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass) ||
        g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear)) {

        const auto push_size = Menu["use.vayne.e.dist"]->GetInt();
        auto target = g_Common->GetTarget(Spells["vayne.e"]->Range() + push_size, DamageType::Physical);

        condemn_target(target); }

    if(!Menu["use.vayne.e.auto"]->GetBool()) {
        for(auto u : g_ObjectManager->GetChampions()) {
            if(u->IsEnemy()) {
                condemn_target(u); } } } }

inline auto EzVayne::condemn_target(IGameObject * unit) -> void {
    const auto push_size = Menu["use.vayne.e.dist"]->GetInt();

    if(unit != nullptr && Menu["use.vayne.e"]->GetBool() && unit->IsValidTarget(Spells["vayne.e"]->Range())) {
        auto end_pos = unit->ServerPosition().Extend(g_LocalPlayer->ServerPosition(), -push_size);
        auto target_pos =  Menu["use.vayne.e.pred"]->GetBool()
            ? g_Common->GetPrediction(unit, g_Common->Ping() / 1000).UnitPosition
            : unit->ServerPosition();

        // - simple condemn xd
        for(auto i = 0; i < target_pos.Distance(end_pos); i += 20) {
            auto v = target_pos + (end_pos - target_pos).Normalized() * i;

            if(v.IsWall()) {
                Spells["vayne.e"]->Cast(unit);
                break; } } } }

inline void EzVayne::on_draw() {

    auto push_size = Menu["use.vayne.e.dist"]->GetInt();
    auto target = g_Common->GetTarget(Spells["vayne.e"]->Range() + push_size, DamageType::Physical);

    if(target != nullptr && Menu["use.vayne.e"]->GetBool() && target->IsValidTarget(Spells["vayne.e"]->Range())) {
        auto end_pos = target->ServerPosition().Extend(g_LocalPlayer->ServerPosition(), -push_size);
        auto target_pos = Menu["use.vayne.e.pred"]->GetBool()
            ? g_Common->GetPrediction(target, g_Common->Ping() / 1000).UnitPosition
            : target->ServerPosition();

        for(auto i = 0; i < target_pos.Distance(end_pos); i+= 20) {

            auto v = target_pos + (end_pos - target_pos).Normalized() * i;

            g_Drawing->AddLineOnScreen(target_pos.WorldToScreen(), v.WorldToScreen(), RGBA(204, 102, 102, 115));
            g_Drawing->AddCircle(v, 5, RGBA(204, 102, 102, 115));

            if(v.IsWall()) {
                g_Drawing->AddCircle(v, 8, RGBA(204, 102, 102, 255), 3);
                break; } } } }

inline void EzVayne::hpbarfill_render() {}

inline auto EzVayne::on_create(IGameObject * unit) -> void {}

inline void EzVayne::on_buff(IGameObject * unit, OnBuffEventArgs * args) {}

inline void EzVayne::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(unit->IsMe()) {
        if(args->IsAutoAttack && Menu["use.vayne.q"]->GetBool()) {
            if(args->Target->IsAIHero() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
                if(Spells["vayne.q"]->IsReady()) {
                    Spells["vayne.q"]->Cast(g_Common->CursorPosition()); } } } } }
