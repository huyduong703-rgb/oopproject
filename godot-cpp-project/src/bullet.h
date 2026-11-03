#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

class Bullet : public godot::Node2D {
    GDCLASS(Bullet, godot::Node2D)

private:
    godot::Vector2 direction = godot::Vector2(0, 1);
    double speed = 900.0;
    int damage = 1;
    double radius = 12.0;
    godot::Ref<godot::Texture2D> texture;

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double p_delta) override;
    void _draw() override;

    void set_direction(godot::Vector2 p_direction);
    godot::Vector2 get_direction() const;
    void set_damage(int p_damage);
    int get_damage() const;
};
