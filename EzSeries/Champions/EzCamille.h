#pragma once
#include "EzChampion.h"
#include "../SDK/PluginSDK.h"


class EzCamille : public EzChampion {
    public:
        static IMenu * on_load(IMenu * menu);

        // -> aura mechanics
        static auto has_q() -> bool;
        static auto has_q2() -> bool;
        static auto charging_w() -> bool;;
        static auto on_wall() -> bool;
        static auto is_dashing() -> bool;
        static auto knocked_back(IGameObject * unit) -> bool;

        // -> ability methods
        static auto use_q(IGameObject * unit) -> void;
        static auto use_w(IGameObject * unit) -> void;
        static auto can_w(IGameObject * unit) -> bool;
        static auto lock_w(IGameObject * unit) -> void;
        static auto lock_w(Vector pos) -> void;
        static auto use_e(Vector pos, bool combo) -> void;
        static auto use_r(IGameObject * unit, bool force) -> void;

        // -> damage
        static auto lethal_target(IGameObject * unit) -> bool;
        static auto c_dmg(IGameObject * unit) -> double;
        static auto q_dmg(IGameObject * unit, bool includeq2 = true) -> double;
        static auto w_dmg(IGameObject * unit, bool bonus = false) -> double;
        static auto e_dmg(IGameObject * unit) -> double;
        static auto r_dmg(double dmg, IGameObject * unit) -> double;

        // -> event hooks
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline IMenu * EzCamille::on_load(IMenu * menu) {
    // todo: setup items
    Spells["camille.q"] = g_Common->AddSpell(SpellSlot::Q);
    Spells["camille.w"] = g_Common->AddSpell(SpellSlot::W, 625);
    Spells["camille.w"]->SetSkillshot(0.5, 200, 9999, kCollidesWithNothing, kSkillshotCone);
    Spells["camille.e"] = g_Common->AddSpell(SpellSlot::E, 975);
    Spells["camille.e"]->SetSkillshot(0.125, g_LocalPlayer->BoundingRadius(), 1750, kCollidesWithNothing, kSkillshotLine);
    Spells["camille.r"] = g_Common->AddSpell(SpellSlot::R, 465);
    return menu; }

#pragma region aura mechanics

inline auto EzCamille::has_q() -> bool {
    return g_LocalPlayer->HasBuff("camilleqprimingstart"); }

inline auto EzCamille::has_q2() -> bool {
    return g_LocalPlayer->HasBuff("camilleqprimingcomplete"); }

inline auto EzCamille::charging_w() -> bool {
    return g_LocalPlayer->HasBuff("camillewconeslashcharge"); }

inline auto EzCamille::on_wall() -> bool {
    return g_LocalPlayer->HasBuff("camilleedashtoggle") ||
        strcmp("CamilleE", g_LocalPlayer->GetSpellbook()->GetSpell(SpellSlot::E)->SData().SpellName.c_str()) != 0; }

inline auto EzCamille::is_dashing() -> bool {
    return g_LocalPlayer->HasBuff("camilleedash1") || g_LocalPlayer->HasBuff("camilleedash2") || g_LocalPlayer->IsDashing(); }

inline auto EzCamille::knocked_back(IGameObject * unit) -> bool {
    return unit != nullptr && unit->HasBuff("camilleeknockback2"); }

#pragma endregion

#pragma region combo mechanics

inline void EzCamille::use_q(IGameObject * unit) {
    if(Spells["camille.q"]->IsReady()) {
        if(!has_q() || has_q2()) {
            if(Spells["camille.q"]->Cast()) {
                g_Orbwalker->ResetAA(); } }

        else {
            if(unit != nullptr && unit->IsAIHero()) {
                if(q_dmg(unit, false) + g_LocalPlayer->AutoAttackDamage(unit, true) * 1
                    >= unit->RealHealth(true, true)) {
                    if(Spells["camille.q"]->Cast()) {
                        g_Orbwalker->ResetAA(); } } } } } }

inline void EzCamille::use_w(IGameObject * unit) {
    if(!charging_w() && !is_dashing() && !on_wall() && can_w(unit)) {
        if(knocked_back(unit)) {
            return; }

        if(Spells["camille.w"]->IsReady() && g_LocalPlayer->Distance(unit) <= Spells["camille.w"]->Range()) {
            Spells["camille.w"]->FastCast(unit->ServerPosition()); } } }

inline auto EzCamille::can_w(IGameObject * unit) -> bool {
    const float time = 2000;

    if(on_wall() || is_dashing() || unit == nullptr) {
        return false; }

    if(g_Common->TickCount() - Ticks["camille.e"] < 1000) {
        // to prevent instant w after e
        return false; }

    if(Spells["camille.q"]->IsReady()) {
        for(auto i : g_LocalPlayer->GetBuffList()) {
            if(strcmp("camilleqprimingstart", i.Name.c_str())) {
                if(i.EndTime - g_Common->Time() * 1000 <= time) {
                    return false; } }

            if(strcmp("camilleqprimingcomplete", i.Name.c_str())) {
                if(i.EndTime - g_Common->Time() * 1000 <= time) {
                    return false; } } }

        if(!has_q() || has_q2()) {
            if(g_LocalPlayer->Distance(unit) <= g_LocalPlayer->AttackRange() + 35) {
                return false; } }

        else {
            if(q_dmg(unit, false) + g_LocalPlayer->AutoAttackDamage(unit, true) *
                1 >= unit->RealHealth(true, true)) {
                return false; } } }

    if(g_LocalPlayer->Distance(unit) <= g_LocalPlayer->AttackRange() + 35) {
        if(g_LocalPlayer->AutoAttackDamage(unit, true) * 3 >= unit->RealHealth(true, true)) {
            return false; } }

    return true; }

inline auto EzCamille::lock_w(IGameObject * unit) -> void {
    if(on_wall() || is_dashing() || unit == nullptr) {
        return; }

    if(charging_w() && g_LocalPlayer->Distance(unit) <= Spells["camille.w"]->Range() + unit->BoundingRadius() + g_LocalPlayer->BoundingRadius()) {
        const auto pred = g_Common->GetPrediction(unit, 100);
        auto pos = pred.CastPosition.Extend(g_LocalPlayer->ServerPosition(), Spells["camille.w"]->Range() - 65);

        if(g_Common->TickCount() - Ticks["issueOrder"] >= 500 && pos.IsValid()) {
            g_LocalPlayer->IssueOrder(IssueOrderType::MoveTo, pos, false);
            Ticks["issueOrder"] = g_Common->TickCount(); } } }

inline auto EzCamille::lock_w(Vector pos) -> void {
    if(on_wall() || is_dashing() || !pos.IsValid()) {
        return; }

    const auto v = pos.Extend(g_LocalPlayer->ServerPosition(), Spells["camille.w"]->Range() - 25);

    if(charging_w() && g_LocalPlayer->Distance(pos) <= Spells["camille.w"]->Range() + g_LocalPlayer->BoundingRadius() * 2) {
        if(g_Common->TickCount() - Ticks["issueOrder"] >= 500 && v.IsValid()) {
            g_LocalPlayer->IssueOrder(IssueOrderType::MoveTo, v, false);
            Ticks["issueOrder"] = g_Common->TickCount(); } } }

inline auto EzCamille::use_e(Vector pos, bool combo) -> void {
    // - moving to extensions
    if(is_dashing() || on_wall() || !Spells["camille.e"]->IsReady()) {} }


inline auto EzCamille::use_r(IGameObject * unit, bool force) -> void {}

#pragma endregion

#pragma region damage

inline auto EzCamille::lethal_target(IGameObject * unit) -> bool {}

inline auto EzCamille::c_dmg(IGameObject * unit) -> double {}

inline auto EzCamille::q_dmg(IGameObject * unit, bool includeq2) -> double {}

inline auto EzCamille::w_dmg(IGameObject * unit, bool bonus) -> double {}

inline auto EzCamille::e_dmg(IGameObject * unit) -> double {}

inline auto EzCamille::r_dmg(double dmg, IGameObject * unit) -> double {}

#pragma endregion

inline void EzCamille::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {}
