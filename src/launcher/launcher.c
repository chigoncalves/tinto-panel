/**************************************************************************
* Tint2 : launcher
*
* Copyright (C) 2010       (mrovi@interfete-web-club.com)
*
* SVG support: https://github.com/ixxra/tint2-svg
* Copyright (C) 2010       Rene Garcia (garciamx@gmail.com)
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
#include "conf.h" // For system checks and `UNUSED'.

#include <string.h>
#include <stdio.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <pango/pangocairo.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#ifdef HAS_RSVG
  #include <librsvg/rsvg.h>
#endif // HAS_RSVG

#include "window.h"
#include "server.h"
#include "area.h"
#include "panel.h"
#include "taskbar.h"
#include "launcher.h"
#include "desktop-entry.h"
#include "icon-theme-common.h"

#ifdef HAS_SN
  #include <unistd.h>
  #include <libsn/sn.h>
  #include <sys/wait.h>

  #include "debug.h"
#endif // HAS_SN


int launcher_enabled;
int launcher_max_icon_size;
int launcher_tooltip_enabled;
int launcher_alpha;
int launcher_saturation;
int launcher_brightness;
char *icon_theme_name_config;
char *icon_theme_name_xsettings;
XSettingsClient *xsettings_client;
int startup_notifications;

Imlib_Image scale_icon(Imlib_Image original, int icon_size);
void free_icon(Imlib_Image icon);

#ifdef HAS_SN
int sn_pipe_valid = 0;
int sn_pipe[2];
int error_trap_depth = 0;

static void
launcher_error_trap_push (SnDisplay *display, Display *xdisplay);

static gint
cmp_ptr (gconstpointer a, gconstpointer b);

static void
launcher_sigchld_handler (int sig);

static void
launcher_error_trap_pop (SnDisplay *display, Display *xdisplay);
#endif // HAS_SN


void
default_launcher (void) {
  launcher_enabled = 0;
  launcher_max_icon_size = 0;
  launcher_tooltip_enabled = 0;
  launcher_alpha = 100;
  launcher_saturation = 0;
  launcher_brightness = 0;
  icon_theme_name_config = NULL;
  icon_theme_name_xsettings = NULL;
  xsettings_client = NULL;
  startup_notifications = 0;

#ifdef HAS_SN
  // Initialize startup-notification
  if (startup_notifications) {
    server.sn_dsp = sn_display_new (server.dsp,
				    launcher_error_trap_push,
				    launcher_error_trap_pop);
    server.pids = g_tree_new (cmp_ptr);
    // Setup a handler for child termination
    if (pipe (sn_pipe) != 0) {
      WARN ("Creating pipe failed.");
    }
    else {
      sn_pipe_valid = 1;
      struct sigaction act;
      memset (&act, 0, sizeof (struct sigaction));
      act.sa_handler = launcher_sigchld_handler;
      if (sigaction (SIGCHLD, &act, NULL)) {
	perror("sigaction");
      }
    }
  }
#endif // HAS_SN

}


void init_launcher()
{
	if (launcher_enabled) {
		// if XSETTINGS manager running, tint2 read the icon_theme_name.
		xsettings_client = xsettings_client_new(server.dsp, server.screen, xsettings_notify_cb, NULL, NULL);
	}
}


void init_launcher_panel(void *p)
{
	Panel *panel =(Panel*)p;
	Launcher *launcher = &panel->launcher;

	launcher->area.parent = p;
	launcher->area.panel = p;
	launcher->area.area_draw_foreground = NULL;
	launcher->area.size_mode = SIZE_BY_CONTENT;
	launcher->area._resize = resize_launcher;
	launcher->area.resize = 1;
	launcher->area.redraw = 1;
	if (!launcher->area.background)
		launcher->area.background = &g_array_index(backgrounds, background_t, 0);

	// check consistency
	if (launcher->list_apps == NULL)
		return;

	launcher->area.visible = 1;
	panel_refresh = 1;

	launcher_load_themes(launcher);
	launcher_load_icons(launcher);
}


void launcher_deinit (void) {
	int i;
	GSList *l;

	if (xsettings_client)
		xsettings_client_destroy(xsettings_client);
	xsettings_client = NULL;

	for (i = 0; i < nb_panel; i++) {
		Panel *panel = &panel1[i];
		Launcher *launcher = &panel->launcher;
		cleanup_launcher_theme(launcher);
	}

	for (l = panel_config.launcher.list_apps; l ; l = l->next) {
		free(l->data);
	}
	g_slist_free(panel_config.launcher.list_apps);
	panel_config.launcher.list_apps = NULL;

	free(icon_theme_name_config);
	icon_theme_name_config = NULL;

	free(icon_theme_name_xsettings);
	icon_theme_name_xsettings = NULL;

	launcher_enabled = 0;

#ifdef HAS_SN
  if (startup_notifications) {
    if (sn_pipe_valid) {
      sn_pipe_valid = 0;
      close(sn_pipe[1]);
      close(sn_pipe[0]);
    }
  }
#endif // HAS_SN

}


void cleanup_launcher_theme(Launcher *launcher)
{
	area_destroy (&launcher->area);
	GSList *l;
	for (l = launcher->list_icons; l ; l = l->next) {
		LauncherIcon *launcherIcon = (LauncherIcon*)l->data;
		if (launcherIcon) {
			free_icon(launcherIcon->icon_scaled);
			free_icon(launcherIcon->icon_original);
			free(launcherIcon->icon_name);
			free(launcherIcon->icon_path);
			free(launcherIcon->cmd);
			free(launcherIcon->icon_tooltip);
		}
		free(launcherIcon);
	}
	g_slist_free(launcher->list_icons);
	launcher->list_icons = NULL;

	free_themes(launcher->list_themes);
	launcher->list_themes = NULL;
}


int resize_launcher(void *obj)
{
	Launcher *launcher = obj;
	GSList *l;
	int count, icon_size;
	int icons_per_column=1, icons_per_row=1, marging=0;

	if (panel_horizontal) {
		icon_size = launcher->area.bounds.height;
	} else {
		icon_size = launcher->area.bounds.width;
	}
	icon_size = icon_size - (2 * launcher->area.background->border.width) - (2 * launcher->area.paddingy);
	if (launcher_max_icon_size > 0 && icon_size > launcher_max_icon_size)
		icon_size = launcher_max_icon_size;

	// Resize icons if necessary
	for (l = launcher->list_icons; l ; l = l->next) {
		LauncherIcon *launcherIcon = (LauncherIcon *)l->data;
		if (launcherIcon->icon_size != icon_size || !launcherIcon->icon_original) {
			launcherIcon->icon_size = icon_size;
			launcherIcon->area.bounds.width = launcherIcon->icon_size;
			launcherIcon->area.bounds.height = launcherIcon->icon_size;

			// Get the path for an icon file with the new size
			char *new_icon_path = get_icon_path(launcher->list_themes, launcherIcon->icon_name, launcherIcon->icon_size);
			if (!new_icon_path) {
				// Draw a blank icon
				free_icon(launcherIcon->icon_original);
				launcherIcon->icon_original = NULL;
				free_icon(launcherIcon->icon_scaled);
				launcherIcon->icon_scaled = NULL;
				continue;
			}
			if (launcherIcon->icon_path && strcmp(new_icon_path, launcherIcon->icon_path) == 0) {
				// If it's the same file just rescale
				free_icon(launcherIcon->icon_scaled);
				launcherIcon->icon_scaled = scale_icon(launcherIcon->icon_original, icon_size);
				free(new_icon_path);
				fprintf(stderr, "launcher.c %d: Using icon %s\n", __LINE__, launcherIcon->icon_path);
			} else {
				// Free the old files
				free_icon(launcherIcon->icon_original);
				free_icon(launcherIcon->icon_scaled);
				launcherIcon->icon_original = launcherIcon->icon_scaled = NULL;
				// Load the new file and scale
#ifdef HAS_RSVG
				if (g_str_has_suffix(new_icon_path, ".svg")) {
					GError* err = NULL;
					RsvgHandle* svg = rsvg_handle_new_from_file(new_icon_path, &err);

					if (err != NULL) {
						fprintf(stderr, "Could not load svg image!: %s", err->message);
						g_error_free(err);
						launcherIcon->icon_original = NULL;
					} else {
						char suffix[128];
						sprintf(suffix, "tmpicon-%d.png", getpid());
						gchar *name = g_build_filename(g_get_user_config_dir(), "tint2", suffix, NULL);
						GdkPixbuf *pixbuf = rsvg_handle_get_pixbuf(svg);
						gdk_pixbuf_save(pixbuf, name, "png", NULL, NULL);
						launcherIcon->icon_original = imlib_load_image_immediately_without_cache(name);
						g_remove(name);
						g_free(name);
						g_object_unref(G_OBJECT(pixbuf));
						g_object_unref(G_OBJECT(svg));
					}
				} else
#endif // HAS_RSVG
				{
					launcherIcon->icon_original = imlib_load_image_immediately(new_icon_path);
				}
				// On loading error, fallback to default
				if (!launcherIcon->icon_original) {
					free(new_icon_path);
					new_icon_path = get_icon_path(launcher->list_themes, DEFAULT_ICON, launcherIcon->icon_size);
					if (new_icon_path)
						launcherIcon->icon_original = imlib_load_image_immediately(new_icon_path);
				}

				if (!launcherIcon->icon_original) {
					// Loading default icon failed, draw a blank icon
					free(new_icon_path);
				} else {
					// Loaded icon successfully
					launcherIcon->icon_scaled = scale_icon(launcherIcon->icon_original, launcherIcon->icon_size);
					free(launcherIcon->icon_path);
					launcherIcon->icon_path = new_icon_path;
					fprintf(stderr, "launcher.c %d: Using icon %s\n", __LINE__, launcherIcon->icon_path);
				}
			}
		}
	}

	count = g_slist_length(launcher->list_icons);

	if (panel_horizontal) {
		if (!count) {
			launcher->area.bounds.width = 0;
		} else {
			int height = launcher->area.bounds.height - 2*launcher->area.background->border.width - 2*launcher->area.paddingy;
			// here icons_per_column always higher than 0
			icons_per_column = (height+launcher->area.paddingx) / (icon_size+launcher->area.paddingx);
			marging = height - (icons_per_column-1)*(icon_size+launcher->area.paddingx) - icon_size;
			icons_per_row = count / icons_per_column + (count%icons_per_column != 0);
			launcher->area.bounds.width = (2 * launcher->area.background->border.width) +
								   (2 * launcher->area.paddingxlr) +
								   (icon_size * icons_per_row) +
								   ((icons_per_row-1) * launcher->area.paddingx);
		}
	}
	else {
		if (!count) {
			launcher->area.bounds.height = 0;
		} else {
			int width = launcher->area.bounds.width - 2*launcher->area.background->border.width - 2*launcher->area.paddingy;
			// here icons_per_row always higher than 0
			icons_per_row = (width+launcher->area.paddingx) / (icon_size+launcher->area.paddingx);
			marging = width - (icons_per_row-1)*(icon_size+launcher->area.paddingx) - icon_size;
			icons_per_column = count / icons_per_row+ (count%icons_per_row != 0);
			launcher->area.bounds.height = (2 * launcher->area.background->border.width) +
									(2 * launcher->area.paddingxlr) +
									(icon_size * icons_per_column) +
									((icons_per_column-1) * launcher->area.paddingx);
		}
	}

	int i, posx, posy;
	int start = launcher->area.background->border.width + launcher->area.paddingy + marging/2;
	if (panel_horizontal) {
		posy = start;
		posx = launcher->area.background->border.width + launcher->area.paddingxlr;
	} else {
		posx = start;
		posy = launcher->area.background->border.width + launcher->area.paddingxlr;
	}

	for (i=1, l = launcher->list_icons; l ; i++, l = l->next) {
		LauncherIcon *launcherIcon = (LauncherIcon*)l->data;

		launcherIcon->y = posy;
		launcherIcon->x = posx;
		launcherIcon->area.bounds.y = ((Area*)launcherIcon->area.parent)->bounds.y + launcherIcon->y;
		launcherIcon->area.bounds.x = ((Area*)launcherIcon->area.parent)->bounds.x + launcherIcon->x;
		launcherIcon->area.bounds.width = launcherIcon->icon_size;
		launcherIcon->area.bounds.height = launcherIcon->icon_size;
		//printf("launcher %d : %d,%d\n", i, posx, posy);
		if (panel_horizontal) {
			if (i % icons_per_column) {
				posy += icon_size + launcher->area.paddingx;
			} else {
				posy = start;
				posx += (icon_size + launcher->area.paddingx);
			}
		} else {
			if (i % icons_per_row) {
				posx += icon_size + launcher->area.paddingx;
			} else {
				posx = start;
				posy += (icon_size + launcher->area.paddingx);
			}
		}
	}
	return 1;
}

// Here we override the default layout of the icons; normally Area layouts its children
// in a stack; we need to layout them in a kind of table
void launcher_icon_on_change_layout(void *obj)
{
	LauncherIcon *launcherIcon = (LauncherIcon*)obj;
	launcherIcon->area.bounds.y = ((Area*)launcherIcon->area.parent)->bounds.y + launcherIcon->y;
	launcherIcon->area.bounds.x = ((Area*)launcherIcon->area.parent)->bounds.x + launcherIcon->x;
	launcherIcon->area.bounds.width = launcherIcon->icon_size;
	launcherIcon->area.bounds.height = launcherIcon->icon_size;
}

const char* launcher_icon_get_tooltip_text(void *obj)
{
	LauncherIcon *launcherIcon = (LauncherIcon*)obj;
	return launcherIcon->icon_tooltip;
}

void draw_launcher_icon(void *obj, cairo_t *c) {
  UNUSED (c);
	LauncherIcon *launcherIcon = (LauncherIcon*)obj;

	Imlib_Image icon_scaled = launcherIcon->icon_scaled;
	// Render
	imlib_context_set_image (icon_scaled);
	imlib_context_set_blend(1);
	imlib_context_set_drawable(launcherIcon->area.pixmap);
	imlib_render_image_on_drawable(0, 0);
}

Imlib_Image scale_icon(Imlib_Image original, int icon_size)
{
	Imlib_Image icon_scaled;
	if (original) {
		imlib_context_set_image (original);
		icon_scaled = imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(), imlib_image_get_height(), icon_size, icon_size);
		imlib_context_set_image (icon_scaled);
		imlib_image_set_has_alpha(1);
		DATA32* data = imlib_image_get_data();
		adjust_asb(data, icon_size, icon_size, launcher_alpha, (float)launcher_saturation/100, (float)launcher_brightness/100);
		imlib_image_put_back_data(data);
	} else {
		icon_scaled = imlib_create_image(icon_size, icon_size);
		imlib_context_set_image (icon_scaled);
		imlib_context_set_color(255, 255, 255, 255);
		imlib_image_fill_rectangle(0, 0, icon_size, icon_size);
	}
	return icon_scaled;
}

void free_icon(Imlib_Image icon)
{
	if (icon) {
		imlib_context_set_image(icon);
		imlib_free_image();
	}
}

void launcher_action(LauncherIcon *icon, XEvent* evt)
{
	char *cmd = calloc(strlen(icon->cmd) + 10, 1);
	sprintf(cmd, "(%s&)", icon->cmd);
#if HAS_SN
	SnLauncherContext* ctx;
	Time time;
	if (startup_notifications) {
		ctx = sn_launcher_context_new(server.sn_dsp, server.screen);
		sn_launcher_context_set_name(ctx, icon->icon_tooltip);
		sn_launcher_context_set_description(ctx, "Application launched from tint2");
		sn_launcher_context_set_binary_name (ctx, icon->cmd);
		// Get a timestamp from the X event
		if (evt->type == ButtonPress || evt->type == ButtonRelease) {
			time = evt->xbutton.time;
		} else {
			fprintf(stderr, "Unknown X event: %d\n", evt->type);
			free(cmd);
			return;
		}
		sn_launcher_context_initiate(ctx, "tint2", icon->cmd, time);
	}
#endif // HAS_SN
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Could not fork\n");
	} else if (pid == 0) {
		// Child process
#if HAS_SN
		if (startup_notifications) {
			sn_launcher_context_setup_child_process(ctx);
		}
#endif // HAS_SN
		// Allow children to exist after parent destruction
		setsid();
		// Run the command
		execl("/bin/sh", "/bin/sh", "-c", icon->cmd, NULL);
		fprintf(stderr, "Failed to execlp %s\n", icon->cmd);
#if HAS_SN
		if (startup_notifications) {
			sn_launcher_context_unref(ctx);
		}
#endif // HAS_SN
		exit(1);
	} else {
		// Parent process
#if HAS_SN
		if (startup_notifications) {
			g_tree_insert(server.pids, GINT_TO_POINTER (pid), ctx);
		}
#endif // HAS_SN
	}
	free(cmd);
}

// Populates the list_icons list from the list_apps list
void
launcher_load_icons(Launcher* launcher) {
	// Load apps (.desktop style launcher items)
	GSList* app = launcher->list_apps;
	while (app != NULL) {
    desktop_entry_t* entry = desktop_entry_create (app->data);
    if (entry->exec) {
			LauncherIcon *launcherIcon = calloc(1, sizeof(LauncherIcon));
			launcherIcon->area.parent = launcher;
			launcherIcon->area.panel = launcher->area.panel;
			launcherIcon->area.area_draw_foreground = draw_launcher_icon;
			launcherIcon->area.size_mode = SIZE_BY_CONTENT;
			launcherIcon->area._resize = NULL;
			launcherIcon->area.resize = 0;
			launcherIcon->area.redraw = 1;
			launcherIcon->area.background = &g_array_index(backgrounds, background_t, 0);
			launcherIcon->area.visible = 1;
			launcherIcon->area._on_change_layout = launcher_icon_on_change_layout;
			if (launcher_tooltip_enabled) {
				launcherIcon->area._get_tooltip_text = launcher_icon_get_tooltip_text;
			} else {
				launcherIcon->area._get_tooltip_text = NULL;
			}
			launcherIcon->is_app_desktop = 1;
      launcherIcon->cmd = strdup (entry->exec);
      launcherIcon->icon_name = entry->icon ? strdup (entry->icon) :
	strdup (DEFAULT_ICON);
			launcherIcon->icon_size = 1;
      launcherIcon->icon_tooltip = entry->name ? strdup (entry->name) :
	strdup (entry->exec);
      desktop_entry_destroy (entry);
			launcher->list_icons = g_slist_append(launcher->list_icons, launcherIcon);
			add_area(&launcherIcon->area);
		}
		app = g_slist_next(app);
	}
}


// Populates the list_themes list
void launcher_load_themes(Launcher *launcher)
{
	launcher->list_themes = load_themes(icon_theme_name_config
										? icon_theme_name_config
										: icon_theme_name_xsettings
										  ? icon_theme_name_xsettings
										  : "hicolor");
}

void
launcher_sigchld_handler_async (void) {
#ifdef HAS_SN
  if (!startup_notifications) return;

  // Wait for all dead processes
  pid_t pid;
  while ((pid = waitpid (-1, NULL, WNOHANG)) > 0) {
    SnLauncherContext *ctx =
      (SnLauncherContext *)g_tree_lookup (server.pids,
					  GINT_TO_POINTER (pid));


    if (ctx) {
      g_tree_remove (server.pids, GINT_TO_POINTER (pid));
      sn_launcher_context_complete (ctx);
      sn_launcher_context_unref (ctx);
    }
    else
      MSG ("Unknown child %d terminated!\n", pid);
  }
#endif // HAS_SN
}

#ifdef HAS_SN
static void
launcher_error_trap_push (SnDisplay *display, Display *xdisplay) {
  UNUSED (display, xdisplay);
  ++error_trap_depth;
}

static void
launcher_error_trap_pop (SnDisplay *display, Display *xdisplay) {
  UNUSED (display);
  if (!error_trap_depth) {
    WARN ("Error trap underflow!\n");
    return;
  }

  XSync (xdisplay, False); /* get all errors out of the queue */
  --error_trap_depth;
}

static void
launcher_sigchld_handler (int sig) {
  UNUSED (sig);
  if (!startup_notifications || !sn_pipe_valid)
    return;

  write (sn_pipe[1], "x", 1);
  fsync (sn_pipe[1]);
}

static gint
cmp_ptr (gconstpointer a, gconstpointer b) {
  if (a < b)
    return -1;
  else if (a == b)
    return 0;
  else
    return 1;
}
#endif // HAS_SN
