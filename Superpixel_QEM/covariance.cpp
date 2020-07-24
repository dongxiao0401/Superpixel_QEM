#include "covariance.h"
#include "Eigen/Dense"
#include "Eigen/Eigenvalues"
#include <algorithm>
#include <vector>
using namespace std;
using namespace Eigen;
//double CovDet::ratio = 1e16
double CovDet::ratio = 0.1;

CovDet::CovDet()
{
	cov1.setZero();
	cov2.setZero();
	area = 0;
	centroid1.setZero();
	centroid2.setZero();
}

CovDet::CovDet(Vector5d v, double a)
{
	area = a;
	centroid1 = Vector2d(v(0), v(1));
	centroid2 = Vector3d(v(2), v(3), v(4));
	cov1.setZero();
	cov2.setZero();
}

double CovDet::energy()
{
	if(area < 8.0)
		return (cov1.trace()+cov2.trace());
	return (ratio * cov1.trace() + cov2.trace());
	//return (ratio * cov1.trace()+ (cov2.determinant() > 0 ? pow(cov2.determinant(), 1/3.0) : 0)  );
}

double CovDet::energy1()
{
	return cov1.trace();
}

double CovDet::energy2()
{
	/*if(area < 8.0)
		return cov2.trace();

	return (cov2.determinant() > 0 ? pow(cov2.determinant(), 1/3.0) : 0);*/

	return cov2.trace();
}