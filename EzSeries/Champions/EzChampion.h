#pragma once
#include "../Helpers/EzExtensions.h"
#include "../Helpers/EzAvoider.h"

class EzChampion {
    public:
        static EzExtensions * Ex;
        static std::map<std::string, IMenuElement *> Menu;
        static std::map<std::string, std::shared_ptr<ISpell>> Spells;
        static std::map<std::string, float> Ticks;
        static std::map<ItemId, std::shared_ptr<ISpell>> Items;
        static std::map<std::string, EzAvoider *> AvoidList;
        static std::map<float, EzAvoider *> DangerPoints; };

EzExtensions * EzChampion::Ex;
std::map<std::string, std::shared_ptr<ISpell>> EzChampion::Spells;
std::map<std::string, IMenuElement *> EzChampion::Menu;
std::map<std::string, float> EzChampion::Ticks;
std::map<ItemId, std::shared_ptr<ISpell>> EzChampion::Items;