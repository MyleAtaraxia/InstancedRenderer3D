#pragma once

#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot {

// Drawn from `distance` until the next level's distance (or the node's
// visibility_range_end).
class InstancedMeshLOD : public Resource {
  GDCLASS(InstancedMeshLOD, Resource)

  Ref<Mesh> mesh;
  real_t distance = 0;

protected:
  static void _bind_methods();

public:
  void set_mesh(const Ref<Mesh> &p_mesh);
  Ref<Mesh> get_mesh() const { return mesh; }
  void set_distance(real_t p_distance);
  real_t get_distance() const { return distance; }
};

} // namespace godot
