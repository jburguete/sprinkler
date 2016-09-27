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
 * \file invert.c
 * \brief Source file to reconstruct drop trajectories with ballistic models.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
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
#include "measurement.h"
#include "trajectory.h"
#include "jet.h"

void
invert_error (char *message)
{
  error_message = g_strconcat (gettext ("Invert file"), ": ", message, NULL);
}

int
invert_run_xml (xmlNode * node, Air * a, Trajectory * t, Jet * j, char *result)
{
  gsl_rng *rng;
  rng = gsl_rng_alloc (gsl_rng_taus);
  gsl_rng_set (rng, RANDOM_SEED);
  node = node->children;
  if (!node)
    {
      invert_error (gettext ("No air"));
      goto exit_on_error;
    }
  if (!air_open_xml (a, node))
    goto exit_on_error;
  node = node->next;
  if (!jet_open_xml (j, node))
    goto exit_on_error;
  for (node = node->next; node; node = node->next)
    {
      if (!trajectory_open_xml (t, a, node, result))
        goto exit_on_error;
	  trajectory_init (t, a, rng);
      air_wind_uncertainty (a, rng);
      //drop_print_parabolic (t->drop);
      trajectory_invert_with_jet (t, a, j);
      //drop_print_ballistic (t->drop);
    }
  gsl_rng_free (rng);
  return 1;

exit_on_error:
  return 0;
}
