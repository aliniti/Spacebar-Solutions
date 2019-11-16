#pragma once
#include "../Helpers/EzExtensions.h"


class EzChampion {
    public:
        static EzExtensions * ezex;
        static std::map<std::string, IMenuElement *> ezmenu;
        static std::map<std::string, std::shared_ptr<ISpell>>  ezspells; };

EzExtensions * EzChampion::ezex;
std::map<std::string, std::shared_ptr<ISpell>>  EzChampion::ezspells;
std::map<std::string, IMenuElement *> EzChampion::ezmenu;