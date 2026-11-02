#include "main_menu.h"

#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/config_file.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

static const char *SETTINGS_PATH = "user://settings.cfg";
static const char *MUSIC_BUS = "Music";
static const char *SFX_BUS = "SFX";
static const double DEFAULT_VOLUME = 80.0;

void MainMenu::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_story_scene_path", "path"), &MainMenu::set_story_scene_path);
    ClassDB::bind_method(D_METHOD("get_story_scene_path"), &MainMenu::get_story_scene_path);
    ClassDB::bind_method(D_METHOD("set_endless_scene_path", "path"), &MainMenu::set_endless_scene_path);
    ClassDB::bind_method(D_METHOD("get_endless_scene_path"), &MainMenu::get_endless_scene_path);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "story_scene_path", PROPERTY_HINT_FILE, "*.tscn"),
            "set_story_scene_path", "get_story_scene_path");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "endless_scene_path", PROPERTY_HINT_FILE, "*.tscn"),
            "set_endless_scene_path", "get_endless_scene_path");
}

void MainMenu::_connect_button(const String &p_path, const Callable &p_callable) {
    Button *button = get_node<Button>(NodePath(p_path));
    if (button == nullptr) {
        UtilityFunctions::push_warning("MainMenu: khong tim thay nut ", p_path);
        return;
    }
    button->connect("pressed", p_callable);
}

void MainMenu::_ready() {
    // Class chay ca trong editor, nen bo qua de khong noi tin hieu luc dang thiet ke.
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    menu_box = get_node<Control>(NodePath("VBoxContainer"));
    mode_popup = get_node<PanelContainer>(NodePath("ModeSelectPopup"));
    settings_overlay = get_node<Control>(NodePath("SettingsOverlay"));
    credits_overlay = get_node<Control>(NodePath("CreditsOverlay"));

    const String settings_box = "SettingsOverlay/Panel/Margin/VBox/";
        music_slider = get_node<HSlider>(NodePath(settings_box + String("MusicRow/MusicSlider")));
    music_value_label = get_node<Label>(NodePath(settings_box + String("MusicRow/MusicValue")));
    sfx_slider = get_node<HSlider>(NodePath(settings_box + String("SfxRow/SfxSlider")));
    sfx_value_label = get_node<Label>(NodePath(settings_box + String("SfxRow/SfxValue")));

    _connect_button("VBoxContainer/BtnPlay", callable_mp(this, &MainMenu::_on_play_pressed));
    _connect_button("VBoxContainer/BtnSetting", callable_mp(this, &MainMenu::_on_setting_pressed));
    _connect_button("VBoxContainer/BtnCredits", callable_mp(this, &MainMenu::_on_credits_pressed));
    _connect_button("VBoxContainer/BtnQuit", callable_mp(this, &MainMenu::_on_quit_pressed));
    _connect_button("ModeSelectPopup/VBoxContainer/BtnStory", callable_mp(this, &MainMenu::_on_story_pressed));
    _connect_button("ModeSelectPopup/VBoxContainer/BtnEndless", callable_mp(this, &MainMenu::_on_endless_pressed));
    _connect_button("ModeSelectPopup/VBoxContainer/BtnBack", callable_mp(this, &MainMenu::_on_back_pressed));
        _connect_button(settings_box + String("BtnSettingBack"), callable_mp(this, &MainMenu::_on_settings_close_pressed));
    _connect_button("CreditsOverlay/Panel/Margin/VBox/BtnCreditsBack", callable_mp(this, &MainMenu::_on_credits_close_pressed));

    if (music_slider) {
        music_slider->connect("value_changed", callable_mp(this, &MainMenu::_on_music_changed));
    }
    if (sfx_slider) {
        sfx_slider->connect("value_changed", callable_mp(this, &MainMenu::_on_sfx_changed));
    }

    _ensure_audio_buses();
    _load_settings();
}

// ---------- Menu chinh ----------

void MainMenu::_on_play_pressed() {
    if (menu_box) {
        menu_box->set_visible(false);
    }
    if (mode_popup) {
        mode_popup->set_visible(true);
    }
}

void MainMenu::_on_back_pressed() {
    if (mode_popup) {
        mode_popup->set_visible(false);
    }
    if (menu_box) {
        menu_box->set_visible(true);
    }
}

void MainMenu::_on_setting_pressed() {
    if (settings_overlay) {
        settings_overlay->set_visible(true);
    }
}

void MainMenu::_on_credits_pressed() {
    if (credits_overlay) {
        credits_overlay->set_visible(true);
    }
}

void MainMenu::_on_quit_pressed() {
    get_tree()->quit();
}

void MainMenu::_on_story_pressed() {
    _go_to_scene(story_scene_path);
}

void MainMenu::_on_endless_pressed() {
    _go_to_scene(endless_scene_path);
}

void MainMenu::_go_to_scene(const String &p_path) {
    if (p_path.is_empty()) {
        UtilityFunctions::push_warning("MainMenu: chua chon scene cho che do nay trong Inspector.");
        return;
    }
    get_tree()->change_scene_to_file(p_path);
}

// ---------- Cai dat & Gioi thieu ----------

void MainMenu::_on_settings_close_pressed() {
    _save_settings();
    if (settings_overlay) {
        settings_overlay->set_visible(false);
    }
}

void MainMenu::_on_credits_close_pressed() {
    if (credits_overlay) {
        credits_overlay->set_visible(false);
    }
}

void MainMenu::_on_music_changed(double p_value) {
    if (music_value_label) {
        music_value_label->set_text(String::num_int64((int64_t)p_value) + String("%"));
    }
    _apply_volume(MUSIC_BUS, p_value);
}

void MainMenu::_on_sfx_changed(double p_value) {
    if (sfx_value_label) {
        sfx_value_label->set_text(String::num_int64((int64_t)p_value) + String("%"));
    }
    _apply_volume(SFX_BUS, p_value);
}

// ---------- Am thanh ----------

void MainMenu::_ensure_audio_buses() {
    AudioServer *server = AudioServer::get_singleton();
    const char *names[] = { MUSIC_BUS, SFX_BUS };
    for (const char *name : names) {
        if (server->get_bus_index(StringName(name)) == -1) {
            server->add_bus();
            int idx = server->get_bus_count() - 1;
            server->set_bus_name(idx, String(name));
            server->set_bus_send(idx, StringName("Master"));
        }
    }
}

void MainMenu::_apply_volume(const String &p_bus, double p_percent) {
    AudioServer *server = AudioServer::get_singleton();
    int idx = server->get_bus_index(StringName(p_bus));
    if (idx < 0) {
        return;
    }
    double linear = p_percent / 100.0;
    server->set_bus_mute(idx, linear <= 0.0);
    server->set_bus_volume_db(idx, UtilityFunctions::linear_to_db(linear > 0.0 ? linear : 0.0001));
}

void MainMenu::_load_settings() {
    double music = DEFAULT_VOLUME;
    double sfx = DEFAULT_VOLUME;

    Ref<ConfigFile> cfg;
    cfg.instantiate();
    if (cfg->load(String(SETTINGS_PATH)) == OK) {
        music = cfg->get_value("audio", "music", DEFAULT_VOLUME);
        sfx = cfg->get_value("audio", "sfx", DEFAULT_VOLUME);
    }

    if (music_slider) {
        music_slider->set_value_no_signal(music);
    }
    if (sfx_slider) {
        sfx_slider->set_value_no_signal(sfx);
    }
    _on_music_changed(music);
    _on_sfx_changed(sfx);
}

void MainMenu::_save_settings() {
    if (!music_slider || !sfx_slider) {
        return;
    }
    Ref<ConfigFile> cfg;
    cfg.instantiate();
    cfg->set_value("audio", "music", music_slider->get_value());
    cfg->set_value("audio", "sfx", sfx_slider->get_value());
    cfg->save(String(SETTINGS_PATH));
}

// ---------- Property ----------

void MainMenu::set_story_scene_path(const String &p_path) {
    story_scene_path = p_path;
}

String MainMenu::get_story_scene_path() const {
    return story_scene_path;
}

void MainMenu::set_endless_scene_path(const String &p_path) {
    endless_scene_path = p_path;
}

String MainMenu::get_endless_scene_path() const {
    return endless_scene_path;
}