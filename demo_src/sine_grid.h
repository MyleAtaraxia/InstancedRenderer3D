#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"

namespace godot {

// Grid of boxes at y = amplitude * sin(f*x + speed*t) * sin(f*z + speed*t).
// step() returns transforms in InstancedRenderer3D.update_instances() layout.
class SineGrid : public RefCounted {
  GDCLASS(SineGrid, RefCounted)

  int size_x = 100;
  int size_z = 100;
  real_t spacing = 1.2;
  real_t amplitude = 2;
  real_t frequency = 0.15;
  real_t speed = 2;
  PackedFloat32Array transforms;

protected:
  static void _bind_methods();

public:
  // Keys match the member names; missing keys keep their value.
  void configure(const Dictionary &params);
  PackedFloat32Array step(double time);
  int get_count() const { return size_x * size_z; }
};

} // namespace godot
