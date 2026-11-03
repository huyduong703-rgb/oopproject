#pragma once

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/h_slider.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>

class MainMenu : public godot::Control {
    GDCLASS(MainMenu, godot::Control)

private:
    godot::Control *menu_box = nullptr;
    godot::PanelContainer *mode_popup = nullptr;
    godot::Control *settings_overlay = nullptr;
    godot::Control *credits_overlay = nullptr;

    godot::HSlider *music_slider = nullptr;
    godot::HSlider *sfx_slider = nullptr;
    godot::Label *music_value_label = nullptr;
    godot::Label *sfx_value_label = nullptr;

    // Scene nap khi chon che do. Mac dinh ca hai deu vao game.tscn, doi trong Inspector neu can.
    godot::String story_scene_path = godot::String("res://game.tscn");
    godot::String endless_scene_path = godot::String("res://game.tscn");

    void _on_play_pressed();
    void _on_setting_pressed();
    void _on_credits_pressed();
    void _on_quit_pressed();
    void _on_story_pressed();
    void _on_endless_pressed();
    void _on_back_pressed();
    void _on_settings_close_pressed();
    void _on_credits_close_pressed();
    void _on_music_changed(double p_value);
    void _on_sfx_changed(double p_value);

    void _go_to_scene(const godot::String &p_path);
    void _connect_button(const godot::String &p_path, const godot::Callable &p_callable);

    void _ensure_audio_buses();
    void _apply_volume(const godot::String &p_bus, double p_percent);
    void _load_settings();
    void _save_settings();

protected:
    static void _bind_methods();

public:
    void _ready() override;

    void set_story_scene_path(const godot::String &p_path);
    godot::String get_story_scene_path() const;
    void set_endless_scene_path(const godot::String &p_path);
    godot::String get_endless_scene_path() const;
};