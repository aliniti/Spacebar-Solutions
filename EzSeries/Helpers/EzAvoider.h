#pragma once
#include "../SDK/PluginSDK.h"

enum EvadeType {
    Targeted,
    SkillshotLine,
    SkillshotCircle,
    SelfCast };

enum AvoidType {
    Inside,
    Outside };

class EzAvoider {
    public:

        float Radius;
        AvoidType aType;
        EvadeType eType;
        IGameObject * Emitter;

        std::string SpellName;
        std::string ChampName;

        EzAvoider(IGameObject * emitter, AvoidType type, float radius) : eType() {
            this->aType = type;
            this->Emitter = emitter;
            this->Radius = radius; }

        EzAvoider(std::string name, EvadeType type, std::string champname) : Radius(0), aType(), Emitter(nullptr) {
            this->SpellName = name;
            this->eType = type;
            this->ChampName = champname; }

        static void generate_avoid_list(); };

inline void EzAvoider::generate_avoid_list() {}
