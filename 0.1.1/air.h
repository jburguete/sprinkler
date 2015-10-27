#ifndef AIR__H
#define AIR__H 1

#include "jb/jb_xml.h"
#include "jb/jb_win.h"
#include "config.h"

typedef struct
{
  float temperature, pressure, density, viscosity, velocity, angle, height,
    uncertainty, humidity, u, v, u0, v0;
} Air;

typedef struct
{
  GtkLabel *label_temperature, *label_pressure, *label_velocity, *label_angle,
    *label_height, *label_uncertainty;
  GtkSpinButton *entry_temperature, *entry_pressure, *entry_velocity,
    *entry_angle, *entry_height, *entry_uncertainty;
  GtkTable *table;
  GtkDialog *window;
} DialogAir;

extern int nAirs;
extern float air_temperature, air_pressure, air_humidity, air_velocity,
  air_uncertainty, air_angle, air_height;
extern Air air[1];

int air_open_xml (xmlNode * node);
void air_init (Air * air);
void air_wind_uncertainty (Air * air, GRand * rand);
void dialog_air_new ();

#endif
