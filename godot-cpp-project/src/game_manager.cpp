#include "game_manager.h"

#include "bullet.h"
#include "enemy_bullet.h"
#include "flying_monster.h"
#include "monster.h"
#include "slime.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_menu_scene_path", "path"), &GameManager::set_menu_scene_path);
    ClassDB::bind_method(D_METHOD("get_menu_scene_path"), &GameManager::get_menu_scene_path);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "menu_scene_path", PROPERTY_HINT_FILE, "*.tscn"),
            "set_menu_scene_path", "get_menu_scene_path");
}

void GameManager::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // GameManager, Player, cac Label va GameOverOverlay la cac node cung cap trong game.tscn
    player = get_node<Player>(NodePath("../Player"));
    score_label = get_node<Label>(NodePath("../ScoreLabel"));
    hp_label = get_node<Label>(NodePath("../HpLabel"));

    game_over_overlay = get_node<Control>(NodePath("../GameOverOverlay"));
    final_score_label = get_node<Label>(NodePath("../GameOverOverlay/Panel/FinalScore"));
    Button *retry_button = get_node<Button>(NodePath("../GameOverOverlay/Panel/BtnRetry"));
    Button *menu_button = get_node<Button>(NodePath("../GameOverOverlay/Panel/BtnMenu"));

    if (retry_button) {
        retry_button->connect("pressed", callable_mp(this, &GameManager::_on_retry_pressed));
    }
    if (menu_button) {
        menu_button->connect("pressed", callable_mp(this, &GameManager::_on_menu_pressed));
    }

    if (player) {
        player->connect("fired", callable_mp(this, &GameManager::_on_player_fired));
        player->connect("health_changed", callable_mp(this, &GameManager::_on_player_health_changed));
        player->connect("died", callable_mp(this, &GameManager::_on_player_died));
        _update_hp_label(player->get_health(), player->get_max_health());
    } else {
        UtilityFunctions::push_warning("GameManager: khong tim thay node ../Player");
    }
    _update_score_label();
}

void GameManager::_process(double p_delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (game_over) {
        return;
    }

    spawn_timer -= p_delta;
    if (spawn_timer <= 0.0) {
        spawn_timer = spawn_interval;
        int count = (int)get_tree()->get_nodes_in_group(StringName("monsters")).size();
        if (count < max_monsters) {
            _spawn_monster();
        }
    }
}

void GameManager::_on_player_fired(Vector2 p_position, Vector2 p_direction) {
    Bullet *bullet = memnew(Bullet);
    bullet->set_direction(p_direction);
    add_child(bullet);
    bullet->set_global_position(p_position);
}

void GameManager::_on_player_health_changed(int p_current, int p_max) {
    _update_hp_label(p_current, p_max);
}

void GameManager::_on_player_died() {
    game_over = true;
    if (final_score_label) {
        final_score_label->set_text(String::utf8("\xC4\x90I\xE1\xBB\x82M: ") + String::num_int64(score));
    }
    if (game_over_overlay) {
        game_over_overlay->set_visible(true);
    }
    // Dung hinh toan bo game. Bang Game Over co process_mode = Always nen nut van bam duoc.
    get_tree()->set_pause(true);
}

void GameManager::_on_retry_pressed() {
    get_tree()->set_pause(false);
    get_tree()->reload_current_scene();
}

void GameManager::_on_menu_pressed() {
    get_tree()->set_pause(false);
    if (menu_scene_path.is_empty()) {
        UtilityFunctions::push_warning("GameManager: chua chon menu_scene_path trong Inspector.");
        return;
    }
    get_tree()->change_scene_to_file(menu_scene_path);
}

void GameManager::_spawn_monster() {
    double width = get_viewport_rect().size.x;
    bool from_left = UtilityFunctions::randf() < 0.5;
    double x = from_left ? -50.0 : width + 50.0;
    double y = 500.0;

    Monster *monster = nullptr;
    if (UtilityFunctions::randf() < 0.65) {
        monster = memnew(Slime);
        y = UtilityFunctions::randf_range(330.0, 490.0);
    } else {
        monster = memnew(FlyingMonster);
        y = UtilityFunctions::randf_range(260.0, 340.0);
    }

    monster->set_facing(from_left ? 1 : -1);
    add_child(monster);
    monster->set_global_position(Vector2(x, y));
    monster->connect("died", callable_mp(this, &GameManager::_on_monster_died));
    monster->connect("shot", callable_mp(this, &GameManager::_on_monster_shot));
}

void GameManager::_on_monster_died(int p_score) {
    score += p_score;
    _update_score_label();
}

void GameManager::_on_monster_shot(Vector2 p_position, Vector2 p_direction) {
    if (game_over) {
        return;
    }
    EnemyBullet *bullet = memnew(EnemyBullet);
    bullet->set_direction(p_direction);
    add_child(bullet);
    bullet->set_global_position(p_position);
}

void GameManager::_update_score_label() {
    if (score_label == nullptr) {
        return;
    }
    // "\xC4\x90I\xE1\xBB\x82M: " la chu "DIEM: " co dau, viet bang ma UTF-8 de khong loi font/ma hoa
    score_label->set_text(String::utf8("\xC4\x90I\xE1\xBB\x82M: ") + String::num_int64(score));
}

void GameManager::_update_hp_label(int p_current, int p_max) {
    if (hp_label == nullptr) {
        return;
    }
    // "M\xC3\x81U: " la chu "MAU: " co dau
    hp_label->set_text(String::utf8("M\xC3\x81U: ") + String::num_int64(p_current) + String("/") + String::num_int64(p_max));
}

void GameManager::set_menu_scene_path(const String &p_path) {
    menu_scene_path = p_path;
}

String GameManager::get_menu_scene_path() const {
    return menu_scene_path;
}