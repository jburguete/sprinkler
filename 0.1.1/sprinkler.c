/*
Sprinkler: a software to calculate sprinkler trajectories in sprinkler irrigation.

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
#include "trajectory.h"
#include "sprinkler.h"

#define DEBUG_SPRINKLER 0       ///< macro to debug sprinkler functions.

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
  d->r[0] = s->x;
  d->r[1] = s->y;
  d->r[2] = s->z;
  v = sqrt (2. * s->pressure / d->density);
  k2 = s->horizontal_angle * M_PI / 180.;
  k3 = s->vertical_angle * M_PI / 180.;
  d->v[0] = v * cos (k3) * cos (k2);
  d->v[1] = v * cos (k3) * sin (k2);
  d->v[2] = v * sin (k3);
  t->jet_length = s->jet_length;
  printf ("Time step size: %le\n", t->dt);
  printf ("Water pressure: %le\nDrop density: %le\n", s->pressure, d->density);
  printf ("Drop velocity: (%le,%le,%le)\n", d->v[0], d->v[1], d->v[2]);
#if DEBUG_SPRINKLER
  fprintf (stderr, "trajectory_init_with_sprinkler: end\n");
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
  error_message	= g_strconcat (gettext ("Sprinkler file"), ": ", message, NULL);
}

/**
 * \fn void sprinkler_open_file (Sprinkler *s, FILE *file)
 * \brief function to open a Sprinkler struct in a file.
 * \param s
 * \brief Sprinkler struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
sprinkler_open_file (Sprinkler * s, FILE * file)
{
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_file: start\n");
#endif
  if (fscanf (file, "%lf%lf%lf%lf%lf%lf%lf%lf",
              &(s->x),
              &(s->y),
              &(s->z),
              &(s->pressure),
              &(s->vertical_angle),
              &(s->horizontal_angle), &(s->jet_length), &(s->diameter)) == 8)
    {
      sprinkler_error (gettext ("unable to open the data"));
      goto exit_on_error;
    }
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_file: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_SPRINKLER
  fprintf (stderr, "sprinkler_open_file: end\n");
#endif
  return 0;
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
  printf ("Jet length: ");
  scanf ("%lf", &(s->jet_length));
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
  s->jet_length
    = xml_node_get_float_with_default (node, XML_JET_LENGTH, 0., &k);
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
  dlg->label_pressure = (GtkLabel *) gtk_label_new ("pressure");
  dlg->spin_pressure = (GtkSpinButton *)
    gtk_spin_button_new_with_range (100000., 600000., 1.);
  gtk_spin_button_set_value (dlg->spin_diameter, s->pressure);
  dlg->label_vertical_angle = (GtkLabel *) gtk_label_new ("vertical_angle");
  dlg->spin_vertical_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-360., 360., 0.1);
  gtk_spin_button_set_value (dlg->spin_diameter, s->vertical_angle);
  dlg->label_horizontal_angle = (GtkLabel *) gtk_label_new ("horizontal_angle");
  dlg->spin_horizontal_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (-360., 360., 0.1);
  gtk_spin_button_set_value (dlg->spin_diameter, s->horizontal_angle);
  dlg->label_jet_length = (GtkLabel *) gtk_label_new ("jet_length");
  dlg->spin_jet_length = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 10., 0.001);
  gtk_spin_button_set_value (dlg->spin_diameter, s->jet_length);
  dlg->label_diameter = (GtkLabel *) gtk_label_new ("Diámetro de la boquilla");
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
    (GtkDialog *) gtk_dialog_new_with_buttons ("Datos del aspersor",
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
      s->jet_length = gtk_spin_button_get_value (dlg->spin_jet_length);
      s->diameter = gtk_spin_button_get_value (dlg->spin_diameter);
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);

#if DEBUG_SPRINKLER
  fprintf (stderr, "dialog_sprinkler_new: end\n");
#endif

}

#endif
