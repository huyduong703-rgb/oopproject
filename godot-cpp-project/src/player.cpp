#include "player.h"

#include <cmath>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

static const double PI_VALUE = 3.14159265358979323846;
static const double MIN_ANGLE = 0.2;                // gioi han goc ngam, tranh ban ngang
static const double MAX_ANGLE = PI_VALUE - 0.2;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_fire_interval", "interval"), &Player::set_fire_interval);
    ClassDB::bind_method(D_METHOD("get_fire_interval"), &Player::get_fire_interval);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "50,1500,10"),
            "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fire_interval", PROPERTY_HINT_RANGE, "0.05,2,0.05"),
            "set_fire_interval", "get_fire_interval");

    ADD_SIGNAL(MethodInfo("fired",
            PropertyInfo(Variant::VECTOR2, "position"),
            PropertyInfo(Variant::VECTOR2, "direction")));
    ADD_SIGNAL(MethodInfo("health_changed",
            PropertyInfo(Variant::INT, "current"),
            PropertyInfo(Variant::INT, "maximum")));
    ADD_SIGNAL(MethodInfo("died"));
}

void Player::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Quai va dan cua quai tim Player thong qua nhom "player"
    add_to_group(StringName("player"));
    health = max_health;

    // Node con tren scene phai ten la "Sprite" (loai AnimatedSprite2D)
    sprite = get_node<AnimatedSprite2D>(NodePath("Sprite"));
    if (sprite == nullptr) {
        UtilityFunctions::push_warning("Player: khong tim thay node con 'Sprite' (AnimatedSprite2D).");
        return;
    }
    sprite->play(StringName("idle_front"));
}

void Player::_process(double p_delta) {
    // Class chay ca trong editor, nen bo qua o day de nhan vat khong tu di chuyen luc thiet ke.
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    // Da chet thi khong dieu khien duoc nua
    if (dead) {
        return;
    }

    Input *input = Input::get_singleton();

    // Di chuyen trai/phai
    double dir = 0.0;
    if (input->is_key_pressed(KEY_A) || input->is_key_pressed(KEY_LEFT)) {
        dir -= 1.0;
    }
    if (input->is_key_pressed(KEY_D) || input->is_key_pressed(KEY_RIGHT)) {
        dir += 1.0;
    }

    Vector2 pos = get_position();
    pos.x += dir * speed * p_delta;
    double width = get_viewport_rect().size.x;
    pos.x = UtilityFunctions::clampf(pos.x, edge_margin, width - edge_margin);
    set_position(pos);

    // Hoat anh: chay sang trai dung anh Left, chay sang phai thi lat ngang, dung yen thi nhin thang
    if (dir < 0.0) {
        _play_animation(StringName("run_left"), false);
    } else if (dir > 0.0) {
        _play_animation(StringName("run_left"), true);
    } else {
        _play_animation(StringName("idle_front"), false);
    }

    _update_aim();

    // Ban
    cooldown -= p_delta;
    flash_time -= p_delta;
    _try_fire(input);

    // Bat tu sau khi bi trung dan: nhap nhay
    if (invulnerable_timer > 0.0) {
        invulnerable_timer -= p_delta;
    }
    _update_blink();

    queue_redraw();
}

void Player::_update_blink() {
    if (sprite == nullptr) {
        return;
    }
    bool faded = invulnerable_timer > 0.0 && ((int)(invulnerable_timer * 12.0) % 2 == 0);
    sprite->set_modulate(Color(1.0, 1.0, 1.0, faded ? 0.35 : 1.0));
}

void Player::_play_animation(const StringName &p_name, bool p_flip) {
    if (sprite == nullptr) {
        return;
    }
    sprite->set_flip_h(p_flip);
    if (sprite->get_animation() != p_name) {
        sprite->play(p_name);
    }
}

void Player::_update_aim() {
    Vector2 origin = get_global_position() + muzzle_origin;
    Vector2 to_mouse = get_global_mouse_position() - origin;
    double angle;
    if (to_mouse.y < 0.0) {
        // Chuot o phia tren: giu sung huong ve phia chuot nhung van chia xuong duoi.
        angle = (to_mouse.x >= 0.0) ? MIN_ANGLE : MAX_ANGLE;
    } else {
        angle = UtilityFunctions::clampf(std::atan2(to_mouse.y, to_mouse.x), MIN_ANGLE, MAX_ANGLE);
    }
    aim_direction = Vector2(std::cos(angle), std::sin(angle));
}

void Player::_try_fire(Input *p_input) {
    bool pressed = p_input->is_key_pressed(KEY_SPACE) || p_input->is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    if (pressed && cooldown <= 0.0) {
        cooldown = fire_interval;
        flash_time = 0.06;
        emit_signal("fired", get_muzzle_position(), aim_direction);
    }
}

void Player::_draw() {
    if (dead) {
        return;
    }
    // Hinh nhan vat do AnimatedSprite2D ve. O day chi ve tia ngam va tia lua dau nong.
    Vector2 start = muzzle_origin + aim_direction * BARREL_LENGTH;
    draw_line(start, start + aim_direction * AIM_LINE_LENGTH, Color(0.3, 0.95, 1.0, 0.35), 2.0);
    if (flash_time > 0.0) {
        draw_circle(start, 9.0, Color(1.0, 0.9, 0.4, 1.0));
    }
}

// ---------- Mau ----------

void Player::take_damage(int p_amount) {
    if (dead || invulnerable_timer > 0.0) {
        return;
    }
    health -= p_amount;
    if (health < 0) {
        health = 0;
    }
    emit_signal("health_changed", health, max_health);

    if (health == 0) {
        dead = true;
        if (sprite) {
            sprite->set_modulate(Color(1.0, 0.3, 0.3, 1.0));
        }
        queue_redraw(); // de xoa tia ngam
        emit_signal("died");
    } else {
        invulnerable_timer = INVULNERABLE_TIME;
    }
}

bool Player::is_dead() const {
    return dead;
}

int Player::get_health() const {
    return health;
}

int Player::get_max_health() const {
    return max_health;
}

double Player::get_hit_radius() const {
    return HIT_RADIUS;
}

// ---------- Getter / Setter ----------

Vector2 Player::get_muzzle_position() const {
    return get_global_position() + muzzle_origin + aim_direction * BARREL_LENGTH;
}

Vector2 Player::get_aim_direction() const {
    return aim_direction;
}

void Player::set_speed(double p_speed) {
    speed = p_speed;
}

double Player::get_speed() const {
    return speed;
}

void Player::set_fire_interval(double p_interval) {
    fire_interval = p_interval;
}

double Player::get_fire_interval() const {
    return fire_interval;
}