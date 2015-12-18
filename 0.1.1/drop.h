/**
 * \file drop.h
 * \brief Header file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef DROP__H
#define DROP__H 1

#define R 8.314                 ///< thermodinamic constant.
#define G 9.81                  ///< gravitational acceleration constant.
#define AIR_MOLECULAR_MASS 0.028964     ///< dry air molecular mass.
#define WATER_MOLECULAR_MASS 0.018016   ///< water molecular mass.
#define KELVIN 273.15           ///< Kelvin to Celsius degree.

/**
 * \struct Air
 * \brief struct to define the atmosphere.
 */
typedef struct
{
  double temperature;           ///< atmospheric temperature.
  double humidity;              ///< atmospheric humidity.
  double pressure;              ///< atmospheric pressure.
  double density;               ///< atmospheric density.
  double saturation_pressure;   ///< atmospheric saturation vapour pressure.
  double vapour_pressure;       ///< atmospheric vapour pressure.
  double viscosity;             ///< atmospheric viscosity.
  double vx;                    ///< wind velocity x component.
  double vy;                    ///< wind velocity y component.
} Air;

double vector_module (double x, double y, double z);

#endif
