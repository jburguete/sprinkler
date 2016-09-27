/*
Sprinkler: a software to calculate drop trajectories in sprinkler irrigation.

AUTHORS: Javier Burguete.

Copyright 2012-2015, AUTHORS.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

/**
 * \file sprinkler.c
 * \brief Source file to define the sprinkler data.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <libintl.h>
#include <gsl/gsl_rng.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"
#include "measurement.h"
#include "trajectory.h"
#include "sprinkler.h"

#define DEBUG_SPRINKLER 1       ///< macro to debug sprinkler functions.

/**
 * \fn void trajectory_init_with_sprinkler (Trajectory *t, Sprinkler *s)
 * \brief function to init drop trajectory variables from sprinkler data.
 * \param t
 * \brief Trajectory struct.
 * \param s
 * \brief Sprinkler struct.
 */
void
trajectory_init_with_sprinkler (Trajectory * t, Sprinkler * s)
{
  Drop *d;
  double v, k2, k3;
#if DEBUG_SPRINKLER
  fprintf (stderr, "trajectory_init_with_sprinkler: start\n");
#endif
  d = t->drop;
  t->t = 0.;
  t->cfl = s->cfl;
  t->dt = s->dt;
  t->bed_level = s->bed_level;
  t->drop_maximum_diameter = s->drop_dmax;
  d->r[0] = s->x;
  d->r[1] = s->y;
  d->r[2] = s->z;
  v = sqrt (2. * s->pressure / d->density);
  k2 = s->horizontal_angle * M_PI / 180.;
  k3 = s->vertical_angle * M_PI / 180.;
  d->v[0] = v * cos (k3) * cos (k2);
  d->v[1] = v * cos (k3) * sin (k2);
  d->v[2] = v * sin (k3);
#if DEBUG_SPRINKLER
  fprintf (stderr, "trajectory_init_with_sprinkler: Time step size=%le\n",
		   t->dt);
  fprintf (stderr, "trajectory_init_with_sprinkler: Water pressure=%le "
		   "Drop density=%le\n", s->pressure, d->density);
  fprintf (stderr, "trajectory_init_with_sprinkler: "
		   "Drop velocity=(%le,%le,%le)\n", d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_init_with_sprinkler: Jet time=%lg\n",
		   t->jet_time);
  fprintf (stderr, "trajectory_init_with_sprinkler: end\n");
#endif
}

/**
 * \fn void trajectory_open_with_sprinkler (Trajectory *t, Sprinkler *s, \
 *   Air * a, gsl_rng * rng)
 * \brief function to init drop trajectory variables from sprinkler data.
 * \param t
 * \brief Trajectory struct.
 * \param s
 * \brief Sprinkler struct.
 * \param a
 * \brief Air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 */
void
trajectory_open_with_sprinkler (Trajectory * t, Sprinkler * s, Air * a,
		                        gsl_rng *rng)
{
  double diameter;
#if DEBUG_SPRINKLER
  fprintf (stderr, "trajectory_open_with_sprinkler: start\n");
#endif
  diameter
    = s->drop_dmin + (s->drop_dmax - s->drop_dmin) * gsl_rng_uniform (rng);
  s->horizontal_angle
    = s->angle_min + (s->angle_max - s->angle_min) * gsl_rng_uniform (rng);
  trajectory_open_data (t, a, rng, diameter, s->jet_model, s->detach_model,
		                s->drag_model, s->drag_coefficient, s->drop_dmax);
#if DEBUG_SPRINKLER
  fprintf (stderr, "trajectory_open_with_sprinkler: end\n");
#endif
}

/**
 * \fn void sprinkler_error (char *message)
 * \brief function to show an error message opening a Sprinkler struct.
 * \param message
 * \brief error message.
 */
void
sprinkler_error (char *message)
{
  error_message = g_strconcat (gettext ("Sprinkler file"), ": ", message, NULL);
}

/**
 * \fn void sprinkler_open_console (Sprinkler *s)
 * \brief function to input a Sprinkler struct in console.
 * \param s
 * \brief Sprinkler struct.
 */
void
sprinkler_open_console (Sprinkler * s)
{
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_console: start\n");
#endif
  printf ("Sprinkler x: ");
  scanf ("%lf", &(s->x));
  printf ("Sprinkler y: ");
  scanf ("%lf", &(s->y));
  printf ("Sprinkler z: ");
  scanf ("%lf", &(s->z));
  printf ("Sprinkler pressure: ");
  scanf ("%lf", &(s->pressure));
  printf ("Jet vertical angle: ");
  scanf ("%lf", &(s->vertical_angle));
  printf ("Jet horizontal angle: ");
  scanf ("%lf", &(s->horizontal_angle));
  printf ("Jet time: ");
  scanf ("%lf", &(s->jet_time));
  printf ("Nozzle diameter: ");
  scanf ("%lf", &(s->diameter));
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_console: end\n");
#endif
}

/**
 * \fn int sprinkler_open_xml (Sprinkler *s, xmlNode *node)
 * \brief function to open a Sprinkler struct on a XML node.
 * \param s
 * \brief Sprinkler struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
sprinkler_open_xml (Sprinkler * s, xmlNode * node)
{
  xmlChar *buffer;
  int k;
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_SPRINKLER))
    {
      sprinkler_error (gettext ("bad label"));
      goto exit_on_error;
    }
  s->x = xml_node_get_float_with_default (node, XML_X, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad x"));
      goto exit_on_error;
    }
  s->y = xml_node_get_float_with_default (node, XML_Y, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad y"));
      goto exit_on_error;
    }
  s->z = xml_node_get_float_with_default (node, XML_Z, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad z"));
      goto exit_on_error;
    }
  s->pressure = xml_node_get_float (node, XML_PRESSURE, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad pressure"));
      goto exit_on_error;
    }
  s->vertical_angle = xml_node_get_float (node, XML_VERTICAL_ANGLE, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad vertical angle"));
      goto exit_on_error;
    }
  s->horizontal_angle
    = xml_node_get_float_with_default (node, XML_HORIZONTAL_ANGLE, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad horizontal angle"));
      goto exit_on_error;
    }
  s->bed_level = xml_node_get_float_with_default (node, XML_BED_LEVEL, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad bed level"));
      goto exit_on_error;
    }
  s->jet_time = xml_node_get_float_with_default (node, XML_JET_TIME, 0., &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad jet length"));
      goto exit_on_error;
    }
  s->diameter = xml_node_get_float (node, XML_DIAMETER, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad nozzle diameter"));
      goto exit_on_error;
    }
  s->dt = xml_node_get_float (node, XML_DT, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad time step size"));
      goto exit_on_error;
    }
  s->cfl = xml_node_get_float (node, XML_CFL, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad CFL number"));
      goto exit_on_error;
    }
	s->drop_dmax
	  = xml_node_get_float_with_default (node, XML_MAXIMUM_DROP_DIAMETER,
			                             MAXIMUM_DROP_DIAMETER, &k);
   if (!k)
     {
       sprinkler_error (gettext ("bad maximum drop diameter"));
       goto exit_on_error;
     }
  s->ntrajectories
    = xml_node_get_uint_with_default (node, XML_DROPS_NUMBER, 0, &k);
  if (!k)
    {
      sprinkler_error (gettext ("bad drops number"));
      goto exit_on_error;
    }
  if (s->ntrajectories)
	{
	  s->drop_dmin
		= xml_node_get_float_with_default (node, XML_MINIMUM_DROP_DIAMETER,
				                           MINIMUM_DROP_DIAMETER, &k);
      if (!k)
        {
          sprinkler_error (gettext ("bad minimum drop diameter"));
          goto exit_on_error;
        }
	  s->angle_min
		= xml_node_get_float_with_default (node, XML_MINIMUM_ANGLE, 0., &k);
      if (!k)
        {
          sprinkler_error (gettext ("bad minimum angle"));
          goto exit_on_error;
        }
	  s->angle_max
		= xml_node_get_float_with_default (node, XML_MAXIMUM_ANGLE, 0., &k);
      if (!k)
        {
          sprinkler_error (gettext ("bad maximum angle"));
          goto exit_on_error;
        }
      if (!xmlHasProp (node, XML_JET_MODEL))
        s->jet_model = 0;
      else
        {
          buffer = xmlGetProp (node, XML_JET_MODEL);
          if (!xmlStrcmp (buffer, XML_NULL_DRAG))
            s->jet_model = TRAJECTORY_JET_MODEL_NULL_DRAG;
          else if (!xmlStrcmp (buffer, XML_PROGRESSIVE))
            s->jet_model = TRAJECTORY_JET_MODEL_PROGRESSIVE;
          else if (!xmlStrcmp (buffer, XML_BIG_DROPS))
            s->jet_model = TRAJECTORY_JET_MODEL_BIG_DROPS;
          else
            {
              sprinkler_error (gettext ("unknown jet model"));
              xmlFree (buffer);
              goto exit_on_error;
            }
          xmlFree (buffer);
        }
      buffer = xmlGetProp (node, XML_DRAG_MODEL);
      if (!buffer)
        {
          sprinkler_error (gettext ("no drag model"));
          goto exit_on_error;
        }
      if (!xmlStrcmp (buffer, XML_CONSTANT))
        {
          s->drag_model = DROP_DRAG_MODEL_CONSTANT;
          s->drag_coefficient
            = xml_node_get_float_with_default (node, XML_DRAG, 0., &k);
          if (!k)
            {
              sprinkler_error (gettext ("bad drag value"));
              goto exit_on_error;
            }
        }
      else if (!xmlStrcmp (buffer, XML_SPHERE))
        s->drag_model = DROP_DRAG_MODEL_SPHERE;
      else if (!xmlStrcmp (buffer, XML_OVOID))
        s->drag_model = DROP_DRAG_MODEL_OVOID;
      else
        {
          sprinkler_error (gettext ("unknown drag resistance model"));
          goto exit_on_error;
        }
      xmlFree (buffer);
      buffer = xmlGetProp (node, XML_DETACH_MODEL);
      if (!buffer || !xmlStrcmp (buffer, XML_TOTAL))
        s->detach_model = DROP_DETACH_MODEL_TOTAL;
      else if (!xmlStrcmp (buffer, XML_RANDOM))
        s->detach_model = DROP_DETACH_MODEL_RANDOM;
      else
        {
          sprinkler_error (gettext ("unknown jet detach model"));
          goto exit_on_error;
        }
    }
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_xml: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn void sprinkler_run_console (Sprinkler * s, Air * a, Trajectory * t, \
 *   char *result)
 * \brief function to run the simulation input all data on console.
 * \param s
 * \brief sprinkler struct.
 * \param a
 * \brief Air struct.
 * \param t.
 * \brief Trajectory struct.
 * \param result
 * \brief result file name.
 */
void
sprinkler_run_console (Sprinkler * s, Air * a, Trajectory * t, char *result)
{
  gsl_rng *rng;
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_console: start\n");
#endif
  rng = gsl_rng_alloc (gsl_rng_taus);
  gsl_rng_set (rng, RANDOM_SEED);
  sprinkler_open_console (s);
  air_open_console (a);
  drop_open_console (t->drop, a);
  trajectory_open_console (t, a, result);
  t->jet_time = s->jet_time;
  trajectory_init (t, a, rng);
  trajectory_init_with_sprinkler (t, s);
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_console: end\n");
#endif
}

/**
 * \fn int sprinkler_run_xml (Sprinkler * s, Air * a, Trajectory * t, \
 *   xmlNode * node, char *result)
 * \brief function to run the simulation open all data in a XML file.
 * \param s
 * \brief sprinkler struct.
 * \param a
 * \brief Air struct.
 * \param t.
 * \brief Trajectory struct.
 * \param node
 * \brief XML node.
 * \param result
 * \brief result file name.
 * \return 1 on success, 0 on error.
 */
int
sprinkler_run_xml (Sprinkler * s, Air * a, Trajectory * t, xmlNode * node,
                   char *result)
{
  Measurement m[1];
  gsl_rng *rng;
  FILE *file;
  unsigned int i;
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_xml: start\n");
#endif
  file = NULL;
  rng = gsl_rng_alloc (gsl_rng_taus);
  gsl_rng_set (rng, RANDOM_SEED);
  if (!sprinkler_open_xml (s, node))
    goto exit_on_error;
  node = node->children;
  if (!air_open_xml (a, node))
    goto exit_on_error;
  s->nmeasurements = 0;
  for (node = node->next; node && !xmlStrcmp (node->name, XML_MEASUREMENT);
       node = node->next)
    {
      if (!measurement_open_xml (m, node))
        goto exit_on_error;
      s->measurement
        = g_realloc (s->measurement,
                     (s->nmeasurements + 1) * sizeof (Measurement));
      memcpy (s->measurement + s->nmeasurements, m, sizeof (Measurement));
	  ++s->nmeasurements;
    }
  if (s->nmeasurements)
	{
      file = fopen (result, "w");
	  if (!file)
        {
		  sprinkler_error (gettext ("unable to open the measurements file"));
		  goto exit_on_error;
		}
	}
  for (; node; node = node->next)
    {
      if (!trajectory_open_xml (t, a, node, result))
        goto exit_on_error;
      t->jet_time = s->jet_time;
      trajectory_init (t, a, rng);
      trajectory_init_with_sprinkler (t, s);
      air_wind_uncertainty (a, rng);
      trajectory_calculate (t, a, s->measurement, s->nmeasurements, file);
    }
  for (i = 0; i < s->ntrajectories; ++i)
	{
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_xml: trajectory=%u\n", i);
#endif
      trajectory_open_with_sprinkler (t, s, a, rng);
      t->jet_time = s->jet_time;
      trajectory_init (t, a, rng);
      trajectory_init_with_sprinkler (t, s);
      air_wind_uncertainty (a, rng);
      trajectory_calculate (t, a, s->measurement, s->nmeasurements, file);
    }
  if (file)
	fclose (file);
  gsl_rng_free (rng);
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_xml: end\n");
#endif
  return 1;

exit_on_error:
  show_error ();
  gsl_rng_free (rng);
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_run_xml: end\n");
#endif
  return 0;
}

#if HAVE_GTK

/**
 * \fn void dialog_sprinkler_new (Sprinkler * s)
 * \brief function to set the sprinkler data in a GtkDialog.
 * \param s
 * \brief Sprinkler struct.
 */
void
dialog_sprinkler_new (Sprinkler * s)
{
  DialogSprinkler dlg[1];

#if DEBUG_SPRINKLER
  fprintf (stderr, "dialog_sprinkler_new: start\n");
#endif

  dlg->label_x = (GtkLabel *) gtk_label_new ("x");
  dlg->spin_x = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-1000., 1000., 0.001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->x);
  dlg->label_y = (GtkLabel *) gtk_label_new ("y");
  dlg->spin_y = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-1000., 1000., 0.001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->y);
  dlg->label_z = (GtkLabel *) gtk_label_new ("z");
  dlg->spin_z = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-1000., 1000., 0.001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->z);
  dlg->label_pressure = (GtkLabel *) gtk_label_new (gettext ("Pressure"));
  dlg->spin_pressure = (GtkSpinButton *)
    gtk_spin_button_new_with_range (100000., 600000., 1.);
  gtk_spin_button_set_value (dlg->spin_diameter, s->pressure);
  dlg->label_vertical_angle
    = (GtkLabel *) gtk_label_new (gettext ("Vertical angle"));
  dlg->spin_vertical_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-360., 360., 0.1);
  gtk_spin_button_set_value (dlg->spin_diameter, s->vertical_angle);
  dlg->label_horizontal_angle
    = (GtkLabel *) gtk_label_new (gettext ("Horizontal angle"));
  dlg->spin_horizontal_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-360., 360., 0.1);
  gtk_spin_button_set_value (dlg->spin_diameter, s->horizontal_angle);
  dlg->label_jet_time = (GtkLabel *) gtk_label_new (gettext ("Jet time"));
  dlg->spin_jet_time = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 10., 0.001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->jet_length);
  dlg->label_diameter
    = (GtkLabel *) gtk_label_new (gettext ("Nozzle diameter"));
  dlg->spin_diameter = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0.0001, 0.0070, 0.0001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->diameter);

  dlg->grid = (GtkGrid *) gtk_grid_new ();
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_x, 0, 0, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_x, 1, 0, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_y, 0, 1, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_y, 1, 1, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_z, 0, 2, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_z, 1, 2, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_pressure, 0, 3, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_pressure, 1, 3, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_vertical_angle,
                   0, 4, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_vertical_angle,
                   1, 4, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_horizontal_angle,
                   0, 5, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_horizontal_angle,
                   1, 5, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_jet_length, 0, 6, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_jet_length, 1, 6, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_diameter, 0, 7, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_diameter, 1, 7, 1, 1);

  dlg->window =
    (GtkDialog *) gtk_dialog_new_with_buttons (gettext ("Sprinkler data"),
                                               window_parent, GTK_DIALOG_MODAL,
                                               GTK_STOCK_OK, GTK_RESPONSE_OK,
                                               GTK_STOCK_CANCEL,
                                               GTK_RESPONSE_CANCEL, NULL);
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (dlg->window)),
                     GTK_WIDGET (dlg->grid));
  gtk_widget_show_all (GTK_WIDGET (dlg->window));

  if (gtk_dialog_run (dlg->window) == GTK_RESPONSE_OK)
    {
      s->x = gtk_spin_button_get_value (dlg->spin_x);
      s->y = gtk_spin_button_get_value (dlg->spin_y);
      s->z = gtk_spin_button_get_value (dlg->spin_z);
      s->pressure = gtk_spin_button_get_value (dlg->spin_pressure);
      s->vertical_angle = gtk_spin_button_get_value (dlg->spin_vertical_angle);
      s->horizontal_angle
        = gtk_spin_button_get_value (dlg->spin_horizontal_angle);
      s->jet_time = gtk_spin_button_get_value (dlg->spin_jet_time);
      s->diameter = gtk_spin_button_get_value (dlg->spin_diameter);
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);

#if DEBUG_SPRINKLER
  fprintf (stderr, "dialog_sprinkler_new: end\n");
#endif

}

#endif
