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
 * \file air.h
 * \brief Header file to define atmospheric data.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef AIR__H
#define AIR__H 1

/**
 * \struct Air
 * \brief struct to define the atmosphere.
 */
typedef struct
{
  double temperature;           ///< atmospheric temperature in Celsius.
  double kelvin;                ///< atmospheric temperature in Kelvin.
  double humidity;              ///< atmospheric humidity.
  double pressure;              ///< atmospheric pressure.
  double density;               ///< atmospheric density.
  double saturation_pressure;   ///< atmospheric saturation vapour pressure.
  double vapour_pressure;       ///< atmospheric vapour pressure.
  double viscosity;             ///< atmospheric viscosity.
  double velocity;              ///< wind velocity.
  double angle;                 ///< wind azimuth angle (in degrees).
  double height;                ///< reference height to measure the wind.
  double uncertainty;           ///< wind velocity uncertainty.
  double vx;                    ///< mean wind velocity x component.
  double vy;                    ///< mean wind velocity y component.
  double u;                     ///< current wind velocity x component.
  double v;                     ///< current wind velocity y component.
} Air;

extern int nAirs;
extern double air_temperature, air_pressure, air_humidity, air_velocity,
  air_uncertainty, air_angle, air_height;

double air_viscosity (Air * a);
double air_saturation_pressure (Air * a);
void air_init (Air * a);
int air_open_file (Air * a, FILE * file);
void air_open_console (Air * a);
int air_open_xml (Air *a, xmlNode * node);
void air_wind_uncertainty (Air * a, gsl_rng *rng);

#if HAVE_GTK

typedef struct
{
  GtkLabel *label_temperature, *label_pressure, *label_velocity, *label_angle,
    *label_height, *label_uncertainty;
  GtkSpinButton *entry_temperature, *entry_pressure, *entry_velocity,
    *entry_angle, *entry_height, *entry_uncertainty;
  GtkTable *table;
  GtkDialog *window;
} DialogAir;

void dialog_air_new (Air *a);

#endif

#endif
