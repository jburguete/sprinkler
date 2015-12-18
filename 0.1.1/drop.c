/**
 * \file drop.c
 * \brief Source file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#include <math.h>
#include "config.h"
#include "drop.h"

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
