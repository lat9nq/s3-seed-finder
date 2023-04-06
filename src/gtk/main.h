#pragma once

#include <cstdlib>
#include <gtk/gtk.h>
#include <nlohmann/json.hpp>
#include "gear_data/scan.h"

class MainWindow {
public:
    explicit MainWindow();
    ~MainWindow();

    void UpdateUi();
    void SetSearchSeed();
    void ImportBinaryDump();

    GtkWindow* window_main;
    GtkMenuItem* menu_item_import_binary_dump;
    GtkMenuItem* menu_item_import_database_backup;
    GtkMenuItem* menu_item_export_json;
    GtkMenuItem* menu_item_quit;
    GtkMenuItem* menu_item_set_seed;
    GtkMenuItem* menu_item_cnzh;
    GtkMenuItem* menu_item_eude;
    GtkMenuItem* menu_item_euen;
    GtkMenuItem* menu_item_eufr;
    GtkMenuItem* menu_item_euit;
    GtkMenuItem* menu_item_eunl;
    GtkMenuItem* menu_item_euru;
    GtkMenuItem* menu_item_jpja;
    GtkMenuItem* menu_item_krko;
    GtkMenuItem* menu_item_usen;
    GtkMenuItem* menu_item_uses;
    GtkMenuItem* menu_item_usfr;
    GtkMenuItem* menu_item_about;
    GtkMenuItem* menu_item_custom_binary_dump;
    GtkComboBox* combo_box_category;
    GtkListBox* list_box_gear;
    GtkEntry* entry_name;
    GtkEntry* entry_brand;
    GtkEntry* entry_id_number;
    GtkEntry* entry_id_code;
    GtkEntry* entry_main;
    GtkEntry* entry_sub_a;
    GtkEntry* entry_sub_b;
    GtkEntry* entry_sub_c;
    GtkEntry* entry_seed;
    GtkButton* button_set_search_seed;
    GtkEntry* entry_offset_headgear;
    GtkEntry* entry_offset_clothes;
    GtkEntry* entry_offset_shoes;

private:
    u_int32_t search_seed{0};

    u_int8_t* dump_data{nullptr};
    std::size_t dump_size{0};

    nlohmann::json json_data{};

    ScanInfo scan_info{};
};

extern "C" G_MODULE_EXPORT void on_window_main_destroy(GtkWindow* self, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_quit_activate(GtkMenuItem* self, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_import_binary_dump_activate(GtkMenuItem* self,
                                                                         gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_set_seed_activate(GtkMenuItem* self,
                                                               gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_custom_binary_dump_activate(GtkMenuItem* self,
                                                                         gpointer user_data);
extern "C" G_MODULE_EXPORT void on_button_set_search_seed_clicked(GtkButton* self,
                                                                  gpointer user_data);
