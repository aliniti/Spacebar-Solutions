#pragma once
#include "EzChampion.h"
#include "../Helpers/EzEnums.h"

class EzJinx : public EzChampion {
    public:
        static auto init() -> void;
        static IMenu * on_boot(IMenu * menu);
        static float q_splash_range();
        static float windup_time(float extraWindup, bool toRockets = false);
        static float jinx_ult_dmg(IGameObject * unit);
        static void handle_rockets();
        static void rocket_farm(IGameObject * unit, OnProcessSpellEventArgs * args);
        static void on_huddraw();
        static void on_hpbar_draw();
        static void on_update();
        static void on_delete_obj(IGameObject * unit);
        static void on_post_update();
        static void on_execute(IGameObject * unit);
        static void on_before_attack(BeforeAttackOrbwalkerArgs * args);
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline void EzJinx::init() {
    ezspells["jinx.q"] = g_Common->AddSpell(SpellSlot::Q);
    ezspells["jinx.w"] = g_Common->AddSpell(SpellSlot::W, 1450);
    ezspells["jinx.w"]->SetSkillshot(0.6, 60, 3300, kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall, kSkillshotLine);
    ezspells["jinx.e"] = g_Common->AddSpell(SpellSlot::E, 925);
    ezspells["jinx.e"]->SetSkillshot(0.6, 120, 3300, kCollidesWithNothing, kSkillshotCircle);
    ezspells["jinx.r"] = g_Common->AddSpell(SpellSlot::R, 3000);
    ezspells["jinx.r"]->SetSkillshot(0.6, 250, 1700, kCollidesWithYasuoWall | kCollidesWithHeroes, kSkillshotLine);
    g_Log->Print("EzSeries: Jinx Loaded!");
    g_Common->ChatPrint(R"(<font color="#FF9966"><b>[EzSeries Jinx]:</b></font><b><font color="#99FF99"> Loaded!</font>)"); }

inline IMenu * EzJinx::on_boot(IMenu * menu) {
    init();
    auto c_menu = menu->AddSubMenu("Jinx: Core", "jinx.core");
    ezmenu["jinx.use.q"] = c_menu->AddCheckBox("Use Switcheroo", "jinx.use.q", true);
    ezmenu["jinx.use.w"] = c_menu->AddCheckBox("Use Zap", "jinx.use.w", true);
    ezmenu["jinx.use.e"] = c_menu->AddCheckBox("Use Chompers", "jinx.use.e", true);
    ezmenu["jinx.use.r"] = c_menu->AddCheckBox("Use Super Mega Death Rocket", "jinx.use.r", true);
    auto d_menu = menu->AddSubMenu("Jinx: Draw", "jinx.draww");
    ezmenu["jinx.draww.aa1"] = d_menu->AddCheckBox("Draw Switcheroo", "jinx.use.aa1", true);
    ezmenu["jinx.draww.aa"] = d_menu->AddColorPicker("-> Color Switcheroo", "jinx.draww.aa", 255, 153, 102, 80);
    ezmenu["jinx.draww.e1"] = d_menu->AddCheckBox("Draw Chompers", "jinx.use.e1", true);
    ezmenu["jinx.draww.e"] = d_menu->AddColorPicker("-> Color Chompers", "jinx.draww.e", 255, 153, 102, 80);
    ezmenu["jinx.draww.w1"] = d_menu->AddCheckBox("Draw Zap", "jinx.use.w1", true);
    ezmenu["jinx.draww.w"] = d_menu->AddColorPicker("-> Color Zap", "jinx.draww.w", 255, 153, 102, 80);
    ezmenu["jinx.draww.r1"] = d_menu->AddCheckBox("Draw Super Mega Death Rocket", "jinx.use.r1", true);
    ezmenu["jinx.draww.r"] = d_menu->AddColorPicker("-> Color Super Mega Death Rocket", "jinx.draww.r", 255, 153, 102, 80);
    ezmenu["jinx.draww.r.hp"] = d_menu->AddCheckBox("Draw HpBarDraw R Dmg", "jinx.draww.r.hp", true);
    ezmenu["jinx.draww.r.color"] = d_menu->AddColorPicker("-> Color HpBarDraw", "jinx.draww.r.color", 255, 153, 102, 185);
    auto mechanics_menu = menu->AddSubMenu("Jinx: Mechanics", "jinx.mechanics");
    auto zap_menu = mechanics_menu->AddSubMenu("Zap Settings", "zap.settings");
    ezmenu["jinx.use.w.mania"] = zap_menu->AddCheckBox("Block Zap on Mania", "jinx.use.w.mania", true); // harass whitelist..

    for(auto i : g_ObjectManager->GetByType(EntityType::AIHeroClient)) {
        if(i->IsEnemy()) {
            ezmenu[i->ChampionName().append("block.zapp")] = zap_menu->AddCheckBox("Block Zap on " + i->ChampionName(), i->ChampionName().append("block.zapp"),
                    false); } }

    ezmenu["jinx.maxx.r.dist"] = mechanics_menu->AddSlider("Max R Distance:", "jinx.maxx.r.dist", 3500, 700, 4500);
    ezmenu["jinx.use.q.splash"] = mechanics_menu->AddCheckBox("Q Splash on Mania", "jinx.use.q.splash", true);
    ezmenu["jinx.use.q.splash2"] = mechanics_menu->AddCheckBox("Q Splash on Pow-Pow Stacks", "jinx.use.q.splash2", true);
    ezmenu["jinx.use.q.splash4"] = mechanics_menu->AddCheckBox("Q Splash Pow-Pow Stacks Harass", "jinx.use.q.splash4", true);
    ezmenu["jinx.use.q.splash3"] = mechanics_menu->AddSlider("Q Splash only after Lvl >=", "jinx.use.q.splash.3", 11, 1, 18);
    menu->AddLabel("Automatic: ", "auto", true);
    ezmenu["jinx.use.e.dash"] = menu->AddCheckBox("Use E on Dashing", "jinx.use.e.dash", true);
    ezmenu["jinx.use.e.int"] = menu->AddCheckBox("Use E on Immobile", "jinx.use.e.int", true);
    return menu; }

inline float EzJinx::q_splash_range() {
    if(g_LocalPlayer->HasItem(ItemId::Runaans_Hurricane)) {
        return 300; }

    return 185; }

inline float EzJinx::windup_time(float extraWindup, bool toRockets) {
    auto realWindup = g_LocalPlayer->AttackCastDelay() * 1000;
    auto delay = 0;

    if(toRockets) {
        delay = 250 - g_Common->Ping() > 0 ? 250 - g_Common->Ping() : 0;
        realWindup = realWindup + (realWindup * 0.25); }

    return delay + extraWindup + realWindup; }

inline float EzJinx::jinx_ult_dmg(IGameObject * unit) {
    // max ult damage from far range
    const auto d = g_Common->CalculateDamageOnUnit(g_LocalPlayer, unit, DamageType::Physical,
            std::vector<double> {250, 350, 450 } [ezspells["jinx.r"]->Level() - 1] + std::vector<double> {25, 30, 35 } [
                ezspells["jinx.r"]->Level() - 1] / 100 * (unit->MaxHealth() - unit->Health()) + 1.5 * g_LocalPlayer->
            FlatPhysicalDamageMod());
    return d; }

inline void EzJinx::handle_rockets() {
    if(!g_LocalPlayer->HasBuff(hash("jinxq"))) {
        if(ezmenu["jinx.use.q"]->GetBool()) {
            // jinx ranges
            const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [ezspells["jinx.q"]->Level() - 1];
            const auto my_range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();
            const auto mana_cap = ezex->get_mana(g_LocalPlayer);
            auto rev_stacks = 0;

            if(g_LocalPlayer->HasBuff(hash("jinxqramp"))) {
                rev_stacks = g_LocalPlayer->GetBuff(hash("jinxqramp")).Count; }

            // combo minigun -> rockets (combo)
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
                auto i = g_Common->GetTarget(ezspells["jinx.w"]->Range(), DamageType::Physical);

                if(i != nullptr && i->IsValidTarget()) {
                    if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash mania
                        if(g_LocalPlayer->HasBuff(hash("jinxpassivekillmovementspeed")) && ezmenu["jinx.use.q.splash"]->GetBool()) {
                            if(mana_cap >= ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // switch
                                if(ezspells["jinx.q"]->IsReady()) {
                                    ezspells["jinx.q"]->Cast(); } } } // q splash pow pow

                        if(ezmenu["jinx.use.q.splash2"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap >= ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(ezspells["jinx.q"]->IsReady()) {
                                        ezspells["jinx.q"]->Cast(); } } } } }

                    if(mana_cap > ManaScenario::NoJuice || g_LocalPlayer->AutoAttackDamage(i, true) * 3 >= i->Health()) {
                        if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                            if(ezspells["jinx.q"]->IsReady()) {
                                ezspells["jinx.q"]->Cast(); } } } } }

            // combo minigun -> rockets (harass)
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
                const auto u = g_Common->GetTarget(ezspells["jinx.w"]->Range(), DamageType::Physical);
                auto i = g_Orbwalker->GetTarget() != nullptr ? g_Orbwalker->GetTarget() : u;

                if(i != nullptr && i->IsValidTarget() && i->IsAIHero()) {
                    if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash pow pow
                        if(ezmenu["jinx.use.q.splash4"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap > ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(ezspells["jinx.q"]->IsReady()) {
                                        ezspells["jinx.q"]->Cast(); } } } } } }

                if(i != nullptr && i->IsValidTarget() && !i->IsAIHero()) {
                    if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash pow pow
                        if(ezmenu["jinx.use.q.splash4"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap > ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(ezspells["jinx.q"]->IsReady()) {
                                        ezspells["jinx.q"]->Cast(); } } } } } }

                if(i != nullptr && i->IsValidTarget()) {
                    if(mana_cap >= ManaScenario::High || g_LocalPlayer->AutoAttackDamage(i, true) * 3 >= i->Health()) {
                        if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                            if(ezspells["jinx.q"]->IsReady()) {
                                ezspells["jinx.q"]->Cast(); } }

                        //else {
                        //  std::vector<IGameObject *> minions;
                        //  auto splash_count = EzExtensions::get_in_range(i, q_splash_range(), EntityType::AIMinionClient, minions);
                        //  if(splash_count >= 3) {
                        //    if(ezspells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                        //      ezspells["jinx.q"]->Cast(); } } }
                    } } }

            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
                for(auto i : g_ObjectManager->GetMinionsAll()) {
                    if(i != nullptr && i->IsValidTarget(my_range + bonus) && !i->IsAIHero()) {
                        if(mana_cap >= ManaScenario::Low && g_LocalPlayer->AutoAttackDamage(i, true) * 1 >= g_HealthPrediction->GetHealthPrediction(
                                i, windup_time(0, true))) {
                            if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                                if(ezspells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                                    ezspells["jinx.q"]->Cast(); } }

                            //else {
                            //  std::vector<IGameObject *> minions;
                            //  auto splash_count = EzExtensions::get_in_range(i, q_splash_range(), EntityType::AIMinionClient, minions);
                            //  if(splash_count >= 3) {
                            //    if(ezspells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                            //      ezspells["jinx.q"]->Cast(); } } }
                        } } } } } } }

inline void EzJinx::rocket_farm(IGameObject * unit, OnProcessSpellEventArgs * args) {}

inline void EzJinx::on_huddraw() {
    const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [ezspells["jinx.q"]->Level() - 1];
    const auto myRange = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();

    if(ezmenu["jinx.draww.aa1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), myRange + bonus, ezmenu["jinx.draww.aa"]->GetColor(), 2); }

    if(ezmenu["jinx.draww.e1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), ezspells["jinx.e"]->Range(), ezmenu["jinx.draww.e"]->GetColor(), 2); }

    if(ezmenu["jinx.draww.w1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), ezspells["jinx.w"]->Range(), ezmenu["jinx.draww.w"]->GetColor(), 2); }

    if(ezmenu["jinx.draww.r1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), ezmenu["jinx.maxx.r.dist"]->GetInt(), ezmenu["jinx.draww.r"]->GetColor(), 2); } }

inline void EzJinx::on_hpbar_draw() {
    if(!ezmenu["jinx.draww.r.hp"]->GetBool()) {
        return; }

    for(auto i : g_ObjectManager->GetChampions()) {
        if(i != nullptr && !i->IsDead() && i->IsVisibleOnScreen() && !i->IsMe() && i->IsValidTarget()) {
            ezex->draw_dmg_hpbar(i, jinx_ult_dmg(i), std::to_string(jinx_ult_dmg(i)).c_str(), ezmenu["jinx.draww.r.color"]->GetColor()); } } }

inline void EzJinx::on_update() {
    // todo jinx ult sucks kinda
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        auto target = g_Common->GetTarget(ezmenu["jinx.maxx.r.dist"]->GetInt(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget() && ezspells["jinx.r"]->IsReady() && ezmenu["jinx.use.r"]->GetBool()) {
            const auto pred = ezex->get_prediction(ezspells["jinx.r"], target); // for max damage

            if(target->Distance(g_LocalPlayer) > 750) {
                if(jinx_ult_dmg(target) >= target->RealHealth(true, true)) {
                    if(pred.Hitchance >= HitChance::High) {
                        ezspells["jinx.r"]->FastCast(pred.CastPosition); } } } } } // todo: jinx zap tweaks

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [ezspells["jinx.q"]->Level() - 1];
        auto target = g_Common->GetTarget(ezspells["jinx.w"]->Range(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget() && target->Distance(g_LocalPlayer) <= ezspells["jinx.w"]->Range()) {
            if(ezspells["jinx.w"]->IsReady() && ezmenu["jinx.use.w"]->GetBool() && !ezmenu[target->ChampionName().append("block.zapp")]->GetBool() && (! ezmenu
                    ["jinx.use.w.mania"]->GetBool() || !g_LocalPlayer->HasBuff("jinxpassivekillmovementspeed"))) {
                const auto pred = ezex->get_prediction(ezspells["jinx.w"], target);
                const auto range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();

                if(g_LocalPlayer->Distance(target) > range + bonus && g_LocalPlayer->CountMyEnemiesInRange(range) < 1) {
                    if(pred.Hitchance >= ezex->get_prefered_hitchance(target)) {
                        ezspells["jinx.w"]->FastCast(pred.CastPosition); } } } } } // todo: jinx chompers

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        auto target = g_Common->GetTarget(ezspells["jinx.e"]->Range(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget()) {
            if(ezspells["jinx.e"]->IsReady() && ezmenu["jinx.use.e"]->GetBool()) {
                const auto pred = ezex->get_prediction(ezspells["jinx.e"], target);

                if(pred.Hitchance == HitChance::Immobile || pred.Hitchance == HitChance::Dashing) {
                    ezspells["jinx.e"]->FastCast(pred.CastPosition); }

                if(ezex->get_combat_scenario(g_LocalPlayer) == ScenarioType::ImSoFuckingDead && !g_LocalPlayer->IsFacing(target)) {
                    ezspells["jinx.e"]->FastCast(g_LocalPlayer->ServerPosition()); }

                if(ezex->get_combat_scenario(g_LocalPlayer) == ScenarioType::Solo && !target->IsFacing(g_LocalPlayer)) {
                    if(target->HealthPercent() < 50 && target->Distance(g_LocalPlayer) <= 420) {
                        ezspells["jinx.e"]->FastCast(pred.CastPosition); } }

                if(ezex->get_combat_scenario(g_LocalPlayer) == ScenarioType::Skirmish) {
                    if(target->HasBuffOfType(BuffType::Slow) || target->HasBuffOfType(BuffType::Knockup) || target->HasBuffOfType(BuffType::Flee)) {
                        ezspells["jinx.e"]->FastCast(pred.CastPosition); } } } } }

    handle_rockets();
    on_post_update(); }

inline void EzJinx::on_delete_obj(IGameObject * unit) {}

inline void EzJinx::on_post_update() {
    if(ezspells["jinx.e"]->IsReady()) {
        for(auto h : g_ObjectManager->GetChampions()) {
            if(h != nullptr && h->IsValidTarget(ezspells["jinx.e"]->Range())) {
                const auto pred = ezex->get_prediction(ezspells["jinx.e"], h);

                if(pred.Hitchance == HitChance::Immobile && ezmenu["jinx.use.e.int"]->GetBool()) {
                    ezspells["jinx.e"]->FastCast(pred.CastPosition); }

                if(pred.Hitchance == HitChance::Dashing && ezmenu["jinx.use.e.dash"]->GetBool()) {
                    ezspells["jinx.e"]->FastCast(pred.CastPosition); } } } } }

inline void EzJinx::on_execute(IGameObject * unit) {}

inline void EzJinx::on_before_attack(BeforeAttackOrbwalkerArgs * args) {
    if(args->Target == nullptr || !args->Target->IsValidTarget()) {
        return; } // jinx has rockets out

    if(g_LocalPlayer->HasBuff(hash("jinxq"))) {
        // jinx ranges
        const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [ezspells["jinx.q"]->Level() - 1];
        const auto rangew_hit_box = g_LocalPlayer->AttackRange() + args->Target->BoundingRadius() + g_LocalPlayer->BoundingRadius();
        const auto my_range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();
        const auto mana_cap = ezex->get_mana(g_LocalPlayer);
        auto rev_stacks = 0;

        if(g_LocalPlayer->HasBuff(hash("jinxqramp"))) {
            rev_stacks = g_LocalPlayer->GetBuff(hash("jinxqramp")).Count; }

        // combo rocket -> minigun (combo)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
            if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                // q splash mania
                if(g_LocalPlayer->HasBuff(hash("jinxpassivekillmovementspeed")) && ezmenu["jinx.use.q.splash"]->GetBool()) {
                    if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                        // don't switch
                        return; } }

                // q splash pow pow
                if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && ezmenu["jinx.use.q.splash2"]->GetBool()) {
                    if(rev_stacks == 3) {
                        if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                            // don't switch
                            return; } } } }

            if(args->Target->IsAIBase() && (ezmenu["jinx.use.q"]->GetBool())) {
                if(g_LocalPlayer->Distance(args->Target) <= my_range - bonus && ezspells["jinx.q"]->IsReady()) {
                    ezspells["jinx.q"]->Cast(); } } }

        // combo rocket -> minigun (mixed)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(args->Target->IsAIHero() && ezmenu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = ezex->get_mana(g_LocalPlayer);

                // leave rockets out to poke turret
                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } }

                // put rockets away if in minigun range (may need tweaking)
                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= my_range - bonus) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } } } }

        // combo rocket -> minigun (farm)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(!args->Target->IsAIHero() && ezmenu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = ezex->get_mana(g_LocalPlayer);

                // leave rockets out to poke turret
                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } } // q splash pow pow

                if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                    if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && ezmenu["jinx.use.q.splash4"]->GetBool()) {
                        if(rev_stacks == 3) {
                            if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // don't switch
                                return; } } } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } } } }

        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(!args->Target->IsAIHero() && ezmenu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = ezex->get_mana(g_LocalPlayer); // leave rockets out to poke turret

                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } }

                // q splash pow pow
                if(g_LocalPlayer->Level() >= ezmenu["jinx.use.q.splash3"]->GetInt()) {
                    if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && ezmenu["jinx.use.q.splash4"]->GetBool()) {
                        if(rev_stacks == 3) {
                            if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // don't switch
                                return; } } } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } } } }

        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
            const auto killable = g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health();

            if(!args->Target->IsAIHero()) {
                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::High && !killable) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(ezspells["jinx.q"]->IsReady()) {
                        ezspells["jinx.q"]->Cast(); } } } } } }

inline void EzJinx::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
        if(g_LocalPlayer->HasBuff(hash("jinxq"))) {
            if(unit->IsMe() && args->IsAutoAttack) {
                if(ezspells["jinx.q"]->IsReady()) {
                    ezspells["jinx.q"]->Cast(); } } } } }
