#include "covariance.h"
#include "Eigen/Dense"
#include "Eigen/Eigenvalues"
#include <algorithm>
#include <vector>
using namespace std;
using namespace Eigen;
//double Covariance::ratio = 1e16
double Covariance::ratio = 0.1;

Covariance::Covariance()
{
	cov1.setZero();
	cov2.setZero();
	area = 0;
	centroid1.setZero();
	centroid2.setZero();
}

Covariance::Covariance(Vector5d v, double a)
{
	area = a;
	centroid1 = Vector2d(v(0), v(1));
	centroid2 = Vector3d(v(2), v(3), v(4));
	cov1.setZero();
	cov2.setZero();
}

double Covariance::energy()
{
	if(area < 8.0)
		return (cov1.trace()+cov2.trace());
	return (ratio * cov1.trace() + cov2.trace());
	//return (ratio * cov1.trace()+ (cov2.determinant() > 0 ? pow(cov2.determinant(), 1/3.0) : 0)  );
}

double Covariance::energy1()
{
	return cov1.trace();
}

double Covariance::energy2()
{
	/*if(area < 8.0)
		return cov2.trace();

	return (cov2.determinant() > 0 ? pow(cov2.determinant(), 1/3.0) : 0);*/

	return cov2.trace();
}