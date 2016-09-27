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
 * \file measurement.h
 * \brief Header file to define the data of a measurement point.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef MEASUREMENT__H
#define MEASUREMENT__H 1

/**
 * \struct Measurement
 * \brief struct to define the data of a measurement point.
 */
typedef struct
{
  double x;                     ///< x-coordenate.
  double y;                     ///< y-coordenate.
  double z;                     ///< z-coordenate.
  double dx;                    ///< x-range.
  double dy;                    ///< y-range.
  double xleft;                 ///< left boundary.
  double xright;                ///< right boundary.
  double ytop;                  ///< top boundary.
  double ybottom;               ///< bottom boundary.
} Measurement;

void measurement_init (Measurement * m);
void measurement_error (char *message);
void measurement_open_console (Measurement * m);
int measurement_open_xml (Measurement * m, xmlNode * node);
void measurement_write (Measurement * m, Drop * d, double *rold, FILE *file);

#if HAVE_GTK

/**
 * \struct DialogMeasurement
 * \brief dialog to set the data of a measurement point.
 */
typedef struct
{
  GtkLabel *label_x;            ///< x GtkLabel.
  GtkLabel *label_y;            ///< y GtkLabel.
  GtkLabel *label_z;            ///< z GtkLabel.
  GtkLabel *label_dx;           ///< dx GtkLabel.
  GtkLabel *label_dy;           ///< dz GtkLabel.
  GtkLabel *label_name;         ///< file name GtkLabel.
  GtkSpinButton *spin_x;        ///< x GtkSpinButton.
  GtkSpinButton *spin_y;        ///< y GtkSpinButton.
  GtkSpinButton *spin_z;        ///< z GtkSpinButton.
  GtkSpinButton *spin_dx;       ///< dx GtkSpinButton.
  GtkSpinButton *spin_dy;       ///< dy GtkSpinButton.
  GtkSpinButton *spin_name;     ///< file name GtkSpinButton.
  GtkGrid *grid;                ///< GtkGrid to pack the widgets.
  GtkDialog *window;            ///< GtkDialog to show the widgets.
} DialogMeasurement;

void dialog_measurement_new (Measurement * m);

#endif

#endif
