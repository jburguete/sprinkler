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
 * \file sprinkler.h
 * \brief Header file to define the sprinkler data.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef SPRINKLER__H
#define SPRINKLER__H 1

/**
 * \struct Sprinkler
 * \brief struct to define a sprinkler.
 */
typedef struct
{
  double x;                     ///< position x component.
  double y;                     ///< position y component.
  double z;                     ///< position z component.
  double pressure;              ///< water pressure.
  double vertical_angle;        ///< vertical angle.
  double horizontal_angle;      ///< horizontal angle.
  double jet_time;              ///< time of the emitted jet.
  double diameter;              ///< nozzle diameter.
} Sprinkler;

void trajectory_init_with_sprinkler (Trajectory * t, Sprinkler * s);
void sprinkler_error (char *message);
int sprinkler_open_file (Sprinkler * s, FILE * file);
void sprinkler_open_console (Sprinkler * s);
int sprinkler_open_xml (Sprinkler * s, xmlNode * node);

#if HAVE_GTK

/**
 * \struct DialogSprinkler
 * \brief dialog to set the sprinkler data.
 */
typedef struct
{
  GtkLabel *label_x;            ///< x GtkLabel.
  GtkLabel *label_y;            ///< y GtkLabel.
  GtkLabel *label_z;            ///< z GtkLabel.
  GtkLabel *label_pressure;     ///< pressure GtkLabel.
  GtkLabel *label_vertical_angle;       ///< vertical angle GtkLabel.
  GtkLabel *label_horizontal_angle;     ///< horizontal angle GtkLabel.
  GtkLabel *label_jet_time;     ///< jet time GtkLabel.
  GtkLabel *label_diameter;     ///< nozzle diameter GtkLabel.
  GtkSpinButton *spin_x;        ///< x GtkSpinButton.
  GtkSpinButton *spin_y;        ///< y GtkSpinButton.
  GtkSpinButton *spin_z;        ///< z GtkSpinButton.
  GtkSpinButton *spin_pressure; ///< pressure GtkSpinButton.
  GtkSpinButton *spin_vertical_angle;   ///< vertical angle GtkSpinButton.
  GtkSpinButton *spin_horizontal_angle; ///< horizontal angle GtkSpinButton.
  GtkSpinButton *spin_jet_time; ///< jet time GtkSpinButton.
  GtkSpinButton *spin_diameter; ///< nozzle diameter GtkSpinButton.
  GtkGrid *grid;                ///< GtkGrid to pack the widgets.
  GtkDialog *window;            ///< GtkDialog to show the widgets.
} DialogSprinkler;

void dialog_sprinkler_new (Sprinkler * s);

#endif

#endif
