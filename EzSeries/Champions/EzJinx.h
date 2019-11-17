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
        static void hpbarfill_render();

        static void on_buff(IGameObject * unit, OnBuffEventArgs * args);
        static void on_update();
        static void on_destory(IGameObject * unit);
        static void on_post_update();
        static void on_execute(IGameObject * unit);
        static void on_before_attack(BeforeAttackOrbwalkerArgs * args);
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline void EzJinx::init() {
    Spells["jinx.q"] = g_Common->AddSpell(SpellSlot::Q);
    Spells["jinx.w"] = g_Common->AddSpell(SpellSlot::W, 1450);
    Spells["jinx.w"]->SetSkillshot(0.6, 60, 3300, kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall, kSkillshotLine);
    Spells["jinx.e"] = g_Common->AddSpell(SpellSlot::E, 925);
    Spells["jinx.e"]->SetSkillshot(0.6, 120, 3300, kCollidesWithNothing, kSkillshotCircle);
    Spells["jinx.r"] = g_Common->AddSpell(SpellSlot::R, 3000);
    Spells["jinx.r"]->SetSkillshot(0.6, 250, 1700, kCollidesWithYasuoWall | kCollidesWithHeroes, kSkillshotLine);
    g_Log->Print("EzSeries: Jinx Loaded!");
    g_Common->ChatPrint(R"(<font color="#FF9966"><b>[EzSeries Jinx]:</b></font><b><font color="#99FF99"> Loaded!</font>)"); }

inline IMenu * EzJinx::on_boot(IMenu * menu) {
    init();
    auto c_menu = menu->AddSubMenu("Jinx: Core", "jinx.core");
    Menu["jinx.use.q"] = c_menu->AddCheckBox("Use Switcheroo", "jinx.use.q", true);
    Menu["jinx.use.w"] = c_menu->AddCheckBox("Use Zap", "jinx.use.w", true);
    Menu["jinx.use.e"] = c_menu->AddCheckBox("Use Chompers", "jinx.use.e", true);
    Menu["jinx.use.r"] = c_menu->AddCheckBox("Use Super Mega Death Rocket", "jinx.use.r", true);
    auto d_menu = menu->AddSubMenu("Jinx: Draw", "jinx.draww");
    Menu["jinx.draww.aa1"] = d_menu->AddCheckBox("Draw Switcheroo", "jinx.use.aa1", true);
    Menu["jinx.draww.aa"] = d_menu->AddColorPicker("-> Color Switcheroo", "jinx.draww.aa", 255, 153, 102, 80);
    Menu["jinx.draww.e1"] = d_menu->AddCheckBox("Draw Chompers", "jinx.use.e1", true);
    Menu["jinx.draww.e"] = d_menu->AddColorPicker("-> Color Chompers", "jinx.draww.e", 255, 153, 102, 80);
    Menu["jinx.draww.w1"] = d_menu->AddCheckBox("Draw Zap", "jinx.use.w1", true);
    Menu["jinx.draww.w"] = d_menu->AddColorPicker("-> Color Zap", "jinx.draww.w", 255, 153, 102, 80);
    Menu["jinx.draww.r1"] = d_menu->AddCheckBox("Draw Super Mega Death Rocket", "jinx.use.r1", true);
    Menu["jinx.draww.r"] = d_menu->AddColorPicker("-> Color Super Mega Death Rocket", "jinx.draww.r", 255, 153, 102, 80);
    Menu["jinx.draww.r.hp"] = d_menu->AddCheckBox("Draw HpBarDraw R Dmg", "jinx.draww.r.hp", true);
    Menu["jinx.draww.r.color"] = d_menu->AddColorPicker("-> Color HpBarDraw", "jinx.draww.r.color", 255, 153, 102, 185);
    auto mechanics_menu = menu->AddSubMenu("Jinx: Mechanics", "jinx.mechanics");
    auto zap_menu = mechanics_menu->AddSubMenu("Zap Settings", "zap.settings");
    Menu["jinx.use.w.mania"] = zap_menu->AddCheckBox("Block Zap on Mania", "jinx.use.w.mania", true);

    // harass whitelist..
    for(auto i : g_ObjectManager->GetByType(EntityType::AIHeroClient)) {
        if(i->IsEnemy()) {
            Menu[i->ChampionName().append("block.zapp")] =
                zap_menu->AddCheckBox("Block Zap on " + i->ChampionName(), i->ChampionName().append("block.zapp"),
                    false); } }

    Menu["jinx.maxx.r.dist"] = mechanics_menu->AddSlider("Max R Distance:", "jinx.maxx.r.dist", 3500, 700, 4500);
    Menu["jinx.use.q.splash"] = mechanics_menu->AddCheckBox("Q Splash on Mania", "jinx.use.q.splash", true);
    Menu["jinx.use.q.splash2"] = mechanics_menu->AddCheckBox("Q Splash on Pow-Pow Stacks", "jinx.use.q.splash2", true);
    Menu["jinx.use.q.splash4"] = mechanics_menu->AddCheckBox("Q Splash Pow-Pow Stacks Harass", "jinx.use.q.splash4", true);
    Menu["jinx.use.q.splash3"] = mechanics_menu->AddSlider("Q Splash only after Lvl >=", "jinx.use.q.splash.3", 11, 1, 18);
    menu->AddLabel("Automatic: ", "auto", true);
    Menu["jinx.use.e.tp"] = menu->AddCheckBox("Use E on Teleport", "jinx.use.e.tp", true);
    Menu["jinx.use.e.dash"] = menu->AddCheckBox("Use E on Dashing", "jinx.use.e.dash", true);
    Menu["jinx.use.e.int"] = menu->AddCheckBox("Use E on Immobile", "jinx.use.e.int", true);
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
            std::vector<double> {250, 350, 450 } [Spells["jinx.r"]->Level() - 1] + std::vector<double> {25, 30, 35 } [
                Spells["jinx.r"]->Level() - 1] / 100 * (unit->MaxHealth() - unit->Health()) + 1.5 * g_LocalPlayer->
            FlatPhysicalDamageMod());
    return d; }

inline void EzJinx::handle_rockets() {
    if(!g_LocalPlayer->HasBuff(hash("jinxq"))) {
        if(Menu["jinx.use.q"]->GetBool()) {
            // jinx ranges
            const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [Spells["jinx.q"]->Level() - 1];
            const auto my_range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();
            const auto mana_cap = Ex->get_mana(g_LocalPlayer);
            auto rev_stacks = 0;

            if(g_LocalPlayer->HasBuff(hash("jinxqramp"))) {
                rev_stacks = g_LocalPlayer->GetBuff(hash("jinxqramp")).Count; }

            // combo minigun -> rockets (combo)
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
                auto i = g_Common->GetTarget(Spells["jinx.w"]->Range(), DamageType::Physical);

                if(i != nullptr && i->IsValidTarget()) {
                    if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash mania
                        if(g_LocalPlayer->HasBuff(hash("jinxpassivekillmovementspeed")) && Menu["jinx.use.q.splash"]->GetBool()) {
                            if(mana_cap >= ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // switch
                                if(Spells["jinx.q"]->IsReady()) {
                                    Spells["jinx.q"]->Cast(); } } } // q splash pow pow

                        if(Menu["jinx.use.q.splash2"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap >= ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(Spells["jinx.q"]->IsReady()) {
                                        Spells["jinx.q"]->Cast(); } } } } }

                    if(mana_cap > ManaScenario::NoJuice || g_LocalPlayer->AutoAttackDamage(i, true) * 3 >= i->Health()) {
                        if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                            if(Spells["jinx.q"]->IsReady()) {
                                Spells["jinx.q"]->Cast(); } } } } }

            // combo minigun -> rockets (harass)
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
                const auto u = g_Common->GetTarget(Spells["jinx.w"]->Range(), DamageType::Physical);
                auto i = g_Orbwalker->GetTarget() != nullptr ? g_Orbwalker->GetTarget() : u;

                if(i != nullptr && i->IsValidTarget() && i->IsAIHero()) {
                    if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash pow pow
                        if(Menu["jinx.use.q.splash4"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap > ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(Spells["jinx.q"]->IsReady()) {
                                        Spells["jinx.q"]->Cast(); } } } } } }

                if(i != nullptr && i->IsValidTarget() && !i->IsAIHero()) {
                    if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                        // q splash pow pow
                        if(Menu["jinx.use.q.splash4"]->GetBool()) {
                            if(rev_stacks >= 3) {
                                if(mana_cap > ManaScenario::Medium && i->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                    // switch
                                    if(Spells["jinx.q"]->IsReady()) {
                                        Spells["jinx.q"]->Cast(); } } } } } }

                if(i != nullptr && i->IsValidTarget()) {
                    if(mana_cap >= ManaScenario::High || g_LocalPlayer->AutoAttackDamage(i, true) * 3 >= i->Health()) {
                        if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                            if(Spells["jinx.q"]->IsReady()) {
                                Spells["jinx.q"]->Cast(); } }

                        //else {
                        //  std::vector<IGameObject *> minions;
                        //  auto splash_count = EzExtensions::get_in_range(i, q_splash_range(), EntityType::AIMinionClient, minions);
                        //  if(splash_count >= 3) {
                        //    if(Spells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                        //      Spells["jinx.q"]->Cast(); } } }
                    } } }

            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
                for(auto i : g_ObjectManager->GetMinionsAll()) {
                    if(i != nullptr && i->IsValidTarget(my_range + bonus) && !i->IsAIHero()) {
                        if(mana_cap >= ManaScenario::Low && g_LocalPlayer->AutoAttackDamage(i, true) * 1 >= g_HealthPrediction->GetHealthPrediction(
                                i, windup_time(0, true))) {
                            if(g_LocalPlayer->Distance(i->ServerPosition()) > my_range) {
                                if(Spells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                                    Spells["jinx.q"]->Cast(); } }

                            //else {
                            //  std::vector<IGameObject *> minions;
                            //  auto splash_count = EzExtensions::get_in_range(i, q_splash_range(), EntityType::AIMinionClient, minions);
                            //  if(splash_count >= 3) {
                            //    if(Spells["jinx.q"]->IsReady() && !g_LocalPlayer->IsWindingUp()) {
                            //      Spells["jinx.q"]->Cast(); } } }
                        } } } } } } }

inline void EzJinx::rocket_farm(IGameObject * unit, OnProcessSpellEventArgs * args) {}

inline void EzJinx::on_huddraw() {
    const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [Spells["jinx.q"]->Level() - 1];
    const auto myRange = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();

    if(Menu["jinx.draww.aa1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), myRange + bonus, Menu["jinx.draww.aa"]->GetColor(), 2); }

    if(Menu["jinx.draww.e1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["jinx.e"]->Range(), Menu["jinx.draww.e"]->GetColor(), 2); }

    if(Menu["jinx.draww.w1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["jinx.w"]->Range(), Menu["jinx.draww.w"]->GetColor(), 2); }

    if(Menu["jinx.draww.r1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Menu["jinx.maxx.r.dist"]->GetInt(), Menu["jinx.draww.r"]->GetColor(), 2); } }

inline void EzJinx::hpbarfill_render() {
    if(!Menu["jinx.draww.r.hp"]->GetBool()) {
        return; }

    for(auto i : g_ObjectManager->GetChampions()) {
        if(i != nullptr && !i->IsDead() && i->IsVisibleOnScreen() && !i->IsMe() && i->IsValidTarget()) {
            Ex->draw_dmg_hpbar(i, jinx_ult_dmg(i), std::to_string(jinx_ult_dmg(i)).c_str(), Menu["jinx.draww.r.color"]->GetColor()); } } }

inline void EzJinx::on_buff(IGameObject * unit, OnBuffEventArgs * args) {
    if(!unit->IsAlly() && unit->IsValid() && !unit->IsDead()) {
        if(unit->HasBuff("teleport_target") && Menu["jinx.use.e.tp"]->GetBool()) {
            if(Spells["jinx.e"]->IsReady() && g_LocalPlayer->Distance(unit) <= Spells["jinx.e"]->Range()) {
                Spells["jinx.e"]->FastCast(unit->ServerPosition()); } } } }

inline void EzJinx::on_update() {
    // todo jinx ult sucks kinda
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        auto target = g_Common->GetTarget(Menu["jinx.maxx.r.dist"]->GetInt(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget() && Spells["jinx.r"]->IsReady() && Menu["jinx.use.r"]->GetBool()) {
            const auto pred = Ex->get_prediction(Spells["jinx.r"], target);

            // for max damage
            if(target->Distance(g_LocalPlayer) > 750) {
                if(jinx_ult_dmg(target) >= target->RealHealth(true, true)) {
                    if(pred.Hitchance >= HitChance::High) {
                        Spells["jinx.r"]->FastCast(pred.CastPosition); } } } } }

    // todo: jinx zap tweaks
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [Spells["jinx.q"]->Level() - 1];
        auto target = g_Common->GetTarget(Spells["jinx.w"]->Range(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget() && target->Distance(g_LocalPlayer) <= Spells["jinx.w"]->Range()) {
            if(Spells["jinx.w"]->IsReady() && Menu["jinx.use.w"]->GetBool()) {
                // - check menu nullptr
                if(Menu[target->ChampionName().append("block.zapp")] == nullptr) {
                    return; }

                // - block zap on here
                if(Menu[target->ChampionName().append("block.zapp")]->GetBool()) {
                    return; }

                // - block zap on mania
                if(Menu["jinx.use.w.mania"]->GetBool() && g_LocalPlayer->HasBuff("jinxpassivekillmovementspeed")) {
                    return; }

                const auto pred = Ex->get_prediction(Spells["jinx.w"], target);
                const auto range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();

                if(g_LocalPlayer->Distance(target) > range + bonus && g_LocalPlayer->CountMyEnemiesInRange(range) < 1) {
                    if(pred.Hitchance >= Ex->get_prefered_hitchance(target)) {
                        Spells["jinx.w"]->FastCast(pred.CastPosition); } } } } }

    // todo: jinx chompers
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        auto target = g_Common->GetTarget(Spells["jinx.e"]->Range(), DamageType::Physical);

        if(target != nullptr && target->IsValidTarget()) {
            if(Spells["jinx.e"]->IsReady() && Menu["jinx.use.e"]->GetBool()) {
                const auto pred = Ex->get_prediction(Spells["jinx.e"], target);

                if(pred.Hitchance == HitChance::Immobile || pred.Hitchance == HitChance::Dashing) {
                    Spells["jinx.e"]->FastCast(pred.CastPosition); }

                if(Ex->get_combat_scenario(g_LocalPlayer) == ScenarioType::ImSoFuckingDead && !g_LocalPlayer->IsFacing(target)) {
                    Spells["jinx.e"]->FastCast(g_LocalPlayer->ServerPosition()); }

                if(Ex->get_combat_scenario(g_LocalPlayer) == ScenarioType::Solo && target->IsFacing(g_LocalPlayer)) {
                    if(target->IsMelee() && target->Distance(g_LocalPlayer) <= 325) {
                        Spells["jinx.e"]->FastCast(g_LocalPlayer->ServerPosition()); } }

                if(Ex->get_combat_scenario(g_LocalPlayer) == ScenarioType::Skirmish) {
                    if(target->HasBuffOfType(BuffType::Sleep) || target->HasBuffOfType(BuffType::Knockup) ||
                        target->HasBuffOfType(BuffType::Flee)) {
                        Spells["jinx.e"]->FastCast(target->ServerPosition()); } } } } }

    handle_rockets();
    on_post_update(); }

inline void EzJinx::on_destory(IGameObject * unit) {}

inline void EzJinx::on_post_update() {
    if(Spells["jinx.e"]->IsReady()) {
        for(auto h : g_ObjectManager->GetChampions()) {
            if(h != nullptr && h->IsValidTarget(Spells["jinx.e"]->Range())) {
                const auto pred = Ex->get_prediction(Spells["jinx.e"], h);

                if(pred.Hitchance == HitChance::Immobile && Menu["jinx.use.e.int"]->GetBool()) {
                    Spells["jinx.e"]->FastCast(pred.CastPosition); }

                if(pred.Hitchance == HitChance::Dashing && Menu["jinx.use.e.dash"]->GetBool()) {
                    Spells["jinx.e"]->FastCast(pred.CastPosition); } } } } }

inline void EzJinx::on_execute(IGameObject * unit) {}

inline void EzJinx::on_before_attack(BeforeAttackOrbwalkerArgs * args) {
    if(args->Target == nullptr || !args->Target->IsValidTarget()) {
        return; } // jinx has rockets out

    if(g_LocalPlayer->HasBuff(hash("jinxq"))) {
        // jinx ranges
        const auto bonus = std::vector<int> {75, 100, 125, 150, 175 } [Spells["jinx.q"]->Level() - 1];
        const auto rangew_hit_box = g_LocalPlayer->AttackRange() + args->Target->BoundingRadius() + g_LocalPlayer->BoundingRadius();
        const auto my_range = g_LocalPlayer->AttackRange() + g_LocalPlayer->BoundingRadius();
        const auto mana_cap = Ex->get_mana(g_LocalPlayer);
        auto rev_stacks = 0;

        if(g_LocalPlayer->HasBuff(hash("jinxqramp"))) {
            rev_stacks = g_LocalPlayer->GetBuff(hash("jinxqramp")).Count; }

        // combo rocket -> minigun (combo)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
            if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                // q splash mania
                if(g_LocalPlayer->HasBuff(hash("jinxpassivekillmovementspeed")) && Menu["jinx.use.q.splash"]->GetBool()) {
                    if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                        // don't switch
                        return; } }

                // q splash pow pow
                if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && Menu["jinx.use.q.splash2"]->GetBool()) {
                    if(rev_stacks == 3) {
                        if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                            // don't switch
                            return; } } } }

            if(args->Target->IsAIBase() && (Menu["jinx.use.q"]->GetBool())) {
                if(g_LocalPlayer->Distance(args->Target) <= my_range - bonus && Spells["jinx.q"]->IsReady()) {
                    Spells["jinx.q"]->Cast(); } } }

        // combo rocket -> minigun (mixed)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(args->Target->IsAIHero() && Menu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = Ex->get_mana(g_LocalPlayer);

                // leave rockets out to poke turret
                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } }

                // put rockets away if in minigun range (may need tweaking)
                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= my_range - bonus) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } } } }

        // combo rocket -> minigun (farm)
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(!args->Target->IsAIHero() && Menu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = Ex->get_mana(g_LocalPlayer);

                // leave rockets out to poke turret
                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } } // q splash pow pow

                if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                    if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && Menu["jinx.use.q.splash4"]->GetBool()) {
                        if(rev_stacks == 3) {
                            if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // don't switch
                                return; } } } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } } } }

        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(!args->Target->IsAIHero() && Menu["jinx.use.q"]->GetBool()) {
                const auto mana_cap = Ex->get_mana(g_LocalPlayer); // leave rockets out to poke turret

                if(args->Target->IsAITurret() || args->Target->IsInhibitor()) {
                    if(mana_cap >= ManaScenario::High && g_LocalPlayer->Distance(args->Target) > my_range - bonus) {
                        return; } }

                // q splash pow pow
                if(g_LocalPlayer->Level() >= Menu["jinx.use.q.splash3"]->GetInt()) {
                    if(g_LocalPlayer->HasBuff(hash("jinxqramp")) && Menu["jinx.use.q.splash4"]->GetBool()) {
                        if(rev_stacks == 3) {
                            if(mana_cap >= ManaScenario::Medium && args->Target->CountMyEnemiesInRange(q_splash_range()) >= 2) {
                                // don't switch
                                return; } } } }

                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::Medium && !(g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health())) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } } } }

        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
            const auto killable = g_LocalPlayer->AutoAttackDamage(args->Target, true) > args->Target->Health();

            if(!args->Target->IsAIHero()) {
                // put rockets away unless they can kill the target/minion
                if(mana_cap <= ManaScenario::High && !killable) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } }

                if(g_LocalPlayer->Distance(args->Target->ServerPosition()) <= rangew_hit_box - bonus) {
                    if(Spells["jinx.q"]->IsReady()) {
                        Spells["jinx.q"]->Cast(); } } } } } }

inline void EzJinx::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm)) {
        if(g_LocalPlayer->HasBuff(hash("jinxq"))) {
            if(unit->IsMe() && args->IsAutoAttack) {
                if(Spells["jinx.q"]->IsReady()) {
                    Spells["jinx.q"]->Cast(); } } } } }
