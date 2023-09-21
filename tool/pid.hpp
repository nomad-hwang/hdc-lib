#pragma once

#include "platform.h"

class PID {
public:
  PID(float Kp, float Ki, float Kd) : m_Kp(Kp), m_Ki(Ki), m_Kd(Kd) {}

  void set(float setpoint) { m_setpoint = setpoint; }

  //   float update(float now, float dt) {
  //     float error = m_setpoint - now;

  //     m_integral += error * dt;
  //     m_derivative = (error - m_previous_error) / dt;
  //     m_previous_error = error;

  //     return m_Kp * error + m_Ki * m_integral + m_Kd * m_derivative;
  //   }
  float update(float error, float dt) {
    m_integral += error * dt;
    m_derivative = (error - m_previous_error) / dt;
    m_previous_error = error;

    return m_Kp * error + m_Ki * m_integral + m_Kd * m_derivative;
  }

private:
  float m_Kp;
  float m_Ki;
  float m_Kd;

  float m_integral = 0.0f;
  float m_derivative = 0.0f;
  float m_previous_error = 0.0f;

  float m_setpoint = 0.0f;
};
