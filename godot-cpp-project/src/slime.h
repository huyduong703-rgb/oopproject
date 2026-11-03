#pragma once

#include "monster.h"

// Quai slime xanh: di cham theo chieu ngang, cham mep man hinh thi quay dau.
class Slime : public Monster {
    GDCLASS(Slime, Monster)

protected:
    static void _bind_methods();
    void _update_movement(double p_delta) override;

public:
    Slime();
};
