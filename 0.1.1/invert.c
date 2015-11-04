/**
 * \file invert.c
 * \brief A program to calculate initial velocity of sprinkler drops.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <libxml/parser.h>
#include "drop.h"

#define XML_AIR			(const xmlChar*)"air"
  ///< air XML label.
#define XML_ANGLE		(const xmlChar*)"angle"
  ///< angle XML label.
#define XML_CFL			(const xmlChar*)"cfl"
  ///< cfl XML label.
#define XML_DIAMETER	(const xmlChar*)"diameter"
  ///< diameter XML label.
#define XML_DRAG		(const xmlChar*)"drag"
  ///< drop XML label.
#define XML_DROP		(const xmlChar*)"drop"
  ///< drop XML label.
#define XML_HUMIDITY	(const xmlChar*)"humidity"
  ///< humidity XML label.
#define XML_PRESSURE	(const xmlChar*)"pressure"
  ///< pressure XML label.
#define XML_TEMPERATURE	(const xmlChar*)"temperature"
  ///< temperature XML label.
#define XML_TRAJECTORY	(const xmlChar*)"trajectory"
  ///< trajectory XML label.
#define XML_VELOCITY	(const xmlChar*)"velocity"
  ///< velocity XML label.
#define XML_X			(const xmlChar*)"x"
  ///< x XML label.
#define XML_Y			(const xmlChar*)"y"
  ///< y XML label.
#define XML_Z			(const xmlChar*)"z"
  ///< z XML label.

/**
 * \struct Drop
 * \brief struct to define a drop.
 */
typedef struct
{
  double x;                     ///< position x component.
  double y;                     ///< position y component.
  double z;                     ///< position z component.
  double t;                     ///< time.
  double vx;                    ///< velocity x component.
  double vy;                    ///< velocity y component.
  double vz;                    ///< velocity z component.
  double ax;                    ///< acceleration x component.
  double ay;                    ///< acceleration y component.
  double az;                    ///< acceleration z component.
  double dt;                    ///< numerical time step size.
  double drag;                  ///< drag resistence factor.
  double diameter;              ///< drop diameter.
  double density;               ///< drop density.
  double cfl;                   ///< CFL number.
} Drop;

double drag_coefficient;        ///< fixed drag resistance factor.
double (*drop_drag) (double Reynolds);
  ///< pointer to the function to calculate the drop drag coefficient.

/**
 * \fn double xml_node_get_double (xmlNode *node, const xmlChar *prop, int *err)
 * \brief function to read a floating point number in a XML node property.
 * \param node
 * \brief XML node.
 * \param prop
 * \brief XML property.
 * \param err
 * \brief error code.
 * \return floating point value.
 */
double
xml_node_get_double (xmlNode * node, const xmlChar * prop, int *err)
{
  double x;
  char *buffer;
  *err = 0;
  if (!xmlHasProp (node, prop))
    return 0.;
  buffer = (char *) xmlGetProp (node, prop);
  *err = sscanf (buffer, "%le", &x);
  xmlFree (buffer);
  return x;
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
          "density=%le\n\tviscosity=%le\n",
          a->temperature, a->pressure, a->humidity, a->density, a->viscosity);
}

/**
 * \fn void air_open_xml (Air *a, xmlNode *node)
 * \brief function to open an Air struct in a XML node.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 */
void
air_open_xml (Air * a, xmlNode * node)
{
  int k;
  double k1, k2, kelvin;
  k1 = xml_node_get_double (node, XML_VELOCITY, &k);
  if (!k) k1 = 0.;
  k2 = M_PI / 180. * xml_node_get_double (node, XML_ANGLE, &k);
  if (!k) k2 = 0.;
  a->vx = k1 * cos (k2);
  a->vy = k1 * sin (k2);
  a->temperature = xml_node_get_double (node, XML_TEMPERATURE, &k);
  if (!k)
    a->temperature = 20.;
  a->humidity = xml_node_get_double (node, XML_HUMIDITY, &k);
  if (!k)
    a->humidity = 100.;
  a->pressure = xml_node_get_double (node, XML_PRESSURE, &k);
  if (!k)
    a->pressure = 100000.;
  a->viscosity = 0.0908e-6 * a->temperature + 13.267e-6;
  kelvin = a->temperature + 273.16;
  a->saturation_pressure = 698.450529 + kelvin *
    (-18.8903931 + kelvin *
     (0.213335768 + kelvin *
      (-0.001288580973 + kelvin *
       (0.000004393587233 + kelvin *
        (-0.000000008023923082 + kelvin * 6.136820929E-12)))));
  a->vapour_pressure = a->saturation_pressure * 0.01 * a->humidity;
  a->density = (0.029 * a->pressure - 0.011 * a->vapour_pressure) /
    (R * kelvin);
  air_print (a);
}

/**
 * \fn void drop_print (Drop *d)
 * \brief function to print the drop variables.
 * \param d
 * \biref Drop struct.
 */
void
drop_print (Drop * d)
{
  printf ("Drop:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n\t"
          "density=%lg\n",
          vector_module (d->vx, d->vy, d->vz),
          180. / M_PI * atan (d->vz / d->vx), d->x, d->z, d->density);
}

/**
 * \fn void drop_density (Drop * d, double t)
 * \brief function to calculate the drop density.
 * \param d
 * \brief Drop struct.
 * \param t
 * \brief temperature in Celsius.
 */
void
drop_density (Drop * d, double t)
{
  t -= 4.;
  d->density = 999.985064 + t * (-0.0037845 + t * (-0.0070759 + t * 0.0000333));
}

/**
 * \fn double drop_drag_Fukui (double Reynolds)
 * \brief function to calculate the drop drag factor with the
 *   Fukui et al. (1980) model.
 * \param Reynolds
 * \brief Reynolds number.
 * \return drop drag coefficient.
 */
double
drop_drag_Fukui (double Reynolds)
{
  if (Reynolds >= 1440.)
    return 0.45;
  else if (Reynolds >= 128.)
    return 72.2 / Reynolds - 0.0000556 * Reynolds + 0.46;
  return 33.3 / Reynolds - 0.0033 * Reynolds + 1.2;
}

/**
 * \fn double drop_drag_Fukui (double Reynolds)
 * \brief function to fix a drop drag factor.
 * \param Reynolds
 * \brief Reynolds number.
 * \return drop drag coefficient.
 */
double
drop_drag_fixed (double Reynolds)
{
  return drag_coefficient;
}

/**
 * \fn void drop_open_xml (Drop * d, Air * a, xmlNode * node)
 * \brief function to open a Drop struct in a XML node.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 */
void
drop_open_xml (Drop * d, Air * a, xmlNode * node)
{
  int k;
  double velocity, angle;
  d->x = xml_node_get_double (node, XML_X, &k);
  d->y = xml_node_get_double (node, XML_Y, &k);
  d->z = xml_node_get_double (node, XML_Z, &k);
  velocity = xml_node_get_double (node, XML_VELOCITY, &k);
  angle = xml_node_get_double (node, XML_ANGLE, &k);
  angle *= M_PI / 180.;
  d->vx = velocity * cos (angle);
  d->vy = 0.;
  d->vz = velocity * sin (angle);
  d->diameter = xml_node_get_double (node, XML_DIAMETER, &k);
  drop_density (d, a->temperature);
  d->cfl = xml_node_get_double (node, XML_CFL, &k);
  if (!k)
    d->cfl = 0.01;
  d->t = 0.;
  drop_print (d);
  drag_coefficient = xml_node_get_double (node, XML_DRAG, &k);
  if (!k)
    drop_drag = drop_drag_Fukui;
  else
    drop_drag = drop_drag_fixed;
}

double
drop_move (Drop * d, Air * a)
{
  double vrx, vry, v;
  vrx = d->vx - a->vx;
  vry = d->vy - a->vy;
  v = vector_module (vrx, vry, d->vz);
  d->drag = -0.75 * v * drop_drag (v * d->diameter / a->viscosity) *
    a->density / (d->density * d->diameter);
  d->ax = d->drag * vrx;
  d->ay = d->drag * vry;
  d->az = -G + d->drag * d->vz;
  return d->drag;
}

void
drop_runge_kutta_4 (Drop * d, Air * a)
{
  double dt2, dt6;
  Drop d2[1], d3[1], d4[1];
  memcpy (d2, d, sizeof (Drop));
  memcpy (d3, d, sizeof (Drop));
  memcpy (d4, d, sizeof (Drop));
  dt2 = 0.5 * d->dt;
  d2->x = d->x + dt2 * d->vx;
  d2->y = d->y + dt2 * d->vy;
  d2->z = d->z + dt2 * d->vz;
  d2->vx = d->vx + dt2 * d->ax;
  d2->vy = d->vy + dt2 * d->ay;
  d2->vz = d->vz + dt2 * d->az;
  drop_move (d2, a);
  d3->x = d->x + dt2 * d2->vx;
  d3->y = d->y + dt2 * d2->vy;
  d3->z = d->z + dt2 * d2->vz;
  d3->vx = d->vx + dt2 * d2->ax;
  d3->vy = d->vy + dt2 * d2->ay;
  d3->vz = d->vz + dt2 * d2->az;
  drop_move (d3, a);
  d4->x = d->x + d->dt * d3->vx;
  d4->y = d->y + d->dt * d3->vy;
  d4->z = d->z + d->dt * d3->vz;
  d4->vx = d->vx + d->dt * d3->ax;
  d4->vy = d->vy + d->dt * d3->ay;
  d4->vz = d->vz + d->dt * d3->az;
  drop_move (d4, a);
  dt6 = 1. / 6. * d->dt;
  d->x += dt6 * (d->vx + d4->vx + 2 * (d2->vx + d3->vx));
  d->y += dt6 * (d->vy + d4->vy + 2 * (d2->vy + d3->vy));
  d->z += dt6 * (d->vz + d4->vz + 2 * (d2->vz + d3->vz));
  d->vx += dt6 * (d->ax + d4->ax + 2 * (d2->ax + d3->ax));
  d->vy += dt6 * (d->ay + d4->ay + 2 * (d2->ay + d3->ay));
  d->vz += dt6 * (d->az + d4->az + 2 * (d2->az + d3->az));
  d->t += d->dt;
}

void
drop_impact_correction (Drop * d, Air * a)
{
  double dt;
  drop_move (d, a);
  dt = (sqrt (d->vx * d->vx - 2 * d->x * d->ax) - d->vx) / d->ax;
  d->x += dt * (d->vx + 0.5 * dt * d->ax);
  d->y += dt * (d->vy + 0.5 * dt * d->ay);
  d->z += dt * (d->vz + 0.5 * dt * d->az);
  d->vx += dt * d->ax;
  d->vy += dt * d->ay;
  d->vz += dt * d->az;
  d->t += dt;
}

void
drop_print_balistic (Drop * d)
{
  printf ("Balistic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->vx, d->vy, d->vz),
          180. / M_PI * atan (d->vz / d->vx), d->x, d->z);
}

void
drop_print_parabolic (Drop * d)
{
  double t, vz;
  t = -d->x / d->vx;
  vz = d->vz - G * t;
  printf ("Parabolic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->vx, 0., vz),
          180. / M_PI * atan (vz / d->vx),
          d->x + t * d->vx, d->z + t * (d->vz - G * t));
}

void
drop_write (Drop * d, FILE * file)
{
  if (file)
    fprintf (file, "%lg %lg %lg %lg %lg %lg %lg %lg\n",
             d->t, d->x, d->y, d->z, d->vx, d->vy, d->vz, -d->drag);
}

int
open_xml (char *name, Air * air, Drop * drop, FILE * file)
{
  xmlDoc *doc;
  xmlNode *node;
  doc = xmlParseFile (name);
  if (!doc)
    return 0;
  node = xmlDocGetRootElement (doc);
  if (!node || xmlStrcmp (node->name, XML_TRAJECTORY))
    return 0;
  node = node->children;
  if (!node || xmlStrcmp (node->name, XML_AIR))
    return 0;
  air_open_xml (air, node);
  for (node = node->next; node; node = node->next)
    {
      if (xmlStrcmp (node->name, XML_DROP))
        return 0;
      drop_open_xml (drop, air, node);
      drop_print_parabolic (drop);
      drop_write (drop, file);
      while (drop->x > 0.)
        {
          drop_write (drop, file);
          drop->dt = drop->cfl / drop_move (drop, air);
          drop_runge_kutta_4 (drop, air);
        }
      drop_impact_correction (drop, air);
      drop_write (drop, file);
      drop_print_balistic (drop);
    }
  return 1;
}

int
main (int argn, char **argc)
{
  FILE *file = NULL;
  Air air[1];
  Drop drop[1];
  xmlKeepBlanksDefault (0);
  if (argn < 2 || argn > 3)
    {
      printf ("Usage of this program is:\n\tinvert file_data [file_output]\n");
      return 1;
    }
  if (argn == 3)
    {
      file = fopen (argc[2], "w");
      if (!file)
        {
          printf ("Unable to open the output file\n");
          return 3;
        }
    }
  if (!open_xml (argc[1], air, drop, file))
    {
      printf ("Unable to open the data file\n");
      return 3;
    }
  if (file)
    fclose (file);
  return 0;
}
