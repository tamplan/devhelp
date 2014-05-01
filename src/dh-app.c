/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2002 CodeFactory AB
 * Copyright (C) 2002 Mikael Hallendal <micke@imendio.com>
 * Copyright (C) 2004-2008 Imendio AB
 * Copyright (C) 2012 Aleksander Morgado <aleksander@gnu.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <sys/stat.h>
#include <glib/gi18n.h>

#include "devhelp.h"
#include "dh-app.h"
#include "dh-profile.h"
#include "dh-preferences.h"
#include "dh-util.h"

typedef struct {
        /* List of available rofiles */
        GList *profiles;
        /* Currently selected profile */
        DhProfile *current;
} DhAppPrivate;

enum {
        PROP_0,
        PROP_CURRENT_PROFILE,
};

G_DEFINE_TYPE_WITH_PRIVATE (DhApp, dh_app, GTK_TYPE_APPLICATION)

/******************************************************************************/

DhProfile *
dh_app_peek_profile (DhApp *app)
{
        DhAppPrivate *priv;

        g_return_val_if_fail (DH_IS_APP (app), NULL);

        priv = dh_app_get_instance_private (app);

        return priv->current;
}

GtkWindow *
dh_app_peek_first_window (DhApp *app)
{
        GList *l;

        g_return_val_if_fail (DH_IS_APP (app), NULL);

        for (l = gtk_application_get_windows (GTK_APPLICATION (app));
             l;
             l = g_list_next (l)) {
                if (DH_IS_WINDOW (l->data)) {
                        return GTK_WINDOW (l->data);
                }
        }

        /* Create a new window */
        dh_app_new_window (app);

        /* And look for the newly created window again */
        return dh_app_peek_first_window (app);
}

GtkWindow *
dh_app_peek_assistant (DhApp *app)
{
        GList *l;

        g_return_val_if_fail (DH_IS_APP (app), NULL);

        for (l = gtk_application_get_windows (GTK_APPLICATION (app));
             l;
             l = g_list_next (l)) {
                if (DH_IS_ASSISTANT (l->data)) {
                        return GTK_WINDOW (l->data);
                }
        }

        return NULL;
}

gboolean
_dh_app_has_app_menu (DhApp *app)
{
        GtkSettings *gtk_settings;
        gboolean show_app_menu;
        gboolean show_menubar;

        g_return_val_if_fail (DH_IS_APP (app), FALSE);

        /* We have three cases:
         * - GNOME 3: show-app-menu true, show-menubar false -> use the app menu
         * - Unity, OSX: show-app-menu and show-menubar true -> use the normal menu
         * - Other WM, Windows: show-app-menu and show-menubar false -> use the normal menu
         */
        gtk_settings = gtk_settings_get_default ();
        g_object_get (G_OBJECT (gtk_settings),
                      "gtk-shell-shows-app-menu", &show_app_menu,
                      "gtk-shell-shows-menubar", &show_menubar,
                      NULL);

        return show_app_menu && !show_menubar;
}

/******************************************************************************/
/* Application action activators */

void
dh_app_new_window (DhApp *app)
{
        g_return_if_fail (DH_IS_APP (app));

        g_action_group_activate_action (G_ACTION_GROUP (app), "new-window", NULL);
}

void
dh_app_quit (DhApp *app)
{
        g_return_if_fail (DH_IS_APP (app));

        g_action_group_activate_action (G_ACTION_GROUP (app), "quit", NULL);
}

void
dh_app_search (DhApp *app,
               const gchar *keyword)
{
        g_return_if_fail (DH_IS_APP (app));

        g_action_group_activate_action (G_ACTION_GROUP (app), "search", g_variant_new_string (keyword));
}

void
dh_app_search_assistant (DhApp *app,
                         const gchar *keyword)
{
        g_return_if_fail (DH_IS_APP (app));

        g_action_group_activate_action (G_ACTION_GROUP (app), "search-assistant", g_variant_new_string (keyword));
}

void
dh_app_raise (DhApp *app)
{
        g_return_if_fail (DH_IS_APP (app));

        g_action_group_activate_action (G_ACTION_GROUP (app), "raise", NULL);
}

/******************************************************************************/
/* Application actions setup */

static void
new_window_cb (GSimpleAction *action,
               GVariant      *parameter,
               gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GtkWidget *window;

        window = dh_window_new (app);
        gtk_application_add_window (GTK_APPLICATION (app), GTK_WINDOW (window));
        gtk_widget_show_all (window);
}

static void
preferences_cb (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GtkWindow *window;

        window = dh_app_peek_first_window (app);

        dh_preferences_show_dialog (window);
}

static  void
profile_switch_cb (GSimpleAction *action,
                   GVariant      *value,
                   gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        DhAppPrivate *priv;
        const gchar *str;
        GList *l;
        DhProfile *target = NULL;

        priv = dh_app_get_instance_private (app);
        str = g_variant_get_string (value, NULL);

        for (l = priv->profiles; l; l = g_list_next (l)) {
                if (g_str_equal (str, dh_profile_get_id (DH_PROFILE (l->data)))) {
                        target = DH_PROFILE (l->data);
                        break;
                }
        }

        g_assert (target);
        g_message ("Switching profile to: %s", dh_profile_get_id (target));
        g_object_set (app, "current-profile", target, NULL);

        g_simple_action_set_state (action, value);
}

static void
about_cb (GSimpleAction *action,
          GVariant      *parameter,
          gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        const gchar  *authors[] = {
                "Mikael Hallendal <micke@imendio.com>",
                "Richard Hult <richard@imendio.com>",
                "Johan Dahlin <johan@gnome.org>",
                "Ross Burton <ross@burtonini.com>",
                "Aleksander Morgado <aleksander@lanedo.com>",
                "Thomas Bechtold <toabctl@gnome.org>",
                NULL
        };
        const gchar **documenters = NULL;
        const gchar  *translator_credits = _("translator-credits");
        GtkWindow *parent;

        parent = dh_app_peek_first_window (app);

        /* i18n: Please don't translate "Devhelp" (it's marked as translatable
         * for transliteration only) */
        gtk_show_about_dialog (parent,
                               "name", _("Devhelp"),
                               "version", PACKAGE_VERSION,
                               "comments", _("A developers' help browser for GNOME"),
                               "authors", authors,
                               "documenters", documenters,
                               "translator-credits",
                               (strcmp (translator_credits, "translator-credits") != 0 ?
                                translator_credits :
                                NULL),
                               "website", PACKAGE_URL,
                               "website-label", _("DevHelp Website"),
                               "logo-icon-name", PACKAGE_TARNAME,
                               NULL);
}

static void
quit_cb (GSimpleAction *action,
         GVariant      *parameter,
         gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GList *l;

        /* Remove all windows registered in the application */
        while ((l = gtk_application_get_windows (GTK_APPLICATION (app)))) {
                gtk_application_remove_window (GTK_APPLICATION (app),
                                               GTK_WINDOW (l->data));
        }
}

static void
search_cb (GSimpleAction *action,
           GVariant      *parameter,
           gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GtkWindow *window;
        const gchar *str;

        window = dh_app_peek_first_window (app);
        str = g_variant_get_string (parameter, NULL);
        if (str[0] == '\0') {
                g_warning ("Cannot search in application window: "
                           "No keyword given");
                return;
        }

        dh_window_search (DH_WINDOW (window), str);
        gtk_window_present (window);
}

static void
search_assistant_cb (GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GtkWindow *assistant;
        const gchar *str;

        str = g_variant_get_string (parameter, NULL);
        if (str[0] == '\0') {
                g_warning ("Cannot look for keyword in Search Assistant: "
                           "No keyword given");
                return;
        }

        /* Look for an already registered assistant */
        assistant = dh_app_peek_assistant (app);
        if (!assistant) {
                assistant = GTK_WINDOW (dh_assistant_new (app));
                gtk_application_add_window (GTK_APPLICATION (app), assistant);
        }

        dh_assistant_search (DH_ASSISTANT (assistant), str);
}

static void
raise_cb (GSimpleAction *action,
          GVariant      *parameter,
          gpointer       user_data)
{
        DhApp *app = DH_APP (user_data);
        GtkWindow *window;

        /* Look for the first application window available and show it */
        window = dh_app_peek_first_window (app);
        gtk_window_present (window);
}

static GActionEntry app_entries[] = {
        /* general  actions */
        { "new-window",       new_window_cb,       NULL, NULL, NULL },
        { "preferences",      preferences_cb,      NULL, NULL, NULL },
        { "about",            about_cb,            NULL, NULL, NULL },
        { "quit",             quit_cb,             NULL, NULL, NULL },
        { "switch-profile",   NULL,                "s",  "'local'", profile_switch_cb },
        /* additional commandline-specific actions */
        { "search",           search_cb,           "s",  NULL, NULL },
        { "search-assistant", search_assistant_cb, "s",  NULL, NULL },
        { "raise",            raise_cb,            NULL, NULL, NULL },
};

/******************************************************************************/

static void
setup_accelerators (DhApp *self)
{
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>0",     "win.zoom-default",     NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>minus", "win.zoom-out",         NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>plus",  "win.zoom-in",          NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>k",     "win.focus-search",     NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>s",     "win.focus-search-alt", NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>f",     "win.find",             NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>c",     "win.copy",             NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>p",     "win.print",            NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>t",     "win.new-tab",          NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Primary>w",     "win.close",            NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "F10",            "win.gear-menu",        NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Alt>Right",     "win.go-forward",       NULL);
        gtk_application_add_accelerator (GTK_APPLICATION (self), "<Alt>Left",      "win.go-back",          NULL);
}

/******************************************************************************/

static void
store_profile (DhApp     *app,
               DhProfile *profile)
{
        DhAppPrivate *priv = dh_app_get_instance_private (app);

        g_message ("Profile '%s' (%s) added", dh_profile_get_id (profile), dh_profile_get_name (profile));
        priv->profiles = g_list_append (priv->profiles, g_object_ref (profile));
}

static void
load_profiles (DhApp *app)
{
        GDir *dir;
        gchar *profiles_path;
        gchar *profiles_path_printable;
        const gchar *name;
        GError *error = NULL;
        DhProfile *profile;

        /* Load the local profile */
        profile = dh_profile_new ("local", _("Local"), g_get_system_data_dirs ());
        store_profile (app, profile);
        g_object_unref (profile);

        profiles_path = g_build_filename (g_get_user_data_dir (), PACKAGE_TARNAME, "profiles", NULL);
        profiles_path_printable = g_filename_to_utf8 (profiles_path, -1, NULL, NULL, NULL);

        /* Open directory */
        dir = g_dir_open (profiles_path, 0, &error);
        if (!dir) {
                g_message ("cannot open profiles directory '%s': %s", profiles_path_printable, error->message);
                g_error_free (error);

                if (g_mkdir_with_parents (profiles_path, S_IRWXU | S_IRWXG) < 0) {
                        g_warning ("cannot create profiles directory '%s'", profiles_path_printable);
                        goto out;
                }

                g_message ("profiles directory created '%s'", profiles_path_printable);

                dir = g_dir_open (profiles_path, 0, &error);
                if (!dir) {
                        g_warning ("cannot open newly created profiles directory '%s': %s", profiles_path_printable, error->message);
                        g_error_free (error);
                        goto out;
                }
        } else
                g_message ("profiles directory available: %s", profiles_path);

        /* And iterate it */
        while ((name = g_dir_read_name (dir)) != NULL) {
                gchar *profile_dir_path;
                gchar *profile_dir_path_printable;
                gchar *profile_name_path;
                gchar *profile_name_path_printable;
                GFile *profile_name_file;
                gchar *profile_name = NULL;

                /* Build the path of the directory where the final
                 * devhelp profile resides */
                profile_dir_path = g_build_filename (profiles_path, name, NULL);
                profile_dir_path_printable = g_filename_to_utf8 (profile_dir_path, -1, NULL, NULL, NULL);
                profile_name_path = g_build_filename (profile_dir_path, "name", NULL);
                profile_name_path_printable = g_filename_to_utf8 (profile_name_path, -1, NULL, NULL, NULL);
                profile_name_file = g_file_new_for_path (profile_name_path);

                if (!g_file_load_contents (profile_name_file,
                                           NULL,
                                           &profile_name,
                                           NULL,
                                           NULL,
                                           &error)) {
                        g_warning ("cannot load profile contents from '%s': %s", profile_name_path_printable, error->message);
                        g_clear_error (&error);
                } else {
                        gchar **split;
                        gchar *paths[2];

                        paths[0] = profile_dir_path;
                        paths[1] = NULL;

                        split = g_strsplit (profile_name, ",", -1);
                        if (split[0] && split[1]) {
                                profile = dh_profile_new (split[0], split[1], (const gchar *const *)paths);
                                store_profile (app, profile);
                                g_object_unref (profile);
                        } else {
                                g_warning ("Profile name file content unusable: '%s'", profile_name);
                        }
                        g_strfreev (split);
                }

                g_free (profile_name);
                g_free (profile_dir_path);
                g_free (profile_dir_path_printable);
                g_free (profile_name_path);
                g_free (profile_name_path_printable);
                g_object_unref (profile_name_file);
        }

        g_dir_close (dir);

out:
        g_free (profiles_path);
}

static void
dh_app_startup (GApplication *application)
{
        DhApp *app = DH_APP (application);
        DhAppPrivate *priv = dh_app_get_instance_private (app);

        /* Chain up parent's startup */
        G_APPLICATION_CLASS (dh_app_parent_class)->startup (application);

        /* Setup actions */
        g_action_map_add_action_entries (G_ACTION_MAP (app),
                                         app_entries, G_N_ELEMENTS (app_entries),
                                         app);

        /* Load profiles */
        load_profiles (app);

        /* Setup app menu */
        if (_dh_app_has_app_menu (app)) {
                GtkBuilder *builder;
                GError *error = NULL;

                /* Setup menu */
                builder = gtk_builder_new ();

                if (!gtk_builder_add_from_resource (builder,
                                                    "/org/gnome/devhelp/devhelp-menu.ui",
                                                    &error)) {
                        g_warning ("loading menu builder file: %s", error->message);
                        g_error_free (error);
                } else {
                        GMenuModel *app_menu;
                        GMenu *submenu;
                        GList *l;

                        app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "app-menu"));
                        gtk_application_set_app_menu (GTK_APPLICATION (application), app_menu);

                        /* Create Profiles submenu */
                        submenu = g_menu_new ();
                        g_menu_insert_submenu (G_MENU (app_menu), 0, _("Profiles"), G_MENU_MODEL (submenu));

                        for (l = priv->profiles; l; l = g_list_next (l)) {
                                DhProfile *profile;
                                gchar *action_name;

                                profile = DH_PROFILE (l->data);
                                action_name = g_strdup_printf ("app.switch-profile::%s", dh_profile_get_id (profile));
                                g_menu_append (submenu, dh_profile_get_name (profile), action_name);
                                g_free (action_name);
                        }
                }

                g_object_unref (builder);
        }

        /* Setup accelerators */
        setup_accelerators (app);

        g_action_group_activate_action (G_ACTION_GROUP (app), "switch-profile", g_variant_new_string ("local"));
}

/******************************************************************************/

DhApp *
dh_app_new (void)
{
        return g_object_new (DH_TYPE_APP,
                             "application-id",   "org.gnome.Devhelp",
                             "flags",            G_APPLICATION_FLAGS_NONE,
                             "register-session", TRUE,
                             NULL);
}

static void
dh_app_init (DhApp *app)
{
        /* i18n: Please don't translate "Devhelp" (it's marked as translatable
         * for transliteration only) */
        g_set_application_name (_("Devhelp"));
        gtk_window_set_default_icon_name ("devhelp");
}

static void
dh_app_set_property (GObject      *object,
                     guint         prop_id,
                     const GValue *value,
                     GParamSpec   *pspec)
{
        DhAppPrivate *priv;

        priv = dh_app_get_instance_private (DH_APP (object));

        switch (prop_id)
        {
        case PROP_CURRENT_PROFILE:
                g_clear_object (&priv->current);
                priv->current = g_value_dup_object (value);
                dh_profile_populate (priv->current);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
dh_app_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
        DhAppPrivate *priv;

        priv = dh_app_get_instance_private (DH_APP (object));

        switch (prop_id)
        {
        case PROP_CURRENT_PROFILE:
                g_value_set_object (value, priv->current);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
dh_app_dispose (GObject *object)
{
        DhApp *app = DH_APP (object);
        DhAppPrivate *priv = dh_app_get_instance_private (app);

        g_clear_object (&priv->current);
        if (priv->profiles) {
                g_list_free_full (priv->profiles, g_object_unref);
                priv->profiles = NULL;
        }

        G_OBJECT_CLASS (dh_app_parent_class)->dispose (object);
}

static void
dh_app_class_init (DhAppClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        application_class->startup = dh_app_startup;

        object_class->dispose = dh_app_dispose;
        object_class->set_property = dh_app_set_property;
        object_class->get_property = dh_app_get_property;

        g_object_class_install_property (object_class,
                                         PROP_CURRENT_PROFILE,
                                         g_param_spec_object ("current-profile",
                                                              "Current profile",
                                                              "Profile currently being used",
                                                              DH_TYPE_PROFILE,
                                                              (G_PARAM_READWRITE |
                                                               G_PARAM_STATIC_NAME |
                                                               G_PARAM_STATIC_NICK |
                                                               G_PARAM_STATIC_BLURB)));
}
