#include "monster.h"

#include "player.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

static const double EDGE_MARGIN = 120.0;

void Monster::_bind_methods() {
    ADD_SIGNAL(MethodInfo("died", PropertyInfo(Variant::INT, "score")));
    ADD_SIGNAL(MethodInfo("shot",
            PropertyInfo(Variant::VECTOR2, "position"),
            PropertyInfo(Variant::VECTOR2, "direction")));
}

void Monster::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    health = max_health;
    add_to_group(StringName("monsters"));

    // Moi con quai ban lech nhau mot chut de khong ban dong loat
    if (attack_interval > 0.0) {
        attack_timer = 0.5 + attack_interval * UtilityFunctions::randf();
    }

    Ref<SpriteFrames> frames = ResourceLoader::get_singleton()->load(frames_path);
    if (!frames.is_valid()) {
        UtilityFunctions::push_warning("Monster: khong nap duoc ", frames_path);
        return;
    }

    sprite = memnew(AnimatedSprite2D);
    sprite->set_sprite_frames(frames);
    add_child(sprite);
    sprite->connect("animation_finished", callable_mp(this, &Monster::_on_animation_finished));
    sprite->play(StringName("run"));
}

void Monster::_process(double p_delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (dead) {
        return;
    }
    _update_movement(p_delta);
    _try_attack(p_delta);
    if (sprite) {
        // Anh goc quay sang trai, nen di sang phai thi lat ngang
        sprite->set_flip_h(facing > 0);
    }
}

void Monster::_update_movement(double p_delta) {
    // Mac dinh dung yen. Lop con se ghi de ham nay.
}

void Monster::_move_horizontal(double p_delta) {
    Vector2 pos = get_position();
    pos.x += facing * speed * p_delta;

    double width = get_viewport_rect().size.x;
    if (pos.x < EDGE_MARGIN) {
        facing = 1;
    } else if (pos.x > width - EDGE_MARGIN) {
        facing = -1;
    }
    set_position(pos);
}

void Monster::_try_attack(double p_delta) {
    if (attack_interval <= 0.0) {
        return;
    }
    attack_timer -= p_delta;
    if (attack_timer > 0.0) {
        return;
    }
    attack_timer = attack_interval;

    // Chi ban khi da vao trong man hinh
    Vector2 pos = get_global_position();
    double width = get_viewport_rect().size.x;
    if (pos.x < 0.0 || pos.x > width) {
        return;
    }

    Node *node = get_tree()->get_first_node_in_group(StringName("player"));
    Player *player = Object::cast_to<Player>(node);
    if (player == nullptr || player->is_dead()) {
        return;
    }

    Vector2 direction = (player->get_global_position() - pos).normalized();
    emit_signal("shot", pos, direction);
}

void Monster::take_damage(int p_amount) {
    if (dead) {
        return;
    }
    health -= p_amount;
    if (health <= 0) {
        dead = true;
        if (sprite) {
            sprite->play(StringName("death"));
        } else {
            queue_free();
        }
        emit_signal("died", score_value);
    } else if (sprite) {
        sprite->play(StringName("hit"));
    }
}

void Monster::_on_animation_finished() {
    if (dead) {
        queue_free();
        return;
    }
    if (sprite) {
        sprite->play(StringName("run"));
    }
}

bool Monster::is_dead() const {
    return dead;
}

double Monster::get_hit_radius() const {
    return hit_radius;
}

void Monster::set_facing(int p_facing) {
    facing = p_facing;
}

int Monster::get_facing() const {
    return facing;
}