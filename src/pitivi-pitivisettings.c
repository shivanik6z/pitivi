/* 
 * PiTiVi
 * Copyright (C) <2004> delettrez marc <delett_m@epita.fr>
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


/*
 * A FAIRE : Dans dispose() g_free de la liste des elements 
 */


#include "pitivi.h"
#include "pitivi-pitivisettings.h"


static     GObjectClass *parent_class;


struct _PitiviPitiviSettingsPrivate
{
  /* instance private members */
  gboolean	dispose_has_run;
};


/*
 * forward definitions
 */





/*
 * * * * * * * * * * Insert "added-value" functions here * * * * * * * * * * 
 */


void		pitivisettings_free_mime_type (PitiviPitiviSettingsMimeType *mime_type)
{
  g_free (mime_type->flux);
  g_list_free (mime_type->encoder);
  g_list_free (mime_type->decoder);
  g_free (mime_type);
  return ;
}


void		pitivisettings_free_list_all (GList *list)
{
  GList		*tmp;

  for (tmp = list; list; list = list->next) {
    pitivisettings_free_mime_type (list->data);
  }
  g_list_free (tmp);
  return ;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* 
   affiche les infos d un element
*/
void		aff_info_factory (GstElementFactory *factory)
{
  g_print ("%s\t%s\t%s\n", 
	   gst_plugin_feature_get_name (GST_PLUGIN_FEATURE(factory)),
	   gst_element_factory_get_longname (factory), 
	   gst_element_factory_get_klass (factory)
	   );
  return ;
}


/* 
   affiche la list des coder 
   (Encoder|Decoder)
*/
void		aff_coder (GList *list)
{
  for (; list; list = g_list_next (list)) {
    g_print ("    %s\n", (gchar *) list->data);
  }
  return ;
}


/* 
   affiche la structure d un flux
*/
void		aff_mime_type (PitiviPitiviSettingsMimeType *mime_type)
{
  g_print ("%s\n", mime_type->flux);
  g_print ("  Encoder:\n");
  aff_coder (mime_type->encoder);
  g_print ("  Decoder:\n");
  aff_coder (mime_type->decoder);
  return ;
}


/* 
   affiche le contenu de la list 
   (Container|Codec)
*/
void		aff_all_list (GList *list)
{
  for (; list; list = g_list_next (list)) {
    aff_mime_type ((PitiviPitiviSettingsMimeType *) list->data);
  }  
  return ;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* 
   initialise une nouvelle structure 
   pour un nouveau flux
*/
PitiviPitiviSettingsMimeType *
init_mime_type (gchar *flux)
{
  PitiviPitiviSettingsMimeType *new;

  new = g_malloc (sizeof (PitiviPitiviSettingsMimeType));
  new->flux = g_strdup (flux);
  new->encoder = 0;
  new->decoder = 0;
  return (new);
}


/* 
   retourne la structure assigne au flux
   si elle existe sinon retourne NULL
*/
PitiviPitiviSettingsMimeType *
search_flux (GList *list, gchar *flux)
{
  PitiviPitiviSettingsMimeType *tmp;
  
  for (; list; list = g_list_next (list)) {
    tmp = (PitiviPitiviSettingsMimeType *) list->data;
    if ((!strcmp(tmp->flux, flux))) {
      return (tmp);
    }
  }
  return (0);
}


/* 
   parcours la list 
   et retourne la list des coder demande
   (Encoder|Decoder) assigne au flux
   si le flux n existe pas retourne -1
   si la valeur de retour est NULL 
   c est que la list est vide
*/
GList *
get_flux_coder_list (GList *list, gchar *flux, gboolean LIST)
{
  PitiviPitiviSettingsMimeType *tmp;

  if ((tmp = search_flux (list, flux))) {
    if (LIST == DEC_LIST) {
      return (tmp->decoder);
    } else if (LIST == ENC_LIST) {
      return (tmp->encoder);
    } else {
      g_print ("Don't know this list\n");
    }
  }
  return (-1);
}


/* 
   ajoute un l element factory name
   dans la list (encoder|decoder) 
   assignee au flux tmp->flux
   suivant son pad  (src|sink) 
*/
PitiviPitiviSettingsMimeType *
ajout_factory_element (PitiviPitiviSettingsMimeType *tmp, 
		       gchar *element, gboolean MY_PAD)
{

  if (MY_PAD == GST_PAD_SRC) {
    tmp->encoder = g_list_append (tmp->encoder, (gpointer) element);
  } else if (MY_PAD == GST_PAD_SINK) {
    tmp->decoder = g_list_append (tmp->decoder, (gpointer) element);
  } else {
    g_print ("ERROR in (ajout_factory_element) : MY_PAD \n");
  }

  return (tmp);
}


/* 
   recupere les flux gerer par l element
   si le flux existe dans la list
   ajoute un l element dans la structure de flux
   sinon cree la struct du flux et lui assigne l element
   
*/
GList *
pitivisettings_ajout_element (GList *list, GstElementFactory *factory, gboolean MY_PAD)
{
  GstPadTemplate *padtemplate;

  if (factory->numpadtemplates) {
    gint i;
    const GList *pads;
    
    pads = factory->padtemplates;
    for (i = 0; pads; i++, pads = g_list_next (pads)) {
      padtemplate = (GstPadTemplate *) (pads->data);
      if (padtemplate->direction == MY_PAD) {
	gint j;
	
	for (j = 0; j < padtemplate->caps->structs->len; j++) {
	  PitiviPitiviSettingsMimeType *tmp;
	  
	  /* CHERCHE SI LE TYPE EST DEJA DEFINI */
	  if ((tmp = search_flux (list, gst_structure_to_string (gst_caps_get_structure (padtemplate->caps, j))))) {
	    tmp = ajout_factory_element (tmp, 
					 (gchar *) gst_plugin_feature_get_name (GST_PLUGIN_FEATURE(factory)), 
					 MY_PAD);
	  } else {
	    /* SINON L AJOUTE */
	    tmp = init_mime_type (gst_structure_to_string (gst_caps_get_structure (padtemplate->caps, j)));
	    tmp = ajout_factory_element (tmp, 
					 (gchar *) gst_plugin_feature_get_name (GST_PLUGIN_FEATURE(factory)), 
					 MY_PAD);
	    list = g_list_append (list, (gpointer) tmp);
	  }
	}
      }      
    }
  }
  return (list);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


GList			*pitivi_pitivisettings_get_flux_codec_list (GObject *object, gchar *flux, gboolean LIST)
{
  PitiviPitiviSettings *self = (PitiviPitiviSettings *) object;

  return (get_flux_coder_list (self->codec, flux, LIST));
}


GList			*pitivi_pitivisettings_get_flux_container_list (GObject *object, gchar *flux, gboolean LIST)
{
  PitiviPitiviSettings *self = (PitiviPitiviSettings *) object;

  return (get_flux_coder_list (self->container, flux, LIST));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


PitiviPitiviSettings *
pitivi_pitivisettings_new (void)
{
  PitiviPitiviSettings	*pitivisettings;

  pitivisettings = (PitiviPitiviSettings *) g_object_new(PITIVI_PITIVISETTINGS_TYPE, NULL);
  g_assert(pitivisettings != NULL);
  return pitivisettings;
}

static GObject *
pitivi_pitivisettings_constructor (GType type,
			     guint n_construct_properties,
			     GObjectConstructParam * construct_properties)
{
  GObject *obj;
  /* Invoke parent constructor. */
  obj = parent_class->constructor (type, n_construct_properties,
				   construct_properties);

  /* do stuff. */

  return obj;
}

static void
pitivi_pitivisettings_instance_init (GTypeInstance * instance, gpointer g_class)
{
  GList			*sv;
  GstElementFactory	*factory;
  PitiviPitiviSettings	*self = (PitiviPitiviSettings *) instance;

  self->private = g_new0(PitiviPitiviSettingsPrivate, 1);
  
  /* initialize all public and private members to reasonable default values. */ 
  
  self->private->dispose_has_run = FALSE;
  
  /* Do only initialisation here */
  /* The construction of the object should be done in the Constructor
     So that properties set at instanciation can be set */

  self->codec = 0;
  self->container = 0;
  self->element = 0;

  self->element = gst_registry_pool_feature_list (GST_TYPE_ELEMENT_FACTORY);
  sv = self->element;
  while (sv) {
    factory = (GstElementFactory *) sv->data;
    if (!strncmp (gst_element_factory_get_klass (factory), "Codec/Demuxer", 13)) {
      self->container = pitivisettings_ajout_element (self->container, factory, GST_PAD_SINK);
    } else if (!strncmp (gst_element_factory_get_klass (factory), "Codec/Muxer", 11)) {
      self->container = pitivisettings_ajout_element (self->container, factory, GST_PAD_SRC);
    } else if (!strncmp (gst_element_factory_get_klass (factory), "Codec/Encoder/Audio", 19) || 
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Audio/Encoder", 19) ||
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Video/Encoder", 19) ||
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Encoder/Video", 19)
	       ) {
      self->codec = pitivisettings_ajout_element (self->codec, factory, GST_PAD_SRC);
    } else if (!strncmp (gst_element_factory_get_klass (factory), "Codec/Audio/Decoder", 19) ||
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Decoder/Audio", 19) ||
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Decoder/Video", 19) ||
	       !strncmp (gst_element_factory_get_klass (factory), "Codec/Video/Decoder", 19)
	       ) {
      self->codec = pitivisettings_ajout_element (self->codec, factory, GST_PAD_SINK);      
    }
    sv = sv->next;
  }

  aff_all_list (self->codec);
  aff_all_list (self->container);

}

static void
pitivi_pitivisettings_dispose (GObject *object)
{
  PitiviPitiviSettings	*self = PITIVI_PITIVISETTINGS(object);

  /* If dispose did already run, return. */
  if (self->private->dispose_has_run)
    return;
  
  /* Make sure dispose does not run twice. */
  self->private->dispose_has_run = TRUE;	

  /* 
   * In dispose, you are supposed to free all types referenced from this 
   * object which might themselves hold a reference to self. Generally, 
   * the most simple solution is to unref all members on which you own a 
   * reference. 
   */

  pitivisettings_free_list_all (self->codec);
  pitivisettings_free_list_all (self->container);

  /*
  g_list_free (self->element);
  */


  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
pitivi_pitivisettings_finalize (GObject *object)
{
  PitiviPitiviSettings	*self = PITIVI_PITIVISETTINGS(object);

  /* 
   * Here, complete object destruction. 
   * You might not need to do much... 
   */

  g_free (self->private);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
pitivi_pitivisettings_set_property (GObject * object,
			      guint property_id,
			      const GValue * value, GParamSpec * pspec)
{
  PitiviPitiviSettings *self = (PitiviPitiviSettings *) object;

  switch (property_id)
    {
      /*   case PITIVI_PITIVISETTINGS_PROPERTY: { */
      /*     g_free (self->private->name); */
      /*     self->private->name = g_value_dup_string (value); */
      /*     g_print ("maman: %s\n",self->private->name); */
      /*   } */
      /*     break; */
    default:
      /* We don't have any other property... */
      g_assert (FALSE);
      break;
    }
}

static void
pitivi_pitivisettings_get_property (GObject * object,
			      guint property_id,
			      GValue * value, GParamSpec * pspec)
{
  PitiviPitiviSettings *self = (PitiviPitiviSettings *) object;

  switch (property_id)
    {
      /*  case PITIVI_PITIVISETTINGS_PROPERTY: { */
      /*     g_value_set_string (value, self->private->name); */
      /*   } */
      /*     break; */
    default:
      /* We don't have any other property... */
      g_assert (FALSE);
      break;
    }
}

static void
pitivi_pitivisettings_class_init (gpointer g_class, gpointer g_class_data)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
  PitiviPitiviSettingsClass *klass = PITIVI_PITIVISETTINGS_CLASS (g_class);

  parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (g_class));

  gobject_class->constructor = pitivi_pitivisettings_constructor;
  gobject_class->dispose = pitivi_pitivisettings_dispose;
  gobject_class->finalize = pitivi_pitivisettings_finalize;

  gobject_class->set_property = pitivi_pitivisettings_set_property;
  gobject_class->get_property = pitivi_pitivisettings_get_property;

  /* Install the properties in the class here ! */
  /*   pspec = g_param_spec_string ("maman-name", */
  /*                                "Maman construct prop", */
  /*                                "Set maman's name", */
  /*                                "no-name-set" /\* default value *\/, */
  /*                                G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE); */
  /*   g_object_class_install_property (gobject_class, */
  /*                                    MAMAN_BAR_CONSTRUCT_NAME, */
  /*                                    pspec); */


}

GType
pitivi_pitivisettings_get_type (void)
{
  static GType type = 0;
 
  if (type == 0)
    {
      static const GTypeInfo info = {
	sizeof (PitiviPitiviSettingsClass),
	NULL,			/* base_init */
	NULL,			/* base_finalize */
	pitivi_pitivisettings_class_init,	/* class_init */
	NULL,			/* class_finalize */
	NULL,			/* class_data */
	sizeof (PitiviPitiviSettings),
	0,			/* n_preallocs */
	pitivi_pitivisettings_instance_init	/* instance_init */
      };
      type = g_type_register_static (G_TYPE_OBJECT,
				     "PitiviPitiviSettingsType", &info, 0);
    }

  return type;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


