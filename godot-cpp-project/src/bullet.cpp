#include "bullet.h"

#include "monster.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Bullet::_bind_methods() {
}

void Bullet::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    Ref<Texture2D> tex = ResourceLoader::get_singleton()->load("res://assets/bullets/player_bullet.png");
    texture = tex;
    queue_redraw();
}

void Bullet::_process(double p_delta) {
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

    // Kiem tra va cham voi quai bang khoang cach giua hai hinh tron
    TypedArray<Node> monsters = get_tree()->get_nodes_in_group(StringName("monsters"));
    for (int64_t i = 0; i < monsters.size(); i++) {
        Object *obj = monsters[i];
        Monster *monster = Object::cast_to<Monster>(obj);
        if (monster == nullptr || monster->is_dead()) {
            continue;
        }
        double dist = pos.distance_to(monster->get_global_position());
        if (dist <= radius + monster->get_hit_radius()) {
            monster->take_damage(damage);
            queue_free();
            return;
        }
    }
}

void Bullet::_draw() {
    if (texture.is_valid()) {
        draw_texture_rect(texture, Rect2(Vector2(-14, -14), Vector2(28, 28)), false);
    } else {
        draw_circle(Vector2(0, 0), 8.0, Color(1.0, 0.2, 0.2, 1.0));
    }
}

void Bullet::set_direction(Vector2 p_direction) {
    direction = p_direction.normalized();
}

Vector2 Bullet::get_direction() const {
    return direction;
}

void Bullet::set_damage(int p_damage) {
    damage = p_damage;
}

int Bullet::get_damage() const {
    return damage;
}
