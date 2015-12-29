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
 * \file air.c
 * \brief Source file to define atmospheric data.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <libintl.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "utils.h"
#include "air.h"

#define DEBUG_AIR 0             ///< macro to debug air functions.

double air_temperature = AIR_TEMPERATURE;       ///< air temperature in Celsius.
double air_pressure = AIR_PRESSURE;     ///< air pressure.
double air_humidity = AIR_HUMIDITY;     ///< air relative humidity.
double air_velocity = WIND_VELOCITY;    ///< wind velocity.
double air_uncertainty = WIND_UNCERTAINTY;      ///< wind velocity uncertainty.
double air_angle = WIND_ANGLE;  ///< wind angle.
double air_height = WIND_HEIGHT;
  ///< reference height to measure the wind velocity.

/**
 * \fn double air_viscosity (Air *a)
 * \brief function to calculate the air dynamic viscosity with the Sutherland
 *   equation.
 * \param a
 * \brief Air struct.
 * \return air dynamic viscosity.
 */
double
air_viscosity (Air * a)
{
  return 1.458e-6 * a->kelvin * sqrt (a->kelvin) / (a->kelvin + 110.4);
}

/**
 * \fn double air_saturation_pressure (Air *a)
 * \brief function to calculate the water saturation pressure in air with the
 *   Antoine equation.
 * \param a
 * \brief Air struct.
 * \return water saturation pressure in air.
 */
double
air_saturation_pressure (Air * a)
{
  return exp (23.7836 - 3782.89 / (a->kelvin - 42.850));
}

/**
 * \fn void air_print (Air *a)
 * \brief function to print the atmospheric variables.
 * \param a
 * \biref Air struct.
 */
void
air_print (Air * a)
{
  printf ("Air:\n\ttemperature=%lg\n\tpressure=%lg\n\thumidity=%lg\n\t"
          "density=%lg\n\tviscosity=%le\n",
          a->temperature, a->pressure, a->humidity, a->density,
          a->dynamic_viscosity);
}

/*
 * \fn void air_init (Air * a)
 * \brief function to init atmospheric variables.
 * \param a
 * \brief Air struct.
 */
void
air_init (Air * a)
{
#if DEBUG_AIR
  fprintf (stderr, "air_init: start\n");
#endif
  a->temperature = air_temperature;
  a->pressure = air_pressure;
  a->humidity = air_humidity;
  a->velocity = air_velocity;
  a->uncertainty = air_uncertainty;
  a->angle = M_PI / 180. * air_angle;
  a->height = air_height;
  a->vx = a->velocity * cos (a->angle);
  a->vy = a->velocity * sin (a->angle);
  a->kelvin = a->temperature + KELVIN_TEMPERATURE;
  a->dynamic_viscosity = air_viscosity (a);
  a->saturation_pressure = air_saturation_pressure (a);
  a->vapour_pressure = a->saturation_pressure * 0.01 * a->humidity;
  a->density = (AIR_MOLECULAR_MASS * a->pressure
                + (WATER_MOLECULAR_MASS -
                   AIR_MOLECULAR_MASS) * a->vapour_pressure) / (R * a->kelvin);
  a->kinematic_viscosity = a->dynamic_viscosity / a->density;
  air_print (a);
#if DEBUG_AIR
  fprintf (stderr, "air_init: end\n");
#endif
}

/**
 * \fn void air_error (char *message)
 * \brief function to show an error message opening an Air struct.
 * \param message
 * \brief error message.
 */
void
air_error (char *message)
{
  error_message = g_strconcat (gettext ("Air file"), ": ", message, NULL);
}

/**
 * \fn void air_open_console(Air *a)
 * \brief function to input an Air struct in console.
 * \param a
 * \brief Air struct.
 */
void
air_open_console (Air * a)
{
#if DEBUG_AIR
  fprintf (stderr, "air_open_console: start\n");
#endif
  printf ("Wind velocity (m/s): ");
  scanf ("%lf", &air_velocity);
  printf ("Wind angle (m/s): ");
  scanf ("%lf", &air_angle);
  printf ("Air temperature (Celsius): ");
  scanf ("%lf", &air_temperature);
  printf ("Air relative humidity (%%): ");
  scanf ("%lf", &air_humidity);
  printf ("Air pressure (Pa): ");
  scanf ("%lf", &air_pressure);
  air_init (a);
#if DEBUG_AIR
  fprintf (stderr, "air_open_console: end\n");
#endif
}

/**
 * \fn int air_open_xml (Air *a, xmlNode * node)
 * \brief function to open an Air struct on a XML node.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
air_open_xml (Air * a, xmlNode * node)
{
  int k;
#if DEBUG_AIR
  fprintf (stderr, "air_open_xml: start\n");
#endif
  if (xmlStrcmp (node->name, XML_AIR))
    {
      air_error (gettext ("bad label"));
      goto exit_on_error;
    };
  air_pressure
    = xml_node_get_float_with_default (node, XML_PRESSURE, AIR_PRESSURE, &k);
  if (!k)
    {
      air_error (gettext ("bad pressure"));
      goto exit_on_error;
    };
  air_temperature
    = xml_node_get_float_with_default (node, XML_TEMPERATURE, AIR_TEMPERATURE,
                                       &k);
  if (!k)
    {
      air_error (gettext ("bad temperature"));
      goto exit_on_error;
    };
  air_humidity
    = xml_node_get_float_with_default (node, XML_HUMIDITY, AIR_HUMIDITY, &k);
  if (!k)
    {
      air_error (gettext ("bad humidity"));
      goto exit_on_error;
    };
  air_velocity
    = xml_node_get_float_with_default (node, XML_VELOCITY, WIND_VELOCITY, &k);
  if (!k)
    {
      air_error (gettext ("bad wind velocity"));
      goto exit_on_error;
    };
  air_angle = xml_node_get_float_with_default (node, XML_ANGLE, WIND_ANGLE, &k);
  if (!k)
    {
      air_error (gettext ("bad wind velocity"));
      goto exit_on_error;
    };
  air_height
    = xml_node_get_float_with_default (node, XML_HEIGHT, WIND_HEIGHT, &k);
  if (!k)
    {
      air_error (gettext ("bad reference height to measure the wind"));
      goto exit_on_error;
    };
  air_uncertainty
    = xml_node_get_float_with_default (node, XML_UNCERTAINTY, WIND_UNCERTAINTY,
                                       &k);
  if (!k)
    {
      air_error (gettext ("bad wind uncertainty"));
      goto exit_on_error;
    };
  air_init (a);
  return 1;

exit_on_error:
#if DEBUG_AIR
  fprintf (stderr, "air_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn void air_wind_uncertainty (Air * a, gsl_rng *rng)
 * \brief function to generate a random wind.
 * \param a
 * \brief Air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 */
void
air_wind_uncertainty (Air * a, gsl_rng * rng)
{
  float uncertainty, angle;
#if DEBUG_AIR
  fprintf (stderr, "air_wind_uncertainty: start\n");
#endif
  angle = 2 * M_PI * gsl_rng_uniform (rng);
  uncertainty = a->uncertainty * fmin (5., fabs (gsl_ran_ugaussian (rng)));
  a->u = a->vx + uncertainty * cos (angle);
  a->v = a->vy + uncertainty * sin (angle);
#if DEBUG_AIR
  fprintf (stderr, "air_wind_uncertainty: end\n");
#endif
}

#if HAVE_GTK

/**
 * \fn void dialog_air_new (Air * a)
 * \brief function to set the atmospheric variables in a GtkDialog.
 * \param a
 * \brief Air struct.
 */
void
dialog_air_new (Air * a)
{
  DialogAir dlg[1];

#if DEBUG_AIR
  fprintf (stderr, "dialog_air_new: start\n");
#endif

  dlg->label_temperature
    = (GtkLabel *) gtk_label_new (gettext ("Air temperature"));
  dlg->label_pressure = (GtkLabel *) gtk_label_new (gettext ("Air pressure"));
  dlg->label_velocity = (GtkLabel *) gtk_label_new (gettext ("Wind velocity"));
  dlg->label_angle = (GtkLabel *) gtk_label_new (gettext ("Wind angle"));
  dlg->label_height
    = (GtkLabel *) gtk_label_new (gettext ("Wind reference height"));
  dlg->label_uncertainty
    = (GtkLabel *) gtk_label_new (gettext ("Wind uncertainty"));

  dlg->spin_temperature = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 100., 0.1);
  dlg->spin_pressure = (GtkSpinButton *)
    gtk_spin_button_new_with_range (90000., 110000., 100.);
  dlg->spin_velocity = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 20., 0.01);
  dlg->spin_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 360., 0.1);
  dlg->spin_height = (GtkSpinButton *)
    gtk_spin_button_new_with_range (1., 50., 0.1);
  dlg->spin_uncertainty = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 20., 0.01);

  gtk_spin_button_set_value (dlg->spin_temperature, air_temperature);
  gtk_spin_button_set_value (dlg->spin_pressure, air_pressure);
  gtk_spin_button_set_value (dlg->spin_velocity, air_velocity);
  gtk_spin_button_set_value (dlg->spin_angle, air_angle);
  gtk_spin_button_set_value (dlg->spin_height, air_height);
  gtk_spin_button_set_value (dlg->spin_uncertainty, air_uncertainty);

  dlg->grid = (GtkGrid *) gtk_grid_new ();
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_temperature, 0, 0, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_temperature, 1, 0, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_pressure, 0, 1, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_pressure, 1, 1, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_velocity, 0, 2, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_velocity, 1, 2, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_angle, 0, 3, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_angle, 1, 3, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_height, 0, 4, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_height, 1, 4, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_uncertainty, 0, 5, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_uncertainty, 1, 5, 1, 1);

  dlg->window = (GtkDialog *)
    gtk_dialog_new_with_buttons (gettext ("Atmospheric conditions"),
                                 window_parent, GTK_DIALOG_MODAL,
                                 GTK_STOCK_OK, GTK_RESPONSE_OK,
                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (dlg->window)),
                     GTK_WIDGET (dlg->grid));
  gtk_widget_show_all (GTK_WIDGET (dlg->window));

  if (gtk_dialog_run (dlg->window) == GTK_RESPONSE_OK)
    {
      air_temperature = gtk_spin_button_get_value (dlg->spin_temperature);
      air_pressure = gtk_spin_button_get_value (dlg->spin_pressure);
      air_velocity = gtk_spin_button_get_value (dlg->spin_velocity);
      air_angle = gtk_spin_button_get_value (dlg->spin_angle);
      air_height = gtk_spin_button_get_value (dlg->spin_height);
      air_uncertainty = gtk_spin_button_get_value (dlg->spin_uncertainty);
      air_init (a);
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);

#if DEBUG_AIR
  fprintf (stderr, "dialog_air_new: end\n");
#endif

}

#endif
