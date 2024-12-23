// KalmanFilter
#include "KalmanFilter.h"
#include <limits> 

KalmanFilter::KalmanFilter(double q, double r) :
		q_(q), r_(r), p_(1.), x_(-1.), isFirstCall_(true) {
}

KalmanFilter::KalmanFilter() :
		q_(0.0001), r_(0.01), p_(1.), x_(-1.), isFirstCall_(true) {
}

void KalmanFilter::update(double measurement) {
	if (isFirstCall_) {
		isFirstCall_ = false;
		x_ = measurement;
	} else {
		measurementUpdate();
		x_ = x_ + (measurement - x_) * k_;
	}
}

double KalmanFilter::get() {
	return x_;
}

void KalmanFilter::measurementUpdate() {
	k_ = 1 - r_ / (p_ + q_ + r_);
	p_ = r_ * k_;
}

DfFilter::DfFilter() :
		dataCnt_(0), dataSum_(0) {
}

void DfFilter::update(double measurement) {
	dataCnt_++;
	dataSum_ += measurement;
}

double DfFilter::get() {
	if (dataCnt_ > 0) {
		dataSum_ = (double) (dataSum_ / dataCnt_);
		dataCnt_ = 1;
	}
	return dataSum_;
}
