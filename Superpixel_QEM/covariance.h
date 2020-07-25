#ifndef COVARIANCE_H
#define COVARIANCE_H
#include "Eigen/Dense"
#include "Eigen/Eigenvalues"
using namespace Eigen;
typedef Matrix<double, 5, 1> Vector5d;

class Covariance
{
public:
	Matrix2d cov1;
	Matrix3d cov2;

	double area;
	Vector2d centroid1;
	Vector3d centroid2;
	static double ratio;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
public:
	Covariance();
	Covariance(Vector5d v, double a);
	Covariance& operator=(const Covariance& Q)
	{ 
		cov1 = Q.cov1; cov2 = Q.cov2;
		area = Q.area; 
		centroid1 = Q.centroid1; centroid2 = Q.centroid2;
		return *this; 
	}
	Covariance& operator+=(const Covariance& Q)
	{ 
		//if (Q.area < 2.0)
		//	return *this;
		Covariance Q_old = *this;
		area = Q_old.area + Q.area;
		centroid1 = (Q_old.area * Q_old.centroid1 + Q.area * Q.centroid1)/(area);
		centroid2 = (Q_old.area * Q_old.centroid2 + Q.area * Q.centroid2)/(area);
		Vector2d ktoi1, ktoj1;
		ktoi1 = Q_old.centroid1 - centroid1;
		ktoj1 = Q.centroid1 - centroid1;
		cov1 = Q_old.cov1 + Q.cov1 + Q_old.area * ktoi1 * ktoi1.transpose() + Q.area * ktoj1 * ktoj1.transpose(); 
		Vector3d ktoi, ktoj;
		ktoi = Q_old.centroid2 - centroid2;
		ktoj = Q.centroid2 - centroid2;
		cov2 = Q_old.cov2 + Q.cov2 + Q_old.area * ktoi * ktoi.transpose() + Q.area * ktoj * ktoj.transpose(); 
		return *this; 
	}

	Covariance& operator-=(const Covariance& Q)
	{ 
		//if (Q.area < 2.0)
		//	return *this;
		Covariance Q_old = *this;
		area = Q_old.area - Q.area;
		centroid1 = (Q_old.area * Q_old.centroid1 - Q.area * Q.centroid1)/(area);
		centroid2 = (Q_old.area * Q_old.centroid2 - Q.area * Q.centroid2)/(area);
		Vector2d ktoi1, ktoj1;
		ktoi1 = Q_old.centroid1 - centroid1;
		ktoj1 = Q_old.centroid1 - Q.centroid1;
		cov1 = Q_old.cov1 - Q.cov1 - area * ktoi1 * ktoi1.transpose() - Q.area * ktoj1 * ktoj1.transpose(); 
		Vector3d ktoi, ktoj;
		ktoi = Q_old.centroid2 - centroid2;
		ktoj = Q_old.centroid2 - Q.centroid2;
		cov2 = Q_old.cov2 - Q.cov2 - area * ktoi * ktoi.transpose() - Q.area * ktoj * ktoj.transpose(); 
		return *this; 
	}

	double energy();
	double energy1();
	double energy2();
};

#endif
