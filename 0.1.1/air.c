#include "air.h"

float air_temperature = AIR_TEMPERATURE;
float air_pressure = AIR_PRESSURE;
float air_humidity = AIR_HUMIDITY;
float air_velocity = WIND_VELOCITY;
float air_uncertainty = WIND_UNCERTAINTY;
float air_angle = WIND_ANGLE;
float air_height = WIND_HEIGHT;

int
air_open_xml (xmlNode * node)
{
  int k;
  if (xmlStrcmp (node->name, XML_AIR))
    return 0;
  if (!xmlHasProp (node, XML_PRESSURE))
    air_pressure = AIR_PRESSURE;
  else
    {
      air_pressure = jb_xml_node_get_float (node, XML_PRESSURE, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_TEMPERATURE))
    air_temperature = AIR_TEMPERATURE;
  else
    {
      air_temperature = jb_xml_node_get_float (node, XML_TEMPERATURE, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_HUMIDITY))
    air_humidity = AIR_HUMIDITY;
  else
    {
      air_humidity = jb_xml_node_get_float (node, XML_HUMIDITY, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_VELOCITY))
    air_velocity = WIND_VELOCITY;
  else
    {
      air_velocity = jb_xml_node_get_float (node, XML_VELOCITY, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_ANGLE))
    air_angle = WIND_ANGLE;
  else
    {
      air_angle = jb_xml_node_get_float (node, XML_ANGLE, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_HEIGHT))
    air_height = WIND_HEIGHT;
  else
    {
      air_height = jb_xml_node_get_float (node, XML_HEIGHT, &k);
      if (k != 1)
        return 0;
    }
  if (!xmlHasProp (node, XML_UNCERTAINTY))
    air_uncertainty = WIND_UNCERTAINTY;
  else
    {
      air_uncertainty = jb_xml_node_get_float (node, XML_UNCERTAINTY, &k);
      if (k != 1)
        return 0;
    }
  return 1;
}

void
air_init (Air * air)
{
  air->temperature = air_temperature + KELVIN_TEMPERATURE;
  air->pressure = air_pressure;
  air->density = 0.029 * air->pressure / (R * air->temperature);
  air->velocity = air_velocity;
  air->uncertainty = air_uncertainty;
  air->angle = M_PI / 180. * air_angle;
  air->height = air_height;
  air->u0 = air->velocity * cos (air->angle);
  air->v0 = air->velocity * sin (air->angle);
  air->viscosity = 18.27e-6 * 411.15 / (120 + air->temperature) *
    pow (air->temperature / 291.15, 1.5) / air->density;
}

void
air_wind_uncertainty (Air * air, GRand * rand)
{
  float angle = 2 * M_PI * g_rand_double (rand);
  air->u = air->u0 + air->uncertainty * cos (angle);
  air->v = air->v0 + air->uncertainty * sin (angle);
}

void
dialog_air_new ()
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
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);
}
