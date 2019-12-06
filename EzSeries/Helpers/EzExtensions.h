#pragma once
#include "../SDK/PluginSDK.h"
#include "../SDK/Vector.h"
#include "EzEnums.h"

class EzExtensions {
    public:

        static auto to_2d(Vector p) -> Vector2;
        static auto to_3d(Vector2 p) -> Vector;
        static auto perpendicular(Vector2 v) -> Vector2;

        static auto dist_2d(Vector from, Vector to) -> float;
        static auto dist_2d(Vector2 from, Vector2 to) -> float;
        static auto dist_2d(Vector2 from, Vector to) -> float;

        static auto get_in_range(IGameObject * unit, float range, EntityType type, std::vector<IGameObject *> & units) -> int;
        static auto is_ready(std::shared_ptr<ISpell> spell, float time) -> bool;
        static auto get_prediction(std::shared_ptr<ISpell> spell, IGameObject * unit) -> IPredictionOutput;
        static auto get_mana(IGameObject * unit) -> ManaScenario;
        static auto get_combat_scenario(IGameObject * unit) -> ScenarioType;
        static auto get_prefered_hitchance(IGameObject * unit) -> HitChance;
        static std::vector<IGameObject *> get_units_in_range(IGameObject * source, EntityType type, float range, bool enemies);
        static int get_in_range(IGameObject * source, EntityType type, float range, bool enemies);
        static auto project_on_path(Vector start, Vector end, float width, float range, std::vector<IGameObject *> & units, bool minion) -> int;

        static auto draw_dmg_hpbar(IGameObject * unit, double damage, const char * text, DWORD color) -> void;
        static auto is_plant(IGameObject * unit) -> bool;
        static auto get_real_position(float delay) -> Vector;
        static auto check_path_collision(IGameObject * unit, Vector pos);
        static auto get_near_wall_point(Vector start, Vector end) -> Vector;
        static auto check_point_collision(IGameObject * unit, Vector pos) -> bool; };


inline Vector2 EzExtensions::to_2d(Vector p) {
    return Vector2(p.x, p.z); }

inline Vector EzExtensions::to_3d(Vector2 p) {
    return Vector(p.x, 0, p.y); }

inline Vector2 EzExtensions::perpendicular(Vector2 v) {
    return Vector2(-v.y, v.x); }

inline auto EzExtensions::dist_2d(Vector from, Vector to) -> float {
    return (from - to).Length(); }

inline auto EzExtensions::dist_2d(Vector2 from, Vector2 to) -> float {
    return (to_3d(from) - to_3d(to)).Length(); }

inline auto EzExtensions::dist_2d(Vector2 from, Vector to) -> float {
    return (to_3d(from) - to).Length(); }

inline auto EzExtensions::get_in_range(IGameObject * unit, float range, EntityType type, std::vector<IGameObject *> & units) -> int {
    std::vector<IGameObject *> mip;

    for(auto i : g_ObjectManager->GetByType(type)) {
        if(i->Distance(unit) <= range) {
            mip.push_back(i); } }

    units = mip;
    return mip.size(); }

inline auto EzExtensions::is_ready(std::shared_ptr<ISpell> spell, float time) -> bool {
    auto io = g_LocalPlayer->GetSpellbook()->GetSpell(spell->Slot())->CooldownTime();
    return io <= time || spell->IsReady(); }

inline auto EzExtensions::get_prediction(std::shared_ptr<ISpell> spell, IGameObject * unit) -> IPredictionOutput {
    return g_Common->GetPrediction(unit, spell->Range(), spell->Delay(), spell->Radius(), spell->Speed(), spell->CollisionFlags(),
            g_LocalPlayer->ServerPosition()); }

inline auto EzExtensions::get_mana(IGameObject * unit) -> ManaScenario {
    auto manapct = unit->Mana() / unit->MaxMana() * 100;

    if(manapct > 80) {
        return ManaScenario::High; }

    if(manapct <= 80 && manapct > 60) {
        return ManaScenario::Medium; }

    if(manapct <= 60 && manapct > 25) {
        return ManaScenario::Low; }

    if(manapct <= 25) {
        return ManaScenario::NoJuice; } }

inline auto EzExtensions::get_combat_scenario(IGameObject * unit) -> ScenarioType {
    auto ene = unit->CountEnemiesInRange(525);
    auto hostile = unit->CountEnemiesInRange(1650);
    auto friendly = unit->CountAlliesInRange(1450);

    if(friendly > 4 && hostile <= 1) {
        return ScenarioType::Overkill; }

    if(friendly >= 2 && hostile > 2 && hostile <= 3) {
        return ScenarioType::Skirmish; }

    if(friendly <= 1 && hostile > 1 && hostile < 3) {
        return ScenarioType::Skirmish; }

    if(friendly <= 1 && hostile == 1) {
        return ScenarioType::Solo; }

    if(friendly > 1 && hostile > 3) {
        return ScenarioType::Teamfight; }

    if(friendly <= 2 && (hostile > 4 || ene > 2)) {
        return ScenarioType::ImSoFuckingDead; }

    return ScenarioType::Chilling; }

inline auto EzExtensions::get_prefered_hitchance(IGameObject * unit) -> HitChance {
    auto manapct = unit->Mana() / unit->MaxMana() * 100;

    if(manapct > 85) {
        return HitChance::High; }

    if(manapct <= 85 && manapct > 55) {
        return HitChance::High; }

    if(manapct <= 55 && manapct > 15) {
        return HitChance::VeryHigh; }

    if(manapct <= 15) {
        return HitChance::VeryHigh; }

    return HitChance::High; }

inline auto EzExtensions::get_units_in_range(IGameObject * source, EntityType type, float range, bool enemies) -> std::vector<IGameObject *> {
    std::vector<IGameObject *> units;

    for(auto b : g_ObjectManager->GetByType(type)) {
        if(source->Distance(b) <= range) {
            if(enemies && source->Team() == g_LocalPlayer->Team()) {
                continue; }

            if(!b->IsDead() && b->IsVisible()) {
                units.push_back(b); } } }

    std::remove_if(units.begin(), units.end(), [&](IGameObject* u) {
        return u == nullptr || !u->IsValidTarget(); });
    return units; }

inline auto EzExtensions::get_in_range(IGameObject * source, EntityType type, float range, bool enemies) -> int {
    std::vector<IGameObject *> units;

    for(auto b : g_ObjectManager->GetByType(type)) {
        if(source->Distance(b) <= range) {
            if(enemies && source->Team() == g_LocalPlayer->Team()) {
                continue; }

            if(!b->IsDead() && b->IsVisible()) {
                units.push_back(b); } } }

    const auto valid_units = std::count_if(units.begin(), units.end(), [&](IGameObject* u) {
        return u != nullptr && u->IsValidTarget(); });
    return valid_units; }

inline auto EzExtensions::project_on_path(Vector start, Vector end, float width, float range, std::vector<IGameObject *> & units, bool minion) -> int {
    auto direction = (end - start).Normalized();
    auto endposition = start + direction * start.Distance(end);
    auto objs = g_ObjectManager->GetByType(EntityType::Any, [&](IGameObject* u) {
        return !u->IsDead() && u->IsAIBase(); });

    for(auto a : objs) {
        auto proj = a->ServerPosition().ProjectOn(start, endposition);
        auto projdist = a->Distance(proj.SegmentPoint);

        if(a->BoundingRadius() + width > projdist && proj.IsOnSegment) {
            units.push_back(a); } }

    return units.size(); }

inline auto EzExtensions::draw_dmg_hpbar(IGameObject * unit, double damage, const char * text, DWORD color) -> void {
    auto barpos = unit->HealthBarPosition();
    auto barsize = Vector2(103 * (min(damage, unit->Health()) / unit->MaxHealth()), 12);
    barpos = Vector2(barpos.x + 33, barpos.y += 0.3);
    auto xd1 = Vector(barpos.x + barsize.x, barpos.y);
    auto xd2 = Vector(barpos.x + barsize.x - 5, barpos.y - 7);
    g_Drawing->AddFilledRectOnScreen(Vector2(barpos.x, barpos.y), barsize, color);
    g_Drawing->AddLine(xd1, xd2, color);
    g_Drawing->AddLine(xd1, xd1 + Vector(0, 8), color);
    g_Drawing->AddText(xd2 - Vector(13, 13), color, 3, text); }

inline auto EzExtensions::is_plant(IGameObject * unit) -> bool {
    return strstr(unit->Name().c_str(), "Plant"); }

inline auto EzExtensions::get_real_position(float delay) -> Vector {

    auto path = g_LocalPlayer->Path();

    if(path.size() < 1) {
        return g_LocalPlayer->ServerPosition(); }

    auto dir = (path[0] - g_LocalPlayer->ServerPosition()).Normalized();
    return  g_LocalPlayer->ServerPosition() + dir * g_LocalPlayer->MoveSpeed() * (delay / 1000); }

inline auto EzExtensions::check_path_collision(IGameObject * unit, Vector pos) {
    auto path = g_LocalPlayer->CreatePath(g_LocalPlayer->ServerPosition(), pos);

    if(path.size() > 0) {
        if(pos.Distance(path[path.size() - 1]) > 5 || path.size() > 2) {
            return true; } }

    return false; }

inline auto EzExtensions::get_near_wall_point(Vector start, Vector end) -> Vector {
    auto dir = (end - start).Normalized();
    auto distance = start.Distance(end);

    for(auto i = 20; i < distance; i += 20) {
        auto v = end - dir * i;

        if(v.IsWall()) {
            return v; } }

    return { 0, 0, 0 }; }

inline auto EzExtensions::check_point_collision(IGameObject * unit, Vector pos) -> bool {
    auto path = unit->CreatePath(pos);

    if(path.size() > 0) {
        if(pos.Distance(path[path.size() - 1]) > 5) {
            return true; } }

    return false; }






