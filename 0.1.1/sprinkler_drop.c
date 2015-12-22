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
 * \file sprinkler_drop.c
 * \brief File to calculate the trajectory of a drop emitted by a sprinkler with
 *   the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#define HAVE_GTK 0
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <libintl.h>
#include <gsl/gsl_rng.h>
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"
#include "trajectory.h"
#include "sprinkler.h"

/**
 * \fn int open(char *name, Sprinkler * sprinkler, Air * air, Drop * drop, \
 *   Trajectory * trajectory)
 * \brief function to open all data in a file.
 * \param name
 * \brief file name.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief Air struct.
 * \param trajectory.
 * \brief Trajectory struct.
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
      || !trajectory_open_file (trajectory, air, file))
    {
      fclose (file);
      return 0;
    }
  trajectory_init_with_sprinkler (trajectory, sprinkler);
  fclose (file);
  return 1;
}

/**
 * \fn void init (Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
 * \brief function to input all data.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief Air struct.
 * \param trajectory.
 * \brief Trajectory struct.
 */
void
init (Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
{
  sprinkler_open_console (sprinkler);
  air_open_console (air);
  drop_open_console (trajectory->drop, air);
  trajectory_open_console (trajectory, air);
  trajectory_init_with_sprinkler (trajectory, sprinkler);
}

/**
 * \fn int open_xml(char *name, Sprinkler * sprinkler, Air * air, Drop * drop, \
 *   Trajectory * trajectory)
 * \brief function to open all data in a file.
 * \param name
 * \brief file name.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief Air struct.
 * \param trajectory.
 * \brief Trajectory struct.
 * \return 1 on success, 0 on error.
 */
int
open_xml (char *name, Sprinkler * sprinkler, Air * air, Trajectory * trajectory)
{
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
  if (!node || !node->children)
	{
	  error_message = g_strconcat (gettext ("Bad input file"), NULL);
	  goto exit_on_error;
    }
  if (!sprinkler_open_xml (sprinkler, node))
	goto exit_on_error;
  node = node->children;
  if (!air_open_xml (air, node))
    goto exit_on_error;
  if (!trajectory_open_xml (trajectory, air, node))
    goto exit_on_error;
  trajectory_init_with_sprinkler (trajectory, sprinkler);
  return 1;

exit_on_error:
  show_error ();
  return 0;
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
  gsl_rng *rng;
  if (argn == 1)
    init (sprinkler, air, trajectory);
  else if (argn == 2 && !open_xml (argc[1], sprinkler, air, trajectory))
    return 2;
  else
    {
      printf ("Usage of this program is:\n\tsprinkler [file_data]\n");
      return 1;
    }
  rng = gsl_rng_alloc (gsl_rng_taus);
  gsl_rng_set (rng, RANDOM_SEED);
  air_wind_uncertainty (air, rng);
  trajectory_calculate (trajectory, air);
  gsl_rng_free (rng);
  return 0;
}
