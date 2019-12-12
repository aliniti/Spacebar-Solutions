#pragma once
#include "EzChampion.h"


class EzVayne : public EzChampion {
    public:
        static auto on_boot(IMenu * menu) -> IMenu*;
        static auto on_update() -> void;
        static auto on_draw() -> void;
        static auto hpbarfill_render() -> void;
        static auto condemn_target(IGameObject * unit, bool checkWall = true) -> void;

        static auto on_create(IGameObject * unit) -> void;
        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void;
        static auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void;
        static auto get_tumble_position(IGameObject * unit, float posRadius, int maxPosChecked) -> Vector; };

inline IMenu * EzVayne::on_boot(IMenu * menu) {
    Spells["vayne.q"] = g_Common->AddSpell(SpellSlot::Q);
    Spells["vayne.e"] = g_Common->AddSpell(SpellSlot::E, 550);
    Spells["vayne.r"] = g_Common->AddSpell(SpellSlot::R);

    auto q_settings = menu->AddSubMenu("Tumble Settings", "vayne.tumble");
    Menu["vayne.q.mode"] = q_settings->AddComboBox("Mode", "vayne.q.mode", std::vector<std::string> { "Mouse Position", "Pathfinder (Beta)" }, 0);

    q_settings->AddLabel("Pathfinder", "vayne.pathfinder");
    Menu["vayne.q.comfort.dist"] = q_settings->AddSlider("Tumble Comfort Distance", "vayne.q.confort.dist", 320, 0, 420);
    Menu["vayne.q.wall"] = q_settings->AddCheckBox("Allow Tumble into Wall", "vayne.q.wall", false);
    Menu["vayne.q.debug"] = q_settings->AddCheckBox("Tumble Debug", "vayne.q.debug", false);

    auto e_settings = menu->AddSubMenu("Condemn Settings", "vayne.condemn");
    Menu["use.vayne.e.pred"] = e_settings->AddCheckBox("Use Movement Prediction", "use.vayne.e.pred", false);
    Menu["use.vayne.e.auto"] = e_settings->AddCheckBox("Condemn Only Combo", "use.vayne.e.auto", false);
    Menu["use.vayne.e.dash"] = e_settings->AddCheckBox("Condemn Dashing", "use.vayne.e.dash", true);
    Menu["use.vayne.e.dist"] = e_settings->AddSlider("Condemn Distance: ", "use.vayne.e.dist", 420, 100, 475);
    Menu["vayne.e.debug"] = e_settings->AddCheckBox("Condemn Debug", "vayne.e.debug", false);

    Menu["use.vayne.q"] = menu->AddCheckBox("Use Tumble", "use.vayne.q", true);
    Menu["use.vayne.e"] = menu->AddCheckBox("Use Condemn", "use.vayne.e", true);

    return menu; }

inline void EzVayne::on_update() {
    if(!Menu["use.vayne.e.auto"]->GetBool() || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        for(auto u : g_ObjectManager->GetChampions()) {
            if(u && u->IsEnemy()) {

                // - gapclosing?
                if(g_LocalPlayer->IsRecalling()) {
                    return; }

                if(Menu["use.vayne.e.dash"]->GetBool()) {
                    const auto pred = g_Common->GetPrediction(u, g_Common->Ping() / 1000);

                    if(pred.Hitchance == HitChance::Dashing && u->IsFacing(g_LocalPlayer)) {
                        if(u->Distance(g_LocalPlayer) <= 400) {
                            condemn_target(u, false); } } }

                // - else
                condemn_target(u); } } }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        const auto push_size = Menu["use.vayne.e.dist"]->GetInt();
        const auto target = g_Common->GetTarget(Spells["vayne.e"]->Range() + push_size, DamageType::Physical);
        condemn_target(target); } }

inline auto EzVayne::condemn_target(IGameObject * unit, bool checkWall) -> void {
    if(!Spells["vayne.e"]->IsReady()) {
        return; }

    const auto push_size = Menu["use.vayne.e.dist"]->GetInt();

    if(unit != nullptr && Menu["use.vayne.e"]->GetBool() && unit->IsValidTarget(Spells["vayne.e"]->Range())) {
        auto end_pos = unit->ServerPosition().Extend(g_LocalPlayer->ServerPosition(), -push_size);
        auto target_pos = Menu["use.vayne.e.pred"]->GetBool()
            ? g_Common->GetPrediction(unit, g_Common->Ping() / 1000).UnitPosition
            : unit->ServerPosition();

        if(!checkWall) {
            Spells["vayne.e"]->Cast(unit);
            return; }

        // - simple condemn xd
        for(auto i = 0; i < target_pos.Distance(end_pos); i += 20) {
            auto dir = (end_pos - target_pos).Normalized();
            auto pos = target_pos + dir * i;

            if(pos.IsWall()) {
                Spells["vayne.e"]->Cast(unit);
                break; } } } }


inline void EzVayne::on_draw() {

    auto push_size = Menu["use.vayne.e.dist"]->GetInt();
    auto target = g_Common->GetTarget(Spells["vayne.e"]->Range() + push_size, DamageType::Physical);
    auto position = get_tumble_position(target, 60, 15);

    if(!Menu["vayne.e.debug"]->GetBool()) {
        return; }

    if(target != nullptr && Menu["use.vayne.e"]->GetBool() && target->IsValidTarget(Spells["vayne.e"]->Range())) {
        auto end_pos = target->ServerPosition().Extend(g_LocalPlayer->ServerPosition(), -push_size);
        auto target_pos = Menu["use.vayne.e.pred"]->GetBool()
            ? g_Common->GetPrediction(target, g_Common->Ping() / 1000).UnitPosition
            : target->ServerPosition();

        for(auto i = 0; i < target_pos.Distance(end_pos); i += 20) {
            auto v = target_pos + (end_pos - target_pos).Normalized() * i;

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
                    auto pos = Menu["vayne.q.mode"]->GetInt() == 0 ? g_Common->CursorPosition() : get_tumble_position(args->Target, 60, 15);
                    Spells["vayne.q"]->Cast(pos); } } } } }


inline auto EzVayne::get_tumble_position(IGameObject * unit, float posRadius, int maxPosChecked) -> Vector {

    if(unit == nullptr) {
        return { 0, 0, 0 }; }

    auto best_positions = std::vector<Vector>();
    auto positions = Ex->get_surrounding_positions(g_LocalPlayer->ServerPosition(), posRadius, maxPosChecked);

    for(auto v : positions) {
        if(v.Distance(unit->ServerPosition()) <= Menu["vayne.q.comfort.dist"]->GetInt()) {
            continue; }

        if(v.CountEnemiesInRange(Menu["vayne.q.comfort.dist"]->GetInt()) > 0) {
            continue; }

        if(v.IsWall() && !Menu["vayne.q.wall"]->GetBool()) {
            continue; }

        if(v.Extend(g_LocalPlayer->ServerPosition(), -300).IsWall()) {
            continue; }

        if(v.Distance(unit->ServerPosition()) > g_LocalPlayer->AttackRange()) {
            continue; }

        if(Menu["vayne.q.debug"]->GetBool()) {
            g_Drawing->AddCircle(v, 60, RGBA(204, 102, 102, 115)); }

        best_positions.push_back(v); }

    // - sort closest to cursor
    std::sort(best_positions.begin(), best_positions.end(), [&](Vector v1, Vector v2) {
        return v1.Distance(g_Common->CursorPosition()) < v2.Distance(g_Common->CursorPosition()); });

    if(!best_positions.empty()) {
        return best_positions.front(); }

    return { 0, 0, 0 }; }
