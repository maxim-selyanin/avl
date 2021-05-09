#pragma once

class color_keeper {
    bool color_is_black = false;//false by default, equivalent to red

public:
    void repaint() {
        color_is_black = !color_is_black;
    }

    [[nodiscard]] bool black() const {
        return color_is_black;
    }

    [[nodiscard]] bool red() const {
        return !color_is_black;
    }

    void set_black() {
        color_is_black = true;
    }

    void swap(color_keeper &other) {
        this->color_is_black ^= other.color_is_black;
        other.color_is_black ^= this->color_is_black;
        this->color_is_black ^= other.color_is_black;
    }
};