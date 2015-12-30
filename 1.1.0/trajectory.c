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
#include "measurement.h"
#include "trajectory.h"

#define DEBUG_TRAJECTORY 0      ///< macro to debug trajectory functions.

void (*trajectory_jet) (Trajectory * t, Air * a);
  ///< pointer to the function to calculate the movement into the jet.

/**
 * \fn void trajectory_init (Trajectory * t, Air * a, gsl_rng * rng)
 * \brief function to init trajectory variables.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 */
void
trajectory_init (Trajectory * t, Air * a, gsl_rng * rng)
{
  Drop *d;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_init: start\n");
#endif
  switch (t->jet_model)
    {
    case TRAJECTORY_JET_MODEL_NULL_DRAG:
      trajectory_jet = trajectory_jet_null_drag;
      break;
    case TRAJECTORY_JET_MODEL_PROGRESSIVE:
      trajectory_jet = trajectory_jet_progressive;
      break;
    default:
      trajectory_jet = trajectory_jet_big_drops;
    }
  d = t->drop;
  d->jet_time = t->jet_time;
  drop_init (d, a, rng);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_init: Jet time=%lg\n", d->jet_time);
  fprintf (stderr, "trajectory_init: end\n");
#endif
}

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
 * \fn void trajectory_open_console (Trajectory * t, Air * a, char *name)
 * \brief function to input a Trajectory struct.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param name
 * \brief results base file name.
 */
void
trajectory_open_console (Trajectory * t, Air * a, char *name)
{
  char buffer[512], buffer2[512];
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_console: start\n");
#endif
  printf ("Bed level: ");
  scanf ("%lf", &(t->bed_level));
  printf ("Time step size: ");
  scanf ("%lf", &(t->dt));
  printf ("CFL number: ");
  scanf ("%lf", &(t->cfl));
  printf ("Jet model (0: null drag, 1: progressive, 2: big drops): ");
  scanf ("%u", &(t->jet_model));
  printf ("Results file name: ");
  scanf ("%512s", buffer);
  snprintf (buffer2, 512, "%s-%s", name, buffer);
  t->file = fopen (buffer2, "w");
  t->filename = g_strdup (buffer2);
  drop_open_console (t->drop, a);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_console: end\n");
#endif
}

/**
 * \fn int trajectory_open_xml (Trajectory *t, Air *a, xmlNode *node, \
 *   char *name)
 * \brief function to open a Trajectory struct on a XML node.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 * \param name
 * \brief results base file name.
 * \return 1 on success, 0 on error.
 */
int
trajectory_open_xml (Trajectory * t, Air * a, xmlNode * node, char *name)
{
  char buffer2[512];
  xmlChar *buffer;
  int k;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_TRAJECTORY))
    {
      trajectory_error (gettext ("bad label"));
      goto exit_on_error;
    }
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
  t->jet_time = xml_node_get_float_with_default (node, XML_JET_TIME, 0., &k);
  if (!k)
    {
      trajectory_error (gettext ("bad jet time"));
      goto exit_on_error;
    }
  if (!xmlHasProp (node, XML_JET_MODEL))
    t->jet_model = 0;
  else
    {
      buffer = xmlGetProp (node, XML_JET_MODEL);
      if (!xmlStrcmp (buffer, XML_NULL_DRAG))
        t->jet_model = TRAJECTORY_JET_MODEL_NULL_DRAG;
      else if (!xmlStrcmp (buffer, XML_PROGRESSIVE))
        t->jet_model = TRAJECTORY_JET_MODEL_PROGRESSIVE;
      else if (!xmlStrcmp (buffer, XML_BIG_DROPS))
        t->jet_model = TRAJECTORY_JET_MODEL_BIG_DROPS;
      else
        {
          trajectory_error (gettext ("unknown jet model"));
          xmlFree (buffer);
          goto exit_on_error;
        }
      xmlFree (buffer);
    }
  buffer = xmlGetProp (node, XML_FILE);
  if (!buffer)
    {
      trajectory_error (gettext ("bad results file"));
      goto exit_on_error;
    }
  snprintf (buffer2, 512, "%s-%s", name, (char *) buffer);
  xmlFree (buffer);
  t->file = fopen (buffer2, "w");
  if (!t->file)
    {
      trajectory_error (gettext ("unable to open the results file"));
      goto exit_on_error;
    }
  t->filename = g_strdup (buffer2);
  if (!node->children)
    {
      trajectory_error (gettext ("no drop"));
      goto exit_on_error;
    }
  if (!drop_open_xml (t->drop, a, node->children))
    goto exit_on_error;
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
 * \fn void trajectory_open_data (Trajectory * t, Air * a, gsl_rng * rng, \
 *   double diameter, double *r, double *v, unsigned int jet_model, \
 *   unsigned int detach_model, unsigned int drag_model, \
 *   double maximum_diameter, double drag_coefficient)
 * \brief function to open a Trajectory struct with data.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 * \param diameter
 * \brief drop diameter.
 * \param jet_model
 * \brief jet model type.
 * \param detach_model
 * \brief jet detach model type.
 * \param drag_model
 * \brief drop drag resistance model.
 * \param maximum_diameter
 * \brief maximum diameter of stable drops.
 * \param drag_coefficient
 * \brief drop drag resistance coefficient for the constant model.
 */
void
trajectory_open_data (Trajectory * t, Air * a, gsl_rng * rng, double diameter,
		              unsigned int jet_model, unsigned int detach_model,
					  unsigned int drag_model, double maximum_diameter,
					  double drag_coefficient)
{
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_data: start\n");
#endif
  t->jet_model = jet_model;
  t->file = NULL;
  drop_diameter = diameter;
  drop_detach_model = detach_model;
  drop_drag_model = drag_model;
  drop_maximum_diameter = maximum_diameter;
  drop_drag_coefficient = drag_coefficient;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_open_data: end\n");
#endif
}

/**
 * \fn void trajectory_runge_kutta_4 (Trajectory * t, Air * a, double factor)
 * \brief function implementing a 4th order Runge-Kutta method to calculate a
 *   drop trajectory.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param factor
 * \brief drag resistance factor.
 */
void
trajectory_runge_kutta_4 (Trajectory * t, Air * a, double factor)
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
  drop_move (d2, a, factor);
  d3->r[0] = d->r[0] + dt2 * d2->v[0];
  d3->r[1] = d->r[1] + dt2 * d2->v[1];
  d3->r[2] = d->r[2] + dt2 * d2->v[2];
  d3->v[0] = d->v[0] + dt2 * d2->a[0];
  d3->v[1] = d->v[1] + dt2 * d2->a[1];
  d3->v[2] = d->v[2] + dt2 * d2->a[2];
  drop_move (d3, a, factor);
  d4->r[0] = d->r[0] + t->dt * d3->v[0];
  d4->r[1] = d->r[1] + t->dt * d3->v[1];
  d4->r[2] = d->r[2] + t->dt * d3->v[2];
  d4->v[0] = d->v[0] + t->dt * d3->a[0];
  d4->v[1] = d->v[1] + t->dt * d3->a[1];
  d4->v[2] = d->v[2] + t->dt * d3->a[2];
  drop_move (d4, a, factor);
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
 * \fn void trajectory_jet_null_drag (Trajectory * t, Air * a)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_jet_null_drag (Trajectory * t, Air * a)
{
  Drop *d;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_null_drag: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_null_drag: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_null_drag: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_null_drag: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  t->t = d->jet_time;
  d->r[0] += t->t * d->v[0];
  d->r[1] += t->t * d->v[1];
  d->r[2] += t->t * (d->v[2] - 0.5 * G * t->t);
  d->v[2] -= G * t->t;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_null_drag: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_null_drag: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_null_drag: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_jet_null_drag: end\n");
#endif
}

/**
 * \fn void trajectory_jet_progressive (Trajectory * t, Air * a)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_jet_progressive (Trajectory * t, Air * a)
{
  Drop *d;
  double dt, factor;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_progressive: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_progressive: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_progressive: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_progressive: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  for (dt = t->dt; t->t < d->jet_time; t->t += t->dt)
    {
      trajectory_write (t);
      factor = 0.1 + 0.9 * t->t / t->jet_time;
      t->dt = fmin (dt, t->cfl / drop_move (d, a, factor));
      trajectory_runge_kutta_4 (t, a, factor);
    }
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_progressive: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_progressive: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_progressive: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_jet_progressive: end\n");
#endif
}

/**
 * \fn void trajectory_jet_big_drops (Trajectory * t, Air * a)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 */
void
trajectory_jet_big_drops (Trajectory * t, Air * a)
{
  Drop *d;
  double diameter;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_big_drops: start\n");
#endif
  d = t->drop;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_big_drops: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_big_drops: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_big_drops: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
#endif
  diameter = d->diameter;
  d->diameter = drop_maximum_diameter;
  trajectory_jet_progressive (t, a);
  d->diameter = diameter;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_jet_big_drops: t=%lg\n", t->t);
  fprintf (stderr, "trajectory_jet_big_drops: r=(%lg,%lg,%lg)\n",
           d->r[0], d->r[1], d->r[2]);
  fprintf (stderr, "trajectory_jet_big_drops: v=(%lg,%lg,%lg)\n",
           d->v[0], d->v[1], d->v[2]);
  fprintf (stderr, "trajectory_jet_big_drops: end\n");
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
  drop_move (d, a, 1.);
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
  drop_move (d, a, 1.);
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
 * \fn void trajectory_calculate (Trajectory * t, Air * a, Measurement * m, \
 *   unsigned int n, FILE *file)
 * \brief function to calculate the drop trajectory.
 * \param t
 * \brief Trajectory struct.
 * \param a
 * \brief Air struct.
 * \param m
 * \brief array of Measurement structs.
 * \param n
 * \brief number of Measurement structs.
 * \param file
 * \brief results file.
 */
void
trajectory_calculate (Trajectory * t, Air * a, Measurement * m, unsigned int n,
		              FILE *file)
{
  double r[3];
  Drop *d;
  double dt;
  unsigned int i;
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_calculate: start\n");
  fprintf (stderr, "trajectory_calculate: nmeasurements=%u\n", n);
#endif
  t->t = 0.;
  if (t->file) trajectory_write (t);
  trajectory_jet (t, a);
  d = t->drop;
  for (dt = t->dt; d->r[2] > t->bed_level || d->v[2] > 0.;)
    {
      if (t->file) trajectory_write (t);
      t->dt = fmin (dt, t->cfl / drop_move (d, a, 1.));
      memcpy (r, d->r, 3 * sizeof (double));
      trajectory_runge_kutta_4 (t, a, 1.);
      for (i = 0; i < n; ++i)
        measurement_write (m + i, d, r, file);
    }
  trajectory_impact_correction (t, a);
  if (t->file)
    {
	  trajectory_write (t);
      fclose (t->file);
      g_free (t->filename);
	}
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
      t->dt = -fmin (dt, t->cfl / drop_move (drop, a, 1.));
      trajectory_runge_kutta_4 (t, a, 1.);
    }
  if (drop->r[2] < t->bed_level)
    trajectory_impact_correction (t, a);
  if (drop->r[0] < 0.)
    trajectory_impact_correction (t, a);
  trajectory_write (t);
  fclose (t->file);
  g_free (t->filename);
#if DEBUG_TRAJECTORY
  fprintf (stderr, "trajectory_invert: end\n");
#endif
}
