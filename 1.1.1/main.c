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
 * \file main.c
 * \brief File to calculate the trajectory of drops emitted by a sprinkler with
 *   ballistic models.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#define HAVE_GTK 0
#include <stdio.h>
#include <string.h>
#include <math.h>
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
#include "sprinkler.h"
#include "jet.h"
#include "invert.h"

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
  Jet jet[1];
  Trajectory trajectory[1];
  Air air[1];
  xmlDoc *doc;
  xmlNode *node;
  xmlKeepBlanksDefault (0);
  if (argn == 2)
    {
      sprinkler_run_console (sprinkler, air, trajectory, argc[1]);
      return 0;
    }
  else if (argn != 3)
    {
      printf ("Usage of this program is:\n"
              "\tsprinkler [file_data] results_name\n");
      return 1;
    }
  doc = xmlParseFile (argc[1]);
  if (!doc)
    {
      error_message
        = g_strconcat (gettext ("Unable to parse the input file"), NULL);
      show_error ();
      return 2;
    }
  node = xmlDocGetRootElement (doc);
  if (!node)
    {
      error_message = g_strconcat (gettext ("Bad input file"), NULL);
      show_error ();
      return 3;
    }
  if (!xmlStrcmp (node->name, XML_INVERT)
      && !invert_run_xml (node, air, trajectory, jet, argc[2]))
    {
      show_error ();
      return 3;
    }
  else if (!xmlStrcmp (node->name, XML_SPRINKLER)
           && !sprinkler_run_xml (sprinkler, air, trajectory, node, argc[2]))
    {
      show_error ();
      return 4;
    }
  else if (!xmlStrcmp (node->name, XML_TRAJECTORY)
		   && !trajectory_run_xml (trajectory, air, node, argc[2]))
	{
      show_error ();
      return 5;
	}
  else
	{
      error_message = g_strconcat (gettext ("Input file"), ": ",
			                       gettext ("Bad node"), NULL);
	  show_error ();
	  return 6;
	}
		  
  return 0;
}
