#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include "player.h"

// Quan ly man choi: nghe tin hieu ban cua Player de tao dan,
// tu sinh quai, cong diem khi quai chet, cap nhat bang diem / mau
// va hien bang Game Over khi Player chet.
class GameManager : public godot::Node2D {
    GDCLASS(GameManager, godot::Node2D)

private:
    Player *player = nullptr;
    godot::Label *score_label = nullptr;
    godot::Label *hp_label = nullptr;
    godot::Control *game_over_overlay = nullptr;
    godot::Label *final_score_label = nullptr;

    godot::String menu_scene_path = godot::String("res://main_menu.tscn");

    int score = 0;
    bool game_over = false;
    double spawn_interval = 1.8;
    double spawn_timer = 0.8;
    int max_monsters = 5;

    void _on_player_fired(godot::Vector2 p_position, godot::Vector2 p_direction);
    void _on_player_health_changed(int p_current, int p_max);
    void _on_player_died();
    void _on_monster_died(int p_score);
    void _on_monster_shot(godot::Vector2 p_position, godot::Vector2 p_direction);
    void _on_retry_pressed();
    void _on_menu_pressed();
    void _spawn_monster();
    void _update_score_label();
    void _update_hp_label(int p_current, int p_max);

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double p_delta) override;

    void set_menu_scene_path(const godot::String &p_path);
    godot::String get_menu_scene_path() const;
};