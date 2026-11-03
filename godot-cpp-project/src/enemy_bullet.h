#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

// Dan cua quai: bay thang ve phia Player, trung thi tru mau Player.
class EnemyBullet : public godot::Node2D {
    GDCLASS(EnemyBullet, godot::Node2D)

private:
    godot::Vector2 direction = godot::Vector2(0, 1);
    double speed = 320.0;
    int damage = 1;
    double radius = 10.0;

protected:
    static void _bind_methods();

public:
    void _process(double p_delta) override;
    void _draw() override;

    void set_direction(godot::Vector2 p_direction);
    godot::Vector2 get_direction() const;
    void set_damage(int p_damage);
    int get_damage() const;
};
