#pragma once

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

class Player : public godot::Node2D {
    GDCLASS(Player, godot::Node2D)

private:
    static constexpr double BARREL_LENGTH = 22.0;     // khoang cach tu tay cam sung toi dau nong
    static constexpr double AIM_LINE_LENGTH = 170.0;  // do dai tia ngam
    static constexpr double HIT_RADIUS = 22.0;        // ban kinh va cham cua nhan vat
    static constexpr double INVULNERABLE_TIME = 1.0;  // thoi gian bat tu sau khi trung dan (giay)

    double speed = 450.0;         // toc do di chuyen (pixel/giay)
    double fire_interval = 0.25;  // thoi gian giua 2 lan ban (giay)
    double edge_margin = 120.0;    // khoang cach toi thieu tu mep man hinh

    int max_health = 5;
    int health = 5;
    bool dead = false;
    double invulnerable_timer = 0.0;

    double cooldown = 0.0;
    double flash_time = 0.0;
    godot::Vector2 aim_direction = godot::Vector2(0, 1);
    godot::Vector2 muzzle_origin = godot::Vector2(0, 22); // vi tri tay cam sung, tinh tu tam nhan vat

    godot::AnimatedSprite2D *sprite = nullptr;

    void _update_aim();
    void _try_fire(godot::Input *p_input);
    void _play_animation(const godot::StringName &p_name, bool p_flip);
    void _update_blink();

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double p_delta) override;
    void _draw() override;

    godot::Vector2 get_muzzle_position() const;
    godot::Vector2 get_aim_direction() const;

    void take_damage(int p_amount);
    bool is_dead() const;
    int get_health() const;
    int get_max_health() const;
    double get_hit_radius() const;

    void set_speed(double p_speed);
    double get_speed() const;
    void set_fire_interval(double p_interval);
    double get_fire_interval() const;
};