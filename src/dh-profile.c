/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2014 Aleksander Morgado <aleksander@aleksander.es>
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
#include <string.h>

#include "dh-profile.h"
#include "dh-book-manager.h"

typedef struct {
        /* Profile setup */
        gchar *id;
        gchar *name;
        gchar **paths;
        /* The book manager of the profile */
        DhBookManager *book_manager;
} DhProfilePrivate;

enum {
        PROP_0,
        PROP_ID,
        PROP_NAME,
        PROP_PATHS,
};

G_DEFINE_TYPE_WITH_PRIVATE (DhProfile, dh_profile, G_TYPE_OBJECT)

/******************************************************************************/

void
dh_profile_populate (DhProfile *self)
{
        DhProfilePrivate *priv;
        guint i;

        priv = dh_profile_get_instance_private (self);

        for (i = 0; priv->paths[i]; i++)
                dh_book_manager_populate (priv->book_manager, priv->paths[i]);
}

/******************************************************************************/

DhBookManager *
dh_profile_peek_book_manager (DhProfile *self)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (self);

        return priv->book_manager;
}

const gchar *
dh_profile_get_id (DhProfile *self)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (self);

        return priv->id;
}

const gchar *
dh_profile_get_name (DhProfile *self)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (self);

        return priv->name;
}

const gchar * const *
dh_profile_get_paths (DhProfile *self)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (self);

        return (const gchar * const *)priv->paths;
}

/******************************************************************************/

DhProfile *
dh_profile_new (const gchar *id,
                const gchar *name,
                const gchar *const *paths)
{
        return g_object_new (DH_TYPE_PROFILE,
                             "id",    id,
                             "name",  name,
                             "paths", paths,
                             NULL);
}

static void
dh_profile_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (DH_PROFILE (object));

        switch (prop_id)
        {
        case PROP_ID:
                priv->id = g_value_dup_string (value);
                break;
        case PROP_NAME:
                priv->name = g_value_dup_string (value);
                break;
        case PROP_PATHS:
                priv->paths = g_value_dup_boxed (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
dh_profile_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (DH_PROFILE (object));

        switch (prop_id)
        {
        case PROP_ID:
                g_value_set_string (value, priv->id);
                break;
        case PROP_NAME:
                g_value_set_string (value, priv->name);
                break;
        case PROP_PATHS:
                g_value_set_boxed (value, priv->paths);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
dh_profile_finalize (GObject *object)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (DH_PROFILE (object));

        g_free (priv->id);
        g_free (priv->name);
        g_strfreev (priv->paths);

        G_OBJECT_CLASS (dh_profile_parent_class)->finalize (object);
}

static void
dh_profile_dispose (GObject *object)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (DH_PROFILE (object));

        g_clear_object (&priv->book_manager);

        G_OBJECT_CLASS (dh_profile_parent_class)->dispose (object);
}

static void
dh_profile_init (DhProfile *object)
{
        DhProfilePrivate *priv;

        priv = dh_profile_get_instance_private (DH_PROFILE (object));

        priv->book_manager = dh_book_manager_new ();
}

static void
dh_profile_class_init (DhProfileClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->set_property = dh_profile_set_property;
        object_class->get_property = dh_profile_get_property;
        object_class->finalize = dh_profile_finalize;
        object_class->dispose = dh_profile_dispose;

        g_object_class_install_property (object_class,
                                         PROP_ID,
                                         g_param_spec_string ("id", "ID", "Profile ID",
                                                              NULL,
                                                              (G_PARAM_READWRITE |
                                                               G_PARAM_STATIC_NAME |
                                                               G_PARAM_STATIC_NICK |
                                                               G_PARAM_STATIC_BLURB)));

        g_object_class_install_property (object_class,
                                         PROP_NAME,
                                         g_param_spec_string ("name", "Name", "Profile name",
                                                              NULL,
                                                              (G_PARAM_READWRITE |
                                                               G_PARAM_STATIC_NAME |
                                                               G_PARAM_STATIC_NICK |
                                                               G_PARAM_STATIC_BLURB)));
        g_object_class_install_property (object_class,
                                         PROP_PATHS,
                                         g_param_spec_boxed ("paths", "Paths", "Profile paths",
                                                              G_TYPE_STRV,
                                                              (G_PARAM_READWRITE |
                                                               G_PARAM_STATIC_NAME |
                                                               G_PARAM_STATIC_NICK |
                                                               G_PARAM_STATIC_BLURB)));
}
