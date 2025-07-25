#include <Eigen/Dense>
#include <cassert>
#include <cmath>

#include "UnicycleModel.hpp"

int UnicycleModel::getStateDim() const { return 3; }

int UnicycleModel::getInputDimension() const { return 2; }

Eigen::Vector3d
UnicycleModel::getNextState(const Eigen::Vector3d &current_state,
                            const Eigen::VectorXd &control_input,
                            double dt) const {
  assert(control_input.size() == 2 &&
         "UnicycleModel expects 2 control inputs (v, omega) for getNextState!");

  double x_k = current_state(0);     // x
  double y_k = current_state(1);     // y
  double theta_k = current_state(2); // theta

  double v_k = control_input(0);     // Linear velocity
  double omega_k = control_input(1); // Angular velocity

  const double EPSILON = 1e-6; // Threshold for near-zero angular velocity

  Eigen::Vector3d next_state;

  if (std::abs(omega_k) < EPSILON) {
    // --- Moving straight (linear approximation) ---
    // Robot moves forward in its current heading direction.
    next_state(0) = x_k + v_k * std::cos(theta_k) * dt;
    next_state(1) = y_k + v_k * std::sin(theta_k) * dt;
    next_state(2) = theta_k; // Orientation does not change
  } else {
    // --- Turning Motion (Non-linear General Case - Analytical Solution) ---
    // Robot moves along a circular arc.
    // Calculations are based on the Instantaneous Center of Curvature (ICC).
    // Here we use the analytical method.
    double turn_radius = v_k / omega_k;

    double icc_global_x = x_k - turn_radius * std::sin(theta_k);
    double icc_global_y = y_k + turn_radius * std::cos(theta_k);

    double next_global_theta = theta_k + omega_k * dt;

    next_state(0) = icc_global_x + turn_radius * std::sin(next_global_theta);
    next_state(1) = icc_global_y - turn_radius * std::cos(next_global_theta);
    next_state(2) = next_global_theta;
  }

  return next_state;
}