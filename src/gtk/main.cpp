#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include "gear_data/leanny.h"
#include "gear_data/scan.h"
#include "gtk/config.h"
#include "gtk/main.glade.h"
#include "gtk/main.h"

MainWindow::MainWindow(std::unique_ptr<Config> config_)
    : config{std::move(config_)}, leanny_db{config->localization} {
    GtkBuilder* builder = gtk_builder_new_from_string(main_glade, -1);
    gtk_builder_connect_signals(builder, this);

    window_main = GTK_WINDOW(gtk_builder_get_object(builder, "window_main"));
    radio_menu_item_cnzh =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_cnzh"));
    radio_menu_item_eude =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_eude"));
    radio_menu_item_euen =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_euen"));
    radio_menu_item_eufr =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_eufr"));
    radio_menu_item_euit =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_euit"));
    radio_menu_item_eunl =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_eunl"));
    radio_menu_item_euru =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_euru"));
    radio_menu_item_jpja =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_jpja"));
    radio_menu_item_krko =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_krko"));
    radio_menu_item_usen =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_usen"));
    radio_menu_item_uses =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_uses"));
    radio_menu_item_usfr =
        GTK_RADIO_MENU_ITEM(gtk_builder_get_object(builder, "radio_menu_item_usfr"));
    menu_item_custom_binary_dump =
        GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_custom_binary_dump"));
    menu_item_custom_import_database_backup =
        GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_custom_import_database_backup"));
    menu_item_about = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_about"));
    combo_box_category = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_box_category"));
    list_box_gear = GTK_LIST_BOX(gtk_builder_get_object(builder, "list_box_gear"));
    entry_name = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name"));
    entry_brand = GTK_ENTRY(gtk_builder_get_object(builder, "entry_brand"));
    entry_id_number = GTK_ENTRY(gtk_builder_get_object(builder, "entry_id_number"));
    entry_id_code = GTK_ENTRY(gtk_builder_get_object(builder, "entry_id_code"));
    entry_main = GTK_ENTRY(gtk_builder_get_object(builder, "entry_main"));
    entry_sub_a = GTK_ENTRY(gtk_builder_get_object(builder, "entry_sub_a"));
    entry_sub_b = GTK_ENTRY(gtk_builder_get_object(builder, "entry_sub_b"));
    entry_sub_c = GTK_ENTRY(gtk_builder_get_object(builder, "entry_sub_c"));
    entry_seed = GTK_ENTRY(gtk_builder_get_object(builder, "entry_seed"));
    button_set_search_seed = GTK_BUTTON(gtk_builder_get_object(builder, "button_set_search_seed"));
    button_export = GTK_BUTTON(gtk_builder_get_object(builder, "button_export"));
    entry_offset_headgear = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_headgear"));
    entry_offset_clothes = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_clothes"));
    entry_offset_shoes = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_shoes"));
    statusbar_main = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar_main"));

    menu_localization[radio_menu_item_cnzh] = SplLocalization::CNzh;
    menu_localization[radio_menu_item_eude] = SplLocalization::EUde;
    menu_localization[radio_menu_item_euen] = SplLocalization::EUen;
    menu_localization[radio_menu_item_eufr] = SplLocalization::EUfr;
    menu_localization[radio_menu_item_euit] = SplLocalization::EUit;
    menu_localization[radio_menu_item_eunl] = SplLocalization::EUnl;
    menu_localization[radio_menu_item_euru] = SplLocalization::EUru;
    menu_localization[radio_menu_item_jpja] = SplLocalization::JPja;
    menu_localization[radio_menu_item_krko] = SplLocalization::KRko;
    menu_localization[radio_menu_item_usen] = SplLocalization::USen;
    menu_localization[radio_menu_item_uses] = SplLocalization::USes;
    menu_localization[radio_menu_item_usfr] = SplLocalization::USfr;

    for (auto& x : menu_localization) {
        if (x.second == config->localization) {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(x.first), true);
            return;
        }
    }

    g_object_ref(window_main);
    g_object_unref(builder);

    UpdateUi();
}

MainWindow::~MainWindow() {
    delete[] dump_data;

    delete[] scan_info.headgear;
    delete[] scan_info.clothes;
    delete[] scan_info.shoes;
}

void MainWindow::UpdateUi() {
    gtk_widget_set_sensitive(GTK_WIDGET(button_export), data_imported);

    GtkListBoxRow* current;
    while ((current = gtk_list_box_get_row_at_index(list_box_gear, 0)) != nullptr) {
        gtk_widget_destroy(GTK_WIDGET(current));
    }
    row_id.clear();

    std::function<void(GearItem*, int, Category)> populate_list =
        [&](GearItem* gear_list, int gear_count, Category gear_type) {
            char buf[255];
            for (int i = 0; i < gear_count; i++) {
                const u_int32_t id = gear_list[i].id;
                const std::string key = leanny_db.GetCode(id, gear_type);
                const std::string name = leanny_db.LocalizedGearName(key);
                std::strncpy(buf, name.c_str(), 255);
                GtkWidget* gear_label = gtk_label_new(buf);
                gtk_widget_set_halign(gear_label, GTK_ALIGN_START);
                gtk_list_box_insert(list_box_gear, gear_label, -1);
                gtk_widget_show(gear_label);

                row_id.push_back(std::make_tuple(id, gear_type, i));
            }
        };
    const ComboBoxCategorySelections selection =
        static_cast<ComboBoxCategorySelections>(gtk_combo_box_get_active(combo_box_category));
    switch (selection) {
    case ComboBoxCategorySelections::All: {
        populate_list(scan_info.headgear, scan_info.headgear_count, Category::Headgear);
        populate_list(scan_info.clothes, scan_info.clothes_count, Category::Clothes);
        populate_list(scan_info.shoes, scan_info.shoes_count, Category::Shoes);
    } break;
    case ComboBoxCategorySelections::Headgear: {
        populate_list(scan_info.headgear, scan_info.headgear_count, Category::Headgear);
    } break;
    case ComboBoxCategorySelections::Clothes: {
        populate_list(scan_info.clothes, scan_info.clothes_count, Category::Clothes);
    } break;
    case ComboBoxCategorySelections::Shoes: {
        populate_list(scan_info.shoes, scan_info.shoes_count, Category::Shoes);
    } break;
    }

    char headgear_address[11];
    char clothes_address[11];
    char shoes_address[11];

    std::snprintf(headgear_address, 11, "0x%08x", scan_info.headgear_address);
    std::snprintf(clothes_address, 11, "0x%08x", scan_info.clothes_address);
    std::snprintf(shoes_address, 11, "0x%08x", scan_info.shoes_address);

    gtk_entry_set_text(entry_offset_headgear, headgear_address);
    gtk_entry_set_text(entry_offset_clothes, clothes_address);
    gtk_entry_set_text(entry_offset_shoes, shoes_address);
}

void MainWindow::UpdateGearView(GtkListBoxRow* row) {
    char id_str[64];
    char seed_str[64];
    const int index = gtk_list_box_row_get_index(row);
    if (index < 0) {
        return;
    }
    const u_int32_t id = std::get<0>(row_id[index]);
    const Category category = std::get<1>(row_id[index]);
    const u_int32_t gear_index = std::get<2>(row_id[index]);
    const std::string key = leanny_db.GetCode(id, category);
    const std::string name = leanny_db.LocalizedGearName(key);
    const std::string brand = leanny_db.GetBrand(id, category);
    const std::string brand_loc = leanny_db.LocalizedBrand(brand);

    const auto& gear_info = [&]() {
        GearItem* gear_list{};
        switch (category) {
        case Category::Headgear: {
            gear_list = scan_info.headgear;
        } break;
        case Category::Clothes: {
            gear_list = scan_info.clothes;
        } break;
        case Category::Shoes: {
            gear_list = scan_info.shoes;
        } break;
        default:
            break;
        }
        return gear_list[gear_index];
    }();

    std::snprintf(id_str, 64, "%d", id);
    std::snprintf(seed_str, 64, "0x%08x", gear_info.seed);

    const char* main_internal = ability_internal_name[static_cast<u_int32_t>(gear_info.main)];
    const char* sub_a_internal = ability_internal_name[static_cast<u_int32_t>(gear_info.subs[0])];
    const char* sub_b_internal = ability_internal_name[static_cast<u_int32_t>(gear_info.subs[1])];
    const char* sub_c_internal = ability_internal_name[static_cast<u_int32_t>(gear_info.subs[2])];

    gtk_entry_set_text(entry_name, name.c_str());
    gtk_entry_set_text(entry_id_number, id_str);
    gtk_entry_set_text(entry_id_code, key.c_str());
    gtk_entry_set_text(entry_brand, brand_loc.c_str());
    gtk_entry_set_text(entry_main, leanny_db.LocalizedAbility(main_internal).c_str());
    gtk_entry_set_text(entry_sub_a, leanny_db.LocalizedAbility(sub_a_internal).c_str());
    gtk_entry_set_text(entry_sub_b, leanny_db.LocalizedAbility(sub_b_internal).c_str());
    gtk_entry_set_text(entry_sub_c, leanny_db.LocalizedAbility(sub_c_internal).c_str());
    gtk_entry_set_text(entry_seed, seed_str);
}

void MainWindow::SetSearchSeed() {
    GtkDialog* dialog_search_seed = GTK_DIALOG(gtk_dialog_new_with_buttons(
        "Set Search Seed", window_main, GTK_DIALOG_DESTROY_WITH_PARENT, "_OK", GTK_RESPONSE_ACCEPT,
        "_Cancel", GTK_RESPONSE_REJECT, nullptr));
    GtkWidget* content_area = gtk_dialog_get_content_area(dialog_search_seed);
    GtkLabel* label_text =
        GTK_LABEL(gtk_label_new("Enter a known seed to search for gear data with:"));
    GtkEntry* entry_search_seed = GTK_ENTRY(gtk_entry_new());

    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(label_text));
    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(entry_search_seed));
    gtk_box_set_spacing(GTK_BOX(content_area), 6);
    gtk_widget_set_margin_end(content_area, 6);
    gtk_widget_set_margin_top(content_area, 6);
    gtk_widget_set_margin_start(content_area, 6);
    gtk_widget_set_margin_bottom(content_area, 6);
    gtk_widget_show_all(GTK_WIDGET(dialog_search_seed));

    const int response = gtk_dialog_run(dialog_search_seed);
    const std::string search_seed_txt{gtk_entry_get_text(entry_search_seed)};

    gtk_widget_destroy(GTK_WIDGET(dialog_search_seed));

    if (response != GTK_RESPONSE_ACCEPT) {
        return;
    }

    search_seed = static_cast<u_int32_t>(std::strtoul(search_seed_txt.c_str(), nullptr, 16));
}

void MainWindow::ImportBinaryDump() {
    // Prompt for a search seed if there is no seed currently available
    if (search_seed == 0 && config->Seeds().size() == 0) {
        SetSearchSeed();
        if (search_seed == 0) {
            // return if cancelled
            return;
        }
    }

    GtkFileChooserNative* native = gtk_file_chooser_native_new(
        "Import Binary Dump", window_main, GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");
    if (!config->last_open_file.empty()) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(native), config->last_open_file.c_str());
    }

    const int result = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    const std::string file_path = [&]() {
        if (result == GTK_RESPONSE_ACCEPT) {
            char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));
            std::string native_selection = filename;
            g_free(filename);
            return native_selection;
        }
        return std::string{};
    }();
    g_object_unref(native);

    if (result != GTK_RESPONSE_ACCEPT) {
        return;
    }

    config->last_open_file = file_path;

    std::fstream dump_stream(file_path, std::ios_base::in | std::ios_base::binary);
    if (!dump_stream.is_open()) {
        GtkDialog* err_diag = GTK_DIALOG(
            gtk_dialog_new_with_buttons("Error", window_main, GTK_DIALOG_DESTROY_WITH_PARENT, "_OK",
                                        GTK_RESPONSE_NONE, nullptr));
        GtkWidget* content_area = gtk_dialog_get_content_area(err_diag);
        GtkLabel* err_label = GTK_LABEL(gtk_label_new("Could not open the file"));
        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(err_label));
        gtk_widget_show_all(GTK_WIDGET(err_diag));

        gtk_dialog_run(err_diag);

        gtk_widget_destroy(GTK_WIDGET(err_diag));

        return;
    }

    struct stat file_stat {};
    stat(file_path.c_str(), &file_stat);
    dump_size = file_stat.st_size;

    if (dump_data != nullptr) {
        delete[] dump_data;
    }

    dump_data = new u_int8_t[dump_size]();

    dump_stream.read(reinterpret_cast<char*>(dump_data), dump_size);
    dump_stream.close();

    if (search_seed != 0) {
        ScanData(dump_data, dump_size, json_data, search_seed, scan_info);
    } else {
        ScanData(dump_data, dump_size, json_data, config->Seeds(), scan_info);
    }

    char message[255];
    const int sum = scan_info.headgear_count + scan_info.clothes_count + scan_info.shoes_count;
    if (sum == 0) {
        std::snprintf(message, 255, "No results");
    } else {
        std::snprintf(message, 255, "Found gear: %d headgear, %d clothes, %d shoes (%d total)",
                      scan_info.headgear_count, scan_info.clothes_count, scan_info.shoes_count,
                      sum);
    }

    const int context_id = gtk_statusbar_get_context_id(statusbar_main, "scan result");
    gtk_statusbar_push(statusbar_main, context_id, message);

    data_imported = true;

    // Register seeds to config
    std::vector<u_int32_t> seeds;
    for (int i = 0; i < scan_info.headgear_count; i++) {
        seeds.push_back(scan_info.headgear[i].seed);
    }
    for (int i = 0; i < scan_info.clothes_count; i++) {
        seeds.push_back(scan_info.clothes[i].seed);
    }
    for (int i = 0; i < scan_info.shoes_count; i++) {
        seeds.push_back(scan_info.shoes[i].seed);
    }
    config->LoadSeeds(seeds);

    UpdateUi();
}

void MainWindow::ImportDatabaseBackup() {
    GtkFileFilter* json_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(json_filter, "JSON files (*.json)");
    gtk_file_filter_add_mime_type(json_filter, "text/json");
    gtk_file_filter_add_pattern(json_filter, "*.json");

    GtkFileChooserNative* native =
        gtk_file_chooser_native_new("Import Lean Database Backup", window_main,
                                    GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");
    if (!config->last_backup.empty()) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(native), config->last_backup.c_str());
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), json_filter);

    const int result = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    const std::filesystem::path file_path = [&]() {
        if (result == GTK_RESPONSE_ACCEPT) {
            char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));
            const std::filesystem::path native_selection = filename;
            g_free(filename);
            return native_selection;
        }
        return std::filesystem::path{};
    }();
    g_object_unref(native);

    if (result != GTK_RESPONSE_ACCEPT) {
        return;
    }

    std::fstream backup_stream(file_path, std::ios_base::in);
    if (!backup_stream.is_open()) {
        GtkDialog* err_diag = GTK_DIALOG(
            gtk_dialog_new_with_buttons("Error", window_main, GTK_DIALOG_DESTROY_WITH_PARENT, "_OK",
                                        GTK_RESPONSE_NONE, nullptr));
        GtkWidget* content_area = gtk_dialog_get_content_area(err_diag);
        GtkLabel* err_label = GTK_LABEL(gtk_label_new("Could not open the file"));
        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(err_label));
        gtk_widget_show_all(GTK_WIDGET(err_diag));

        gtk_dialog_run(err_diag);

        gtk_widget_destroy(GTK_WIDGET(err_diag));

        return;
    }

    nlohmann::json backup_data = nlohmann::json::parse(backup_stream);
    backup_stream.close();

    int count{0};
    for (auto& [key, value] : backup_data.items()) {
        if (key == "GearDB") {
            continue;
        }
        json_data[key] = backup_data[key];
        count++;
    }

    config->last_backup = file_path;

    std::stringstream message;
    message << "Imported database backup " << file_path.filename() << " (" << count
            << " fields entered)";

    const int context_id = gtk_statusbar_get_context_id(statusbar_main, "export result");
    gtk_statusbar_push(statusbar_main, context_id, message.str().c_str());
}

void MainWindow::Export() {
    GtkFileFilter* json_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(json_filter, "JSON files (*.json)");
    gtk_file_filter_add_mime_type(json_filter, "text/json");
    gtk_file_filter_add_pattern(json_filter, "*.json");

    GtkFileChooserNative* native = gtk_file_chooser_native_new(
        "Export Binary Dump", window_main, GTK_FILE_CHOOSER_ACTION_SAVE, "_Export", "_Cancel");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), json_filter);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(native), true);
    if (!config->last_export.empty()) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(native), config->last_export.c_str());
    }

    const int result = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    const std::filesystem::path file_path = [&]() {
        if (result == GTK_RESPONSE_ACCEPT) {
            char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));
            const std::filesystem::path native_selection = filename;
            g_free(filename);
            return native_selection;
        }
        return std::filesystem::path{};
    }();
    g_object_unref(native);

    if (result != GTK_RESPONSE_ACCEPT) {
        return;
    }

    std::fstream export_file(file_path, std::ios_base::out | std::ios_base::trunc);
    if (!export_file.is_open()) {
        GtkDialog* err_diag = GTK_DIALOG(
            gtk_dialog_new_with_buttons("Error", window_main, GTK_DIALOG_DESTROY_WITH_PARENT, "_OK",
                                        GTK_RESPONSE_NONE, nullptr));
        GtkWidget* content_area = gtk_dialog_get_content_area(err_diag);
        GtkLabel* err_label = GTK_LABEL(gtk_label_new("Could not open the file for writing"));
        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(err_label));
        gtk_widget_show_all(GTK_WIDGET(err_diag));

        gtk_dialog_run(err_diag);

        gtk_widget_destroy(GTK_WIDGET(err_diag));

        return;
    }

    export_file << json_data.dump();

    export_file.close();

    config->last_export = file_path;

    std::stringstream message_stream;
    message_stream << "Exported " << file_path.filename();

    const int context_id = gtk_statusbar_get_context_id(statusbar_main, "export result");
    gtk_statusbar_push(statusbar_main, context_id, message_stream.str().c_str());
}

void MainWindow::ChangeLocalization(SplLocalization localization) {
    config->localization = localization;
    leanny_db.ChangeLocalization(localization);

    GtkListBoxRow* selected_row = gtk_list_box_get_selected_row(list_box_gear);
    if (selected_row == nullptr) {
        UpdateUi();
        return;
    }

    const int index = gtk_list_box_row_get_index(selected_row);

    UpdateUi();

    selected_row = gtk_list_box_get_row_at_index(list_box_gear, index);
    gtk_list_box_select_row(list_box_gear, selected_row);
}

void on_list_box_gear_row_selected(GtkListBox* self, GtkListBoxRow* row, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->list_box_gear);

    if (row == nullptr) {
        return;
    }

    main_window->UpdateGearView(row);
}

void on_window_main_destroy(GtkWindow* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->window_main);

    gtk_main_quit();
}

void on_menu_item_custom_binary_dump_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_custom_binary_dump);

    main_window->ImportBinaryDump();
}

void on_menu_item_custom_import_database_backup_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_custom_import_database_backup);

    main_window->ImportDatabaseBackup();
}

void on_button_set_search_seed_clicked(GtkButton* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->button_set_search_seed);

    main_window->SetSearchSeed();
}

void on_combo_box_category_changed(GtkComboBox* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->combo_box_category);

    main_window->UpdateUi();
}

void on_button_export_clicked(GtkButton* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->button_export);

    main_window->Export();
}

void on_radio_menu_item_localization_toggled(GtkRadioMenuItem* self, gpointer user_data) {
    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(self))) {
        return;
    }

    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    const SplLocalization selected = main_window->menu_localization.at(self);

    main_window->ChangeLocalization(selected);
}

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    // Get default config path
    const std::filesystem::path config_path = [&]() {
#ifdef __linux__
        std::filesystem::path config_path_;
        const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
        const char* home = std::getenv("HOME");

        if (xdg_config_home == nullptr) {
            config_path_ = home;
            config_path_ /= ".config/s3-seed-finder/config.json";
        } else {
            config_path_ = xdg_config_home;
            config_path_ /= "s3-seed-finder/config.json";
        }
        return config_path_;
#else // Generic
        return "config.json";
#endif
    }();

    std::unique_ptr<Config> config = std::make_unique<Config>(config_path);

    std::unique_ptr<MainWindow> main_window = std::make_unique<MainWindow>(std::move(config));
    gtk_widget_show(GTK_WIDGET(main_window->window_main));

    gtk_main();

    return 0;
}
