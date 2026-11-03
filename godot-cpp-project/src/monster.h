#pragma once

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/string.hpp>

// Lop cha cua tat ca quai. Cac lop con (Slime, FlyingMonster...) chi can
// doi chi so va ghi de (override) cach di chuyen _update_movement().
class Monster : public godot::Node2D {
    GDCLASS(Monster, godot::Node2D)

protected:
    static void _bind_methods();

    int max_health = 3;
    int health = 3;
    double speed = 100.0;
    int score_value = 10;
    double hit_radius = 38.0;
    int facing = -1; // -1: di sang trai, 1: di sang phai
    bool dead = false;
    double attack_interval = 0.0; // giay giua 2 lan ban; 0 = quai khong ban
    double attack_timer = 0.0;
    godot::String frames_path;
    godot::AnimatedSprite2D *sprite = nullptr;

    virtual void _update_movement(double p_delta);
    void _move_horizontal(double p_delta);
    void _try_attack(double p_delta);
    void _on_animation_finished();

public:
    void _ready() override;
    void _process(double p_delta) override;

    void take_damage(int p_amount);
    bool is_dead() const;
    double get_hit_radius() const;
    void set_facing(int p_facing);
    int get_facing() const;
};