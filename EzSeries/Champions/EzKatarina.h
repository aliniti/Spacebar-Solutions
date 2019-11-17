#pragma once
#include "EzChampion.h"


class EzKatarina : public EzChampion {
    public:

        static auto on_boot(IMenu * menu) -> IMenu*;
        static auto on_issue_order(IGameObject * unit, OnIssueOrderEventArgs * args) -> void;
        static auto on_update() -> void;
        static auto on_hud_draw() -> void;
        static auto hpbarfill_render() -> void;

        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args);
        static auto on_create(IGameObject * unit) -> void;
        static auto on_destroy(IGameObject * unit) -> void;
        static auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void;
        static auto on_spell_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void; };

inline auto EzKatarina::on_boot(IMenu * menu) -> IMenu * {
    Spells["katarina.q"] = g_Common->AddSpell(SpellSlot::Q, 625);
    Spells["katarina.w"] = g_Common->AddSpell(SpellSlot::W, 325);
    Spells["katarina.e"] = g_Common->AddSpell(SpellSlot::E, 725);
    Spells["katarina.r"] = g_Common->AddSpell(SpellSlot::R, 550);
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
    auto m = menu->AddSubMenu("Katarina: Mechanics", "katarina.mechanics");
    Menu["katarina.mode"] = m->AddComboBox("Combo Style", "katarina.mode", std::vector<std::string> {"Pro", "Hardcore/Urf" }, 1);
    return menu; }

inline auto EzKatarina::on_issue_order(IGameObject * unit, OnIssueOrderEventArgs * args) -> void {
    if(unit->IsMe() && unit->HasBuff("katarinarsound")) {
        args->Process = false; } }

inline auto EzKatarina::on_update() -> void {}

inline auto EzKatarina::on_hud_draw() -> void {
    if(Menu["katarina.draw.q"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.q"]->Range(), Menu["katarina.draw.q1"]->GetColor(), 2); }

    if(Menu["katarina.draw.e"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.e"]->Range(), Menu["katarina.draw.e1"]->GetColor(), 2); }

    if(Menu["katarina.draw.r"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["katarina.r"]->Range(), Menu["katarina.draw.r1"]->GetColor(), 2); } }

inline auto EzKatarina::hpbarfill_render() -> void {}

inline auto EzKatarina::on_buff(IGameObject * unit, OnBuffEventArgs * args) {
    if(unit->IsMe()) { } }

inline auto EzKatarina::on_create(IGameObject * unit) -> void {}

inline auto EzKatarina::on_destroy(IGameObject * unit) -> void {}

inline auto EzKatarina::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {}

inline auto EzKatarina::on_spell_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {}
