/* 
 * PiTiVi
 * Copyright (C) <2004> Edward G. Hervey <hervey_e@epita.fr>
 *                      Guillaume Casanova <casano_g@epita.fr>
 *
 * This software has been written in EPITECH <http://www.epitech.net>
 * EPITECH is a computer science school in Paris - FRANCE -
 * under the direction of Flavien Astraud and Jerome Landrieu.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef PITIVI_PITIVISETTINGS_H
#define PITIVI_PITIVISETTINGS_H

/*
 * Potentially, include other headers on which this header depends.
 */
#include <gst/gst.h>
#include <string.h>



/*
 * Type macros.
 */

#define PITIVI_PITIVISETTINGS_TYPE (pitivi_pitivisettings_get_type ())
#define PITIVI_PITIVISETTINGS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PITIVI_PITIVISETTINGS_TYPE, PitiviPitiviSettings))
#define PITIVI_PITIVISETTINGS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PITIVI_PITIVISETTINGS_TYPE, PitiviPitiviSettingsClass))
#define PITIVI_IS_PITIVISETTINGS(obj) (G_TYPE_CHECK_TYPE ((obj), PITIVI_PITIVISETTINGS_TYPE))
#define PITIVI_IS_PITIVISETTINGS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PITIVI_PITIVISETTINGS_TYPE))
#define PITIVI_PITIVISETTINGS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), PITIVI_PITIVISETTINGS_TYPE, PitiviPitiviSettingsClass))


typedef struct _PitiviPitiviSettings PitiviPitiviSettings;
typedef struct _PitiviPitiviSettingsClass PitiviPitiviSettingsClass;
typedef struct _PitiviPitiviSettingsPrivate PitiviPitiviSettingsPrivate;
typedef struct _PitiviPitiviSettingsMimeType PitiviPitiviSettingsMimeType;

enum {
  ENC_LIST,
  DEC_LIST
};


struct _PitiviPitiviSettingsMimeType
{
  gchar		*flux;
  GList		*encoder;
  GList		*decoder;
};


struct _PitiviPitiviSettings
{
  GObject parent;

  /* instance public members */
  GList		*container;
  GList		*codec;
  GList		*element;
  
  /* private */
  PitiviPitiviSettingsPrivate *private;
};


struct _PitiviPitiviSettingsClass
{
  GObjectClass parent;
  /* class members */
};




/* used by PITIVI_PITIVISETTINGS_TYPE */
GType pitivi_pitivisettings_get_type (void);

/*
 * Method definitions.
 */

PitiviPitiviSettings	*pitivi_pitivisettings_new(void);
GList			*pitivi_pitivisettings_get_flux_codec_list (GObject *object, 
								    gchar *flux, 
								    gboolean LIST);
GList			*pitivi_pitivisettings_get_flux_container_list (GObject *object, 
									gchar *flux, 
									gboolean LIST);


#endif
