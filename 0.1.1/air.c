/*
Sprinkler: a software to calculate drop trajectories in sprinkler irrigation.

AUTHORS: Javier Burguete.

Copyright 2012-2014, AUTHORS.

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
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <gsl/gsl_rng.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "air.h"

double air_temperature = AIR_TEMPERATURE; ///< air temperature in Celsius.
double air_pressure = AIR_PRESSURE; ///< air pressure.
double air_humidity = AIR_HUMIDITY; ///< air relative humidity.
double air_velocity = WIND_VELOCITY; ///< wind velocity.
double air_uncertainty = WIND_UNCERTAINTY; ///< wind velocity uncertainty.
double air_angle = WIND_ANGLE; ///< wind angle.
double air_height = WIND_HEIGHT;
  ///< reference height to measure the wind velocity.

double
xml_node_get_float (xmlNode *node, const xmlChar *prop, double default_value,
		            int *error_code)
{
  const xmlChar *buffer;
  double x;
  *error_code = 1;
  if (!xmlHasProp (node, prop))
	return default_value;
  buffer = xmlGetProp(node, prop);
  if (!buffer || sscanf ((char*)buffer, "%lf", &x) != 1)
    {
	  *error_code = 0;
	  return 0;
	}
  return x;
}

/**
 * \fn double air_viscosity (Air *a)
 * \brief function to calculate the air dynamic viscosity with the Sutherland
 *   equation.
 * \param a
 * \brief air struct.
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
 * \brief air struct.
 * \return water saturation pressure in air.
 */
double
air_saturation_pressure (Air * a)
{
  return exp (23.7836 - 37.8289 / (a->kelvin - 42.850));
}

/*
 * \fn void air_init (Air * a)
 * \brief function to init atmospheric variables.
 * \param a
 * \brief air struct.
 */
void
air_init (Air * a)
{
  a->temperature = air_temperature;
  a->pressure = air_pressure;
  a->velocity = air_velocity;
  a->uncertainty = air_uncertainty;
  a->angle = M_PI / 180. * air_angle;
  a->height = air_height;
  a->vx = a->velocity * cos (a->angle);
  a->vy = a->velocity * sin (a->angle);
  a->kelvin = a->temperature + KELVIN_TEMPERATURE;
  a->viscosity = air_viscosity (a);
  a->saturation_pressure = air_saturation_pressure (a);
  a->vapour_pressure = a->saturation_pressure * 0.01 * a->humidity;
  a->density = (AIR_MOLECULAR_MASS * a->pressure
                + (WATER_MOLECULAR_MASS -
                   AIR_MOLECULAR_MASS) * a->vapour_pressure) / (R * a->kelvin);
  printf ("Air density = %lg\n", a->density);
  printf ("Air viscosity = %lg\n", a->viscosity);
}

/**
 * \fn void air_open_file(Air *a, FILE *file)
 * \brief function to open an air struct in a file.
 * \param a
 * \brief air struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
air_open_file (Air * a, FILE * file)
{
  if (fscanf (file, "%lf%lf%lf%lf%lf", &air_velocity, &air_angle,
			  &air_temperature, &air_humidity, &air_pressure) != 5)
    return 0;
  air_init (a);
  return 1;
}

/**
 * \fn void air_open_console(Air *a)
 * \brief function to input an air struct in console.
 * \param a
 * \brief air struct.
 */
void
air_open_console (Air * a)
{
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
}

/**
 * \fn int air_open_xml (Air *a, xmlNode * node)
 * \brief function to open an air struct on a XML node.
 * \param a
 * \brief air struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */ 
int
air_open_xml (Air *a, xmlNode * node)
{
  int k;
  if (xmlStrcmp (node->name, XML_AIR))
    return 0;
  air_pressure = xml_node_get_float (node, XML_PRESSURE, AIR_PRESSURE, &k);
  if (k != 1)
	return 0;
  air_temperature
	= xml_node_get_float (node, XML_TEMPERATURE, AIR_TEMPERATURE, &k);
  if (k != 1)
	return 0;
  air_humidity = xml_node_get_float (node, XML_HUMIDITY, AIR_HUMIDITY, &k);
  if (k != 1)
	return 0;
  air_velocity = xml_node_get_float (node, XML_VELOCITY, WIND_VELOCITY, &k);
  if (k != 1)
	return 0;
  air_angle = xml_node_get_float (node, XML_ANGLE, WIND_ANGLE, &k);
  if (k != 1)
	return 0;
  air_height = xml_node_get_float (node, XML_HEIGHT, WIND_HEIGHT, &k);
  if (k != 1)
	return 0;
  air_uncertainty
	= xml_node_get_float (node, XML_UNCERTAINTY, WIND_UNCERTAINTY, &k);
  if (k != 1)
	return 0;
  air_init (a);
  return 1;
}

/**
 * \fn void air_wind_uncertainty (Air * a, gsl_rng *rng)
 * \brief function to generate a random wind.
 * \param a
 * \brief air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 */
void
air_wind_uncertainty (Air * a, gsl_rng *rng)
{
  float uncertainty, angle;
  angle = 2 * M_PI * gsl_rng_uniform (rng);
  uncertainty = a->uncertainty * gsl_rng_uniform (rng);
  a->u = a->vx + uncertainty * cos (angle);
  a->v = a->vy + uncertainty * sin (angle);
}

#if HAVE_GTK

void
dialog_air_new (Air *a)
{
  DialogAir dlg[1];

  dlg->label_temperature = (GtkLabel *) gtk_label_new ("Temperatura del aire");
  dlg->label_pressure = (GtkLabel *) gtk_label_new ("Presión atmosférica");
  dlg->label_velocity = (GtkLabel *) gtk_label_new ("Velocidad del viento");
  dlg->label_angle = (GtkLabel *) gtk_label_new ("Ángulo del viento");
  dlg->label_height = (GtkLabel *) gtk_label_new
    ("Altura de referencia del viento");
  dlg->label_uncertainty =
    (GtkLabel *) gtk_label_new ("Incertidumbre del viento");

  dlg->entry_temperature = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 100., 0.1);
  dlg->entry_pressure = (GtkSpinButton *)
    gtk_spin_button_new_with_range (90000., 110000., 100.);
  dlg->entry_velocity = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 20., 0.01);
  dlg->entry_angle = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 360., 0.1);
  dlg->entry_height = (GtkSpinButton *)
    gtk_spin_button_new_with_range (1., 50., 0.1);
  dlg->entry_uncertainty = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0., 20., 0.01);

  gtk_spin_button_set_value (dlg->entry_temperature, air_temperature);
  gtk_spin_button_set_value (dlg->entry_pressure, air_pressure);
  gtk_spin_button_set_value (dlg->entry_velocity, air_velocity);
  gtk_spin_button_set_value (dlg->entry_angle, air_angle);
  gtk_spin_button_set_value (dlg->entry_height, air_height);
  gtk_spin_button_set_value (dlg->entry_uncertainty, air_uncertainty);

  dlg->table = (GtkTable *) gtk_table_new (0, 0, 1);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_temperature, 0, 1, 0, 1);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_temperature, 1, 2, 0, 1);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_pressure, 0, 1, 1, 2);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_pressure, 1, 2, 1, 2);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_velocity, 0, 1, 2, 3);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_velocity, 1, 2, 2, 3);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_angle, 0, 1, 3, 4);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_angle, 1, 2, 3, 4);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_height, 0, 1, 4, 5);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_height, 1, 2, 4, 5);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->label_uncertainty, 0, 1, 5, 6);
  gtk_table_attach_defaults (dlg->table,
                             (GtkWidget *) dlg->entry_uncertainty, 1, 2, 5, 6);

  dlg->window =
    (GtkDialog *) gtk_dialog_new_with_buttons ("Condiciones atmosféricas",
                                               window_parent, GTK_DIALOG_MODAL,
                                               GTK_STOCK_OK, GTK_RESPONSE_OK,
                                               GTK_STOCK_CANCEL,
                                               GTK_RESPONSE_CANCEL, NULL);
  gtk_box_pack_start_defaults ((GtkBox *) dlg->window->vbox,
                               (GtkWidget *) dlg->table);
  gtk_widget_show_all ((GtkWidget *) dlg->window);

  if (gtk_dialog_run (dlg->window) == GTK_RESPONSE_OK)
    {
      air_temperature = gtk_spin_button_get_value (dlg->entry_temperature);
      air_pressure = gtk_spin_button_get_value (dlg->entry_pressure);
      air_velocity = gtk_spin_button_get_value (dlg->entry_velocity);
      air_angle = gtk_spin_button_get_value (dlg->entry_angle);
      air_height = gtk_spin_button_get_value (dlg->entry_height);
      air_uncertainty = gtk_spin_button_get_value (dlg->entry_uncertainty);
	  air_init (a);
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);
}

#endif
