#include "flying_monster.h"

#include <cmath>

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void FlyingMonster::_bind_methods() {
}

FlyingMonster::FlyingMonster() {
    max_health = 2;
    health = 2;
    speed = 140.0;
    score_value = 20;
    hit_radius = 42.0;
    attack_interval = 2.5;
    frames_path = String("res://assets/monster2_frames.tres");
}

void FlyingMonster::_update_movement(double p_delta) {
    if (!initialized) {
        base_y = get_position().y;
        initialized = true;
    }

    _move_horizontal(p_delta);

    time_passed += p_delta;
    Vector2 pos = get_position();
    pos.y = base_y + std::sin(time_passed * 2.5) * amplitude;
    set_position(pos);
}
