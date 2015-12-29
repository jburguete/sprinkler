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
 * \file jet.c
 * \brief Source file to define the data of a sprinkler main jet.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
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
#include "jet.h"

#define DEBUG_JET 0             ///< macro to debug jet functions.

/**
 * \fn double jet_height (Jet * j, double x)
 * \brief function to calculate the height level of a sprinkler main jet.
 * \param j
 * \brief Jet struct.
 * \param x
 * \brief distance to the sprinkler.
 */
double
jet_height (Jet * j, double x)
{
  double *a;
  a = j->a;
  return a[0] + x * (a[1] + x * (a[2] + x * (a[3] + x * a[4])));
}

/**
 * \fn void jet_error (char *message)
 * \brief function to show an error message opening a Jet struct.
 * \param message
 * \brief error message.
 */
void
jet_error (char *message)
{
  error_message = g_strconcat (gettext ("Jet file"), ": ", message, NULL);
}

/**
 * \fn void jet_open_console (Jet * j)
 * \brief function to open a Jet struct in console.
 * \param j
 * \brief Jet struct.
 */
void
jet_open_console (Jet * j)
{
  double *a;
#if DEBUG_JET
  fprintf (stderr, "jet_open_console: start\n");
#endif
  a = j->a;
  printf ("Jet a0: ");
  scanf ("%lf", a);
  printf ("Jet a1: ");
  scanf ("%lf", a + 1);
  printf ("Jet a2: ");
  scanf ("%lf", a + 2);
  printf ("Jet a3: ");
  scanf ("%lf", a + 3);
  printf ("Jet a4: ");
  scanf ("%lf", a + 4);
#if DEBUG_JET
  fprintf (stderr, "jet_open_console: end\n");
#endif
}

/**
 * \fn int jet_open_xml (Jet * j, xmlNode *node)
 * \brief function to open a Jet struct on a XML node.
 * \param j
 * \brief Jet struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
jet_open_xml (Jet * j, xmlNode * node)
{
  int k;
#if DEBUG_JET
  fprintf (stderr, "jet_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_JET))
    {
      jet_error (gettext ("bad label"));
      goto exit_on_error;
    }
  j->a[0] = xml_node_get_float_with_default (node, XML_A0, 0., &k);
  if (!k)
    {
      jet_error (gettext ("bad a0"));
      goto exit_on_error;
    }
  j->a[1] = xml_node_get_float_with_default (node, XML_A1, 0., &k);
  if (!k)
    {
      jet_error (gettext ("bad a1"));
      goto exit_on_error;
    }
  j->a[2] = xml_node_get_float_with_default (node, XML_A2, 0., &k);
  if (!k)
    {
      jet_error (gettext ("bad a2"));
      goto exit_on_error;
    }
  j->a[3] = xml_node_get_float_with_default (node, XML_A3, 0., &k);
  if (!k)
    {
      jet_error (gettext ("bad a3"));
      goto exit_on_error;
    }
  j->a[4] = xml_node_get_float_with_default (node, XML_A4, 0., &k);
  if (!k)
    {
      jet_error (gettext ("bad a4"));
      goto exit_on_error;
    }
#if DEBUG_JET
  fprintf (stderr, "jet_open_xml: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_JET
  fprintf (stderr, "jet_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn void trajectory_invert_with_jet (Trajectory *t, Air *a, Jet * j)
 * \brief function to calculate the inverse drop trajectory to the sprinkler
 *   main jet.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_invert_with_jet (Trajectory * t, Air * a, Jet * j)
{
  char *buffer;
  double x[9];
  Drop *drop;
  FILE *file;
  double dt, h1, h2, xmin;
#if DEBUG_JET
  fprintf (stderr, "trajectory_invert_with_jet: start\n");
#endif
  t->t = 0.;
  drop = t->drop;
  h1 = h2 = jet_height (j, drop->r[0]) - drop->r[2];
  xmin = drop->r[0];
#if DEBUG_JET
  fprintf (stderr, "trajectory_invert_with_jet: h1=%lg\n", h1);
#endif
  for (dt = t->dt; drop->r[2] > t->bed_level && drop->r[0] > 0. && h1 > 0.;)
    {
      trajectory_write (t);
      t->dt = -fmin (dt, t->cfl / drop_move (drop, a, 1.));
      trajectory_runge_kutta_4 (t, a, 1.);
      h1 = jet_height (j, drop->r[0]) - drop->r[2];
      if (h1 < h2)
        {
          h2 = h1;
          xmin = drop->r[0];
        }
    }
  if (drop->r[2] < t->bed_level)
    trajectory_impact_correction (t, a);
  if (drop->r[0] < 0.)
    trajectory_impact_correction (t, a);
  trajectory_write (t);
  fclose (t->file);
  t->file = fopen (t->filename, "r");
  file = fopen ("out", "w");
  while (fscanf (t->file, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", x, x + 1, x + 2, x + 3,
                 x + 4, x + 5, x + 6, x + 7, x + 8) == 9 && x[1] >= xmin)
    fprintf (file, "%lg %lg %lg %lg %lg %lg %lg %lg %lg\n", x[0], x[1], x[2],
             x[3], x[4], x[5], x[6], x[7], x[8]);
  fclose (t->file);
  fclose (file);
  buffer = g_strconcat ("mv out ", t->filename, NULL);
  system (buffer);
  g_free (buffer);
  g_free (t->filename);
#if DEBUG_JET
  fprintf (stderr, "trajectory_invert_with_jet: end\n");
#endif
}

#if HAVE_GTK

/**
 * \fn void dialog_jet_new (Jet * j)
 * \brief function to set the data of a sprinkler main jet in a GtkDialog.
 * \param j
 * \brief Jet struct.
 */
void
dialog_jet_new (Jet * j)
{
  char *label[5] = { "a0", "a1", "a2", "a3", "a4" };
  DialogJet dlg[1];
  unsigned int i;

#if DEBUG_JET
  fprintf (stderr, "dialog_jet_new: start\n");
#endif

  dlg->grid = (GtkGrid *) gtk_grid_new ();
  for (i = 0; i < 5; ++i)
    {
      dlg->label_a[i] = (GtkLabel *) gtk_label_new ("x");
      dlg->spin_a[i] = (GtkSpinButton *)
        gtk_spin_button_new_with_range (-10., 10., 1e-9);
      gtk_spin_button_set_value (dlg->spin_a[i], j->a[i]);
      gtk_grid_attach (dlg->grid, GTK_WIDGET (dlg->label_a[i]), 0, i, 1, 1);
      gtk_grid_attach (dlg->grid, GTK_WIDGET (dlg->spin_a[i]), 1, i, 1, 1);
    }

  dlg->window =
    (GtkDialog *) gtk_dialog_new_with_buttons ("Datos del chorro del aspersor",
                                               window_parent, GTK_DIALOG_MODAL,
                                               GTK_STOCK_OK, GTK_RESPONSE_OK,
                                               GTK_STOCK_CANCEL,
                                               GTK_RESPONSE_CANCEL, NULL);
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (dlg->window)),
                     GTK_WIDGET (dlg->grid));
  gtk_widget_show_all (GTK_WIDGET (dlg->window));

  if (gtk_dialog_run (dlg->window) == GTK_RESPONSE_OK)
    for (i = 0; i < 5; ++i)
      j->a[i] = gtk_spin_button_get_value (dlg->spin_a[i]);
  gtk_widget_destroy ((GtkWidget *) dlg->window);

#if DEBUG_JET
  fprintf (stderr, "dialog_jet_new: end\n");
#endif

}

#endif
