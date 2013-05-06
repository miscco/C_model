/*****************************************************************************************************/
/***********************			ODE function of the system			******************************/
/*****************************************************************************************************/
#include <vector>
#include "Cortical_Column.h"

// function that evaluates ODE using stochastic Runge Kutta
inline void ODE(Cortical_Column& Col, double ue1, double ue2) {
	for (auto i=1; i<=4; ++i) {
		Col.set_RK(i, ue1, ue2);
	}
	Col.add_RK(ue1);
}
