#pragma once
#include "SDK/EventHandler.h"
#include "Champions/EzTristana.h"
#include "Champions/EzJinx.h"
#include "Champions/EzCamille.h"
#include "Champions/EzKatarina.h"
// oh dis...
PLUGIN_API const char PLUGIN_PRINT_NAME[32] = "EzSeries";
PLUGIN_API const char PLUGIN_PRINT_AUTHOR[32] = "Kurisu";
PLUGIN_API ChampionId PLUGIN_TARGET_CHAMP = ChampionId::Unknown;

IMenu * config = { };
int default_skin_id;

auto on_issue_order(IGameObject * unit, OnIssueOrderEventArgs * args) {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Katarina:
            return EzKatarina::on_issue_order(unit, args);

        default: ; } }


auto on_pre_create(OnPreCreateObjectEventArgs * args) -> void {
    if(args->ChampionName == g_LocalPlayer->ChampionName()) {
        default_skin_id = g_LocalPlayer->GetSkinId(); } }

auto on_create(IGameObject * unit) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Katarina: return EzKatarina::on_create(unit);

        default: ; } }

auto on_delete(IGameObject * unit) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Jinx: return EzJinx::on_destory(unit);

        case ChampionId::Katarina: return EzKatarina::on_destroy(unit);

        default: ; } }

auto on_update() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana: return EzTristana::on_update();

        case ChampionId::Jinx: return EzJinx::on_update();

        case ChampionId::Katarina: return EzKatarina::on_update();

        default: ; }

    if(config->GetSubMenu("skins")->GetElement("skin.changer")->GetBool()) {
        // - new skin
        if(g_LocalPlayer->GetSkinId() != config->GetSubMenu("skins")->GetElement("skin.id")->GetInt()) {
            const std::string model_name;
            g_LocalPlayer->SetSkin(config->GetSubMenu("skins")->GetElement("skin.id")->GetInt(), model_name); } }

    else {
        // - old skin
        if(g_LocalPlayer->GetSkinId() != default_skin_id) {
            const std::string model_name;
            g_LocalPlayer->SetSkin(default_skin_id, model_name); } } }


auto on_draw() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana: return EzTristana::on_huddraw();

        case ChampionId::Jinx: return EzJinx::on_huddraw();

        case ChampionId::Katarina: return EzKatarina::on_hud_draw();

        default: ; } }

auto hpbarfill_render() -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::hpbarfill_render();

        case ChampionId::Jinx:
            return EzJinx::hpbarfill_render();

        case ChampionId::Katarina:
            return EzKatarina::hpbarfill_render();

        default: ; } }

auto on_buff(IGameObject * unit, OnBuffEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Katarina:
            return EzKatarina::on_buff(unit, args);

        case ChampionId::Jinx:
            return EzJinx::on_buff(unit, args);

        default: ; } }

void on_teleport(IGameObject * sender, OnTeleportEventArgs * args) {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {} }

auto on_cast_spell(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana: return EzTristana::on_dash(unit, args);

        default: ; } }

auto on_play_animation(IGameObject * unit, OnPlayAnimationEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {} }

auto on_before_attack(BeforeAttackOrbwalkerArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana: return EzTristana::on_before_attack(args);

        case ChampionId::Jinx: return EzJinx::on_before_attack(args);

        default: ; } }

auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void {
    if(g_LocalPlayer->IsDead() || !g_Common->IsWindowFocused()) {
        return; }

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Jinx: return EzJinx::on_do_cast(unit, args);

        case ChampionId::Vayne: return EzCamille::on_do_cast(unit, args);

        case ChampionId::Katarina: return EzKatarina::on_do_cast(unit, args);

        default: ; } }

auto build_menu(IMenu * menu) -> IMenu * {

    switch(g_LocalPlayer->ChampionId()) {
        case ChampionId::Tristana:
            return EzTristana::on_boot(menu);

        case ChampionId::Jinx:
            return EzJinx::on_boot(menu);

        case ChampionId::Katarina:
            return EzKatarina::on_boot(menu);

        default: {
                g_Common->ChatPrint(R"(<font color="#CC6666"><b>[EzSeries]:</b></font><b><font color="#99FF99"> Not Supported!</font>)"); } } }

void on_crypt_str(const char * str, int hash) {}


PLUGIN_API bool OnLoadSDK(IPluginsSDK * plugin_sdk) {
    DECLARE_GLOBALS(plugin_sdk);

    config = build_menu(g_Menu->CreateMenu("EzSeries", "EzSeries.v3"));
    config->AddSubMenu(std::string(g_LocalPlayer->ChampionName()).append(": Skins"), "skins");
    config->AddCheckBox("Enable Skins", "skin.changer", false);
    config->AddSlider("SkinId", "skin.id", 1, 1, 50);

    EventHandler<Events::OnIssueOrder>::AddEventHandler(on_issue_order);
    EventHandler<Events::OnPreCreateObject>::AddEventHandler(on_pre_create);
    EventHandler<Events::OnTeleport>::AddEventHandler(on_teleport);
    EventHandler<Events::OnCryptString>::AddEventHandler(on_crypt_str);
    EventHandler<Events::GameUpdate>::AddEventHandler(on_update);
    EventHandler<Events::OnHudDraw>::AddEventHandler(on_draw);
    EventHandler<Events::OnHPBarDraw>::AddEventHandler(hpbarfill_render);
    EventHandler<Events::OnBuff>::AddEventHandler(on_buff);
    EventHandler<Events::OnCreateObject>::AddEventHandler(on_create);
    EventHandler<Events::OnDeleteObject>::AddEventHandler(on_delete);
    EventHandler<Events::OnPlayAnimation>::AddEventHandler(on_play_animation);
    EventHandler<Events::OnProcessSpellCast>::AddEventHandler(on_cast_spell);
    EventHandler<Events::OnBeforeAttackOrbwalker>::AddEventHandler(on_before_attack);
    EventHandler<Events::OnDoCast>::AddEventHandler(on_do_cast);
    return true; }

PLUGIN_API void OnUnloadSDK() {
    EventHandler<Events::OnIssueOrder>::RemoveEventHandler(on_issue_order);
    EventHandler<Events::OnPreCreateObject>::RemoveEventHandler(on_pre_create);
    EventHandler<Events::OnTeleport>::RemoveEventHandler(on_teleport);
    EventHandler<Events::OnCryptString>::RemoveEventHandler(on_crypt_str);
    EventHandler<Events::GameUpdate>::RemoveEventHandler(on_update);
    EventHandler<Events::OnHudDraw>::RemoveEventHandler(on_draw);
    EventHandler<Events::OnHPBarDraw>::RemoveEventHandler(hpbarfill_render);
    EventHandler<Events::OnBuff>::RemoveEventHandler(on_buff);
    EventHandler<Events::OnCreateObject>::RemoveEventHandler(on_create);
    EventHandler<Events::OnDeleteObject>::RemoveEventHandler(on_delete);
    EventHandler<Events::OnPlayAnimation>::RemoveEventHandler(on_play_animation);
    EventHandler<Events::OnProcessSpellCast>::RemoveEventHandler(on_cast_spell);
    EventHandler<Events::OnBeforeAttackOrbwalker>::RemoveEventHandler(on_before_attack);
    EventHandler<Events::OnDoCast>::RemoveEventHandler(on_do_cast);
    config->Remove(); }
