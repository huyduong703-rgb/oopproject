#include "slime.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Slime::_bind_methods() {
}

Slime::Slime() {
    max_health = 3;
    health = 3;
    speed = 90.0;
    score_value = 10;
    hit_radius = 40.0;
    attack_interval = 4.0;
    frames_path = String("res://assets/monster1_frames.tres");
}

void Slime::_update_movement(double p_delta) {
    _move_horizontal(p_delta);
}
