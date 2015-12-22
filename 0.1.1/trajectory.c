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
 * \file trajectory.c
 * \brief File to calculate the trajectory of a drop with the ballistic model.
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
#include "trajectory.h"

#define DEBUG_TRAJECTORY 0      ///< macro to debug trajectory functions.

/**
 * \fn void trajectory_error (char *message)
 * \brief function to show an error message opening a Trajectory struct.
 * \param message
 * \brief error message.
 */
void
trajectory_error (char *message)
{
  error_message
	= g_strconcat (gettext ("Trajectory file"), ": ", message, NULL);
}

/**
 * \fn int trajectory_open_file (Trajectory * t, Air *a, FILE * file)
 * \brief function to open a Trajectory struct in a file.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
trajectory_open_file (Trajectory * t, Air * a, FILE * file)
{
  char buffer[512];
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_file: start\n");
#endif
  if (!drop_open_file (t->drop, a, file))
    goto exit_on_error;
  if (fscanf (file, "%lf%lf%lf%s", &(t->bed_level), &(t->dt), &(t->cfl), buffer)
      != 4)
    {
      trajectory_error (gettext ("unable to open the data"));
      goto exit_on_error;
    }
  t->file = fopen (buffer, "w");
  if (!t->file)
    {
      trajectory_error (gettext ("unable to open the results file"));
      goto exit_on_error;
    }
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_file: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_file: end\n");
#endif
  return 0;
}

/**
 * \fn void trajectory_open_console (Trajectory * t)
 * \brief function to input a Trajectory struct.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_open_console (Trajectory * t, Air * a)
{
  char buffer[512];
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_console: start\n");
#endif
  drop_open_console (t->drop, a);
  printf ("Bed level: ");
  scanf ("%lf", &(t->bed_level));
  printf ("Time step size: ");
  scanf ("%lf", &(t->dt));
  printf ("CFL number: ");
  scanf ("%lf", &(t->cfl));
  printf ("Results file name: ");
  scanf ("%512s", buffer);
  t->file = fopen (buffer, "w");
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_console: end\n");
#endif
}

/**
 * \fn int trajectory_open_xml (Trajectory *t, Air *a, xmlNode *node)
 * \brief function to open a Trajectory struct on a XML node.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
trajectory_open_xml (Trajectory * t, Air * a, xmlNode * node)
{
  int k;
  xmlChar *buffer;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_TRAJECTORY))
    {
      trajectory_error (gettext ("bad label"));
      goto exit_on_error;
    }
  if (!node->children)
	{
      trajectory_error (gettext ("no drop"));
	  goto exit_on_error;
	}
  if (!drop_open_xml (t->drop, a, node->children))
    goto exit_on_error;
  t->bed_level = xml_node_get_float_with_default (node, XML_BED_LEVEL, 0., &k);
  if (!k)
    {
      trajectory_error (gettext ("bad bed level"));
      goto exit_on_error;
    }
  t->dt = xml_node_get_float (node, XML_DT, &k);
  if (!k)
    {
      trajectory_error (gettext ("bad time step size"));
      goto exit_on_error;
    }
  t->cfl = xml_node_get_float (node, XML_CFL, &k);
  if (!k)
    {
      trajectory_error (gettext ("bad CFL number"));
      goto exit_on_error;
    }
  t->jet_length
    = xml_node_get_float_with_default (node, XML_JET_LENGTH, 0., &k);
  if (!k)
    {
      trajectory_error (gettext ("bad jet length"));
      goto exit_on_error;
    }
  buffer = xmlGetProp (node, XML_FILE);
  if (!buffer)
    {
      trajectory_error (gettext ("bad results file"));
      goto exit_on_error;
    }
  t->file = fopen ((char *) buffer, "w");
  xmlFree (buffer);
  if (!t->file)
    {
      trajectory_error (gettext ("unable to open the results file"));
      goto exit_on_error;
    }
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_xml: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn void trajectory_jet (Trajectory * t)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief Trajectory struct.
 */
void
trajectory_jet (Trajectory * t)
{
  Drop *d;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  t->t = t->jet_length / sqrt (d->v[0] * d->v[0] + d->v[1] * d->v[1]);
  d->r[0] += t->t * d->v[0];
  d->r[1] += t->t * d->v[1];
  d->r[2] += t->t * (d->v[2] - 0.5 * G * t->t);
  d->v[2] -= G * t->t;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_jet: end\n");
#endif
}

/**
 * \fn void trajectory_runge_kutta_4 (Trajectory * t, Air * a)
 * \brief function implementing a 4th order Runge-Kutta method to calculate a
 *   drop trajectory.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_runge_kutta_4 (Trajectory * t, Air * a)
{
  Drop d2[1], d3[1], d4[1], *d;
  double dt2, dt6;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_runge_kutta_4: start\n");
#endif
  d = t->drop;
  memcpy (d2, d, sizeof (Drop));
  memcpy (d3, d, sizeof (Drop));
  memcpy (d4, d, sizeof (Drop));
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_runge_kutta_4: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_runge_kutta_4: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_runge_kutta_4: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  dt2 = 0.5 * t->dt;
  d2->r[0] = d->r[0] + dt2 * d->v[0];
  d2->r[1] = d->r[1] + dt2 * d->v[1];
  d2->r[2] = d->r[2] + dt2 * d->v[2];
  d2->v[0] = d->v[0] + dt2 * d->a[0];
  d2->v[1] = d->v[1] + dt2 * d->a[1];
  d2->v[2] = d->v[2] + dt2 * d->a[2];
  drop_move (d2, a);
  d3->r[0] = d->r[0] + dt2 * d2->v[0];
  d3->r[1] = d->r[1] + dt2 * d2->v[1];
  d3->r[2] = d->r[2] + dt2 * d2->v[2];
  d3->v[0] = d->v[0] + dt2 * d2->a[0];
  d3->v[1] = d->v[1] + dt2 * d2->a[1];
  d3->v[2] = d->v[2] + dt2 * d2->a[2];
  drop_move (d3, a);
  d4->r[0] = d->r[0] + t->dt * d3->v[0];
  d4->r[1] = d->r[1] + t->dt * d3->v[1];
  d4->r[2] = d->r[2] + t->dt * d3->v[2];
  d4->v[0] = d->v[0] + t->dt * d3->a[0];
  d4->v[1] = d->v[1] + t->dt * d3->a[1];
  d4->v[2] = d->v[2] + t->dt * d3->a[2];
  drop_move (d4, a);
  dt6 = 1. / 6. * t->dt;
  d->r[0] += dt6 * (d->v[0] + d4->v[0] + 2.0 * (d2->v[0] + d3->v[0]));
  d->r[1] += dt6 * (d->v[1] + d4->v[1] + 2.0 * (d2->v[1] + d3->v[1]));
  d->r[2] += dt6 * (d->v[2] + d4->v[2] + 2.0 * (d2->v[2] + d3->v[2]));
  d->v[0] += dt6 * (d->a[0] + d4->a[0] + 2.0 * (d2->a[0] + d3->a[0]));
  d->v[1] += dt6 * (d->a[1] + d4->a[1] + 2.0 * (d2->a[1] + d3->a[1]));
  d->v[2] += dt6 * (d->a[2] + d4->a[2] + 2.0 * (d2->a[2] + d3->a[2]));
  t->t += t->dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_runge_kutta_4: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_runge_kutta_4: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_runge_kutta_4: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_runge_kutta_4: end\n");
#endif
}

/**
 * \fn void trajectory_impact_correction (Trajectory * t, Air * a)
 * \brief function to correct the trajectory at the soil impact.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_impact_correction (Trajectory * t, Air * a)
{
  Drop *d;
  double dt, h;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_impact_correction: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_impact_correction: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_impact_correction: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_impact_correction: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  drop_move (d, a);
  h = t->bed_level - d->r[2];
  dt = (-sqrt (d->v[2] * d->v[2] - 2. * h * d->a[2]) - d->v[2]) / d->a[2];
  d->r[0] -= dt * (d->v[0] - 0.5 * dt * d->a[0]);
  d->r[1] -= dt * (d->v[1] - 0.5 * dt * d->a[1]);
  d->r[2] -= dt * (d->v[2] - 0.5 * dt * d->a[2]);
  d->v[0] -= dt * d->a[0];
  d->v[1] -= dt * d->a[1];
  d->v[2] -= dt * d->a[2];
  t->t -= dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_impact_correction: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_impact_correction: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_impact_correction: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_impact_correction: end\n");
#endif
}

/**
 * \fn void trajectory_initial_correction (Trajectory * t, Air * a)
 * \brief function to correct the trajectory at the initial point.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_initial_correction (Trajectory * t, Air * a)
{
  Drop *d;
  double dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_initial_correction: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_initial_correction: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_initial_correction: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_initial_correction: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  drop_move (d, a);
  dt = (-sqrt (d->v[0] * d->v[0] + 2. * d->r[0] * d->a[2]) - d->v[0]) / d->a[0];
  d->r[0] -= dt * (d->v[0] - 0.5 * dt * d->a[0]);
  d->r[1] -= dt * (d->v[1] - 0.5 * dt * d->a[1]);
  d->r[2] -= dt * (d->v[2] - 0.5 * dt * d->a[2]);
  d->v[0] -= dt * d->a[0];
  d->v[1] -= dt * d->a[1];
  d->v[2] -= dt * d->a[2];
  t->t -= dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_initial_correction: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_initial_correction: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_initial_correction: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_initial_correction: end\n");
#endif
}

/**
 * \fn void trajectory_write (Trajectory * t)
 * \brief function to write a trajectory in a file.
 * \param t
 * \brief Trajectory struct.
 */
void
trajectory_write (Trajectory * t)
{
  Drop *d;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_write: start\n");
#endif
  d = t->drop;
  fprintf (t->file, "%lg %lg %lg %lg %lg %lg %lg %lg %lg\n",
           t->t, d->r[0], d->r[1], d->r[2], d->v[0], d->v[1], d->v[2],
           -d->drag, d->diameter);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_write: end\n");
#endif
}

/**
 * \fn void trajectory_calculate (Trajectory *t, Air *a)
 * \brief function to calculate the drop trajectory.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_calculate (Trajectory * t, Air * a)
{
  Drop *drop;
  double dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_calculate: start\n");
#endif
  t->t = 0.;
  trajectory_write (t);
  trajectory_jet (t);
  drop = t->drop;
  for (dt = t->dt; drop->r[2] > t->bed_level;)
    {
      trajectory_write (t);
      t->dt = fmin (dt, t->cfl / drop_move (drop, a));
      trajectory_runge_kutta_4 (t, a);
    }
  trajectory_impact_correction (t, a);
  trajectory_write (t);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_calculate: end\n");
#endif
}

/**
 * \fn void trajectory_invert (Trajectory *t, Air *a)
 * \brief function to calculate the inverse drop trajectory.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_invert (Trajectory * t, Air * a)
{
  Drop *drop;
  double dt;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_invert: start\n");
#endif
  t->t = 0.;
  drop = t->drop;
  for (dt = t->dt; drop->r[2] > t->bed_level && drop->r[0] > 0.;)
    {
      trajectory_write (t);
      t->dt = -fmin (dt, t->cfl / drop_move (drop, a));
      trajectory_runge_kutta_4 (t, a);
    }
  if (drop->r[2] < t->bed_level)
    trajectory_impact_correction (t, a);
  if (drop->r[0] < 0.)
    trajectory_impact_correction (t, a);
  trajectory_write (t);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_invert: end\n");
#endif
}
