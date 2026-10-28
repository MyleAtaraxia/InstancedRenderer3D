#include "instanced_mesh_lod.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot {

void InstancedMeshLOD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &InstancedMeshLOD::set_mesh);
  ClassDB::bind_method(D_METHOD("get_mesh"), &InstancedMeshLOD::get_mesh);
  ClassDB::bind_method(D_METHOD("set_distance", "distance"),
                       &InstancedMeshLOD::set_distance);
  ClassDB::bind_method(D_METHOD("get_distance"), &InstancedMeshLOD::get_distance);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE,
                            "Mesh"),
               "set_mesh", "get_mesh");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distance", PROPERTY_HINT_RANGE,
                            "0,4096,0.1,or_greater,suffix:m"),
               "set_distance", "get_distance");
}

void InstancedMeshLOD::set_mesh(const Ref<Mesh> &p_mesh) {
  mesh = p_mesh;
  emit_changed();
}

void InstancedMeshLOD::set_distance(real_t p_distance) {
  distance = p_distance;
  emit_changed();
}

} // namespace godot
