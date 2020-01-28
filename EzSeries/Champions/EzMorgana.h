#pragma once
#include "EzChampion.h"


class EzMorgana : public EzChampion {
    public:

        static auto on_boot(IMenu * menu)->IMenu*;
        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void;
        static auto on_draw() -> void;
        static auto on_create(IGameObject * obj) -> void;
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args);
        static void on_post_update();
        static void on_update();
        static void on_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline auto EzMorgana::on_boot(IMenu * menu) -> IMenu * {

    auto c_menu = menu->AddSubMenu("Morgana: Core", "morgana.core");
    Menu["morgana.use.q"] = c_menu->AddCheckBox("Use Dark Binding (Q)", "morgana.use.q", true);
    Menu["morgana.use.w"] = c_menu->AddCheckBox("Use Tormented Shadow (W)", "morgana.use.w", true);
    Menu["morgana.use.e"] = c_menu->AddCheckBox("Use Black Shield (E)", "morgana.use.e", true);
    //Menu["morgana.use.r"] = c_menu->AddCheckBox("Use (R)", "morgana.use.r", true);

    auto d_menu = menu->AddSubMenu("Morgana: Draw", "morgana.draw");
    Menu["morgana.draw.q"] = d_menu->AddCheckBox("Draw Dark Binding (Q)", "morgana.draw.q", true);
    Menu["morgana.draw.w"] = d_menu->AddCheckBox("Draw Tormented Shadow (W)", "morgana.draw.w", true);
    Menu["morgana.draw.e"] = d_menu->AddCheckBox("Draw Black Shield (E)", "morgana.draw.e", true);
    Menu["morgana.draw.r"] = d_menu->AddCheckBox("Draw Soul Shackles (R)", "morgana.draw.r", true);

    auto x_menu = menu->AddSubMenu("Morgana: Mechanics", "morgana.mech");
    Menu["morgana.use.fastw"] = x_menu->AddCheckBox("Use Fast (W)", "morgana.use.fastw", false);
    Menu["morgana.use.w2"] = x_menu->AddCheckBox("Use (W) Only Immobile", "morgana.use.w2", true);

    auto s_menu = menu->AddSubMenu("Morgana: BlackShield", "morgana.shield");
    Menu["morgana.use.e.mana"] = s_menu->AddSlider("Minimum Mana (%)", "morgana.use.e.mana", 55, 0, 100);
    Menu["morgana.use.e.t"] = s_menu->AddCheckBox("Shield Targeted Spells", "morgana.use.e.t", true);
    Menu["morgana.use.e.s"] = s_menu->AddCheckBox("Shield Line Spells", "morgana.use.e.s", true);
    Menu["morgana.use.e.cs"] = s_menu->AddCheckBox("Shield Circle Spells", "morgana.use.e.cs", true);
    Menu["morgana.use.e.missile"] = s_menu->AddCheckBox("Shield Missiles", "morgana.use.e.missile", true);

    for(auto i : g_ObjectManager->GetByType(EntityType::AIHeroClient)) {
        if(i->IsAlly()) {
            Menu[i->ChampionName().append("shield.enable")] = s_menu->AddCheckBox("Shield " + i->ChampionName(),
                    i->ChampionName().append("shield.enable"), true); } }

    auto a_menu = menu->AddSubMenu("Morgana: Auto", "morgana.auto");
    Menu["morgana.auto.q"] = a_menu->AddCheckBox("Use (Q) Immobile", "morgana.auto.q", true);
    Menu["morgana.auto.w"] = a_menu->AddCheckBox("Use (W) Immobile", "morgana.auto.w", true);



    Spells["morgana.q"] = g_Common->AddSpell(SpellSlot::Q, 1175);
    Spells["morgana.q"]->SetSkillshot(0.25f, 225, 1200, kCollidesWithHeroes | kCollidesWithMinions, kSkillshotLine);
    Spells["morgana.w"] = g_Common->AddSpell(SpellSlot::W, 900);
    Spells["morgana.w"]->SetSkillshot(0.25f, 275, 3800, kCollidesWithNothing, kSkillshotCircle);
    Spells["morgana.e"] = g_Common->AddSpell(SpellSlot::E, 800);
    Spells["morgana.r"] = g_Common->AddSpell(SpellSlot::R, 625);

    return menu; }

inline auto EzMorgana::on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void {}

inline void EzMorgana::on_draw() {

    if(Menu["morgana.draw.q"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["morgana.q"]->Range(), RGBA(204, 102, 204, 115)); }

    if(Menu["morgana.draw.w"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["morgana.w"]->Range(), RGBA(217, 140, 217, 115)); }

    if(Menu["morgana.draw.e"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["morgana.e"]->Range(), RGBA(217, 140, 217, 115)); }

    if(Menu["morgana.draw.r"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["morgana.r"]->Range(), RGBA(204, 102, 204, 115)); } }


inline void EzMorgana::on_create(IGameObject * obj) {

}

inline void EzMorgana::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(Menu["morgana.use.e.spell"] && unit->IsAIHero() && unit->IsEnemy()) {

        if(g_LocalPlayer->ManaPercent() < Menu["morgana.use.e.mana"]->GetInt()) {
            return; }

        if(Spells["morgana.e"]->IsReady() && Menu["morgana.use.e"]->GetBool()) {
            for(auto hero : g_ObjectManager->GetChampions()) {
                if(hero->IsAlly() && Menu[hero->ChampionName().append("shield.enable")]->GetBool()) {
                    if(hero->IsValidTarget(Spells["morgana.e"]->Range(), false)) {

                        if(args->SpellData->TargetingType == SpellTargeting::Area) {
                            if(hero->Distance(args->EndPosition) <= args->SpellData->CastRadius[0]) {
                                Spells["morgana.e"]->Cast(hero); } }

                        if(args->SpellData->TargetingType == SpellTargeting::Direction) {
                            auto startPos = unit->ServerPosition();
                            auto endPos = startPos + (args->EndPosition - startPos).Normalized() * args->SpellData->CastRange[0];
                            auto proj = hero->ServerPosition().ProjectOn(startPos, endPos);

                            if(proj.IsOnSegment && hero->Distance(proj.SegmentPoint) <= args->SpellData->LineWidth) {
                                Spells["morgana.e"]->Cast(hero); } } } } } } }

    if(unit->IsMe() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        if(Spells["morgana.q"]->IsReady() && Menu["morgana.use.q"]->GetBool() && Menu["morgana.use.fastw"]->GetBool()) {
            if(strcmp(args->SpellData->SpellName.c_str(), "MorganaQ") == 0 && Menu["morgana.use.q"]->GetBool()) {
                auto q_target = g_Common->GetTarget(Spells["morgana.q"]->Range(), DamageType::Magical);

                if(q_target != nullptr && q_target->IsValidTarget(Spells["morgana.w"]->Range())) {
                    auto startPos = unit->ServerPosition();
                    auto endPos = startPos + (args->EndPosition - startPos).Normalized() * Spells["morgana.q"]->Range();
                    auto proj = q_target->ServerPosition().ProjectOn(startPos, endPos);

                    if(proj.IsOnSegment && q_target->Distance(proj.SegmentPoint) <= Spells["morgana.q"]->Radius()) {
                        Spells["morgana.w"]->FastCast(q_target->ServerPosition()); } } } } } }

inline void EzMorgana::on_post_update() {
    if(Menu["morgana.use.w"]->GetBool() && Spells["morgana.w"]->IsReady() && Menu["morgana.auto.w"]->GetBool()) {
        for(auto hero : g_ObjectManager->GetChampions()) {
            if(hero->IsEnemy() && hero->IsValidTarget(Spells["morgana.w"]->Range())) {
                auto pred = Ex->get_prediction(Spells["morgana.w"], hero);

                if(pred.Hitchance == HitChance::Immobile) {
                    Spells["morgana.w"]->FastCast(pred.CastPosition); } } } }

    if(Menu["morgana.use.q"]->GetBool() && Spells["morgana.q"]->IsReady() && Menu["morgana.auto.q"]->GetBool()) {
        for(auto hero : g_ObjectManager->GetChampions()) {
            if(hero->IsEnemy() && hero->IsValidTarget(Spells["morgana.q"]->Range())) {
                auto pred = Ex->get_prediction(Spells["morgana.q"], hero);

                if(pred.Hitchance == HitChance::Immobile) {
                    Spells["morgana.q"]->FastCast(pred.CastPosition); } } } } }

inline void EzMorgana::on_update() {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        if(Menu["morgana.use.q"]->GetBool() && Spells["morgana.q"]->IsReady()) {
            auto target = g_Common->GetTarget(Spells["morgana.q"]->Range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                const auto pred = Ex->get_prediction(Spells["morgana.q"], target);

                if(pred.Hitchance >= Ex->get_prefered_hitchance(target)) {
                    Spells["morgana.q"]->FastCast(pred.CastPosition); } } }

        if(Menu["morgana.use.w"]->GetBool() && Spells["morgana.w"]->IsReady()) {
            auto target = g_Common->GetTarget(Spells["morgana.w"]->Range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                const auto pred = Ex->get_prediction(Spells["morgana.w"], target);

                if(Ex->has_cc(target) || !Menu["morgana.use.w2"]->GetBool()) {
                    if(pred.Hitchance >= Ex->get_prefered_hitchance(target)) {
                        Spells["morgana.w"]->FastCast(pred.CastPosition); } } } } }

    on_post_update(); }

inline void EzMorgana::on_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {}
