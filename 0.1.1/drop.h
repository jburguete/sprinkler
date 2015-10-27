/**
 * \file drop.h
 * \brief Header file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef DROP__H
#define DROP__H 1

/**
 * \def R
 * \brief thermodinamic constant.
 * \def G
 * \brief gravitational acceleration constant.
 */
#define R 8.314
#define G 9.81

/**
 * \struct Air
 * \brief struct to define the atmosphere.
 */
typedef struct
{
    /**
	 * \var temperature
	 * \brief atmospheric temperature.
	 * \var humidity
	 * \brief atmospheric humidity.
	 * \var pressure
	 * \brief atmospheric pressure.
	 * \var density
	 * \brief atmospheric density.
	 * \var saturation_pressure
	 * \brief atmospheric saturation vapour pressure.
	 * \var vapour_pressure
	 * \brief atmospheric vapour pressure.
	 * \var viscosity
	 * \brief atmospheric viscosity.
	 * \var vx
	 * \brief wind velocity x component.
	 * \var vy
	 * \brief wind velocity y component.
	 */
  double temperature, humidity, pressure, density, saturation_pressure,
    vapour_pressure, viscosity, vx, vy;
} Air;

double vector_module (double x, double y, double z);

#endif
