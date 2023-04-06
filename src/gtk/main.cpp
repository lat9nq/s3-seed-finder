#include <cassert>
#include <cstdio>
#include <fstream>
#include <memory>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include "gear_data/scan.h"
#include "gtk/main.glade.h"
#include "gtk/main.h"

MainWindow::MainWindow() {
    GtkBuilder* builder = gtk_builder_new_from_string(main_glade, -1);
    gtk_builder_connect_signals(builder, this);

    window_main = GTK_WINDOW(gtk_builder_get_object(builder, "window_main"));
    menu_item_import_binary_dump =
        GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_import_binary_dump"));
    menu_item_import_database_backup =
        GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_import_database_backup"));
    menu_item_export_json = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_export_json"));
    menu_item_quit = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_quit"));
    menu_item_set_seed = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_set_seed"));
    menu_item_cnzh = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_cnzh"));
    menu_item_eude = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_eude"));
    menu_item_euen = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_euen"));
    menu_item_eufr = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_eufr"));
    menu_item_euit = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_euit"));
    menu_item_eunl = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_eunl"));
    menu_item_euru = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_euru"));
    menu_item_jpja = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_jpja"));
    menu_item_krko = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_krko"));
    menu_item_usen = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_usen"));
    menu_item_uses = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_uses"));
    menu_item_usfr = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_usfr"));
    menu_item_custom_binary_dump =
        GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_custom_binary_dump"));
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
    entry_offset_headgear = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_headgear"));
    entry_offset_clothes = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_clothes"));
    entry_offset_shoes = GTK_ENTRY(gtk_builder_get_object(builder, "entry_offset_shoes"));

    g_object_ref(window_main);
    g_object_unref(builder);
}

MainWindow::~MainWindow() {
    delete[] dump_data;

    delete[] scan_info.headgear;
    delete[] scan_info.clothes;
    delete[] scan_info.shoes;
}

void MainWindow::UpdateUi() {
    GearItem* headgear = scan_info.headgear;
    GearItem* clothes = scan_info.clothes;
    GearItem* shoes = scan_info.shoes;

    GtkListBoxRow* current;
    while ((current = gtk_list_box_get_row_at_index(list_box_gear, 0)) != nullptr) {
        gtk_widget_destroy(GTK_WIDGET(current));
    }

    for (int i = 0; i < scan_info.headgear_count; i++) {
        char buf[255];
        std::snprintf(buf, 255, "%d", headgear[i].id);
        GtkWidget* gear_label = gtk_label_new(buf);
        gtk_widget_set_halign(gear_label, GTK_ALIGN_START);
        gtk_list_box_insert(list_box_gear, gear_label, -1);
        gtk_widget_show(gear_label);
    }

    for (int i = 0; i < scan_info.clothes_count; i++) {
        char buf[255];
        std::snprintf(buf, 255, "%d", clothes[i].id);
        GtkWidget* gear_label = gtk_label_new(buf);
        gtk_widget_set_halign(gear_label, GTK_ALIGN_START);
        gtk_list_box_insert(list_box_gear, gear_label, -1);
        gtk_widget_show(gear_label);
    }

    for (int i = 0; i < scan_info.shoes_count; i++) {
        char buf[255];
        std::snprintf(buf, 255, "%d", shoes[i].id);
        GtkWidget* gear_label = gtk_label_new(buf);
        gtk_widget_set_halign(gear_label, GTK_ALIGN_START);
        gtk_list_box_insert(list_box_gear, gear_label, -1);
        gtk_widget_show(gear_label);
    }
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
    if (search_seed == 0) {
        SetSearchSeed();
    }
    if (search_seed == 0) {
        // return if cancelled
        return;
    }

    GtkFileChooserNative* native = gtk_file_chooser_native_new(
        "Import Binary Dump", window_main, GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");

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

    scan_data(dump_data, dump_size, json_data, search_seed, scan_info);

    if (scan_info.headgear_count == 0 && scan_info.clothes_count == 0 &&
        scan_info.shoes_count == 0) {
        std::fprintf(stderr, "No gear seeds were found\n");
    } else {
        std::fprintf(
            stderr,
            "Found seeds:\nHeadgear: %d @ 0x%08x\nClothes: %d @ 0x%08x\nShoes: %d @ 0x%08x\n",
            scan_info.headgear_count, scan_info.headgear_address, scan_info.clothes_count,
            scan_info.clothes_address, scan_info.shoes_count, scan_info.shoes_address);
    }

    UpdateUi();
}

void on_window_main_destroy(GtkWindow* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->window_main);

    gtk_main_quit();
}

void on_menu_item_quit_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_quit);

    gtk_main_quit();
}

void on_menu_item_import_binary_dump_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_import_binary_dump);

    main_window->ImportBinaryDump();
}

void on_menu_item_custom_binary_dump_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_custom_binary_dump);

    main_window->ImportBinaryDump();
}

void on_menu_item_set_seed_activate(GtkMenuItem* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->menu_item_set_seed);

    main_window->SetSearchSeed();
}

void on_button_set_search_seed_clicked(GtkButton* self, gpointer user_data) {
    MainWindow* main_window = static_cast<MainWindow*>(user_data);
    assert(self == main_window->button_set_search_seed);

    main_window->SetSearchSeed();
}

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    std::unique_ptr<MainWindow> main_window = std::make_unique<MainWindow>();
    gtk_widget_show(GTK_WIDGET(main_window->window_main));

    gtk_main();

    return 0;
}
