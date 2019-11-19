#pragma once
#include "EzChampion.h"
#include "../SDK/PluginSDK_Enums.h"
#include "../SDK/PluginSDK.h"
#include "../Helpers/EzExtensions.h"


class EzTristana : public EzChampion {
    public:

        static IMenu * on_boot(IMenu * menu);
        static void on_boot();
        static void on_update();
        static void on_post_update();
        static void on_huddraw();
        static void hpbarfill_render();
        static void on_dash(IGameObject * unit, OnProcessSpellEventArgs * args);

        static float edmg(IGameObject * unit);
        static float rdmg(IGameObject * unit);
        static float drawdmg(IGameObject * unit);

        static void on_execute(IGameObject * unit);
        static void on_before_attack(BeforeAttackOrbwalkerArgs * args); };

inline void EzTristana::on_boot() {
    Spells["tristana.q"] = g_Common->AddSpell(SpellSlot::Q);
    Spells["tristana.e"] = g_Common->AddSpell(SpellSlot::E);
    Spells["tristana.r"] = g_Common->AddSpell(SpellSlot::R);
    g_Common->ChatPrint(R"(<font color="#FFCC00"><b>[EzSeries Tristana]:</b></font><b><font color="#99FF99"> Loaded!</font>)"); }

inline float EzTristana::drawdmg(IGameObject * unit) {
    return rdmg(unit) + edmg(unit); }

inline IMenu * EzTristana::on_boot(IMenu * menu) {
    on_boot(); // initial load :^)
    auto d_menu = menu->AddSubMenu("Tristana: Draw", "tristana.draww");
    Menu["tristana.draww.en.w"] = d_menu->AddCheckBox("Draw W Range", "tristana.draww.en.w", true);
    Menu["tristana.draww.w"] = d_menu->AddColorPicker("W Range Color", "tristana.draww.w", 255, 204, 0, 185);
    Menu["tristana.draww.en.r"] = d_menu->AddCheckBox("Draw R Range", "tristana.draww.en.r", true);
    Menu["tristana.draww.r"] = d_menu->AddColorPicker("R Range Color", "tristana.draww.r", 255, 204, 0, 185);
    Menu["tristana.draww.en.hp"] = d_menu->AddCheckBox("Draw R HpBarFill", "tristana.draww.en.hp", true);
    Menu["tristana.draww.hp"] = d_menu->AddColorPicker("R HpBarFill Color", "tristana.draww.hp", 255, 204, 0, 185); // menu setup..
    auto c_menu = menu->AddSubMenu("Tristana: Core", "tristana.core");
    Menu["tristana.use.q"] = c_menu->AddCheckBox("Use Q", "tristana.use.q", true);
    Menu["tristana.use.e"] = c_menu->AddCheckBox("Use E", "tristana.use.e", true);
    Menu["tristana.use.e.turret"] = c_menu->AddCheckBox("Use E on Turrets", "tristana.use.e.turret", true);
    Menu["tristana.focus.e.target"] = c_menu->AddCheckBox("Focus E Target", "tristana.focus.e.target", true);
    Menu["tristana.use.r"] = c_menu->AddCheckBox("Use R", "tristana.use.r", true);
    Menu["tristana.use.er.finish"] = c_menu->AddCheckBox("Use E + R Finish", "tristana.use.er.finish", true);
    Menu["tristana.use.er.min.stacks"] = c_menu->AddSlider("Min Stacks E + R Finish ->", "tristana.use.er.min.stacks", 2, 1, 3);
    auto h_menu = menu->AddSubMenu("Tristana: Harass", "tristana.harass");
    auto h_whitelist = h_menu->AddSubMenu("Explosive Charge Settings", "tristana.harass.whitelist"); // harass whitelist..

    for(auto i : g_ObjectManager->GetByType(EntityType::AIHeroClient)) {
        if(i->IsEnemy()) {
            Menu[i->ChampionName().append("harass.enable")] = h_whitelist->AddCheckBox("Use on " + i->ChampionName(),
                    i->ChampionName().append("harass.enable"), true); } }

    Menu["tristana.use.q.h"] = h_menu->AddCheckBox("Use Q", "tristana.use.q.h", true);
    Menu["tristana.use.e.h"] = h_menu->AddCheckBox("Use E", "tristana.use.e.h", true);
    auto f_menu = menu->AddSubMenu("Tristana: Farm", "tristana.farm");
    Menu["tristana.use.q.f"] = f_menu->AddCheckBox("Use Q", "tristana.use.q.f", false);
    Menu["tristana.use.e.f"] = f_menu->AddCheckBox("Use E", "tristana.use.e.f", false);
    menu->AddLabel("Tristana Auto:", "tristana.auto");
    Menu["tristana.use.r.auto"] = menu->AddCheckBox("Use R", "tristana.use.r.auto", true);
    Menu["tristana.use.r.gap"] = menu->AddCheckBox("Use R on Gapcloser", "tristana.use.r.gap", true);
    Menu["tristana.use.r.int"] = menu->AddCheckBox("Use R on Channeling", "tristana.use.r.int", true);
    return menu; }

inline void EzTristana::hpbarfill_render() {
    if(!Menu["tristana.draww.en.hp"]->GetBool()) {
        return; }

    for(auto i : g_ObjectManager->GetChampions()) {
        if(i != nullptr && !i->IsDead() && i->IsVisibleOnScreen() && !i->IsAlly() && i->IsValidTarget()) {
            EzExtensions::draw_dmg_hpbar(i, drawdmg(i), std::to_string(drawdmg(i)).c_str(), Menu["tristana.draww.hp"]->GetColor()); } } }

inline void EzTristana::on_dash(IGameObject * unit, OnProcessSpellEventArgs * args) {
    return;

    if(unit->IsEnemy() && unit->IsAIHero()) {
        auto dashdata = unit->GetDashData();

        if(dashdata.EndPosition.Distance(g_LocalPlayer->Position()) <= 250) {
            if(Menu["tristana.use.r.gap"]->GetBool() && Spells["tristnana.r"]->IsReady()) {
                // should already be in range but safe checking
                if(g_LocalPlayer->IsInAutoAttackRange(unit)) {
                    Spells["tristana.r"]->Cast(args->Target); } } } } }

inline void EzTristana::on_update() {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        const auto target = g_Common->GetTarget(g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(target)) {
            if(target->HasBuff(hash("tristanaechargesound")) && Menu["tristana.focus.e.target"]->GetBool()) {
                g_Orbwalker->SetOrbwalkingTarget(target); }

            if(Spells["tristana.q"]->IsReady() && Menu["tristana.use.q"]->GetBool()) {
                Spells["tristana.q"]->Cast(); }

            if(Spells["tristana.e"]->IsReady() && Menu["tristana.use.e"]->GetBool()) {
                Spells["tristana.e"]->Cast(target); }

            if(Spells["tristana.r"]->IsReady() && Menu["tristana.use.r"]->GetBool()) {
                on_execute(target); } } }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass)) {
        const auto target = g_Common->GetTarget(g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius(), DamageType::Physical);

        if(g_LocalPlayer->ManaPercent() > 70) {
            if(target != nullptr && target->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(target)) {
                if(Menu[target->ChampionName().append("harass.enable")]->GetBool()) {
                    if(target->HasBuff(hash("tristanaechargesound")) && Menu["tristana.focus.e.target"]->GetBool()) {
                        g_Orbwalker->SetOrbwalkingTarget(target); }

                    if(Spells["tristana.q"]->IsReady() && Menu["tristana.use.q.h"]->GetBool()) {
                        Spells["tristana.q"]->Cast(); }

                    if(Spells["tristana.e"]->IsReady() && Menu["tristana.use.e.h"]->GetBool()) {
                        Spells["tristana.e"]->Cast(target); } } } } }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear)) {
        if(Menu["tristana.use.e.f"]->GetBool() || Menu["tristana.use.q.f"]->GetBool()) {
            if(g_LocalPlayer->ManaPercent() > 35 && g_LocalPlayer->CountMyEnemiesInRange(950) < 1) {
                auto big_mobs = g_ObjectManager->GetJungleMobs();
                big_mobs.erase(std::remove_if(big_mobs.begin(), big_mobs.end(), [&](IGameObject* u) {
                    return strstr(u->Name().c_str(), "Mini"); }), big_mobs.end()); // prioritize bigger jungle mobs for E
                std::sort(big_mobs.begin(), big_mobs.end(), [&](IGameObject* a, IGameObject* b) {
                    return a->MaxHealth() > b->MaxHealth(); });

                for(auto m : big_mobs) {
                    if(m != nullptr && m->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(m)) {
                        if(m->HasBuff(hash("tristanaechargesound")) && Menu["tristana.focus.e.target"]->GetBool()) {
                            g_Orbwalker->SetOrbwalkingTarget(m); }

                        if(Menu["tristana.use.e.f"]->GetBool() && Spells["tristana.e"]->IsReady()) {
                            Spells["tristana.e"]->Cast(m); }

                        if(Menu["tristana.use.q.f"]->GetBool() && Spells["tristana.q"]->IsReady()) {
                            Spells["tristana.q"]->Cast(); } } } }

            if(g_LocalPlayer->ManaPercent() > 65 && (g_LocalPlayer->IsUnderMyAllyTurret() || g_LocalPlayer->CountMyEnemiesInRange(1200) < 1)) {
                auto creeps = g_ObjectManager->GetMinionsEnemy(); // prioritize bigger creeps for E
                std::sort(creeps.begin(), creeps.end(), [&](IGameObject* a, IGameObject* b) {
                    return a->MaxHealth() > b->MaxHealth(); });

                for(auto m : creeps) {
                    if(m != nullptr && m->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(m)) {
                        if(m->HasBuff(hash("tristanaechargesound")) && Menu["tristana.focus.e.target"]->GetBool()) {
                            g_Orbwalker->SetOrbwalkingTarget(m); }

                        if(Menu["tristana.use.e.f"]->GetBool() && Spells["tristana.e"]->IsReady()) {
                            Spells["tristana.e"]->Cast(m); } } } } } }

    if(Menu["tristana.use.r.auto"]->GetBool() && Spells["tristana.r"]->IsReady()) {
        for(auto r : g_ObjectManager->GetChampions()) {
            if(r != nullptr && r->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(r)) {
                on_execute(r); } } }

    if(Menu["tristana.use.r.int"]->GetBool() && Spells["tristana.r"]->IsReady()) {
        for(auto r : g_ObjectManager->GetChampions()) {
            if(r != nullptr && r->IsValidTarget() && g_LocalPlayer->IsInAutoAttackRange(r) && r->GetSpellbook()->IsChanneling()) {
                Spells["tristana.r"]->Cast(r); } } }

    on_post_update(); }

inline void EzTristana::on_post_update() {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        if(Menu["tristana.use.r.gap"]->GetBool() && Spells["tristana.r"]->IsReady()) {
            for(auto r : g_ObjectManager->GetChampions()) {
                if(r != nullptr && r->IsValidTarget(250)) {
                    auto pred = g_Common->GetPrediction(r, 250);

                    if(pred.Hitchance == HitChance::Dashing && r->IsFacing(g_LocalPlayer)) {
                        Spells["tristana.r"]->Cast(r); } } } } } }

inline void EzTristana::on_huddraw() {
    if(Menu["tristana.draww.en.w"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), 900, Menu["tristana.draww.w"]->GetColor(), 2); }

    if(Menu["tristana.draww.en.r"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), g_LocalPlayer->AttackRange(), Menu["tristana.draww.r"]->GetColor(), 2); }

    if(Menu["tristana.draww.en.hp"]->GetBool()) {
        g_Drawing->AddFilledRectOnScreen(g_LocalPlayer->HealthBarPosition(), Vector2(10, 10), Menu["tristana.draww.hp"]->GetColor()); } }

inline float EzTristana::edmg(IGameObject * unit) {
    if(!unit->HasBuff(hash("tristanaechargesound")) || !Menu["tristana.use.er.finish"]->GetBool()) {
        return 0; }

    auto multiplier = unit->HasBuff(hash("tristanaecharge")) ? unit->GetBuff(hash("tristanaecharge")).Count + 1 : 1;

    if(multiplier < Menu["tristana.use.er.min.stacks"]->GetInt()) {
        return 0; }

    auto phys = g_Common->CalculateDamageOnUnit(g_LocalPlayer, unit, DamageType::Physical,
    std::vector<double> {70, 80, 90, 100, 110 } [Spells["tristana.e"]->Level() - 1] + (std::vector<double> {
        0.5, 0.7, 0.9, 1.1, 1.3 } [Spells["tristana.e"]->Level() - 1] * g_LocalPlayer->AdditionalAttackDamage()) + (0.5 * g_LocalPlayer->
        FlatMagicDamageMod()));
    auto bonus = g_Common->CalculateDamageOnUnit(g_LocalPlayer, unit, DamageType::Physical,
    std::vector<double> {21, 24, 27, 30, 33 } [Spells["tristana.e"]->Level() - 1] + std::vector<double> {
        0.15, 0.195, 0.24, 0.285, 0.33 } [Spells["tristana.e"]->Level() - 1] * g_LocalPlayer->AdditionalAttackDamage() + (0.15 * g_LocalPlayer->
        FlatMagicDamageMod()));
    return (phys + (bonus * multiplier)); }

inline float EzTristana::rdmg(IGameObject * unit) {
    auto dmg = g_Common->CalculateDamageOnUnit(g_LocalPlayer, unit, DamageType::Magical,
            std::vector<double> {280, 380, 480 } [Spells["tristana.r"]->Level() - 1] + 1 * g_LocalPlayer->
            FlatMagicDamageMod());
    return dmg; }

inline void EzTristana::on_execute(IGameObject * unit) {
    if(unit != nullptr && !unit->IsDead() && !unit->IsZombie()) {
        if(Spells["tristana.r"]->IsReady() && Menu["tristana.use.r"]->GetBool()) {
            if(rdmg(unit) + edmg(unit) >= unit->Health()) {
                Spells["tristana.r"]->Cast(unit); }

            if(rdmg(unit) >= unit->Health()) {
                Spells["tristana.r"]->Cast(unit); } } } }

inline void EzTristana::on_before_attack(BeforeAttackOrbwalkerArgs * args) {
    if(args->Target->IsAITurret() && args->Target->CountMyEnemiesInRange(1000) < 1) {
        if(Menu["tristana.use.e.turret"]->GetBool()) {
            if(Spells["tristana.q"]->IsReady() && Spells["tristana.e"]->IsReady()) {
                Spells["tristana.e"]->Cast(args->Target); }

            if(Spells["tristana.q"]->IsReady()) {
                Spells["tristana.q"]->Cast(); } } } }
