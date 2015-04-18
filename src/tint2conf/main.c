/**************************************************************************
*
* Tint2conf
*
* Copyright (C) 2009 Thierry lorthiois (lorthiois@bbsoft.fr) from Omega distribution
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**************************************************************************/

#include <time.h>
#ifdef HAVE_VERSION_H
#include "version.h"
#endif

#include "main.h"
#include "common.h"
#include "theme_view.h"
#include "properties.h"
#include "properties_rw.h"

// Default path to the tint2 config file
char *g_path_config = NULL;
// Default path to the tint2 config directory
char *g_path_dir = NULL;
char *g_default_theme = NULL;
int g_width, g_height;

GtkWidget *g_window;

static GtkUIManager *globalUIManager = NULL;

static void menuAddWidget(GtkUIManager *, GtkWidget *, GtkContainer *);

// action on menus
static void menuAdd();
static void menuSaveAs();
static void menuDelete();
static void menuProperties();
static void menuQuit();
static void menuRefresh();
static void menuRefreshAll();
static void menuAbout();

static gboolean view_onPopupMenu(GtkWidget *treeview, gpointer userdata);
static gboolean view_onButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata);
static void windowSizeAllocated();
static void viewRowActivated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);


// theme files
static void selectTheme(const gchar *name);
static gboolean searchTheme(const gchar *name_theme, GtkTreeModel *model, GtkTreeIter *iter);
static void load_theme();


// define menubar, toolbar and popup
static const char *global_ui =
	"<ui>"
	"  <menubar name='MenuBar'>"
	"    <menu action='ThemeMenu'>"
	"      <menuitem action='ThemeAdd'/>"
	"      <menuitem action='ThemeSaveAs'/>"
	"      <separator/>"
	"      <menuitem action='ThemeDelete'/>"
	"      <separator/>"
	"      <menuitem action='ThemeProperties'/>"
	"      <separator/>"
	"      <menuitem action='ThemeQuit'/>"
	"    </menu>"
	"    <menu action='EditMenu'>"
	"      <menuitem action='EditRefresh'/>"
	"      <menuitem action='EditRefreshAll'/>"
	"      <separator/>"
	"    </menu>"
	"    <menu action='HelpMenu'>"
	"      <menuitem action='HelpAbout'/>"
	"    </menu>"
	"  </menubar>"
	"  <toolbar  name='ToolBar'>"
	"    <toolitem action='ThemeProperties'/>"
	"    <toolitem action='ViewApply'/>"
	"  </toolbar>"
	"  <popup  name='ThemePopup'>"
	"    <menuitem action='ThemeProperties'/>"
	"    <menuitem action='EditRefresh'/>"
	"    <menuitem action='ViewApply'/>"
	"    <separator/>"
	"    <menuitem action='ThemeDelete'/>"
	"  </popup>"
	"</ui>";


// define menubar and toolbar action
static GtkActionEntry entries[] = {
	{"ThemeMenu", NULL, _("Theme"), NULL, NULL, NULL},
	{"ThemeAdd", GTK_STOCK_ADD, _("_Add..."), "<Control>N", _("Add theme"), G_CALLBACK(menuAdd)},
	{"ThemeSaveAs", GTK_STOCK_SAVE_AS, _("_Save as..."), NULL, _("Save theme as"), G_CALLBACK(menuSaveAs)},
	{"ThemeDelete", GTK_STOCK_DELETE, _("_Delete"), NULL, _("Delete theme"), G_CALLBACK(menuDelete)},
	{"ThemeProperties", GTK_STOCK_PROPERTIES, _("_Properties..."), NULL, _("Show properties"), G_CALLBACK(menuProperties)},
	{"ThemeQuit", GTK_STOCK_QUIT, _("_Quit"), "<control>Q", _("Quit"), G_CALLBACK(menuQuit)},
	{"EditMenu", NULL, "Edit", NULL, NULL, NULL},
	{"EditRefresh", GTK_STOCK_REFRESH, _("Refresh"), NULL, _("Refresh"), G_CALLBACK(menuRefresh)},
	{"EditRefreshAll", GTK_STOCK_REFRESH, _("Refresh all"), NULL, _("Refresh all"), G_CALLBACK(menuRefreshAll)},
//	{"EditPreferences", GTK_STOCK_PREFERENCES, "Preferences", NULL, "Preferences", G_CALLBACK(menuPreferences)},
	{"HelpMenu", NULL, _("Help"), NULL, NULL, NULL},
	{"HelpAbout", GTK_STOCK_ABOUT, _("_About"), "<Control>A", _("About"), G_CALLBACK(menuAbout)}
};


int main(int argc, char **argv)
{
	GtkWidget *vBox = NULL, *scrollbar = NULL;
	GtkActionGroup *actionGroup;

	gtk_init(&argc, &argv);
	g_thread_init((NULL));

	g_path_dir = g_build_filename(g_get_user_config_dir(), "tint2", NULL);
	if (!g_file_test(g_path_dir, G_FILE_TEST_IS_DIR))
		g_mkdir(g_path_dir, 0777);

	g_path_config = g_build_filename(g_get_user_config_dir(), "tint2", "tint2rc", NULL);

	g_set_application_name(_("tint2conf"));
	gtk_window_set_default_icon_name("taskbar");
	
	// config file use '.' as decimal separator
	setlocale(LC_NUMERIC, "POSIX");

	// define main layout : container, menubar, toolbar
	g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(g_window), _("Panel theming"));
	g_width = 800, g_height = 600;
	gtk_window_resize(GTK_WINDOW(g_window), g_width, g_height);
	g_signal_connect(G_OBJECT(g_window), "destroy", G_CALLBACK(menuQuit), NULL);
	g_signal_connect(g_window, "size-allocate", G_CALLBACK(windowSizeAllocated), NULL);
	vBox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(g_window), vBox);

	actionGroup = gtk_action_group_new("menuActionGroup");
	gtk_action_group_add_actions(actionGroup, entries, G_N_ELEMENTS(entries), NULL);
	globalUIManager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(globalUIManager, actionGroup, 0);
	gtk_ui_manager_add_ui_from_string(globalUIManager, global_ui, -1, (NULL));
	g_signal_connect(globalUIManager, "add_widget", G_CALLBACK(menuAddWidget), vBox);
	gtk_ui_manager_ensure_update(globalUIManager);
	scrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vBox), scrollbar, TRUE, TRUE, 0);

	// define theme view
	g_theme_view = create_view();
	gtk_container_add(GTK_CONTAINER(scrollbar), g_theme_view);
	gtk_widget_show(g_theme_view);
	g_signal_connect(g_theme_view, "button-press-event", (GCallback)view_onButtonPressed, NULL);
	g_signal_connect(g_theme_view, "popup-menu", (GCallback)view_onPopupMenu, NULL);
	g_signal_connect(g_theme_view, "row-activated", G_CALLBACK(viewRowActivated), NULL);

	// load themes
	load_theme(g_theme_view);

	gtk_widget_show_all(g_window);
	gtk_main();
	return 0;
}


static void menuAddWidget(GtkUIManager * p_uiManager, GtkWidget * p_widget, GtkContainer * p_box)
{
	gtk_box_pack_start(GTK_BOX(p_box), p_widget, FALSE, FALSE, 0);
	gtk_widget_show(p_widget);
}


static void menuAbout()
{
	const char *authors[] = { "Thierry Lorthiois <lorthiois@bbsoft.fr>", "Andreas Fink <andreas.fink85@googlemail.com>", "Christian Ruppert <Spooky85@gmail.com> (Build system)", "Euan Freeman <euan04@gmail.com> (tintwizard)\n  See http://code.google.com/p/tintwizard/", (NULL)};

	gtk_show_about_dialog(GTK_WINDOW(g_window), "name", g_get_application_name( ),
								"comments", _("Theming tool for tint2 panel"),
								"version", VERSION_STRING,
								"copyright", _("Copyright 2009 tint2 team\nTint2 License GNU GPL version 2\nTintwizard License GNU GPL version 3"),
								"logo-icon-name", "taskbar", "authors", authors,
								/* Translators: translate "translator-credits" as
									your name to have it appear in the credits in the "About"
									dialog */
								"translator-credits", _("translator-credits"),
								NULL);
}


static void menuAdd()
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkFileFilter *filter;

	dialog = gtk_file_chooser_dialog_new(_("Add a theme"), GTK_WINDOW(g_window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_ADD, GTK_RESPONSE_ACCEPT, NULL);
	chooser = GTK_FILE_CHOOSER(dialog);

	gchar *config_dir;
	config_dir = g_build_filename(g_get_home_dir(), ".config", "tint2", NULL);
	gtk_file_chooser_set_current_folder(chooser, config_dir);
	g_free(config_dir);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Tint2 theme files"));
	gtk_file_filter_add_pattern(filter, "*tint2rc");
	gtk_file_chooser_add_filter(chooser, filter);
	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy(dialog);
		return;
	}

	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_theme_view));
	GSList *l, *list = gtk_file_chooser_get_filenames(chooser);
	gchar *file, *pt1, *name, *path, *name_first=NULL;
	for (l = list; l ; l = l->next) {
		file = (char *)l->data;
		pt1 = strrchr(file, '/');
		if (pt1 == NULL) 	continue;
		pt1++;
		if (*pt1 == 0)	continue;

		name = g_strdup(pt1);
		path = g_build_filename(g_get_user_config_dir(), "tint2", name, NULL);

		// check existing
		if (searchTheme(path, model, &iter)) {
			continue;
		}

		// append theme
		copy_file(file, path);
		custom_list_append(path);
		if (name_first == NULL)
			name_first = g_strdup(path);
		g_free(path);
		g_free(name);
	}
	g_slist_foreach(list, (GFunc)g_free, NULL);
	g_slist_free(list);
	gtk_widget_destroy(dialog);

	selectTheme(name_first);
	g_free(name_first);
	g_timeout_add(SNAPSHOT_TICK, (GSourceFunc)update_snapshot, NULL);
}


static void menuSaveAs()
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *file, *pt1;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view));
	if (!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), &model, &iter)) {
		GtkWidget *w = gtk_message_dialog_new(GTK_WINDOW(g_window), 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, _("Select the theme to be saved."));
		g_signal_connect_swapped(w, "response", G_CALLBACK(gtk_widget_destroy), w);
		gtk_widget_show(w);
		return;
	}

	gtk_tree_model_get(model, &iter, COL_THEME_FILE, &file, -1);
	pt1 = strrchr(file, '/');
	if (pt1) pt1++;

	dialog = gtk_file_chooser_dialog_new(_("Save theme as"), GTK_WINDOW(g_window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	chooser = GTK_FILE_CHOOSER(dialog);

	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	gchar *config_dir;
	config_dir = g_build_filename(g_get_home_dir(), ".config", "tint2", NULL);
	gtk_file_chooser_set_current_folder(chooser, config_dir);
	g_free(config_dir);
	gtk_file_chooser_set_current_name(chooser, pt1);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename(chooser);
		copy_file(file, filename);
		g_free(filename);
	}
	g_free(file);
	gtk_widget_destroy(dialog);
}


static void menuDelete()
{
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *filename;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), &model, &iter)) {
		gtk_tree_model_get(model, &iter, COL_THEME_FILE, &filename, -1);
		gtk_tree_selection_unselect_all(sel);

		// remove (gui and file)
		gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		GFile *file = g_file_new_for_path(filename);
		g_file_trash(file, NULL, NULL);
		g_object_unref(G_OBJECT(file));
		g_free(filename);
	}
}

char *get_current_theme_file_name()
{
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *file;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), &model, &iter)) {
		gtk_tree_model_get(model, &iter, COL_THEME_FILE, &file,  -1);
		return file;
	}
	return NULL;
}

static void menuProperties()
{
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *file;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), &model, &iter)) {
		gtk_tree_model_get(model, &iter, COL_THEME_FILE, &file,  -1);
		GtkWidget *prop;
		prop = create_properties();
		config_read_file(file);
		gtk_window_present(GTK_WINDOW(prop));
		g_free(file);
	}
}


static void menuQuit()
{
	if (g_path_config)
		g_free(g_path_config);
	if (g_path_dir)
		g_free(g_path_dir);
	if (g_default_theme)
		g_free(g_default_theme);

	gtk_main_quit();
}


static void menuRefresh()
{
	GtkTreeSelection *sel;
	GtkTreeIter iter;
	GtkTreeModel *model;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(sel), &model, &iter)) {
		gtk_list_store_set(g_store, &iter, COL_SNAPSHOT, NULL, -1);
	}

	g_timeout_add(SNAPSHOT_TICK, (GSourceFunc)update_snapshot, NULL);
}


static void menuRefreshAll()
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean have_iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_theme_view));
	have_iter = gtk_tree_model_get_iter_first(model, &iter);
	while (have_iter) {
		gtk_list_store_set(g_store, &iter, COL_SNAPSHOT, NULL, -1);
		have_iter = gtk_tree_model_iter_next(model, &iter);
	}

	g_timeout_add(SNAPSHOT_TICK, (GSourceFunc)update_snapshot, NULL);
}

static void view_popup_menu(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	GtkWidget *w = gtk_ui_manager_get_widget(globalUIManager, "/ThemePopup");

	gtk_menu_popup(GTK_MENU(w), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*)event));
}

static gboolean view_onButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	// single click with the right mouse button?
	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {
		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

		if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
			GtkTreePath *path;

			// Get tree path for row that was clicked
			if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL)) {
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}

		view_popup_menu(treeview, event, userdata);
		return TRUE;
	}
	return FALSE;
}

static gboolean view_onPopupMenu(GtkWidget *treeview, gpointer userdata)
{
	view_popup_menu(treeview, NULL, userdata);
	return TRUE;
}

static void viewRowActivated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{}

static void windowSizeAllocated()
{
	const gboolean isMaximized = g_window->window &&
								 (gdk_window_get_state(g_window->window) & GDK_WINDOW_STATE_MAXIMIZED);

	if (!isMaximized)
		gtk_window_get_size(GTK_WINDOW(g_window), &g_width, &g_height);
}

int endswith(const char *str, const char *suffix)
{
	return strlen(str) >= strlen(suffix) &&
			strcmp(str + strlen(str) - strlen(suffix), suffix) == 0;
}

static void load_theme(GtkWidget *list)
{
	GDir *dir;
	gchar *name;
	const gchar *file_name;
	int found_theme;

	dir = g_dir_open(g_path_dir, 0, NULL);
	if (dir == NULL)
		return;
	while ((file_name = g_dir_read_name(dir))) {
		if (!g_file_test(file_name, G_FILE_TEST_IS_DIR) &&
			!strstr(file_name, "backup") &&
			!strstr(file_name, "copy") &&
			!strstr(file_name, "~") &&
			(endswith(file_name, "tint2rc") ||
			 endswith(file_name, ".conf"))) {
			found_theme = TRUE;
			name = g_build_filename(g_path_dir, file_name, NULL);
			custom_list_append(name);
			g_free(name);
		}
	}

	if (!found_theme) {
		// create default config // TODO

		// copy tint2rc from system directory to user directory
		gchar *path2 = 0;
		const gchar * const * system_dirs = g_get_system_config_dirs();
		int i;
		for (i = 0; system_dirs[i]; i++) {
			path2 = g_build_filename(system_dirs[i], "tint2", "tint2rc", NULL);
			if (g_file_test(path2, G_FILE_TEST_EXISTS))
				break;
			g_free(path2);
			path2 = 0;
		}

		if (path2) {
			// copy file in user directory (path1)
			gchar *dir = g_build_filename(g_get_user_config_dir(), "tint2", NULL);
			if (!g_file_test(dir, G_FILE_TEST_IS_DIR))
				g_mkdir(dir, 0777);
			g_free(dir);

			gchar *path1 = g_build_filename (g_get_user_config_dir(), "tint2", "tint2rc", NULL);
			copy_file(path2, path1);
			g_free(path2);
			g_free(path1);
		} else {
			// create empty file
			gchar *path1 = g_build_filename (g_get_user_config_dir(), "tint2", "tint2rc", NULL);
			copy_file("/dev/null", path1);
			g_free(path1);
		}
		load_theme(list);
		return;
	}

	g_dir_close(dir);

	selectTheme(g_default_theme);

	g_timeout_add(SNAPSHOT_TICK, (GSourceFunc)update_snapshot, NULL);
}

void selectTheme(const gchar *name_theme)
{
	gboolean have_iter, found_theme;
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_theme_view));
	if (!name_theme) {
		have_iter = gtk_tree_model_get_iter_first(model, &iter);
		if (have_iter) {
			GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
			gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view)), &iter);
			gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(g_theme_view), path, NULL, FALSE, 0, 0);
			gtk_tree_path_free(path);
		}
		return;
	}

	found_theme = searchTheme(name_theme, model, &iter);

	GtkTreePath *path = NULL;
	if (found_theme) {
		path = gtk_tree_model_get_path(model, &iter);
	} else {
		have_iter = gtk_tree_model_get_iter_first(model, &iter);
		if (have_iter)
			path = gtk_tree_model_get_path(model, &iter);
	}
	if (path) {
		gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(g_theme_view)), &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(g_theme_view), path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
	}
}

gboolean searchTheme(const gchar *name_theme, GtkTreeModel *model, GtkTreeIter *iter)
{
	gchar *name;
	gboolean have_iter, found = FALSE;

	have_iter = gtk_tree_model_get_iter_first(model, iter);
	while (have_iter) {
		gtk_tree_model_get(model, iter, COL_THEME_FILE, &name,  -1);
		found = (strcmp(name, name_theme) == 0);
		g_free(name);
		if (found)
			break;
		have_iter = gtk_tree_model_iter_next(model, iter);
	}
	return found;
}
