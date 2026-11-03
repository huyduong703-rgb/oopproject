#pragma once

#include "monster.h"

// Quai bay: di ngang va luon luon len xuong theo duong song (sin).
class FlyingMonster : public Monster {
    GDCLASS(FlyingMonster, Monster)

private:
    double time_passed = 0.0;
    double base_y = 0.0;
    double amplitude = 45.0;
    bool initialized = false;

protected:
    static void _bind_methods();
    void _update_movement(double p_delta) override;

public:
    FlyingMonster();
};
