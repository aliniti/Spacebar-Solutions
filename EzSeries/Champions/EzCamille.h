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
        static bool lethal_target(IGameObject * unit);
        static double c_dmg(IGameObject * unit);
        static double q_dmg(IGameObject * unit, bool includeq2 = true);
        static double w_dmg(IGameObject * unit, bool bonus = false);
        static double e_dmg(IGameObject * unit);
        static double r_dmg(double dmg, IGameObject * unit);

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
    g_LocalPlayer->HasBuff("camilleedash1") || g_LocalPlayer->HasBuff("camilleedash2") || g_LocalPlayer->IsDashing();
    return false; }

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
    const float animationtime = 2000;

    if(on_wall() || is_dashing() || unit == nullptr) {
        return false; }

    if(g_Common->TickCount() - Ticks["camille.e"] < 1000) {
        // to prevent instant w after e
        return false; }

    if(Spells["camille.q"]->IsReady()) {
        for(auto i : g_LocalPlayer->GetBuffList()) {
            if(strcmp("camilleqprimingstart", i.Name.c_str())) {
                if(i.EndTime - g_Common->Time() * 1000 <= animationtime) {
                    return false; } }

            if(strcmp("camilleqprimingcomplete", i.Name.c_str())) {
                if(i.EndTime - g_Common->Time() * 1000 <= animationtime) {
                    return false; } } }

        if(!has_q() || has_q2()) {
            if(g_LocalPlayer->Distance(unit) <= g_LocalPlayer->AttackRange() + 35) {
                return false; } }

        else {
            if(q_dmg(unit, false) + g_LocalPlayer->AutoAttackDamage(unit, true) * 1 >= unit->RealHealth(true, true)) {
                return  false; } } }

    if(g_LocalPlayer->Distance(unit) <= g_LocalPlayer->AttackRange() + 35) {
        if(g_LocalPlayer->AutoAttackDamage(unit, true) * 3 >= unit->RealHealth(true, true)) {
            return false; } }

    return true; }

inline auto EzCamille::lock_w(IGameObject * unit) -> void {
    if(on_wall() ||is_dashing() || unit == nullptr) {
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
    if(is_dashing() || on_wall() || !Spells["camille.e"]->IsReady()) {
        return; }

    if(combo) {
        if(g_LocalPlayer->Distance(pos) - g_LocalPlayer->BoundingRadius() * 2 < Menu["minimum.e.range"]->GetInt()) {
            return; }

        if(pos.IsUnderEnemyTurret() && Menu["dont.e.under.enemy.turret"]->GetBool()) {
            return; } }

    auto posChecked = 0;
    auto maxPosChecked = 40;
    auto posRadius = 145;
    auto radiusIndex = 0;

    if(Menu["enhanced.e.precision"]->GetBool()) {
        maxPosChecked = 80;
        posRadius = 65; }

    auto candidate_pos_list = std::vector<Vector>();

    while(posChecked < maxPosChecked) {
        radiusIndex++;
        const auto cur_radius = radiusIndex * (0x2 * posRadius);
        const auto cur_curcle_checks = static_cast<int>(ceil((0x2 * M_PI * cur_radius) / (0x2 * static_cast<double>(posRadius))));

        for(auto i = 1; i < cur_curcle_checks; i++) {
            posChecked++;
            const auto c_radians = (0x2 * M_PI / (cur_curcle_checks - 0x1)) * i;
            const auto x_pos = static_cast<float>(floor(pos.x + cur_radius * cos(c_radians)));
            const auto y_pos = static_cast<float>(floor(pos.y + cur_radius * sin(c_radians)));
            const auto v = Vector2(x_pos, y_pos);
            auto pos_ex = Ex->to_3d(v);
            auto any_dangerous_pos = false;

            for(auto entry : DangerPoints) {
                auto obj = entry.second;

                if(obj->aType == Outside && Ex->dist_2d(pos_ex, obj->Emitter->Position()) > obj->Radius) {
                    any_dangerous_pos = true;
                    break; }

                if(obj->aType == Inside) {
                    const auto & start_pos = pos_ex;
                    const auto & end_pos = pos;
                    // emitter pos.. check intersection
                    const auto init_pos = entry.second->Emitter->Position();
                    const auto proj = init_pos.ProjectOn(start_pos, end_pos);

                    if(proj.IsOnSegment && Ex->dist_2d(proj.SegmentPoint, init_pos) <= obj->Radius + g_LocalPlayer->BoundingRadius()) {
                        any_dangerous_pos = true;
                        break; } } }

            if(any_dangerous_pos) {
                continue; }

            auto extra_unit = g_Common->GetTarget(Spells["camille.w"]->Range(), DamageType::Physical);

            if(extra_unit != nullptr && extra_unit->IsValidTarget() && charging_w()) {
                if(Ex->dist_2d(Ex->to_2d(extra_unit->ServerPosition()), pos_ex) > Spells["camille.w"]->Range() - 100) {
                    continue; } }

            if(g_NavMesh->HasFlag(pos_ex, kNavFlagsWall)) {
                candidate_pos_list.push_back(pos_ex); }

            std::sort(candidate_pos_list.begin(), candidate_pos_list.end(), [&](Vector a, Vector b) {
                switch(Menu["preferred.hook.position"]->GetInt()) {
                    case 0:
                        return a.Distance(pos) < b.Distance(pos);

                    case 1:
                        return a.Distance(g_LocalPlayer->ServerPosition()) < b.Distance(g_LocalPlayer->ServerPosition());

                    default: return a.Distance(pos) < b.Distance(pos); } }); } }

    if(charging_w() == false) {
        for(auto vec : candidate_pos_list) {
            if(vec.IsValid()) {
                if(g_LocalPlayer->ServerPosition().Distance(vec) <= Spells["camille.e"]->Range() && vec.Distance(pos) <= Spells["camille.e"]->Range()) {
                    if(vec.IsValid()) {
                        if(Spells["camille.e"]->FastCast(vec)) {
                            Ticks["camille.e"] = g_Common->TickCount(); } } } } } } }


inline auto EzCamille::use_r(IGameObject * unit, bool force) -> void {}


inline bool EzCamille::lethal_target(IGameObject * unit) {}

inline double EzCamille::c_dmg(IGameObject * unit) {}
inline double EzCamille::q_dmg(IGameObject * unit, bool includeq2) {}
inline double EzCamille::w_dmg(IGameObject * unit, bool bonus) {}
inline double EzCamille::e_dmg(IGameObject * unit) {}
inline double EzCamille::r_dmg(double dmg, IGameObject * unit) {}


#pragma endregion
inline void EzCamille::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(unit->IsMe()) {
        if(args->IsAutoAttack) {
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
                // AA -> E
                if(Spells["vayne.e"]->IsReady() && args->Target->IsAIHero()) {
                    if(args->Target->Distance(unit) <= Spells["vayne.e"]->Range()) {
                        Spells["vayne.e"]->Cast(args->Target); } } // AA -> Q

                if(Spells["vayne.q"]->IsReady()) {
                    if(Spells["vayne.q"]->Cast(g_Common->CursorPosition())) {
                        g_Orbwalker->ResetAA(); } } } } } }