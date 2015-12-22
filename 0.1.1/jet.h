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
 * \file jet.h
 * \brief Header file to define the data of a sprinkler main jet.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef JET__H
#define JET__H 1

/**
 * \struct 
 * \brief struct to define the main jet of a sprinkler.
 */
typedef struct
{
  double a[5];                  ///< interpolation coefficients.
} Jet;

double jet_height (Jet * j, double x);
void jet_error (char *message);
int jet_open_file (Jet * j, FILE * file);
void jet_open_console (Jet * j);
int jet_open_xml (Jet * j, xmlNode * node);
void trajectory_invert_with_jet (Trajectory * t, Air * a, Jet * j);

#if HAVE_GTK

/**
 * \struct DialogJet
 * \brief dialog to set the data of a sprinkler main jet.
 */
typedef struct
{
  GtkLabel *label_a[5];         ///< array of a GtkLabels.
  GtkSpinButton *spin_a[5];     ///< array of a GtkSpinButtons.
  GtkGrid *grid;                ///< GtkGrid to pack the widgets.
  GtkDialog *window;            ///< GtkDialog to show the widgets.
} DialogJet;

void dialog_jet_new (Jet * j);

#endif

#endif
