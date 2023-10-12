#pragma once

#include "platform.h"

class Lpf {
public:
  Lpf(float cut_off_frequency, float ts) : m_cut_off_frequency(cut_off_frequency), m_ts(ts) { m_tau = get_tau(); }

  float get_tau() { return 1 / (2 * PI * m_cut_off_frequency); }

  float filter(float data) {
    float val = (m_ts * data + m_tau * m_prev_data) / (m_tau + m_ts);
    m_prev_data = val;
    return val;
  }

  float get() { return m_prev_data; }

private:
  float m_cut_off_frequency;
  float m_ts;
  float m_tau;

  float m_prev_data = 0.0f;
};
