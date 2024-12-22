#ifndef __KALMAN_FILTER__
#define __KALMAN_FILTER__

class KalmanFilter{
private: 
  double q_;
  double r_;
  double p_;
  double x_;
  double k_;
  bool  isFirstCall_;
  void measurementUpdate();

public:
  KalmanFilter(double q, double r);
  KalmanFilter();
  void update(double measurement);
  double get();
};

class DfFilter{
  int dataCnt_;
  double dataSum_;

public:
  DfFilter();
  void update(double measurement);
  double get();
};

#endif // __KALMAN_FILTER__
