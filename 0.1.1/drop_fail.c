/*******************************************************************************

drop_fail: program to calculate fail velocity of drops

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <libxml/parser.h>

#define R 8.314	/* R thermodinamic constant */
#define G 9.81 /* gravitational acceleration constant */

#define DT 0.01

#define XML_AIR			(const xmlChar*)"air"
#define XML_ANGLE		(const xmlChar*)"angle"
#define XML_CFL			(const xmlChar*)"cfl"
#define XML_DIAMETER	(const xmlChar*)"diameter"
#define XML_DROP		(const xmlChar*)"drop"
#define XML_HUMIDITY	(const xmlChar*)"humidity"
#define XML_FAIL		(const xmlChar*)"fail"
#define XML_PRESSURE	(const xmlChar*)"pressure"
#define XML_TEMPERATURE	(const xmlChar*)"temperature"
#define XML_VELOCITY	(const xmlChar*)"velocity"
#define XML_X			(const xmlChar*)"x"
#define XML_Y			(const xmlChar*)"y"
#define XML_Z			(const xmlChar*)"z"

/*******************************************************************************

Air: struct to define the atmospheric conditions
	temperature: atmospheric temperature
	humidity: atmospheric humidity
	pressure: atmospheric pressure
	density: atmospheric density
	saturation_pressure: atmospheric saturation vapour pressure
	vapour_pressure: atmospheric vapour pressure
	viscosity: atmospheric viscosity

*******************************************************************************/

typedef struct
{
	double temperature, humidity, pressure, density, saturation_pressure,
		vapour_pressure, viscosity;
} Air;

/*******************************************************************************

Drop: struct to define a drop
	z: position z component
	t: time
	vz: velocity z component
	az: acceleration z component
	dt: numerical time step size
	drag: drag resistence factor
	diameter: drop diameter
	density: drop density
	cfl: CFL number

*******************************************************************************/

typedef struct
{
	double z, t, vz, az, dt, drag, diameter, density, cfl;
} Drop;

/*******************************************************************************

double xml_node_get_double(xmlNode *node, const xmlChar *prop, int *err)

	function to read a real in a XML node property

inputs:
	node: XML node
	prop: XML property
	err: error code

output:
	real

*******************************************************************************/

double xml_node_get_double(xmlNode *node, const xmlChar *prop, int *err)
{
	double x;
	char *buffer;
	*err = 0;
	if (!xmlHasProp(node, prop)) return 0.;
	buffer = (char*)xmlGetProp(node, prop);
	*err = sscanf(buffer, "%le", &x);
#if !G_OS_WIN32
	xmlFree(buffer);
#endif
	return x;
}

/*******************************************************************************

void air_print(Air *a)

	function to print the atmospheric variables

inputs:
	a: air struct

*******************************************************************************/

void air_print(Air *a)
{
	printf("Air:\n\ttemperature=%lg\n\tpressure=%lg\n\thumidity=%lg\n\t"
		"density=%le\n\tviscosity=%le\n",
		a->temperature,
		a->pressure,
		a->humidity,
		a->density,
		a->viscosity);
}

/*******************************************************************************

void air_open_xml(Air *a, xmlNode *node)

	function to open an air struct in a XML node

inputs:
	a: air struct
	node: XML node
	
*******************************************************************************/

void air_open_xml(Air *a, xmlNode *node)
{
	int k;
	double kelvin;
	a->temperature = xml_node_get_double(node, XML_TEMPERATURE, &k);
	if (!k) a->temperature = 20.;
	a->humidity = xml_node_get_double(node, XML_HUMIDITY, &k);
	if (!k) a->humidity = 100.;
	a->pressure = xml_node_get_double(node, XML_PRESSURE, &k);
	if (!k) a->pressure = 100000.;
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
	air_print(a);
}

void drop_density(Drop *d, double t)
{
	t -= 4.;
	d->density =
	   999.985064 + t * (-0.0037845 + t * (-0.0070759 + t * 0.0000333));
}

void drop_open_xml(Drop *d, Air *a, xmlNode *node)
{
	int k;
	d->z = xml_node_get_double(node, XML_Z, &k);
	d->vz = 0.;
	d->diameter = xml_node_get_double(node, XML_DIAMETER, &k);
    drop_density(d, a->temperature);
	d->cfl = xml_node_get_double(node, XML_CFL, &k);
	if (!k) d->cfl = 0.01;
	d->t = 0.;
}

static inline double drop_drag(double Reynolds)
{
	if (Reynolds>=1440.) return 0.45;
	else if (Reynolds>=128.) return 72.2/Reynolds-0.0000556*Reynolds+0.46;
	else if (Reynolds==0.) return 0.;
	return 33.3/Reynolds-0.0033*Reynolds+1.2;
}

double drop_move(Drop *d,Air *a)
{
	d->drag = 0.75 * d->vz * drop_drag(fabs(d->vz) * d->diameter
		/ a->viscosity) * a->density / (d->density * d->diameter);
	d->az = -G + d->drag * d->vz;
	return d->drag;
}

void drop_runge_kutta_4(Drop *d,Air *a)
{
	double dt2, dt6;
	Drop d2[1], d3[1], d4[1];
	memcpy(d2, d, sizeof(Drop));
	memcpy(d3, d, sizeof(Drop));
	memcpy(d4, d, sizeof(Drop));
	dt2 = 0.5 * d->dt;
	d2->z = d->z + dt2 * d->vz;
	d2->vz = d->vz + dt2 * d->az;
	drop_move(d2, a);
	d3->z = d->z + dt2 * d2->vz;
	d3->vz = d->vz + dt2 * d2->az;
	drop_move(d3, a);
	d4->z = d->z + d->dt * d3->vz;
	d4->vz = d->vz + d->dt * d3->az;
	drop_move(d4, a);
	dt6 = 1./6. * d->dt;
	d->z += dt6 * (d->vz + d4->vz + 2 * (d2->vz + d3->vz));
	d->vz += dt6 * (d->az + d4->az + 2 * (d2->az + d3->az));
	d->t += d->dt;
}

void drop_write(Drop *d,FILE *file)
{
	if (file) fprintf(file,"%lg %lg %lg %lg\n", d->t, d->z, d->vz, -d->drag);
}

int open_xml(char *name,Air *air,Drop *drop,FILE *file)
{
	xmlDoc *doc;
	xmlNode *node;
	doc=xmlParseFile(name);
	if (!doc) return 0;
	node=xmlDocGetRootElement(doc);
	if (!node || xmlStrcmp(node->name, XML_FAIL)) return 0;
	node = node->children;
	if (!node || xmlStrcmp(node->name, XML_AIR)) return 0;
	air_open_xml(air, node);
	for (node=node->next; node; node=node->next)
	{
		if (xmlStrcmp(node->name, XML_DROP)) return 0;
		drop_open_xml(drop, air, node);
		drop_write(drop, file);
		while (drop->z > 0.)
		{
			drop->dt = fmin(DT, drop->cfl / fabs(drop_move(drop, air)));
			drop_runge_kutta_4(drop, air);
			drop_write(drop, file);
		}
	}
	return 1;
}

int main(int argn,char **argc)
{
	FILE *file=NULL;
	Air air[1];
	Drop drop[1];
	xmlKeepBlanksDefault(0);
	if (argn<2 || argn>3)
	{
		printf
		("Usage of this program is:\n\tdrop_fail file_data [file_output]\n");
		return 1;
	}
	if (argn==3)
	{
		file=fopen(argc[2],"w");
		if (!file)
		{
			printf("Unable to open the output file\n");
			return 3;
		}
	}
	if (!open_xml(argc[1],air,drop,file))
	{
		printf("Unable to open the data file\n");
		return 3;
	}
	if (file) fclose(file);
	return 0;
}

