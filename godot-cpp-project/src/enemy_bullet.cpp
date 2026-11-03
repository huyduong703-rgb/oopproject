#include "enemy_bullet.h"

#include "player.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void EnemyBullet::_bind_methods() {
}

void EnemyBullet::_process(double p_delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    set_position(get_position() + direction * speed * p_delta);

    // Ra khoi man hinh thi xoa
    Vector2 pos = get_global_position();
    Vector2 size = get_viewport_rect().size;
    if (pos.x < -60.0 || pos.x > size.x + 60.0 || pos.y < -60.0 || pos.y > size.y + 60.0) {
        queue_free();
        return;
    }

    // Va cham voi Player (hai hinh tron)
    Node *node = get_tree()->get_first_node_in_group(StringName("player"));
    Player *player = Object::cast_to<Player>(node);
    if (player == nullptr || player->is_dead()) {
        return;
    }
    if (pos.distance_to(player->get_global_position()) <= radius + player->get_hit_radius()) {
        player->take_damage(damage);
        queue_free();
    }
}

void EnemyBullet::_draw() {
    draw_circle(Vector2(0, 0), radius, Color(1.0, 0.45, 0.1, 1.0));
    draw_circle(Vector2(0, 0), radius * 0.5, Color(1.0, 0.95, 0.6, 1.0));
}

void EnemyBullet::set_direction(Vector2 p_direction) {
    direction = p_direction.normalized();
}

Vector2 EnemyBullet::get_direction() const {
    return direction;
}

void EnemyBullet::set_damage(int p_damage) {
    damage = p_damage;
}

int EnemyBullet::get_damage() const {
    return damage;
}
