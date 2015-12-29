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
 * \file utils.c
 * \brief Source file to define some useful functions.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "utils.h"

char *error_message = NULL;     ///< error message.

#if HAVE_GTK

GtkWindow *window_parent;       ///< parent window.

void
show_error ()
{
  GtkMessageDialog *dlg;
  dlg = (GtkMessageDialog *)
    gtk_message_dialog_new (window_parent, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_CLOSE, error_message);
  gtk_dialog_run (GTK_DIALOG (dlg));
  gtk_widget_destory (GTK_WIDGET (dlg));
  g_free (error_message);
  error_message = NULL;
}

#else

void
show_error ()
{
  puts (error_message);
}

#endif

/**
 * \fn double interpolate (double x, double x1, double x2, double y1, double y2)
 * \brief function to calculate the interpolation between 2 points.
 * \param x
 * \brief x-coordinate of the interpolation point.
 * \param x1
 * \brief 1st point x-coordinate.
 * \param x2
 * \brief 2nd point x-coordinate.
 * \param y1
 * \brief 1st point y-coordinate.
 * \param y2
 * \brief 2nd point y-coordinate.
 * \return y-coordinate of the interpolation point.
 */
double
interpolate (double x, double x1, double x2, double y1, double y2)
{
  return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

/**
 * \fn double vector_module(double x, double y, double z)
 * \brief function to calculate a 3D vector module.
 * \param x
 * \brief x vector coordenate.
 * \param y
 * \brief y vector coordenate.
 * \param z
 * \brief z vector coordenate.
 * \return vector module.
 */
double
vector_module (double x, double y, double z)
{
  return sqrt (x * x + y * y + z * z);
}

/**
 * \fn double xml_node_get_float (xmlNode * node, const xmlChar * prop, int *error_code)
 * \brief function to get a floating point number from a XML node property.
 * \param node
 * \brief XML node.
 * \param prop
 * \brief XML property.
 * \param error_code
 * \brief error code.
 * \return floating point number.
 */
double
xml_node_get_float (xmlNode * node, const xmlChar * prop, int *error_code)
{
  const xmlChar *buffer;
  double x;
  if (!xmlHasProp (node, prop))
    {
      *error_code = 0;
      return 0.;
    }
  buffer = xmlGetProp (node, prop);
  if (!buffer || sscanf ((char *) buffer, "%lf", &x) != 1)
    {
      *error_code = 0;
      return 0.;
    }
  *error_code = 1;
  return x;
}

/**
 * \fn double xml_node_get_float_with_default (xmlNode * node, const xmlChar * prop, \
 *   double default_value, int *error_code)
 * \brief function to get a floating point number from a XML node property with a default value.
 * \param node
 * \brief XML node.
 * \param prop
 * \brief XML property.
 * \param default_value
 * \brief default value.
 * \param error_code
 * \brief error code.
 * \return floating point number.
 */
double
xml_node_get_float_with_default (xmlNode * node, const xmlChar * prop,
                                 double default_value, int *error_code)
{
  const xmlChar *buffer;
  double x;
  *error_code = 1;
  if (!xmlHasProp (node, prop))
    return default_value;
  buffer = xmlGetProp (node, prop);
  if (!buffer || sscanf ((char *) buffer, "%lf", &x) != 1)
    {
      *error_code = 0;
      return 0;
    }
  return x;
}
