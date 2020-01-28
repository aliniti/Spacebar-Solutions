#pragma once
#include "EzChampion.h"


class EzNeeko : public EzChampion {
    public:

        static auto on_boot(IMenu * menu)->IMenu*;
        static auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void;
        static void on_draw();
        static void on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args);
        static void on_update();
        static void on_cast(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline auto EzNeeko::on_boot(IMenu * menu) -> IMenu * {

    auto c_menu = menu->AddSubMenu("Neeko: Core", "neeko.core");
    Menu["neeko.use.q"] = c_menu->AddCheckBox("Use (Q)", "neeko.use.q", true);
    Menu["neeko.use.q2"] = c_menu->AddCheckBox("-> Only Immobile", "neeko.use.q2", true);
    Menu["neeko.use.e"] = c_menu->AddCheckBox("Use (E)", "neeko.use.e", true);
    Menu["neeko.use.r"] = c_menu->AddCheckBox("Use (R)", "neeko.use.r", true);

    auto d_menu = menu->AddSubMenu("Neeko: Draw", "neeko.draw");
    Menu["neeko.draw.q"] = d_menu->AddCheckBox("Draw (Q)", "neeko.draw.q", true);
    Menu["neeko.draw.e"] = d_menu->AddCheckBox("Draw (E)", "neeko.draw.e", true);
    Menu["neeko.draw.r"] = d_menu->AddCheckBox("Draw (R)", "neeko.draw.r", true);

    menu->AddLabel("Neeko: Mechancis", "neeko.mechanics");
    Menu["neeko.use.fastq"] = menu->AddCheckBox("Use Fast (Q)", "neeko.use.fastq", false);

    menu->AddLabel("Neeko: Auto", "neeko.auto");
    Menu["neeko.auto.q"] = menu->AddCheckBox("Use (Q) Immobile", "neeko.auto.q", true);
    Menu["neeko.auto.e"] = menu->AddCheckBox("Use (E) Immobile", "neeko.auto.e", true);

    Spells["neeko.q"] = g_Common->AddSpell(SpellSlot::Q, 800);
    Spells["neeko.q"]->SetSkillshot(0.25f, 225, 500, kCollidesWithNothing, kSkillshotCircle);
    Spells["neeko.w"] = g_Common->AddSpell(SpellSlot::W);
    Spells["neeko.e"] = g_Common->AddSpell(SpellSlot::E, 1000);
    Spells["neeko.e"]->SetSkillshot(0.25f, 140, 1300, kCollidesWithNothing, kSkillshotCircle);
    Spells["neeko.r"] = g_Common->AddSpell(SpellSlot::R);


    return menu; }

inline auto EzNeeko::on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void {}

inline void EzNeeko::on_draw() {

    if(Menu["neeko.draw.q"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["neeko.q"]->Range(), RGBA(204, 102, 204, 115)); }

    if(Menu["neeko.draw.e"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["neeko.e"]->Range(), RGBA(217, 140, 217, 115)); }

    if(Menu["neeko.draw.r"]->GetBool()) {
        g_Drawing->AddCircle(g_LocalPlayer->Position(), Spells["neeko.r"]->Range(), RGBA(204, 102, 204, 115)); } }

inline void EzNeeko::on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {
    if(unit->IsMe() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        if(Spells["neeko.q"]->IsReady() && Menu["neeko.use.q"]->GetBool() && Menu["neeko.use.fastq"]->GetBool()) {
            if(strcmp(args->SpellData->SpellName.c_str(), "NeekoE") == 0 && Menu["neeko.use.q"]->GetBool()) {
                auto e_target = g_Common->GetTarget(Spells["neeko.e"]->Range(), DamageType::Magical);

                if(e_target != nullptr && e_target->IsValidTarget(Spells["neeko.q"]->Range())) {
                    auto startPos = unit->ServerPosition();
                    auto endPos = startPos + (args->EndPosition - startPos).Normalized() * Spells["neeko.e"]->Range();
                    auto proj = e_target->ServerPosition().ProjectOn(startPos, endPos);

                    if(proj.IsOnSegment && e_target->Distance(proj.SegmentPoint) <= Spells["neeko.e"]->Radius()) {
                        Spells["neeko.q"]->FastCast(e_target->ServerPosition()); } } } } }

    if(unit->IsMe() && args->IsAutoAttack) {
        if(Menu["neeko.use.q"]->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
            if(args->Target->IsAIHero() && Spells["neeko.q"]->IsReady()) {
                if(g_LocalPlayer->IsInAutoAttackRange(args->Target)) {
                    auto pred = Ex->get_prediction(Spells["neeko.q"], args->Target);

                    if(!args->Target->HasCC() && Menu["neeko.use.q2"]->GetBool()) {
                        return; }

                    if(pred.Hitchance >= Ex->get_prefered_hitchance(args->Target)) {
                        Spells["neeko.q"]->FastCast(pred.CastPosition); } } } }

        if(Menu["neeko.use.q"]->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeMixed)) {
            if(args->Target->IsAIHero() && g_LocalPlayer->ManaPercent() > 68) {
                if(g_LocalPlayer->IsInAutoAttackRange(args->Target)) {
                    auto pred = Ex->get_prediction(Spells["neeko.q"], args->Target);

                    if(pred.Hitchance >= Ex->get_prefered_hitchance(args->Target)) {
                        Spells["neeko.q"]->FastCast(pred.CastPosition); } } } } } }

inline void EzNeeko::on_update() {
    if(g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo)) {
        if(Menu["neeko.use.e"]->GetBool() && Spells["neeko.e"]->IsReady()) {
            auto target = g_Common->GetTarget(Spells["neeko.e"]->Range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                const auto pred = Ex->get_prediction(Spells["neeko.e"], target);

                if(pred.Hitchance >= Ex->get_prefered_hitchance(target)) {
                    Spells["neeko.e"]->FastCast(pred.CastPosition); } } }

        if(Menu["neeko.use.q"]->GetBool() && Spells["neeko.q"]->IsReady()) {
            auto target = g_Common->GetTarget(Spells["neeko.q"]->Range(), DamageType::Magical);

            if(target != nullptr && target->IsValidTarget()) {
                if(!g_LocalPlayer->IsInAutoAttackRange(target) || target->HasCC()) {

                    const auto pred = Ex->get_prediction(Spells["neeko.q"], target);

                    if(!target->HasCC() && Menu["neeko.use.q2"]->GetBool()) {
                        return; }

                    if(pred.Hitchance >= Ex->get_prefered_hitchance(target)) {
                        Spells["neeko.q"]->FastCast(pred.CastPosition); } } } } } }

inline void EzNeeko::on_cast(IGameObject * unit, OnProcessSpellEventArgs * args) {}
