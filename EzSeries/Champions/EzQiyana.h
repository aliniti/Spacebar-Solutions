#pragma once
#include "EzChampion.h"

class EzQiyana : public EzChampion {
    public:

        static IMenu * on_boot(IMenu * menu);
        static auto on_boot() -> void;
        static auto on_update() -> void;
        static auto on_post_update() -> void;
        static auto on_huddraw() -> void;
        static auto hpbarfill_render() -> void;
        static auto on_do_cast(IGameObject * unit, OnProcessSpellEventArgs * args) -> void;
        static void on_dash(IGameObject * unit, OnProcessSpellEventArgs * args); };

inline auto EzQiyana::on_boot() -> void {}

inline auto EzQiyana::on_update() -> void {}

inline void EzQiyana::on_post_update() {}

inline auto EzQiyana::on_huddraw() -> void {

}
