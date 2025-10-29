#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QWidget>

#include "dock_widget.hpp"
#include "setup_dialog.hpp"

/*
 * This file implements the OBS module entry points.  When the plugin is
 * loaded OBS will call obs_module_load().  We create our dock widget and
 * register a new Tools menu item that opens the setup dialog.
 */

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("velutan-image-manager", "en-US")

static VelutanDockWidget *g_dock = nullptr;

/* The module's name shown in the OBS Plugin list */
const char *obs_module_name(void)
{
    return "Velutan Image Manager";
}

/* A short description displayed in the OBS plugin window */
const char *obs_module_description(void)
{
    return "Manage background and character images with filtering and tagging.";
}

/* Entry point: create UI elements and register menu items */
bool obs_module_load(void)
{
    blog(LOG_INFO, "[Velutan] Loading Velutan Image Manager plugin");

    // Create the dock widget (QWidget, not QDockWidget)
    blog(LOG_INFO, "[Velutan] Creating dock widget...");
    try {
        g_dock = new VelutanDockWidget(nullptr);
        blog(LOG_INFO, "[Velutan] Dock widget created successfully");
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Velutan] Exception creating dock widget: %s", e.what());
        return false;
    } catch (...) {
        blog(LOG_ERROR, "[Velutan] Unknown exception creating dock widget");
        return false;
    }
    
    if (!g_dock) {
        blog(LOG_ERROR, "[Velutan] Dock widget is null");
        return false;
    }
    
    // Register with OBS frontend using add_dock_by_id API
    // This API takes a QWidget and creates a QDockWidget wrapper for us
    blog(LOG_INFO, "[Velutan] Registering dock with OBS frontend...");
    bool result = obs_frontend_add_dock_by_id(
        "VelutanImageManager",           // Unique ID
        "Velutan Image Manager",          // Title that appears in Docks menu
        g_dock                             // Our QWidget
    );
    blog(LOG_INFO, "[Velutan] obs_frontend_add_dock_by_id returned: %s", result ? "true" : "false");
    
    if (!result) {
        blog(LOG_ERROR, "[Velutan] Failed to register dock with OBS");
        delete g_dock;
        g_dock = nullptr;
        return false;
    }
    
    blog(LOG_INFO, "[Velutan] Dock widget registered successfully");

    // Add an item to the Tools menu that opens the setup dialog.  The
    // obs_frontend_add_tools_menu_item API accepts a C function pointer
    // callback, so we wrap our lambda in a static function.
    obs_frontend_add_tools_menu_item(
        obs_module_text("Velutan Image Manager (Setup)"),
        [](void *) {
            // The main OBS window is used as the parent for the dialog so
            // that it stays on top of OBS.
            QWidget *parent = (QWidget *)obs_frontend_get_main_window();
            auto *dlg = new VelutanSetupDialog(parent);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            
            // Connect to dock widget to reload library when changes are made
            if (g_dock) {
                // Reload when library is saved
                QObject::connect(dlg, &VelutanSetupDialog::libraryChanged, g_dock, &VelutanDockWidget::reloadLibrary);
                
                // Also reload when dialog is closed (in case user made changes)
                QObject::connect(dlg, &QDialog::finished, g_dock, [](int) {
                    if (g_dock) {
                        g_dock->reloadLibrary();
                    }
                });
            }
            
            dlg->show();
            dlg->raise();
            dlg->activateWindow();
        },
        nullptr);

    return true;
}

/* Clean up when the module is unloaded. */
void obs_module_unload(void)
{
    blog(LOG_INFO, "[Velutan] Unloading Velutan Image Manager plugin");
    
    // Remove the dock from OBS
    if (g_dock) {
        obs_frontend_remove_dock("VelutanImageManager");
        // The dock widget will be deleted by OBS
        g_dock = nullptr;
    }
}