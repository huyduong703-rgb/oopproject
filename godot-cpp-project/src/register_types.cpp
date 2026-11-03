#include "register_types.h"
#include "bullet.h"
#include "example.h"
#include "flying_monster.h"
#include "game_manager.h"
#include "main_menu.h"
#include "monster.h"
#include "player.h"
#include "slime.h"
#include "enemy_bullet.h" 

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    GDREGISTER_CLASS(GDExample);
    GDREGISTER_CLASS(MainMenu);
    GDREGISTER_CLASS(Player);
    GDREGISTER_CLASS(EnemyBullet);

    // Lop cha phai duoc dang ky truoc cac lop con
    GDREGISTER_ABSTRACT_CLASS(Monster);
    GDREGISTER_CLASS(Slime);
    GDREGISTER_CLASS(FlyingMonster);

    GDREGISTER_CLASS(Bullet);
    GDREGISTER_CLASS(GameManager);
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT example_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization) {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_example_module);
    init_obj.register_terminator(uninitialize_example_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}
