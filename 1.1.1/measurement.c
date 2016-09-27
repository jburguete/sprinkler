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
 * \file measurement.c
 * \brief Source file to define the data of a measurement point.
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

#define DEBUG_MEASUREMENT 0     ///< macro to debug measurement functions.

/**
 * \fn void measurement_init (Measurement * m)
 * \brief function to init the measurement variables.
 * \param m
 * \brief Measurement struct.
 */
void
measurement_init (Measurement * m)
{
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_init: start\n");
#endif
  m->xleft = m->x - m->dx;
  m->xright = m->x + m->dx;
  m->ybottom = m->y - m->dy;
  m->ytop = m->y + m->dy;
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_init: end\n");
#endif
}

/**
 * \fn void measurement_error (char *message)
 * \brief function to show an error message opening a Measurement struct.
 * \param message
 * \brief error message.
 */
void
measurement_error (char *message)
{
  error_message
    = g_strconcat (gettext ("Measurement file"), ": ", message, NULL);
}

/**
 * \fn void measurement_open_console (Measurement * m)
 * \brief function to open a Measurement struct in console.
 * \param j
 * \brief Measurement struct.
 */
void
measurement_open_console (Measurement * m)
{
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_open_console: start\n");
#endif
  printf ("Measurement x: ");
  scanf ("%lf", &(m->x));
  printf ("Measurement y: ");
  scanf ("%lf", &(m->y));
  printf ("Measurement z: ");
  scanf ("%lf", &(m->z));
  printf ("Measurement dx: ");
  scanf ("%lf", &(m->dx));
  printf ("Measurement dy: ");
  scanf ("%lf", &(m->dy));
  measurement_init (m);
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_open_console: end\n");
#endif
}

/**
 * \fn int measurement_open_xml (Measurement * m, xmlNode *node)
 * \brief function to open a Measurement struct on a XML node.
 * \param j
 * \brief Measurement struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
measurement_open_xml (Measurement * m, xmlNode * node)
{
  int k;
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_MEASUREMENT))
    {
      measurement_error (gettext ("bad label"));
      goto exit_on_error;
    }
  m->x = xml_node_get_float_with_default (node, XML_X, 0., &k);
  if (!k)
    {
      measurement_error (gettext ("bad x"));
      goto exit_on_error;
    }
  m->y = xml_node_get_float_with_default (node, XML_Y, 0., &k);
  if (!k)
    {
      measurement_error (gettext ("bad y"));
      goto exit_on_error;
    }
  m->z = xml_node_get_float_with_default (node, XML_Z, 0., &k);
  if (!k)
    {
      measurement_error (gettext ("bad z"));
      goto exit_on_error;
    }
  m->dx = xml_node_get_float_with_default (node, XML_DX, 0., &k);
  if (!k)
    {
      measurement_error (gettext ("bad dx"));
      goto exit_on_error;
    }
  m->dy = xml_node_get_float_with_default (node, XML_DY, 0., &k);
  if (!k)
    {
      measurement_error (gettext ("bad dy"));
      goto exit_on_error;
    }
  measurement_init (m);

#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_open_xml: end\n");
#endif
  return 1;

exit_on_error:
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn void measurement_write (Measurement * m, Drop * d, double *rold)
 * \brief function to write on the measurement file the drop data.
 * \param m
 * \brief Measurement struct.
 * \param d
 * \brief Drop struct.
 * \param rold
 * \brief old drop position vector.
 * \param file
 * \brief results file.
 */
void
measurement_write (Measurement * m, Drop * d, double *rold, FILE *file)
{
  double *rnew;
  double x, y, dz;
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_write: start\n");
#endif
  rnew = d->r;
  dz = (rold[2] - m->z) * (rnew[2] - m->z);
  if (dz > 0.)
    goto outside;
  x = interpolate (m->z, rold[2], rnew[2], rold[0], rnew[0]);
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_write: x=%lg xleft=%lg yleft=%lg\n",
		   x, m->xleft, m->xright);
#endif
  if (x < m->xleft || x > m->xright)
    goto outside;
  y = interpolate (m->z, rold[2], rnew[2], rold[1], rnew[1]);
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_write: y=%lg ybottom=%lg ytop=%lg\n",
		   x, m->ybottom, m->ytop);
#endif
  if (y < m->ybottom || y > m->ytop)
    goto outside;
  fprintf (file, "%lg %lg %lg %lg %lg %lg %lg\n",
           m->x, m->y, m->z, d->diameter, d->v[0], d->v[1], d->v[2]);

outside:
#if DEBUG_MEASUREMENT
  fprintf (stderr, "measurement_write: end\n");
#endif
  return;
}
