#pragma once
#include "..\SDK\PluginSDK.h"
#include "..\SDK\EventArgs.h"
#include "..\Helpers\EzExtensions.h"

class EzKaisa {
  public:
    static float kaisa_ult_range();
    static std::map<std::string, std::shared_ptr<ISpell>> spells;
    static std::map<std::string, IMenuElement *> elements;
    static auto on_load(IMenu * menu) -> IMenu*;
    static auto on_update() -> void;
    static auto on_hpbardraw() -> void;
    static void on_huddraw();
    static void on_dash(IGameObject * unit, OnProcessSpellEventArgs * args);
    static void on_before_attack(BeforeAttackOrbwalkerArgs * args);
    static void on_process_spell(IGameObject * sender, OnProcessSpellEventArgs * args);
    static void on_create(IGameObject * sender);
    static void on_delete(IGameObject * sender);
    static void on_do_cast(IGameObject * sender, OnProcessSpellEventArgs * args);

    // damage
    static float qdmg(IGameObject * unit, int missiles);
    static float wdmg(IGameObject * unit);
    static float passivedmg(IGameObject * unit);
};

std::map<std::string, std::shared_ptr<ISpell>> EzKaisa::spells = {};
std::map<std::string, IMenuElement *> EzKaisa::elements = {};

inline auto EzKaisa::kaisa_ult_range() -> float {
    return std::vector<int> {1500, 2000, 2500} [spells["r"]->Level() - 1];
}

inline auto EzKaisa::on_load(IMenu * menu) -> IMenu * {
    auto drawMenu = menu->AddSubMenu("Kaisa Draw", "kaisa.draww");
    auto draw_q = drawMenu->AddSubMenu("Icathian Rain", "kaisa.draww.subq");
    elements["kaisa.draww.q1"] = draw_q->AddCheckBox("Enable Q", "kaisa.use.q1", true);
    elements["kaisa.draww.q"] = draw_q->AddColorPicker(" -> Color", "kaisa.draww.q", 255, 204, 0, 185);
    auto draw_w = drawMenu->AddSubMenu("Void Seeker", "kaisa.draww.subw");
    elements["kaisa.draww.w1"] = draw_w->AddCheckBox("Enable W", "kaisa.use.w1", true);
    elements["kaisa.draww.w"] = draw_w->AddColorPicker("-> Color", "kaisa.draww.w", 255, 204, 0, 185);
    auto draw_r = drawMenu->AddSubMenu("Killer Instinct", "kaisa.draww.subr");
    elements["kaisa.draww.r1"] = draw_r->AddCheckBox("Enable R", "kaisa.use.r1", true);
    elements["kaisa.draww.r"] = draw_r->AddColorPicker("-> Color", "kaisa.draww.r", 255, 204, 0, 185);
    auto draw_combo = drawMenu->AddSubMenu("Combo Damage", "kaisa.draww.subcombo");
    elements["kaisa.draw.combo1"] = draw_combo->AddCheckBox("Enable Combo Damage", "kaisa.draw.combo1", true);
    elements["kaisa.draw.combo"] = draw_combo->AddColorPicker("-> Color", "kaisa.draw.combo", 255, 204, 0, 185);
    auto comboMenu = menu->AddSubMenu("Kaisa Combo", "kaisa.combo");
    auto combo_q = comboMenu->AddSubMenu("Icathian Rain", "kaisa.combo.subq");
    elements["kaisa.use.q"] = combo_q->AddCheckBox("Enable Q", "kaisa.use.q", true);
    elements["kaisa.use.q.after.attack"] = combo_q->AddCheckBox("-> Only after attack", "kaisa.use.q.after.attack", false);
    elements["kaisa.missile.split"] = combo_q->AddSlider("-> Max missile split (units)", "kaisa.missile.split", 5, 1, 10, true);
    auto combo_w = comboMenu->AddSubMenu("Void Seeker", "kaisa.combo.subw");
    elements["kaisa.use.w"] = combo_w->AddCheckBox("Enable W", "kaisa.use.w", true);
    elements["kaisa.use.q.after.attack2"] = combo_w->AddCheckBox("-> Use after attack", "kaisa.use.q.after.attack", true);
    elements["kaisa.use.q.after.attack3"] = combo_w->AddCheckBox("-> Trigger Passive", "kaisa.use.q.explode.passive", true);
    auto combo_r = comboMenu->AddSubMenu("Killer Instinct", "kaisa.combo.subr");
    elements["kaisa.use.r"] = combo_r->AddCheckBox("Enable R", "kaisa.use.r", true);
    auto harassMenu = menu->AddSubMenu("Kaisa Harass", "kaisa.harass");
    auto harassWhitelistMenu = harassMenu->AddSubMenu("Harass Whitelist", "harass.whitelist");
    harassMenu->AddLabel("", "ok");

    // harass whitelist..
    for(auto i : g_ObjectManager->GetByType(EntityType::AIHeroClient)) {
        if(i->IsEnemy()) {
            elements[i->ChampionName().append("harass.enable")] = harassWhitelistMenu->AddCheckBox("Use on " + i->ChampionName(),
                    i->ChampionName().append("harass.enable"), true);
        }
    }

    auto harass_q = harassMenu->AddSubMenu("Icathian Rain", "kaisa.harass.subq");
    elements["kaisa.use.q.harass"] = harass_q->AddCheckBox("Enable Q", "kaisa.use.q.harass", true);
    elements["kaisa.use.q.harass.after.attack"] = harass_q->AddCheckBox("-> Only after attack", "kaisa.use.q.harass.after.attack", true);
    elements["kaisa.missile.split2"] = harass_q->AddSlider("-> Max missile split (units)", "kaisa.missile.split2", 7, 1, 10, true);
    auto harass_w = harassMenu->AddSubMenu("Void Seeker", "kaisa.harass.subw");
    elements["kaisa.use.w.harass"] = harass_w->AddCheckBox("Enable W", "kaisa.use.w.harass", true);
    elements["kaisa.use.w.harass.after.attack"] = harass_w->AddCheckBox("-> Only after attack", "kaisa.use.w.harass.after.attack", false);
    auto farmMenu = menu->AddSubMenu("Kaisa Farm", "kaisa.farm");
    auto farm_q = farmMenu->AddSubMenu("Icathian Rain", "kaisa.farm.subq");
    elements["kaisa.use.q.farm"] = farm_q->AddCheckBox("Enable Q", "kaisa.use.farm", true);
    elements["kaisa.killable.q"] = farm_q->AddSlider("-> Min killable units", "kaisa.killable.q", 3, 1, 10, true);
    auto a_menu = menu->AddSubMenu("Kaisa Misc", "kaisa.misc");
    elements["kaisa.use.w.imm"] = a_menu->AddCheckBox("Enable W on Immobile", "kaisa.use.w.imm", true);
    spells["q"] = g_Common->AddSpell(SpellSlot::Q, 600);
    spells["w"] = g_Common->AddSpell(SpellSlot::W, 3000);
    spells["w"]->SetSkillshot(0.5, 80, 1350, kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall, kSkillshotCircle);
    spells["r"] = g_Common->AddSpell(SpellSlot::R);
    return menu;
}

bool CompareDistanceToCursor(IGameObject * a, IGameObject * b) {
    return a->Distance(g_Common->CursorPosition()) < b->Distance(g_Common->CursorPosition());
}

inline void EzKaisa::on_update() {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo) && elements["kaisa.use.q"]->GetBool() &&
        !elements["kaisa.use.q.after.attack"]->GetBool()) {
        auto noob = g_Common->GetTarget(spells["q"]->Range(), DamageType::Physical);

        if(noob != nullptr && noob->IsValidTarget()) {
            const auto p = EzExtensions::get_in_range(noob, EntityType::AIMinionClient, 250, true);
            const auto prox = EzExtensions::get_in_range(noob, EntityType::AIHeroClient, 250, true) + p;

            if(spells["q"]->IsReady() && prox <= elements["kaisa.missile.split"]->GetInt()) {
                spells["q"]->Cast();
            }
        }
    }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFarm) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear)) {
        if(elements["kaisa.use.q.farm"]->GetBool()) {
            auto minions = EzExtensions::get_units_in_range(g_LocalPlayer, EntityType::AIMinionClient, spells["q"]->Range(), true);
            const auto valid_units = std::count_if(
            minions.begin(), minions.end(), [&](IGameObject *u) {
                return u != nullptr && u->IsValidTarget();
            });
            g_Common->ChatPrint(std::to_string(valid_units).c_str());

            if(valid_units >= elements["kaisa.killable.q"]->GetInt()) {
                if(spells["q"]->IsReady()) {
                    spells["q"]->Cast();
                }
            }
        }
    }

    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
        if(elements["kaisa.use.q.harass"]->GetBool() && !elements["kaisa.use.q.harass.after.attack"]->GetBool()) {
            auto noob = g_Common->GetTarget(spells["q"]->Range(), DamageType::Physical);

            if(noob != nullptr && noob->IsValidTarget()) {
                const auto p = EzExtensions::get_in_range(noob, EntityType::AIMinionClient, 250, true);
                const auto prox = EzExtensions::get_in_range(noob, EntityType::AIHeroClient, 250, true) + p;

                if(spells["q"]->IsReady() && prox <= elements["kaisa.missile.split2"]->GetInt()) {
                    spells["q"]->Cast();
                }
            }
        }
    }

    if(spells["w"]->IsReady() && elements["kaisa.use.w.imm"]->GetBool()) {
        for(auto h : g_ObjectManager->GetChampions()) {
            if(h != nullptr && h->IsValidTarget(spells["w"]->Range())) {
                const auto pred = EzExtensions::get_prediction(spells["w"], h);

                if(pred.Hitchance == HitChance::Immobile) {
                    spells["w"]->FastCast(pred.CastPosition);
                }
            }
        }
    }
}

inline void EzKaisa::on_hpbardraw() {
    if(elements["kaisa.draw.combo1"]->GetBool()) {
        for(auto i : g_ObjectManager->GetChampions()) {
            if(i != nullptr && !i->IsDead() && i->IsVisibleOnScreen() && i->IsEnemy()) {
                EzExtensions::draw_dmg_hpbar(i, qdmg(i, 6), std::to_string(qdmg(i, 6)).c_str(), elements["kaisa.draw.combo"]->GetColor());
            }
        }
    }
}

inline void EzKaisa::on_huddraw() {
    if(elements["kaisa.draww.q1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), spells["q"]->Range(), elements["kaisa.draww.q"]->GetColor(), 2);
    }

    if(elements["kaisa.draww.w1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), spells["w"]->Range(), elements["kaisa.draww.w"]->GetColor(), 2);
    }

    if(elements["kaisa.draww.r1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), spells["r"]->Range(), elements["kaisa.draww.r"]->GetColor(), 2);
    }

    if(elements["kaisa.draww.r1"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), kaisa_ult_range(), elements["kaisa.draww.r"]->GetColor(), 2);
    }
}

inline void EzKaisa::on_dash(IGameObject * unit, OnProcessSpellEventArgs * args) {}
inline void EzKaisa::on_before_attack(BeforeAttackOrbwalkerArgs * args) {}
inline void EzKaisa::on_process_spell(IGameObject * sender, OnProcessSpellEventArgs * args) {}
inline void EzKaisa::on_create(IGameObject * sender) {}
inline void EzKaisa::on_delete(IGameObject * sender) {}

inline void EzKaisa::on_do_cast(IGameObject * sender, OnProcessSpellEventArgs * args) {
    #pragma region after attack w

    if(sender->IsMe() && args->IsAutoAttack && spells["w"]->IsReady()) {
        const auto plasmaBuffName = "kaisapassivemarker";

        if(args->Target != nullptr && args->Target->IsValidTarget() && args->Target->IsAIHero()) {
            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo) && elements["kaisa.use.w"]->GetBool()) {
                const auto plasma_stacks = args->Target->GetBuff(hash(plasmaBuffName)).Count + 1;

                if(plasma_stacks >= 3) {
                    const auto pred = EzExtensions::get_prediction(spells["w"], args->Target);

                    if(pred.Hitchance >= HitChance::High) {
                        spells["w"]->FastCast(pred.CastPosition);
                    }
                }
            }

            if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo) && elements["kaisa.use.w"]->GetBool()) {
                const auto plasma_stacks = args->Target->GetBuff(hash(plasmaBuffName)).Count + 1;

                if(plasma_stacks >= 3) {
                    const auto pred = EzExtensions::get_prediction(spells["w"], args->Target);

                    if(pred.Hitchance >= EzExtensions::get_prefered_hitchance(g_LocalPlayer)) {
                        spells["w"]->FastCast(pred.CastPosition);
                    }
                }
            }
        }
    }

    #pragma endregion
    #pragma region after attack q

    if(sender->IsMe() && args->IsAutoAttack && spells["q"]->IsReady()) {
        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
            auto noob = g_Common->GetTarget(spells["q"]->Range(), DamageType::Physical);

            if(noob != nullptr && noob->IsValidTarget() && elements["kaisa.use.q"]->GetBool()) {
                const auto p = EzExtensions::get_in_range(noob, EntityType::AIMinionClient, 250, true);
                const auto prox = EzExtensions::get_in_range(noob, EntityType::AIHeroClient, 250, true) + p;

                if(spells["q"]->IsReady() && prox <= elements["kaisa.missile.split"]->GetInt()) {
                    spells["q"]->Cast();
                }
            }
        }

        if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) || g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass)) {
            auto noob = g_Common->GetTarget(spells["q"]->Range(), DamageType::Physical);

            if(noob != nullptr && noob->IsValidTarget() && elements["kaisa.use.q.harass"]->GetBool()) {
                const auto p = EzExtensions::get_in_range(noob, EntityType::AIMinionClient, 250, true);
                const auto prox = EzExtensions::get_in_range(noob, EntityType::AIHeroClient, 250, true) + p;

                if(spells["q"]->IsReady() && prox <= elements["kaisa.missile.split2"]->GetInt()) {
                    spells["q"]->Cast();
                }
            }
        }
    }

    #pragma endregion
}

inline float EzKaisa::qdmg(IGameObject * unit, int missiles) {
    const auto phys_dmg = g_Common->CalculateDamageOnUnit(g_LocalPlayer, unit, DamageType::Physical, std::vector<double> {43, 59.25, 75.3, 91.23, 108} [spells["q"]->Level()
                - 1] + (0.35 * g_LocalPlayer->AdditionalAttackDamage()) + (0.4 * g_LocalPlayer->FlatMagicDamageMod()));
    const auto first_missile_dmg = phys_dmg;
    const auto reduced_missile_dmg = 30 * first_missile_dmg / 100;
    const auto heroes_in_range = EzExtensions::get_in_range(unit, EntityType::AIHeroClient, spells["q"]->Range(), true);
    const auto minions_in_range = EzExtensions::get_in_range(unit, EntityType::AIMinionClient, spells["q"]->Range(), true);
    const auto i = missiles - 1 < 1 ? 1 : missiles - 1;
    const auto units = heroes_in_range + minions_in_range < 1 ? 1 : heroes_in_range + minions_in_range;
    const auto fixed_reduced_missile_dmg = (reduced_missile_dmg * i) / units;
    const auto enddmg = first_missile_dmg + fixed_reduced_missile_dmg;

    if(unit->IsMinion() || unit->IsMonster()) {
        if(unit->HealthPercent() < 35) {
            return enddmg * 2;
        }
    }

    return enddmg;
}

inline float EzKaisa::wdmg(IGameObject * unit) {
    return 0;
}
