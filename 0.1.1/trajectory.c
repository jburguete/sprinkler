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
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <gsl/gsl_rng.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"
#include "sprinkler.h"
#include "trajectory.h"

double bed_level; ///< bed level.

/**
 * \fn void trajectory_init_sprinkler (Trajectory *t, Sprinkler *s)
 * \brief function to init drop trajectory variables from sprinkler data.
 * \param t
 * \brief Trajectory struct.
 * \param s
 * \brief Sprinkler struct.
 */
void
trajectory_init_sprinkler (Trajectory *t, Sprinkler *s)
{
  Drop *d;
  double v, k2, k3;
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
  printf ("Time step size: %le\n", t->dt);
  printf ("Water pressure: %le\nDrop density: %le\n", s->pressure, d->density);
  printf ("Drop velocity: (%le,%le,%le)\n", d->v[0], d->v[1], d->v[2]);
}

/**
 * \fn int trajectory_open_file (Trajectory * t, Sprinkler * s, FILE * file)
 * \brief function to open a Trajectory struct in a file.
 * \param t
 * \brief trajectory struct.
 * \param s
 * \brief sprinkler struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
trajectory_open_file (Trajectory * t, FILE * file)
{
  if (fscanf (file, "%le", &(t->dt)) != 1)
    return 0;
  return 1;
}

/**
 * \fn void trajectory_open_console (Trajectory * t)
 * \brief function to input a trajectory struct.
 * \param t
 * \brief trajectory struct.
 */
void
trajectory_open_console (Trajectory * t)
{
  printf ("Time step size: ");
  scanf ("%lf", &(t->dt));
}

/**
 * \fn void trajectory_jet (Trajectory * t, Sprinkler * s)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief trajectory struct.
 * \param s
 * \brief sprinkler struct.
 */
void
trajectory_jet (Trajectory * t, Sprinkler * s)
{
  Drop *d;
  if (s->jet_length == 0.)
    return;
  d = t->drop;
  t->t = s->jet_length / sqrt (d->v[0] * d->v[0] + d->v[1] * d->v[1]);
  d->r[0] += t->t * d->v[0];
  d->r[1] += t->t * d->v[1];
  d->r[2] += t->t * (d->v[2] - 0.5 * G * t->t);
  d->v[2] -= G * t->t;
}

/**
 * \fn void trajectory_runge_kutta_4 (Trajectory * t, Air * a)
 * \brief function implementing a 4th order Runge-Kutta method to calculate a
 *   drop trajectory.
 * \param t
 * \brief trajectory struct.
 * \param a
 * \brief air struct.
 */
void
trajectory_runge_kutta_4 (Trajectory * t, Air * a)
{
  Drop d2[1], d3[1], d4[1], *d;
  double dt2, dt6;
  d = t->drop;
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
}

/**
 * \fn void trajectory_impact_correction (Trajectory * t, Air * a)
 * \brief function to correct the trajectory at the soil impact.
 * \param t
 * \brief trajectory struct.
 * \param a
 * \brief air struct.
 */
void
trajectory_impact_correction (Trajectory * t, Air * a)
{
  Drop *d;
  double dt, h;
  d = t->drop;
  drop_move (d, a);
  h = bed_level - d->r[2];
  dt = (-sqrt (d->v[2] * d->v[2] - 2. * h * d->a[2]) - d->v[2]) / d->a[2];
  d->r[0] -= dt * (d->v[0] - 0.5 * dt * d->a[0]);
  d->r[1] -= dt * (d->v[1] - 0.5 * dt * d->a[1]);
  d->r[2] -= dt * (d->v[2] - 0.5 * dt * d->a[2]);
  d->v[0] -= dt * d->a[0];
  d->v[1] -= dt * d->a[1];
  d->v[2] -= dt * d->a[2];
  t->t -= dt;
}

/**
 * \fn void trajectory_write (Trajectory * t, FILE * file)
 * \brief function to write a trajectory in a file.
 * \param t
 * \brief trajectory struct.
 * \param file
 * \brief file.
 */
void
trajectory_write (Trajectory * t, FILE * file)
{
  Drop *d;
  d = t->drop;
  fprintf (file, "%lg %lg %lg %lg %lg %lg %lg %lg\n",
           t->t, d->r[0], d->r[1], d->r[2], d->v[0], d->v[1], d->v[2],
		   -d->drag);
}

/**
 * \fn int open(char *name, Sprinkler * sprinkler, Air * air, Drop * drop, \
 *   Trajectory * trajectory)
 * \brief function to open all data in a file.
 * \param name
 * \brief file name.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief air struct.
 * \param trajectory.
 * \brief trajectory struct.
 * \return 1 on success, 0 on error.
 */
int
open (char *name, Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
{
  FILE *file;
  file = fopen (name, "r");
  if (!file)
    return 0;
  if (!sprinkler_open_file (sprinkler, file)
      || !air_open_file (air, file)
	  || !drop_open_file (trajectory->drop, air, file))
    {
      fclose (file);
      return 0;
    }
  trajectory_open_file (trajectory, file);
  trajectory_init_sprinkler (trajectory, sprinkler);
  fclose (file);
  return 1;
}

/**
 * \fn void init (Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
 * \brief function to input all data.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief air struct.
 * \param trajectory.
 * \brief trajectory struct.
 */
void
init (Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
{
  sprinkler_open_console (sprinkler);
  air_open_console (air);
  drop_open_console (trajectory->drop, air);
  trajectory_open_console (trajectory);
  trajectory_init_sprinkler (trajectory, sprinkler);
}

/**
 * \fn int main (int argn, char **argc)
 * \brief main function.
 * \param argn
 * \brief arguments number.
 * \param argc
 * \brief argument strings.
 * \return code error.
 */
int
main (int argn, char **argc)
{
  Sprinkler sprinkler[1];
  Air air[1];
  Trajectory trajectory[1];
  Drop *drop;
  FILE *file;
  double dt, dtmax;
  if (argn == 2)
    {
      init (sprinkler, air, trajectory);
      file = fopen (argc[1], "w");
    }
  else if (argn == 3)
    {
      if (!open (argc[1], sprinkler, air, trajectory))
        {
          printf ("Unable to open the data file\n");
          return 2;
        }
      file = fopen (argc[2], "w");
    }
  else
    {
      printf
        ("Usage of this program is:\n\ttrajectory file_data file_output\n");
      return 1;
    }
  if (!file)
    {
      printf ("Unable to open the output file\n");
      return 3;
    }
  trajectory_write (trajectory, file);
  trajectory_jet (trajectory, sprinkler);
  drop = trajectory->drop;
  for (dt = trajectory->dt; drop->r[2] > bed_level;)
    {
      trajectory_write (trajectory, file);
      dtmax = 1. / drop_move (drop, air);
      if (dt > dtmax)
        trajectory->dt = dtmax;
      else
        trajectory->dt = dt;
      trajectory_runge_kutta_4 (trajectory, air);
    }
  trajectory_impact_correction (trajectory, air);
  trajectory_write (trajectory, file);
  return 0;
}
