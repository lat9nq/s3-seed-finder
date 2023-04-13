#pragma once

#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>
#include <gtk/gtk.h>
#include <nlohmann/json.hpp>
#include "gear_data/leanny.h"
#include "gear_data/scan.h"

class Config;

enum class ComboBoxCategorySelections { All, Headgear, Clothes, Shoes };

class MainWindow {
public:
    explicit MainWindow(std::unique_ptr<Config> config_);
    ~MainWindow();

    void UpdateUi();
    void SetSearchSeed();
    void ImportBinaryDump();
    void ImportDatabaseBackup();
    void UpdateGearView(GtkListBoxRow* row);
    void Export();

    void ChangeLocalization(SplLocalization localization);

    GtkWindow* window_main;
    GtkRadioMenuItem* radio_menu_item_cnzh;
    GtkRadioMenuItem* radio_menu_item_eude;
    GtkRadioMenuItem* radio_menu_item_euen;
    GtkRadioMenuItem* radio_menu_item_eufr;
    GtkRadioMenuItem* radio_menu_item_euit;
    GtkRadioMenuItem* radio_menu_item_eunl;
    GtkRadioMenuItem* radio_menu_item_euru;
    GtkRadioMenuItem* radio_menu_item_jpja;
    GtkRadioMenuItem* radio_menu_item_krko;
    GtkRadioMenuItem* radio_menu_item_usen;
    GtkRadioMenuItem* radio_menu_item_uses;
    GtkRadioMenuItem* radio_menu_item_usfr;
    GtkMenuItem* menu_item_about;
    GtkMenuItem* menu_item_custom_binary_dump;
    GtkMenuItem* menu_item_custom_import_database_backup;
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
    GtkButton* button_export;
    GtkEntry* entry_offset_headgear;
    GtkEntry* entry_offset_clothes;
    GtkEntry* entry_offset_shoes;
    GtkStatusbar* statusbar_main;

    std::map<GtkRadioMenuItem*, SplLocalization> menu_localization;

private:
    u_int32_t search_seed{0};
    std::vector<std::tuple<u_int32_t, Category, u_int32_t>> row_id;

    u_int8_t* dump_data{nullptr};
    std::size_t dump_size{0};
    nlohmann::json json_data{};
    ScanInfo scan_info{};

    std::unique_ptr<Config> config;
    LeannyDB leanny_db;

    bool data_imported{false};
};

extern "C" G_MODULE_EXPORT void on_button_export_clicked(GtkButton* self, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_button_set_search_seed_clicked(GtkButton* self,
                                                                  gpointer user_data);
extern "C" G_MODULE_EXPORT void on_combo_box_category_changed(GtkComboBox* self,
                                                              gpointer user_data);
extern "C" G_MODULE_EXPORT void on_list_box_gear_row_selected(GtkListBox* self, GtkListBoxRow* row,
                                                              gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_custom_binary_dump_activate(GtkMenuItem* self,
                                                                         gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_custom_import_database_backup_activate(
    GtkMenuItem* self, gpointer user_data);
extern "C" G_MODULE_EXPORT void on_menu_item_export_json_activate(GtkMenuItem* self,
                                                                  gpointer user_data);
extern "C" G_MODULE_EXPORT void on_radio_menu_item_localization_toggled(GtkRadioMenuItem* self,
                                                                        gpointer user_data);
extern "C" G_MODULE_EXPORT void on_window_main_destroy(GtkWindow* self, gpointer user_data);
