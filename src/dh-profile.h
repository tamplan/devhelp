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

#ifndef __DH_PROFILE_H__
#define __DH_PROFILE_H__

#include <gtk/gtk.h>

#include "dh-book-manager.h"

G_BEGIN_DECLS

typedef struct _DhProfile         DhProfile;
typedef struct _DhProfileClass    DhProfileClass;

#define DH_TYPE_PROFILE         (dh_profile_get_type ())
#define DH_PROFILE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DH_TYPE_PROFILE, DhProfile))
#define DH_PROFILE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), DH_TYPE_PROFILE, DhProfileClass))
#define DH_IS_PROFILE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DH_TYPE_PROFILE))
#define DH_IS_PROFILE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DH_TYPE_PROFILE))
#define DH_PROFILE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DH_TYPE_PROFILE, DhProfileClass))

struct _DhProfile {
        GObject parent_instance;
};

struct _DhProfileClass {
        GObjectClass parent_class;
};

GType      dh_profile_get_type  (void) G_GNUC_CONST;
DhProfile *dh_profile_new       (const gchar *id,
                                 const gchar *name,
                                 const gchar * const *paths);
void       dh_profile_populate  (DhProfile *self);

const gchar        *dh_profile_get_id            (DhProfile *self);
const gchar        *dh_profile_get_name          (DhProfile *self);
const gchar *const *dh_profile_get_paths         (DhProfile *self);
DhBookManager      *dh_profile_peek_book_manager (DhProfile *self);


G_END_DECLS

#endif /* __DH_PROFILE_H__ */
