#include "sine_grid.h"
#include <cmath>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

void SineGrid::_bind_methods() {
  ClassDB::bind_method(D_METHOD("configure", "params"), &SineGrid::configure);
  ClassDB::bind_method(D_METHOD("step", "time"), &SineGrid::step);
  ClassDB::bind_method(D_METHOD("get_count"), &SineGrid::get_count);
}

void SineGrid::configure(const Dictionary &params) {
  size_x = params.get("size_x", size_x);
  size_z = params.get("size_z", size_z);
  spacing = params.get("spacing", spacing);
  amplitude = params.get("amplitude", amplitude);
  frequency = params.get("frequency", frequency);
  speed = params.get("speed", speed);
  transforms.resize(get_count() * 12);
}

PackedFloat32Array SineGrid::step(double time) {
  float *w = transforms.ptrw();
  real_t phase = speed * time;
  real_t x0 = -(size_x - 1) * spacing * 0.5;
  real_t z0 = -(size_z - 1) * spacing * 0.5;
  for (int z = 0; z < size_z; z++) {
    real_t pz = z0 + z * spacing;
    real_t wave_z = std::sin(frequency * pz + phase);
    for (int x = 0; x < size_x; x++, w += 12) {
      real_t px = x0 + x * spacing;
      real_t y = amplitude * std::sin(frequency * px + phase) * wave_z;
      // Identity basis rows, each followed by its origin component.
      w[0] = 1, w[1] = 0, w[2] = 0, w[3] = px;
      w[4] = 0, w[5] = 1, w[6] = 0, w[7] = y;
      w[8] = 0, w[9] = 0, w[10] = 1, w[11] = pz;
    }
  }
  return transforms;
}

} // namespace godot
