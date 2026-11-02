#pragma once

#include <godot_cpp/classes/node.hpp>

class GDExample : public godot::Node {
    GDCLASS(GDExample, godot::Node)

protected:
    static void _bind_methods();

public:
    void _ready() override;
};