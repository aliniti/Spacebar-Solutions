#pragma once
#include "EzChampion.h"


class EzKatarina : public EzChampion {
    public:
        static auto on_boot(IMenu * menu) -> IMenu*;
        static auto on_issue_order(IGameObject * unit, OnIssueOrderEventArgs * args) -> void;
        static auto on_update() -> void;
        static auto on_postupdate() -> void;
        static auto on_hud_draw() -> void;
        static auto hpbarfill_render() -> void;

        static std::map<float, Vector> blade_vectors;
        static std::map<float, IGameObject *> all_blades;
        static auto shunpo_position(IGameObject * unit) -> Vector;

        static auto dynamic_range() -> float;
        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args);
        static auto on_create(IGameObject * unit) -> void;
        static auto on_destroy(IGameObject * unit) -> void;
        static auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void;
        static auto on_spell_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void; };

std::map<float, Vector> EzKatarina::blade_vectors;
std::map<float, IGameObject *> EzKatarina::all_blades;

inline auto EzKatarina::on_boot(IMenu * menu) -> IMenu * {
    auto c = menu->AddSubMenu("Katarina: Core", "katarina.core");
    Menu["katarina.use.q"] = c->AddCheckBox("Use Bouncing Blades", "katarina.use.q", true);
    Menu["katarina.use.w"] = c->AddCheckBox("Use Preparation", "katarina.use.w", true);
    Menu["katarina.use.e"] = c->AddCheckBox("Use Shunpo", "katarina.use.e", true);
    Menu["katarina.use.r"] = c->AddCheckBox("Use Death Lotus", "katarina.use.r", true);
    auto d = menu->AddSubMenu("Katarina: Draw", "katarina.draw");
    Menu["katarina.draw.q"] = d->AddCheckBox("Draw Q Range", "katarina.draw.q", true);
    Menu["katarina.draw.q1"] = d->AddColorPicker("Q Range Color", "katarina.draw.q1", 255, 153, 0, 145);
    Menu["katarina.draw.e"] = d->AddCheckBox("Draw E Range", "katarina.draw.w2", true);
    Menu["katarina.draw.e1"] = d->AddColorPicker("E Range Color", "katarina.draw.e1", 255, 153, 0, 145);
    Menu["katarina.draw.r"] = d->AddCheckBox("Draw R Range", "katarina.draw.r", true);
    Menu["katarina.draw.r1"] = d->AddColorPicker("R Range Color", "katarina.draw.r1", 255, 153, 0, 145);
    Menu["katarina.draw.dagger"] = d->AddCheckBox("Debug Shunpo Position", "katarina.draw.dagger", true);
    Menu["katarina.draw.dagger1"] = d->AddColorPicker("Position Color", "katarina.draw.dagger1", 255, 153, 0, 145);
    auto m = menu->AddSubMenu("Katarina: Mechanics", "katarina.mechanics");
    auto death_lotus_menu = m->AddSubMenu("Death Lotus Settings", "death.lotus.menu");
    Menu["katarina.ult.issueorder"] = death_lotus_menu->AddCheckBox("Block Issue Order during Ult", "katarina.ult.issueorder", true);
    Menu["katarina.ult.orbwalk"] = death_lotus_menu->AddCheckBox("Block Orbwalking during Ult", "katarina.ult.orbwalk", true);
    Spells["katarina.q"] = g_Common->AddSpell(SpellSlot::Q, 625);
    Spells["katarina.w"] = g_Common->AddSpell(SpellSlot::W, 340);
    Spells["katarina.e"] = g_Common->AddSpell(SpellSlot::E, 725);
    Spells["katarina.r"] = g_Common->AddSpell(SpellSlot::R, 550);
    g_Common->Log("[EzSeries]: Katarina Loaded!");
    return menu; }

inline auto EzKatarina::on_issue_order(IGameObject * unit, OnIssueOrderEventArgs * args) -> void {
    if(unit->IsMe() && unit->HasBuff("katarinarsound")) {
        args->Process = false; } }

inline auto EzKatarina::on_postupdate() -> void {
    // - dagger handling
    for(auto i : all_blades) {
        auto key = i.first;
        auto blade = i.second;

        if(!blade->IsValid() || blade == nullptr || !blade->IsVisible()) {
            all_blades.erase(key);
            break; }

        if(g_Common->Time() - key > 6) {
            all_blades.erase(key);
            break; } }

    for(auto i : blade_vectors) {
        auto key = i.first;
        auto blade = i.second;

        if(!blade.IsValid()) {
            blade_vectors.erase(key);
            break; }

        if(g_Common->Time() - key > 6) {
            blade_vectors.erase(key);
            break; } } }

inline auto EzKatarina::on_update() -> void {
    // todo:
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        // - shunpo target
        if(Spells["katarina.e"]->IsReady() && Menu["katarina.use.e"]->GetBool()) {
            auto target = g_Common->GetTarget(Spells["katarina.e"]->Range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget() && shunpo_position(target).IsValid()) {
                if(Spells["katarina.e"]->FastCast(shunpo_position(target))) {
                    g_Orbwalker->ResetAA(); }  } }

        // - preparation
        if(Spells["katarina.w"]->IsReady() && Menu["katarina.use.w"]->GetBool()) {
            auto target = g_Common->GetTarget(285, DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                Spells["katarina.w"]->Cast(); } }

        // - bouncing blades
        if(Spells["katarina.q"]->IsReady() && Menu["katarina.use.q"]->GetBool()) {
            auto target = g_Common->GetTarget(dynamic_range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                if(!target->IsInAutoAttackRange(g_LocalPlayer)) {
                    Spells["katarina.q"]->Cast(target); } } } }

    on_postupdate(); }

inline auto EzKatarina::on_hud_draw() -> void {
    // - debug
    if(Menu["katarina.draw.dagger"]->GetBool()) {
        for(auto b : all_blades) {
            g_Drawing->AddCircle(b.second->Position(), 50, Menu["katarina.draw.dagger1"]->GetColor(), 2); } }

    if(Menu["katarina.draw.q"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.q"]->Range(), Menu["katarina.draw.q1"]->GetColor(), 2); }

    if(Menu["katarina.draw.e"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.e"]->Range(), Menu["katarina.draw.e1"]->GetColor(), 2); }

    if(Menu["katarina.draw.r"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.r"]->Range(), Menu["katarina.draw.r1"]->GetColor(), 2); } }

inline auto EzKatarina::hpbarfill_render() -> void {}

inline auto EzKatarina::shunpo_position(IGameObject * unit) -> Vector {
    for(auto b : all_blades) {
        auto obj = b.second;

        if(obj->Distance(g_LocalPlayer) <= Spells["katarina.e"]->Range() && g_LocalPlayer->Distance(unit) > Spells["katarina.w"]->Range()) {
            return obj->Position() + (unit->Position() - obj->Position()).Normalized() * 100; }  }

    if(!g_LocalPlayer->IsInAutoAttackRange(unit) && g_LocalPlayer->Distance(unit) > Spells["katarina.w"]->Range()) {
        auto front = unit->Position() + (g_LocalPlayer->Position() - unit->ServerPosition()).Normalized() * 100;
        return front; } }

inline auto EzKatarina::dynamic_range() -> float {
    if(!Spells["katarina.e"]->IsReady()) {
        if(g_Common->TickCount() - Spells["katarina.e"]->LastCastTime() < 500) {
            return Spells["katarina.w"]->Range(); } }

    return Spells["katarina.q"]->Range(); }

inline auto EzKatarina::on_buff(IGameObject * unit, OnBuffEventArgs * args) {}

inline auto EzKatarina::on_create(IGameObject * unit) -> void {
    if(strcmp("Dagger_Land", unit->Name().c_str()) != 0) {
        if(unit != nullptr && unit->IsValid() && unit->IsVisible()) {
            auto target = g_Common->GetTarget(dynamic_range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                if(target->Distance(unit) <= Spells["katarina.w"]->Range()) {
                    blade_vectors[g_Common->Time()] = unit->ServerPosition(); } }

            g_Common->Log("OK");
            g_Common->Log(unit->Name().c_str());
            all_blades[g_Common->Time()] = unit; } } }

inline auto EzKatarina::on_destroy(IGameObject * unit) -> void {}

inline auto EzKatarina::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {
    if(!unit->IsMe() || !args->IsAutoAttack) {
        return; }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        // - bouncing blades
        if(Spells["katarina.q"]->IsReady() && Menu["katarina.use.q"]->GetBool()) {
            if(args->Target != nullptr && args->Target->IsValidTarget()) {
                Spells["katarina.q"]->Cast(args->Target); } } } }

inline auto EzKatarina::on_spell_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {}
