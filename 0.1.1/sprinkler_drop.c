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
 * \brief air struct.
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
 * \brief air struct.
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
  FILE *file;
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
  trajectory_calculate (trajectory, air, file);
  fclose (file);
  return 0;
}
