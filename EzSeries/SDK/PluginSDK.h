#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <array>
#include <string>
#include <climits>
#include <cmath>
#include <functional>
#include <map>

#include <d3d9.h>
//#include <d3dx9.h>

#include "PluginSDK_Enums.h"
#include "Vector.h"
#include "Geometry.h"

/*      About plugin system:
     - Plugins can export these entries to make system more readable:
     - PLUGIN_API const char PLUGIN_PRINT_NAME[32] = "ReadableName";
     - PLUGIN_API const char PLUGIN_PRINT_AUTHOR[32] = "Spacebar";
     - PLUGIN_API ChampionId PLUGIN_TARGET_CHAMP = ChampionId::Unknown;
       First two are used for printing readable stuff in the menu.
       Champion target is used for filtering out unwated plugins,
       if exported and different than 0 the plugins will only show up
       on the list on correct champion.

     - Plugins are also expected to export and implement these two functions:
     - PLUGIN_API bool OnLoadSDK(IPluginsSDK* plugin_sdk);
     - PLUGIN_API void OnUnloadSDK();

     - Plugin MUST export SDK target version (PLUGIN_SDK_VERSION define)
       under name PLUGIN_TARGET_SDK, but that should be done automatically
       via PluginSDK.cpp. Plugins will be ignored in case of mismatch.
*/

#define PLUGIN_API  extern "C" __declspec(dllexport)
#define PLUGIN_SDK_VERSION 21

#define DECLARE_GLOBALS(PLUGIN_SDK) \
    g_PluginSDK         = PLUGIN_SDK; \
    g_ObjectManager     = PLUGIN_SDK->GetObjectManager(); \
    g_LocalPlayer       = PLUGIN_SDK->GetObjectManager()->GetLocalPlayer(); \
    g_Camera            = PLUGIN_SDK->GetCamera(); \
    g_NavMesh           = PLUGIN_SDK->GetNavMesh(); \
    g_Renderer          = PLUGIN_SDK->GetRenderer(); \
    g_Shop              = PLUGIN_SDK->GetShop(); \
    g_Drawing           = PLUGIN_SDK->GetDrawing(); \
    g_Menu              = PLUGIN_SDK->GetMainMenu(); \
    g_Orbwalker         = PLUGIN_SDK->GetOrbwalker(); \
    g_Common            = PLUGIN_SDK->GetCommon(); \
    g_ChampionManager   = PLUGIN_SDK->GetChampionManager();\
    g_HealthPrediction  = PLUGIN_SDK->GetHealthPrediction();\
    g_ItemManager       = PLUGIN_SDK->GetItemManager();\
    g_Log               = PLUGIN_SDK->GetLogSystem();

#define RGBA(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)

bool StringContains(const char * strA, const char * strB, bool ignore_case = FALSE);
bool StringEquals(const char * strA, const char * strB, bool ignore_case = FALSE);

#pragma warning( push )
#pragma warning( disable : 4018)
// FNV hash algorithm, use for buffs
constexpr uint32_t inline const fnv_hash(const char * str) {
    uint32_t hash = 0x811C9DC5;
    uint32_t len = 0;

    while(str[len] != '\0') {
        len++; }

    for(auto i = 0; i < len; ++i) {
        char current = str[i];
        char current_upper = current + 0x20;

        if(static_cast<uint8_t>(current - 0x41) > 0x19u) {
            current_upper = current; }

        hash = 16777619 * (hash ^ current_upper); }

    return hash; }
#pragma warning( pop )

class IGameObject;
struct IPerk;
struct ISpellData;
struct IItemData;
struct ICharacterData;
struct IHeroStats;
class ISpellbook;
class IAvatar;
struct DamageInput;
struct IPredictionInput;
struct IPredictionOutput;
struct ISkinInfo;
struct IDashData;
struct IItemBuildInfo;
struct IRecommendedItemInfo;
class ISpell;

struct IBuffInstance {
    std::string Name;
    std::string DisplayName;

    bool Valid;
    bool Alive;
    bool Permanent;

    float RemainingTime;
    int Count;
    int MaxCount;

    IGameObject * Caster;
    IGameObject * Owner;

    BuffType Type;

    float StartTime;
    float EndTime;

    uint32_t NameHash; };

class IGameObject {
    public:
        virtual uint32_t Index() = 0;
        virtual int32_t NetworkId() = 0;
        virtual int Handle() = 0;

        virtual EntityType Type() = 0;
        virtual GameObjectTeam Team() = 0;

        virtual std::string Name() = 0;
        virtual std::string ActiveModel() = 0;
        virtual std::string ChampionName() = 0;
        virtual std::string BaseSkinName() = 0;

        virtual Vector Position() = 0;
        virtual Vector ServerPosition() = 0;
        virtual Vector LeashedPosition() = 0;
        virtual Vector Velocity() = 0;
        virtual Vector2 HealthBarPosition() = 0;
        virtual Vector DirectionPerpendicular() = 0;
        virtual Vector Direction() = 0;
        virtual Vector PathingDirection() = 0;

        virtual ChampionId ChampionId() = 0;

        virtual bool IsMissileClient() = 0;
        virtual bool MissileIsAutoAttack() = 0;
        virtual bool MissileIsTargeted() = 0;

        virtual Vector MissileStartPosition() = 0;
        virtual Vector MissileEndPosition() = 0;

        virtual ISpellData MissileSpellData() = 0;
        virtual IGameObject * MissileSender() = 0;
        virtual IGameObject * MissileTarget() = 0;

        virtual bool IsValid() = 0;
        virtual bool IsDead() = 0;
        virtual bool IsVisible() = 0;
        virtual bool IsAlly() = 0;
        virtual bool IsEnemy() = 0;
        virtual bool IsAttackable() = 0;
        virtual bool IsVisibleOnScreen() = 0;
        virtual bool IsMoving() = 0;
        virtual bool IsRanged() = 0;
        virtual bool IsMelee() = 0;
        virtual bool IsMinion() = 0;
        virtual bool IsMonster() = 0;
        virtual bool IsMe() = 0;
        virtual bool IsZombie() = 0;
        virtual bool IsAIHero() = 0;
        virtual bool IsAIBase() = 0;
        virtual bool IsAITurret() = 0;
        virtual bool IsNexus() = 0;
        virtual bool IsInhibitor() = 0;
        virtual bool IsMonsterRenderedOnMinimap() = 0;
        virtual bool IsRecalling() = 0;
        virtual bool InFountain() = 0;
        virtual bool IsWindingUp() = 0;
        virtual bool IsInvulnerable() = 0;
        virtual bool IsPhysicalImmune() = 0;
        virtual bool IsMagicImmune() = 0;
        virtual bool IsLifestealImmune() = 0;
        virtual bool IsTargetable() = 0;

        virtual bool IsEpicMonster() = 0;
        virtual bool IsJungleBuff() = 0;
        virtual bool IsLaneMinion() = 0;

        // GetBuffList method is slow, you should use methods with buff_hash, unless necessary!
        virtual std::vector<IBuffInstance> GetBuffList() = 0;
        virtual IBuffInstance GetBuff(const uint32_t buff_hash) = 0;
        virtual bool HasBuff(const uint32_t buff_hash) = 0;
        virtual bool HasBuffOfType(BuffType buffType) = 0;
        virtual float BuffTimeLeft(const uint32_t buff_hash) = 0;

        // Wrappers for easier usage
        __forceinline IBuffInstance GetBuff(const char * BuffName) {
            return GetBuff(fnv_hash(BuffName)); }
        __forceinline bool HasBuff(const char * BuffName) {
            return HasBuff(fnv_hash(BuffName)); }
        __forceinline bool BuffTimeLeft(const char * BuffName) {
            return BuffTimeLeft(fnv_hash(BuffName)); }

        virtual bool HasPerk(std::string const & perk_name) = 0;
        virtual bool HasPerk(int32_t perk_id) = 0;

        virtual int CurrentPathNode() = 0;
        virtual int Level() = 0;
        virtual int NumNeutralMinionsKilled() = 0;
        virtual int EvolvePoints() = 0;
        virtual int IsTargetableToTeam() = 0;
        virtual uint8_t EvolveFlag() = 0;

        virtual float BoundingRadius() = 0;
        virtual float MovementBlockedDebuffDuration() = 0;
        virtual float MovementReducedDebuffDuration() = 0;
        virtual float Health() = 0;
        virtual float MaxHealth() = 0;
        virtual float HPMaxPenalty() = 0;
        virtual float HealthPercent() = 0;
        virtual float AllShield() = 0;
        virtual float PhysicalShield() = 0;
        virtual float MagicalShield() = 0;
        virtual float StopShieldFade() = 0;
        virtual float Mana() = 0;
        virtual float MaxMana() = 0;
        virtual float ManaPercent() = 0;
        virtual float Exp() = 0;
        virtual float Gold() = 0;
        virtual float ExperiencePercent() = 0;
        virtual float AttackDelay(int slot) = 0;
        virtual float AttackCastDelay(int slot) = 0;
        virtual float AdditionalAttackDamage() = 0;
        virtual float TotalAttackDamage() = 0;
        virtual float TotalAbilityPower() = 0;
        virtual float PercentDamageToBarracksMinionMod() = 0;
        virtual float FlatDamageReductionFromBarracksMinionMod() = 0;
        virtual float PhysicalDamagePercentageModifier() = 0;
        virtual float MagicalDamagePercentageModifier() = 0;
        virtual float RealHealth(bool physicalShield, bool magicalShield) = 0;
        virtual float AutoAttackDamage(IGameObject * to, bool respect_passives) = 0;
        virtual float Distance(Vector const & to) = 0;
        virtual float Distance(IGameObject * to) = 0;
        virtual float DistanceOnSegment(Vector const & segment_end, bool only_if_on_segment, bool squared) = 0;
        virtual float DistanceOnSegment(IGameObject * segment_end, bool only_if_on_segment, bool squared) = 0;

        virtual eSpellState GetSpellState(SpellSlot spell_slot) = 0;

        virtual void SetName(std::string const & name) = 0;
        virtual void SetSkinModel(std::string const & model, int skin_id) = 0;
        virtual void SetSkin(int skin_id, std::string const & model) = 0;
        virtual int GetSkinId() = 0;

        virtual bool LevelUpSpell(SpellSlot spell_slot) = 0;
        virtual bool GetItemSpellSlot(ItemId item_id, SpellSlot & out_slot) = 0;

        virtual bool HasCC() = 0;
        virtual bool IsRecallingFow() = 0;
        virtual bool IsDashing() = 0;
        virtual bool IsUnderMyEnemyTurret() = 0;
        virtual bool IsUnderMyAllyTurret() = 0;
        virtual bool IsUnderEnemyTurret() = 0;
        virtual bool IsUnderAllyTurret() = 0;
        virtual bool IsInAutoAttackRange(IGameObject * to) = 0;
        virtual bool IsInAutoAttackRange(Vector const & to) = 0;
        virtual bool HasItem(ItemId item_id) = 0;
        virtual bool CanUseItem(ItemId item_id) = 0;
        virtual bool CanUseItem(SpellSlot spell_slot) = 0;
        virtual bool IsSpellEvolved(SpellSlot spell_slot) = 0;
        virtual bool CanSpellBeLeveledUp(SpellSlot spell_slot) = 0;
        virtual bool CanUseSpell(SpellSlot spell_slot) = 0;
        virtual bool IsInRange(float range) = 0;
        virtual bool IsValidTarget(float range = FLT_MAX, bool checkTeam = true, Vector const & from = Vector()) = 0;
        virtual bool IsFacing(IGameObject * unit) = 0;
        virtual bool IsFacing(const Vector & position) = 0;

        virtual int IsCastingInterruptibleSpell() = 0;
        virtual int CountMyAlliesInRange(float range) = 0;
        virtual int CountMyEnemiesInRange(float range) = 0;
        virtual int CountAlliesInRange(float range) = 0;
        virtual int CountEnemiesInRange(float range) = 0;

        virtual uintptr_t * CircleIconPortrait() = 0;
        virtual uintptr_t * SquareIconPortrait() = 0;

        virtual IDashData GetDashData() = 0;

        virtual std::vector<IItemData> GetItems() = 0;
        virtual ISpellbook * GetSpellbook() = 0;
        virtual ISpellData GetAutoAttack() = 0;
        virtual ICharacterData GetCharacterData() = 0;
        virtual IAvatar * GetAvatar() = 0;
        virtual IHeroStats GetHeroStats() = 0;
        virtual IGameObject * GetOwner() = 0;

        virtual std::vector<Vector> CreatePath(Vector const & end, bool smooth_path) = 0;
        virtual std::vector<Vector> CreatePath(Vector const & start, Vector const & end, bool smooth_path) = 0;
        virtual std::vector<Vector> CreatePath(Vector const & start, Vector const & end) = 0;
        virtual std::vector<Vector> CreatePath(Vector const & end) = 0;
        virtual std::vector<Vector> Path() = 0;
        virtual std::vector<Vector> RealPath() = 0;

        virtual bool IssueOrder(IssueOrderType type, const Vector & target_position) = 0;
        virtual bool IssueOrder(IssueOrderType type, const Vector & target_position, bool trigger_event) = 0;
        virtual bool IssueOrder(IssueOrderType type, const Vector & target_position, bool is_attack_move, bool is_pet_command) = 0;
        virtual bool IssueOrder(IssueOrderType type, const Vector & target_position, bool is_attack_move, bool is_pet_command, bool trigger_event) = 0;
        virtual bool IssueOrder(IssueOrderType type, IGameObject * target) = 0;
        virtual bool IssueOrder(IssueOrderType type, IGameObject * target, bool trigger_event) = 0;
        virtual bool IssueOrder(IssueOrderType type, IGameObject * target, bool is_attack_move, bool is_pet_command) = 0;
        virtual bool IssueOrder(IssueOrderType type, IGameObject * target, bool is_attack_move, bool is_pet_command, bool trigger_event) = 0;

        virtual bool CastSpell(SpellSlot spell_slot) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, bool trigger_event) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, IGameObject * target) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, IGameObject * target, bool trigger_event) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, const Vector & target_position) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, const Vector & target_position, bool trigger_event) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, const Vector & from_position, const Vector & target_position) = 0;
        virtual bool CastSpell(SpellSlot spell_slot, const Vector & from_position, const Vector & target_position, bool trigger_event) = 0;

        virtual bool UpdateChargeableSpell(SpellSlot spell_slot, bool release_cast) = 0;
        virtual bool UpdateChargeableSpell(SpellSlot spell_slot, bool release_cast, bool trigger_event) = 0;
        virtual bool UpdateChargeableSpell(SpellSlot spell_slot, Vector const & position, bool release_cast) = 0;
        virtual bool UpdateChargeableSpell(SpellSlot spell_slot, Vector const & position, bool release_cast, bool trigger_event) = 0;

        virtual bool CastItem(ItemId item_id) = 0;
        virtual bool CastItem(ItemId item_id, Vector const & position) = 0;
        virtual bool CastItem(ItemId item_id, IGameObject * target) = 0;

        virtual float PercentCooldownMod() = 0;
        virtual float PercentCooldownCapMod() = 0;
        virtual float PassiveCooldownEndTime() = 0;
        virtual float PassiveCooldownTotalTime() = 0;
        virtual float FlatPhysicalDamageMod() = 0;
        virtual float PercentPhysicalDamageMod() = 0;
        virtual float PercentBonusPhysicalDamageMod() = 0;
        virtual float PercentBasePhysicalDamageAsFlatBonusMod() = 0;
        virtual float FlatMagicDamageMod() = 0;
        virtual float PercentMagicDamageMod() = 0;
        virtual float FlatMagicReduction() = 0;
        virtual float PercentMagicReduction() = 0;
        virtual float FlatCastRangeMod() = 0;
        virtual float AttackSpeedMod() = 0;
        virtual float PercentAttackSpeedMod() = 0;
        virtual float PercentMultiplicativeAttackSpeedMod() = 0;
        virtual float BaseAttackDamage() = 0;
        virtual float BaseAttackDamageSansPercentScale() = 0;
        virtual float FlatBaseAttackDamageMod() = 0;
        virtual float PercentBaseAttackDamageMod() = 0;
        virtual float BaseAbilityDamage() = 0;
        virtual float CritDamageMultiplier() = 0;
        virtual float Dodge() = 0;
        virtual float Crit() = 0;
        virtual float Armor() = 0;
        virtual float BonusArmor() = 0;
        virtual float SpellBlock() = 0;
        virtual float BonusSpellBlock() = 0;
        virtual float HPRegenRate() = 0;
        virtual float BaseHPRegenRate() = 0;
        virtual float MoveSpeed() = 0;
        virtual float MoveSpeedBaseIncrease() = 0;
        virtual float AttackRange() = 0;
        virtual float FlatBubbleRadiusMod() = 0;
        virtual float PercentBubbleRadiusMod() = 0;
        virtual float FlatArmorPenetration() = 0;
        virtual float PhysicalLethality() = 0;
        virtual float PercentArmorPenetration() = 0;
        virtual float PercentBonusArmorPenetration() = 0;
        virtual float PercentCritBonusArmorPenetration() = 0;
        virtual float PercentCritTotalArmorPenetration() = 0;
        virtual float FlatMagicPenetration() = 0;
        virtual float MagicLethality() = 0;
        virtual float PercentMagicPenetration() = 0;
        virtual float PercentBonusMagicPenetration() = 0;
        virtual float PercentLifeStealMod() = 0;
        virtual float PercentSpellVampMod() = 0;
        virtual float PathfindingRadiusMod() = 0;
        virtual float PercentCCReduction() = 0;
        virtual float PercentEXPBonus() = 0;
        virtual float PARRegenRate() = 0;
        virtual float PrimaryARRegenRateRep() = 0;
        virtual float SecondaryARRegenRateRep() = 0;
        virtual float SecondaryARBaseRegenRateRep() = 0;

        virtual int CampNumber() = 0;
        virtual void SetVisibilityOverride(bool visible) = 0;

        // CharacterState
        virtual bool CanAttack() = 0;
        virtual bool CanMove() = 0;
        virtual bool CanCast() = 0;
        virtual bool IsImmovable() = 0;
        virtual bool IsSelectable() = 0;
        virtual bool IsCritImmune() = 0;
        virtual bool IsUnkillable() = 0;
        virtual bool IsRooted() = 0;
        virtual bool IsStunned() = 0;
        virtual bool IsGrounded() = 0;
        virtual bool IsStealthed() = 0;
        virtual bool IsObscured() = 0;
        virtual bool IsFleeing() = 0;
        virtual bool IsCharmed() = 0;
        virtual bool IsSuppressed() = 0;
        virtual bool IsFeared() = 0;
        virtual bool IsAsleep() = 0;
        virtual bool IsSlowed() = 0;
        virtual bool IsGhosted() = 0; };

class IObjectManager {
    public:
        virtual IGameObject * GetLocalPlayer() = 0;
        virtual IGameObject * GetEntity(std::int32_t Index) = 0;
        virtual std::vector<IGameObject *> GetChampions() = 0;
        virtual std::vector<IGameObject *> GetChampions(bool Friendly) = 0;

        virtual std::vector<IGameObject *> GetByType(EntityType type = EntityType::Any, std::function<bool(IGameObject * unit)> predicate = nullptr) = 0;

        virtual IGameObject * GetEntityByNetworkID(std::int32_t NetworkID) = 0;

        virtual std::vector<IGameObject *> GetMinionsEnemy() = 0;
        virtual std::vector<IGameObject *> GetMinionsAll() = 0;
        virtual std::vector<IGameObject *> GetMinionsAlly() = 0;
        virtual std::vector<IGameObject *> GetJungleMobs() = 0;
        virtual std::vector<IGameObject *> GetPlants() = 0;
        virtual std::vector<IGameObject *> GetWards() = 0;
        virtual std::vector<IGameObject *> GetOtherObjEnemy() = 0;

        virtual std::vector<IGameObject *> GetNexus() = 0;
        virtual std::vector<IGameObject *> GetTurrets() = 0;
        virtual std::vector<IGameObject *> GetInhibitors() = 0;
        virtual std::vector<IGameObject *> GetSpawnPoints() = 0; };

class IChampionManager {
    public:
        virtual std::map<ChampionId, std::vector<ISkinInfo>> GetDatabase() = 0; };

class IItemManager {
    public:
        virtual std::map<uint32_t, IItemBuildInfo> GetItems() = 0;
        virtual std::map<std::string, std::vector<IRecommendedItemInfo>> GetRecommendedItems() = 0;

        virtual void SmartBuyItem(ItemId item_id) = 0; };

class IHealthPrediction {
    public:
        virtual float GetHealthPrediction(IGameObject * unit, float time) = 0;
        virtual float GetLaneClearHealthPrediction(IGameObject * unit, float time) = 0;
        virtual bool HasMinionAggro(IGameObject * unit) = 0;
        virtual bool HasTurretAggro(IGameObject * unit) = 0;
        virtual float GetIncomingDamage(IGameObject * unit, float time, bool includeSkillShot = true, bool includeAutoAttacks = true) = 0; };

class ICamera {
    public:
        // All of those are actually writable
        virtual float & CurrentZoomDistance() = 0;
        virtual float & TargetZoomDistance() = 0;

        virtual Vector & CameraPos1() = 0;
        virtual Vector & CameraPos2() = 0;
        virtual Vector & CameraAngles() = 0;
        virtual float & CameraFOV() = 0;

        virtual void SetZoomDistance(float value) = 0; };

class INavMesh {
    public:
        virtual float CellWidth() = 0;
        virtual float CellHeight() = 0;
        virtual unsigned int Width() = 0;
        virtual unsigned int Height() = 0;

        virtual float GetHeightForPosition(Vector const & position) = 0;
        virtual bool HasFlag(Vector const & position, int eNavCollisionFlags) = 0;

        virtual eNavCollisionFlags GetCollisionFlags(Vector const & position) = 0; };

class IRenderer {
    public:
        virtual IDirect3DDevice9 * D3D9Device() = 0;

        virtual Vector2 WorldToScreen(Vector const & world) = 0;
        virtual bool WorldToScreen(Vector * world, Vector2 * screen) = 0;
        virtual bool WorldToScreen(Vector const & world, Vector2 & screen) = 0;

        virtual bool IsOnScreen(Vector2 * screen, int tolerance) = 0;
        virtual bool IsOnScreen(Vector2 const & screen, int tolerance) = 0;

        virtual unsigned int ScreenWidth() = 0;
        virtual unsigned int ScreenHeight() = 0;

        virtual Vector CameraPosition() = 0;

        //virtual D3DXMATRIX ViewMatrix() = 0;
        //virtual D3DXMATRIX ProjectionMatrix() = 0;

        virtual Vector ToWorldCoord(Vector const & position) = 0;
        virtual Vector2 ToMapCoord(Vector const & position) = 0; };

class IShop {
    public:
        virtual bool IsOpen() = 0;

        virtual void BuyItem(ItemId item_id) = 0;
        virtual void SellItem(SpellSlot slot) = 0;
        virtual void SellItem(ItemId item_id) = 0; };

class IAvatar {
    public:
        virtual std::vector<IPerk> GetPerks() = 0;

        virtual bool HasPerk(std::string const & perk_name) = 0;
        virtual bool HasPerk(int32_t perk_id) = 0; };

class ISpellCastInfo {
    public:
        virtual bool IsValid() = 0;

        virtual IGameObject * Target() = 0;

        virtual ISpellData SData() = 0;
        virtual SpellSlot SpellSlot() = 0;

        virtual int32_t CastedCount() = 0;
        virtual int32_t MissileNetworkId() = 0;

        virtual Vector StartPosition() = 0;
        virtual Vector EndPosition() = 0;

        virtual int Level() = 0;

        virtual float CastDelay() = 0;
        virtual float SpellDelay() = 0;
        virtual float Cooldown() = 0;
        virtual float ManaCost() = 0;
        virtual float CastStartTime() = 0;
        virtual float SpellDelayEndTime() = 0;

        virtual bool IsAutoAttack() = 0; };

class ISpellInstance {
    public:
        virtual bool IsValid() = 0;
        virtual bool IsLearned() = 0;

        virtual float Cooldown() = 0;
        virtual float CooldownStart() = 0;
        virtual float CooldownEnd() = 0;
        virtual float CooldownTime() = 0;

        virtual int16_t ToogleState() = 0;
        virtual int16_t CastEndTime() = 0;
        virtual int32_t Ammo() = 0;
        virtual int32_t Level() = 0;

        virtual ISpellData SData() = 0;

        virtual uintptr_t * IconTexture() = 0; };

class ISpellbook {
    public:
        virtual IGameObject * Owner() = 0;
        virtual ISpellInstance * GetSpell(SpellSlot slot) = 0;
        virtual std::vector<ISpellInstance *> GetSpells() = 0;

        virtual eSpellState GetSpellState(SpellSlot slot) = 0;
        virtual SpellSlot GetSpellSlotFromName(std::string const & name) = 0;

        virtual bool IsWindingUp() = 0;
        virtual bool SpellHasBeenCasted() = 0;
        virtual bool IsAutoAttacking() = 0;
        virtual bool IsCharging() = 0;
        virtual bool IsChanneling() = 0;
        virtual bool IsCastingSpell() = 0;

        virtual float CastStartTime() = 0;
        virtual float CastEndTime() = 0;

        virtual ISpellCastInfo * ActiveSpell() = 0;
        virtual float QManaCost() = 0;
        virtual float WManaCost() = 0;
        virtual float EManaCost() = 0;
        virtual float RManaCost() = 0; };

class IDrawing {
    public:
        void virtual AddText(Vector const & point, uint32_t color, int font_size, const char * format, ...) = 0;
        void virtual AddText(Vector const & point, uint32_t color, text_flags flags, int font_size, const char * format, ...) = 0;

        void virtual AddTextOnScreen(Vector2 const & point, uint32_t color, int font_size, const char * format, ...) = 0;
        void virtual AddTextOnScreen(Vector2 const & point, uint32_t color, text_flags flags, int font_size, const char * format, ...) = 0;

        void virtual AddLine(Vector const & start, Vector const & end, uint32_t color, float thickness = 1.0f) = 0;
        void virtual AddLineOnScreen(Vector2 const & start, Vector2 const & end, uint32_t color, float thickness = 1.0f) = 0;

        void virtual AddPolyline(Vector const * start, int points_num, uint32_t color, float thickness = 1.0f) = 0;
        void virtual AddPolylineOnScreen(Vector2 const * start, int points_num, uint32_t color, float thickness = 1.0f) = 0;

        void virtual AddCircle(Vector const & center, float radius, uint32_t color, float thickness = 1.0f, int num_segments = 80) = 0;
        void virtual AddCircleOnScreen(Vector2 const & screen_pos, float radius, uint32_t color, float thickness, int num_segments = 80) = 0;

        void virtual AddRectOnScreen(const Vector2 & pos, const Vector2 & size, uint32_t color) = 0;
        void virtual AddFilledRectOnScreen(const Vector2 & pos, const Vector2 & size, uint32_t color) = 0;

        void virtual AddImage(uintptr_t * user_texture_id, const Vector2 & pos, const Vector2 & size, float rounding = 0.0f, const Vector2 & uv0 = Vector2(0, 0), const Vector2 & uv1 = Vector2(1, 1),
            const Vector4 & tint_col = Vector4(1, 1, 1, 1), const Vector4 & border_col = Vector4(0, 0, 0, 0)) = 0; };

class IOrbwalker {
    public:
        virtual IGameObject * GetTarget() = 0;
        virtual IGameObject * GetLastTarget() = 0;
        virtual Vector GetLastMovePosition() = 0;

        virtual int GetOrbwalkingMode() = 0; // Check eOrbwalkingMode
        virtual int GetLastAATick() = 0;
        virtual int GetLastMoveTick() = 0;

        virtual float GetMyProjectileSpeed() = 0;
        virtual float GetAttackRange(IGameObject * target) = 0;

        virtual bool CanAttack() = 0;
        virtual bool CanMove() = 0;
        virtual bool ShouldWait() = 0;

        virtual void ResetAA() = 0;
        virtual void MoveTo(Vector const & position) = 0;
        virtual void Orbwalk(IGameObject * target, Vector const & position) = 0;

        virtual void SetAttack(bool enable) = 0;
        virtual void SetMovement(bool enable) = 0;

        virtual void SetOrbwalkingTarget(IGameObject * unit) = 0;
        virtual void SetOrbwalkingPoint(Vector const & position) = 0;
        virtual void SetOrbwalkingMode(eOrbwalkingMode mode) = 0;

        bool IsModeActive(eOrbwalkingMode mode) {
            if(mode == eOrbwalkingMode::kModeNone) {
                return GetOrbwalkingMode() == 0; }

            const auto t = static_cast<int>(mode);
            return (GetOrbwalkingMode() & t) == t; } };

class IMenuElement {
    public:
        virtual std::string GetKey() = 0;
        virtual std::string GetDisplayName() = 0;
        virtual bool GetVisibilty() = 0;

        virtual bool GetBool() = 0;
        virtual int GetInt() = 0;
        virtual uint32_t GetColor() = 0;
        virtual float GetFloat() = 0;

        virtual void SetDisplayName(std::string const & display_name) = 0;
        virtual void SetVisibilty(bool const & _value) = 0;

        virtual void SetBool(bool const & _value) = 0;
        virtual void SetInt(int const & _value) = 0;
        virtual void SetColor(uint32_t const & _value) = 0;
        virtual void SetFloat(float const _value) = 0;

        virtual void SetTooltip(const char * _value) = 0; };

class IMenu {
    public:
        virtual void Remove() = 0;

        virtual IMenuElement * AddSlider(std::string const & display_name, std::string const & key, int default_value, int min_value, int max_value, bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddSliderF(std::string const & display_name, std::string const & key, float default_value, float min_value, float max_value, const char * format = nullptr,
            bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddCheckBox(std::string const & display_name, std::string const & key, bool default_value, bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddKeybind(std::string const & display_name, std::string const & key, int vkey, bool default_value, KeybindType type, bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddComboBox(std::string const & display_name, std::string const & key, std::vector<std::string> const & names, int default_value, bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddColorPicker(std::string const & display_name, std::string const & key, float r, float g, float b, float a, bool visible_in_menu = true) = 0;
        virtual IMenuElement * AddLabel(std::string const & display_name, std::string const & key, bool visible_in_menu = true) = 0;

        virtual IMenuElement * GetElement(std::string const & key) = 0;
        virtual IMenuElement * operator[](std::string const & key) = 0;

        virtual IMenu * AddSubMenu(std::string const & display_name, std::string const & key) = 0;
        virtual IMenu * GetSubMenu(std::string const & key) = 0;

        virtual std::vector<IMenuElement *> GetElements() = 0;
        virtual bool ElementExists(std::string const & key) = 0; };

class IMainMenu {
    public:
        virtual IMenu * CreateMenu(std::string const & name, std::string const & key) = 0;
        virtual IMenu * GetMenu(std::string const & key) = 0;
        virtual IMenu * operator[](std::string const & key) = 0;

        virtual std::vector<IMenu *> GetMainMenus() = 0; };

class ICommon {
    public:
        virtual void DelayAction(int time, std::function<void()> function) = 0;

        virtual bool IsWindowFocused() = 0;
        virtual bool IsCustomGame() = 0;
        virtual void ShowCursorClick(CursorClickType type, Vector const & position) = 0;

        virtual float Time() = 0;
        virtual int TickCount() = 0;
        virtual int Ping() = 0;

        virtual Vector CursorPosition() = 0;

        virtual GameState GameState() = 0;
        virtual GameMapId GetMapId() = 0;

        virtual std::string RegionName() = 0;
        virtual std::string GameModeName() = 0;
        virtual std::string MapName() = 0;
        virtual std::string GameTypeName() = 0;

        virtual void DisplayMastery() = 0;
        virtual void DoEmote(EmoteType type) = 0;

        virtual void CastPing(Vector const & position, PingType type, int32_t network_id) = 0;
        virtual void CastLocalPing(Vector const & position, PingType type, bool play_sound) = 0;

        virtual void ChatPrint(const char * str) = 0;
        virtual void ChatSend(const char * str) = 0;
        virtual void SendFakePing(uint16_t ping) = 0;

        virtual bool LevelUpSpell(SpellSlot spell_slot) = 0;
        virtual bool GetItemSpellSlot(ItemId item_id, SpellSlot & out_slot) = 0;

        virtual void ApplyGlow(IGameObject * object, int r, int g, int b, int a, int thickness, int blur) = 0;
        virtual void ApplyGlow(IGameObject * object, uint32_t color, int thickness, int blur) = 0;

        // Returns spell for easier casting, running prediction etc.
        virtual std::shared_ptr<ISpell> AddSpell(SpellSlot spell_slot, float range = FLT_MAX) = 0;

        // Prediction
        virtual IPredictionOutput GetPrediction(IGameObject * unit, float range, float delay, float radius, float speed, eCollisionFlags collisionFlags, Vector from) = 0;
        virtual IPredictionOutput GetPrediction(IGameObject * unit, float delay) = 0;
        virtual IPredictionOutput GetPrediction(IPredictionInput * prediction_input) = 0;

        // Target selector
        virtual IGameObject * GetTarget(float range, DamageType damage_type) = 0;
        virtual IGameObject * GetTargetFromCoreTS(float range, DamageType damage_type) = 0;

        // Damage Library
        virtual float CalculateDamageOnUnit(IGameObject * source, IGameObject * target, DamageInput * input) = 0;
        virtual float CalculateDamageOnUnit(IGameObject * source, IGameObject * target, DamageType damage_type, float raw_damage) = 0;
        virtual float GetSpellDamage(IGameObject * from, IGameObject * target, SpellSlot slot, bool return_raw_damage) = 0;

        // Returns Object that's seleceted by "Focus Target" option from Core TS
        virtual IGameObject * GetSelectedTarget() = 0;

        virtual int GetDragonBuffCount(GameObjectTeam Team, eDragonIds DragonType) = 0;

        // DEPRECATED, provided for compatibility
        void Log(const char * _fmt, ...); };

class ISpell {
    public:
        virtual SpellSlot Slot() = 0;
        virtual std::string Name() = 0;
        virtual ISpellInstance * Handle() = 0;

        virtual bool IsReady() = 0;
        virtual bool IsCharging() = 0;
        virtual bool IsFullyCharged() = 0;
        virtual bool StartCharging() = 0;
        virtual bool StartCharging(Vector const & position) = 0;
        virtual bool Cast() = 0;
        virtual bool Cast(IGameObject * unit, HitChance minimum_hit_chance) = 0;
        virtual bool Cast(Vector const & position) = 0;
        virtual bool Cast(IGameObject * unit) = 0;
        virtual bool Cast(Vector const & start_position, Vector const & end_position) = 0;
        virtual bool FastCast(Vector const & position) = 0;

        virtual bool CastOnBestFarmPosition(int min_minions = 2) = 0;

        virtual float Radius() = 0;
        virtual float Speed() = 0;
        virtual float Delay() = 0;
        virtual float Range() = 0;
        virtual float CooldownTime() = 0;
        virtual float Cooldown() = 0;
        virtual float ManaCost() = 0;
        virtual float ChargedPercentage() = 0;
        virtual float Damage(IGameObject * target) = 0;

        // Returns spell damage including predicted(incoming) damage on a target.
        virtual float KsDamage(IGameObject * target) = 0;

        virtual int Ammo() = 0;
        virtual int ToogleState() = 0;
        virtual int Level() = 0;
        virtual int LastCastTime() = 0;

        virtual eCollisionFlags CollisionFlags() = 0;
        virtual Vector RangeCheckFrom() = 0;

        virtual void SetRadius(float radius) = 0;
        virtual void SetSpeed(float speed) = 0;
        virtual void SetDelay(float delay) = 0;
        virtual void SetRange(float range) = 0;
        virtual void SetCollisionFlags(eCollisionFlags flags) = 0;
        virtual void SetRangeCheckFrom(Vector const & position) = 0;

        virtual void SetSkillshot(float delay, float radius, float speed, int eCollisionFlags, eSkillshotType skillshot_type) = 0;
        virtual void SetCharged(float range_min, float range_max, float charge_duration) = 0; };

class ILogSystem {
    public:
        virtual void ToggleOutputToFile(bool _log_to_file) = 0;

        virtual void Print(const char * _fmt, ...) = 0;
        virtual void Print(uint32_t _color, const char * _fmt, ...) = 0;
        virtual void PrintToFile(const char * _fmt, ...) = 0;

        virtual void PrintVA(const char * _fmt, va_list _va) = 0;
        virtual void PrintVA(uint32_t _color, const char * _fmt, va_list _va) = 0;
        virtual void PrintToFileVA(const char * _fmt, va_list _va) = 0; };

// This can be used to fix possibly outdated library inside the Core, if needed.
class IDamageLibraryAPI {
    public:
        virtual float GetAutoAttackDamage(IGameObject * source, IGameObject * target, bool respect_passives = true) = 0;
        virtual float GetSpellDamage(IGameObject * source, IGameObject * target, SpellSlot slot, bool return_raw_damage = false) = 0;
        virtual float CalculateDamageOnUnit(IGameObject * source, IGameObject * target, DamageInput * input) = 0; };

// This can be used to limit requests sent to the server
class IRateLimiterAPI {
    public:
        virtual bool AllowOnTickCallback() = 0;
        virtual bool AllowIssueOrder(IssueOrderType type, const Vector * position, IGameObject * target, bool is_attack_move, bool is_pet_command) = 0;
        virtual bool AllowCastSpell(SpellSlot spell_slot, const Vector * from_position, const Vector * target_position, const IGameObject * target) = 0; };

class IEventManager {
    public:
        virtual void AddEventCallback(std::uint32_t hashed_name, void * func_address) = 0;
        virtual void RemoveEventCallback(std::uint32_t hashed_name, void * func_address) = 0;

        // Type of event (arguments) must match!
        virtual void CallEvent(std::uint32_t hashed_name) = 0;
        virtual void CallEvent(std::uint32_t hashed_name, void * data) = 0;
        virtual void CallEvent(std::uint32_t hashed_name, IGameObject * sender, void * data) = 0; };

class IPluginsSDK {
    public:
        virtual IObjectManager * GetObjectManager() = 0;
        virtual ICamera * GetCamera() = 0;
        virtual INavMesh * GetNavMesh() = 0;
        virtual IRenderer * GetRenderer() = 0;
        virtual IShop * GetShop() = 0;
        virtual IDrawing * GetDrawing() = 0;
        virtual IMainMenu * GetMainMenu() = 0;
        virtual IOrbwalker * GetOrbwalker() = 0;
        virtual ICommon * GetCommon() = 0;
        virtual IChampionManager * GetChampionManager() = 0;
        virtual IHealthPrediction * GetHealthPrediction() = 0;
        virtual IItemManager * GetItemManager() = 0;
        virtual ILogSystem * GetLogSystem() = 0;
        virtual IDamageLibraryAPI * GetCoreDamageLib() = 0;
        virtual IEventManager * GetEventManager() = 0;

        //  API to override some parts of Core internals
        // Register funcs can return true/false depending on if given name/callback/instance is already registered.
        // Force functions will force the certain setting for this game session, but do not override the default library in config.
        // These can be used if your champion plugin requires special logic, but it wouldn't work for other champs.
        using PredictionCallbackFn = IPredictionOutput(__cdecl *)(IPredictionInput * input);
        virtual bool RegisterPredictionOverride(std::string const & prediction_name, PredictionCallbackFn callback) = 0;
        virtual void RemovePredictionOverride(PredictionCallbackFn callback) = 0;
        virtual void ForcePredictionOverride(PredictionCallbackFn callback) = 0;
        virtual std::pair<std::string, PredictionCallbackFn> GetCurrentPredictionCallback() = 0; // Returns nullptr if there are none registered

        using TargetSelectorCallbackFn = IGameObject*(__cdecl *)(float range, DamageType damage_type);
        virtual bool RegisterTargetSelectorOverride(std::string const & ts_name, TargetSelectorCallbackFn callback) = 0;
        virtual void RemoveTargetSelectorOverride(TargetSelectorCallbackFn calllback) = 0;
        virtual void ForceTargetSelectorOverride(TargetSelectorCallbackFn callback) = 0;
        virtual std::pair<std::string, TargetSelectorCallbackFn> GetCurrentTargetSelectorCallback() = 0; // Returns nullptr if there are none registered

        virtual bool RegisterDamageLibrary(std::string const & lib_name, IDamageLibraryAPI * instance) = 0;
        virtual void RemoveDamageLibrary(IDamageLibraryAPI * instance) = 0;
        virtual void ForceDamageLibraryOverride(IDamageLibraryAPI * instance) = 0;
        virtual std::pair<std::string, IDamageLibraryAPI *> GetCurrentDamageLibrary() = 0; // Returns internal damage lib instance if there are none registered

        virtual bool RegisterRateLimiter(std::string const & lib_name, IRateLimiterAPI * instance) = 0;
        virtual void RemoveRateLimiter(IRateLimiterAPI * instance) = 0;
        virtual void ForceRateLimiter(IRateLimiterAPI * instance) = 0;
        virtual std::pair<std::string, IRateLimiterAPI *> GetCurrentRateLimiterLibrary() = 0; // Returns nullptr if there are none registered
};

struct IHeroStats {
    float TotalMinionsKilled;
    float MinionsKilled;
    float NeturalMinionKilled;
    int ChampionsKilled;
    int DeathsNum;
    int Assists; };

struct IPerk {
    int Id;
    std::string Name; };

struct IItemData {
    std::string ItemName;

    float Price;
    float Stacks;
    float ScaleBonus;

    byte ItemCount;
    byte Ammo;

    ItemId ItemId; };

struct ISkinInfo {
    const char * SkinName;
    const char * Model;
    int SkinID; };

struct ISpellData {
    std::string SpellName;

    bool IsToggleSpell;
    bool CantCancelWhileWindingUp;
    bool CantCancelWhileChanneling;
    bool DoesntBreakChannels;
    bool CantCastWhileRooted;
    bool CannotBeSuppressed;
    bool CanMoveWhileChanneling;
    bool DoNotNeedToFaceTarget;
    bool SpellRevealsChampion;

    float MissileSpeed;
    float LineWidth;

    std::array<float, 6> Cooldown;
    std::array<float, 6> CastRadius;
    std::array<float, 6> CastRadiusSecondary;
    std::array<float, 6> CastRange;
    std::array<float, 6> CastRangeDisplayOverride;
    std::array<float, 6> ManaCost;

    SpellTargeting TargetingType;

    float CastFrame;
    bool IgnoreAnimContinueUntilCastFrame; };

struct ICharacterData {
    std::string BaseSkinName;

    float BaseHealth;
    float HealthPerLevel;
    float BaseHealthRegen;
    float HealthRegenPerLevel;
    float BaseMana;
    float ManaPerLevel;
    float BaseManaRegen;
    float ManaRegenPerLevel;
    float BaseAttackDamage;
    float AttackDamagePerLevel;
    float BaseArmor;
    float ArmorPerLevel;
    float BaseSpellBlock;
    float SpellBlockPerLevel;
    float BaseMovementSpeed;
    float BaseAttackRange;
    float BaseAttackSpeed;
    float AttackSpeedPerLevel; };

struct DamageInput {
    bool IsAutoAttack = FALSE;
    bool IsCriticalAttack = FALSE;
    bool IsAbility = TRUE;
    bool DontIncludePassives = FALSE;
    bool DontCalculateItemDamage = FALSE;
    bool IsOnHitDamage = FALSE;
    bool IsTargetedAbility = FALSE;
    bool AppliesOnHitDamage = FALSE;
    bool DoesntTriggerOnHitEffects = FALSE;

    float RawPhysicalDamage{ };
    float RawMagicalDamage{ };
    float RawTrueDamage{ };

    DamageInput() = default; };

struct IPredictionInput {
    Vector From;
    Vector RangeCheckFrom;

    float Delay = 0.f;
    float Radius = 1.f;
    float Range = FLT_MAX;
    float Speed = FLT_MAX;

    eSkillshotType Type = kSkillshotLine;

    SpellSlot Slot = SpellSlot::Invalid;
    IGameObject * Target = nullptr;

    bool UseBoundingRadius = true;
    bool Aoe = true;
    bool Collision = true;
    eCollisionFlags CollisionFlags = kCollidesWithYasuoWall; };

struct IPredictionOutput {
    IPredictionInput * PredictionInput;
    HitChance Hitchance;
    Vector CastPosition;
    Vector UnitPosition;
    std::vector<IGameObject *> AoeTargetsHit;
    std::vector<IGameObject *> CollisionObjects; };

struct IDashData {
    Vector StartPosition;
    Vector EndPosition;
    int Duration;
    int StartTick;
    int EndTick;
    float Speed;
    std::vector<Vector> Path; };

struct IRecommendedItemInfo {
    uint32_t id;
    uint32_t count; };

struct IItemBuildInfo {
    uint32_t id;

    int32_t total_cost;
    int32_t base_cost;

    std::vector<uint32_t> from; };

extern IPluginsSDK * g_PluginSDK;
extern IObjectManager * g_ObjectManager;
extern ICamera * g_Camera;
extern INavMesh * g_NavMesh;
extern IRenderer * g_Renderer;
extern IShop * g_Shop;
extern IGameObject * g_LocalPlayer;
extern IDrawing * g_Drawing;
extern IMainMenu * g_Menu;
extern IOrbwalker * g_Orbwalker;
extern ICommon * g_Common;
extern IChampionManager * g_ChampionManager;
extern IHealthPrediction * g_HealthPrediction;
extern IItemManager * g_ItemManager;
extern ILogSystem * g_Log;