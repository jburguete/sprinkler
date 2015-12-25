/**
 * \file invert.c
 * \brief A program to calculate initial velocity of sprinkler drops.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#define HAVE_GTK 0
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <libxml/parser.h>
#include <glib.h>
#include <libintl.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"
#include "trajectory.h"
#include "jet.h"

void
drop_print_ballistic (Drop * d)
{
  printf ("Balistic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->v[0], d->v[1], d->v[2]),
          180. / M_PI * atan (d->v[2] / d->v[0]), d->r[0], d->r[1]);
}

void
drop_print_parabolic (Drop * d)
{
  double t, vz;
  t = -d->r[0] / d->v[0];
  vz = d->v[2] - G * t;
  printf ("Parabolic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->v[0], 0., vz),
          180. / M_PI * atan (vz / d->v[0]),
          d->r[0] + t * d->v[0], d->r[2] + t * (d->v[2] - 0.5 * G * t));
}

int
open_xml (char *name, Air * air, Trajectory * trajectory, Jet * jet,
		  char *result)
{
  Drop *drop;
  xmlDoc *doc;
  xmlNode *node;
  gsl_rng *rng;
  rng = gsl_rng_alloc (gsl_rng_taus);
  gsl_rng_set (rng, RANDOM_SEED);
  doc = xmlParseFile (name);
  if (!doc)
    {
      error_message
        = g_strconcat (gettext ("Unable to parse the input file"), NULL);
      goto exit_on_error;
    }
  node = xmlDocGetRootElement (doc);
  if (!node || xmlStrcmp (node->name, XML_INVERT) || !node->children)
    {
      error_message = g_strconcat (gettext ("Bad input file"), NULL);
      goto exit_on_error;
    }
  node = node->children;
  if (!air_open_xml (air, node))
    goto exit_on_error;
  node = node->next;
  if (!jet_open_xml (jet, node))
    goto exit_on_error;
  drop = trajectory->drop;
  for (node = node->next; node; node = node->next)
    {
      if (!trajectory_open_xml (trajectory, air, node, result))
        goto exit_on_error;
      air_wind_uncertainty (air, rng);
      drop_print_parabolic (drop);
      trajectory_invert_with_jet (trajectory, air, jet);
      drop_print_ballistic (drop);
    }
  gsl_rng_free (rng);
  return 1;

exit_on_error:
  show_error ();
  return 0;
}

int
main (int argn, char **argc)
{
  FILE *file = NULL;
  Air air[1];
  Trajectory trajectory[1];
  Jet jet[1];
  xmlKeepBlanksDefault (0);
  if (argn > 3)
    {
      printf ("Usage of this program is:\n\tinvert file_data results_name\n");
      return 1;
    }
  if (!open_xml (argc[1], air, trajectory, jet, argc[2]))
    return 3;
  if (file)
    fclose (file);
  return 0;
}
