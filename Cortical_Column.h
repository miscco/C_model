/************************************************************************************************/
/*								header file of a cortical module								*/
/************************************************************************************************/
#pragma once
#include <cmath>
#include <vector>
#include "macros.h"
#include "parameters.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
using std::vector;

// typedefs for the RNG
typedef boost::mt11213b                    	ENG;    // Mersenne Twister
typedef boost::normal_distribution<double>	DIST;   // Normal Distribution
typedef boost::variate_generator<ENG,DIST> 	GEN;    // Variate generator

// implementation of the cortical module
class Cortical_Column {
public:
	// Constructors
	Cortical_Column(void)
	: Ve		(_INIT(E_L_e)),	Vi 	   	(_INIT(E_L_i)),	Na	 	(_INIT(Na_eq)),
	  Phi_ee	(_INIT(0.0)), 	Phi_ei 	(_INIT(0.0)), 	Phi_ie 	(_INIT(0.0)), 	Phi_ii	(_INIT(0.0)),
	  x_ee 		(_INIT(0.0)), 	x_ei   	(_INIT(0.0)),	x_ie   	(_INIT(0.0)), 	x_ii	(_INIT(0.0)),
	  alpha_Na 	(0), 			tau_Na	(0),			g_KNa	(0),	  		theta_e	(0),
	  sigma_e 	(0), 			dphi_c	(0),			input 	(0)
	{}

	Cortical_Column(double* Par)
	: Ve		(_INIT(E_L_e)),	Vi 	   	(_INIT(E_L_i)),	Na	 	(_INIT(Na_eq)),
	  Phi_ee	(_INIT(0.0)), 	Phi_ei 	(_INIT(0.0)), 	Phi_ie 	(_INIT(0.0)), 	Phi_ii	(_INIT(0.0)),
	  x_ee 		(_INIT(0.0)), 	x_ei   	(_INIT(0.0)),	x_ie   	(_INIT(0.0)), 	x_ii	(_INIT(0.0)),
	  alpha_Na 	(Par[0]), 		tau_Na	(Par[1]),		g_KNa	(Par[2]),	  	theta_e	(Par[3]),
	  sigma_e 	(Par[4]), 		dphi_c	(30E-3),			input 	(0)
	{}

	void 	set_RNG		(void);

	// firing rate functions
	double 	get_Qe		(int) const;
	double 	get_Qi		(int) const;

	// current functions
	double 	I_ee		(int) const;
	double 	I_ei		(int) const;
	double 	I_ie		(int) const;
	double 	I_ii		(int) const;
	double 	I_L_e		(int) const;
	double 	I_L_i		(int) const;
	double 	I_KNa		(int) const;

	// potassium concentration
	double 	Na_pump		(int) const;

	// noise functions
	double 	noise_xRK 	(int, int) const;

	// ODE functions
	void 	set_RK		(int);
	void 	add_RK	 	(void);

	friend void get_data (int, Cortical_Column&, _REPEAT(double*, 1));

private:
	// population variables
	vector<double> 	Ve,			// exitatory 		  membrane voltage
					Vi,			// exitatory 		  membrane voltage
					Na,			// Na concentration
					Phi_ee,		// PostSP from exitatory  		  to exitatory  		population
					Phi_ei,		// PostSP from exitatory  		  to inhibitory 		population
					Phi_ie,		// PostSP from inhibitory 		  to exitatory  		population
					Phi_ii,		// PostSP from inhibitory 		  to inhibitory 		population
					x_ee,		// derivative of Phi_ee
					x_ei,		// derivative of Phi_ei
					x_ie,		// derivative of Phi_ie
					x_ii;		// derivative of Phi_ii

	// adaption parameters
	double			alpha_Na,	// Sodium influx per spike
					tau_Na,		// Sodium time constant
					g_KNa;		// KNa conductance

	// resting potentials
	double			theta_e,		// pyramidal  leak
					sigma_e;		// inhibitory leak

	// Noise parameters
	double 			dphi_c;
	double			input;

	// random number generators
	vector<GEN>		MTRands;

	// container for random numbers
	vector<double>	Rand_vars;
};

