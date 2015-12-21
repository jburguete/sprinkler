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
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "sprinkler.h"

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
  return fscanf (file, "%lf%lf%lf%lf%lf%lf%lf%lf",
                 &(s->x),
                 &(s->y),
                 &(s->z),
                 &(s->pressure),
                 &(s->vertical_angle),
                 &(s->horizontal_angle),
                 &(s->jet_length), &(s->diameter)) == 8;
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
}

#endif
