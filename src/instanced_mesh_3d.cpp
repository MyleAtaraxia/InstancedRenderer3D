#include "instanced_mesh_3d.h"
#include "instance_manager.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

void InstancedMesh3D::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_mesh", "mesh"),
                       &InstancedMesh3D::set_mesh);
  ClassDB::bind_method(D_METHOD("get_mesh"), &InstancedMesh3D::get_mesh);
  ClassDB::bind_method(D_METHOD("set_material", "material"),
                       &InstancedMesh3D::set_material);
  ClassDB::bind_method(D_METHOD("get_material"),
                       &InstancedMesh3D::get_material);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh",
                            PROPERTY_HINT_RESOURCE_TYPE, "Mesh"),
               "set_mesh", "get_mesh");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material",
                            PROPERTY_HINT_RESOURCE_TYPE,
                            "BaseMaterial3D,ShaderMaterial"),
               "set_material", "get_material");

  ClassDB::bind_method(D_METHOD("set_is_static", "is_static"),
                       &InstancedMesh3D::set_is_static);
  ClassDB::bind_method(D_METHOD("get_is_static"),
                       &InstancedMesh3D::get_is_static);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_static"), "set_is_static",
               "get_is_static");

  ClassDB::bind_method(D_METHOD("set_chunk_size", "size"),
                       &InstancedMesh3D::set_chunk_size);
  ClassDB::bind_method(D_METHOD("get_chunk_size"),
                       &InstancedMesh3D::get_chunk_size);
  ClassDB::bind_method(D_METHOD("set_cast_shadow", "setting"),
                       &InstancedMesh3D::set_cast_shadow);
  ClassDB::bind_method(D_METHOD("get_cast_shadow"),
                       &InstancedMesh3D::get_cast_shadow);
  ClassDB::bind_method(D_METHOD("set_layers", "layers"),
                       &InstancedMesh3D::set_layers);
  ClassDB::bind_method(D_METHOD("get_layers"), &InstancedMesh3D::get_layers);
  ADD_GROUP("Rendering", "");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chunk_size", PROPERTY_HINT_RANGE,
                            "0,1024,0.1,or_greater,suffix:m"),
               "set_chunk_size", "get_chunk_size");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "cast_shadow", PROPERTY_HINT_ENUM,
                            "Off,On,Double-Sided,Shadows Only"),
               "set_cast_shadow", "get_cast_shadow");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "layers",
                            PROPERTY_HINT_LAYERS_3D_RENDER),
               "set_layers", "get_layers");
  ClassDB::bind_method(D_METHOD("set_visibility_range_end", "distance"),
                       &InstancedMesh3D::set_visibility_range_end);
  ClassDB::bind_method(D_METHOD("get_visibility_range_end"),
                       &InstancedMesh3D::get_visibility_range_end);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "visibility_range_end",
                            PROPERTY_HINT_RANGE,
                            "0,4096,0.1,or_greater,suffix:m"),
               "set_visibility_range_end", "get_visibility_range_end");

  ClassDB::bind_method(D_METHOD("set_lods", "lods"), &InstancedMesh3D::set_lods);
  ClassDB::bind_method(D_METHOD("get_lods"), &InstancedMesh3D::get_lods);
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "lods", PROPERTY_HINT_ARRAY_TYPE,
                            vformat("%d/%d:InstancedMeshLOD", Variant::OBJECT,
                                    PROPERTY_HINT_RESOURCE_TYPE)),
               "set_lods", "get_lods");
}

void InstancedMesh3D::_notification(int p_what) {
  switch (p_what) {
  case NOTIFICATION_ENTER_TREE:
    set_notify_transform(true);
    register_instance();
    break;
  case NOTIFICATION_READY:
    // Kept in the editor so the scene stays editable.
    if (is_static && !Engine::get_singleton()->is_editor_hint()) {
      renderer.unref();
      instance_id = -1;
      queue_free();
    }
    break;
  case NOTIFICATION_TRANSFORM_CHANGED:
    if (renderer.is_valid()) {
      Transform3D t = get_global_transform();
      if (sleeping) {
        sleeping = false;
        reregister(); // first tick snaps if interpolated
      } else if (InstanceManager::get_cell(t.origin, chunk_size) != cell) {
        reregister(); // first tick snaps if interpolated
      } else {
        renderer->update_instance(instance_id, t);
        mark_moved();
      }
    }
    break;
  case NOTIFICATION_RESET_PHYSICS_INTERPOLATION:
    if (renderer.is_valid()) {
      renderer->reset_instance_interpolation(instance_id);
    }
    break;
  case NOTIFICATION_VISIBILITY_CHANGED:
    reregister();
    break;
  case NOTIFICATION_EXIT_TREE:
    unregister_instance();
    break;
  }
}

void InstancedMesh3D::register_instance() {
  if (mesh.is_null() || !is_visible_in_tree()) {
    return;
  }
  InstanceManager *manager = InstanceManager::get_singleton();
  Transform3D t = get_global_transform();
  cell = InstanceManager::get_cell(t.origin, chunk_size);
  renderer = manager->get_renderer(
      get_world_3d(),
      InstanceManager::make_lod_levels(mesh, lods, visibility_range_end),
      material, cell, chunk_size, cast_shadow, layers, is_static,
      visibility_range_end, sleeping);
  if (renderer.is_valid()) {
    instance_id = renderer->add_instance(t);
    if (!is_static && !sleeping) {
      mark_moved(); // nodes that never move fall asleep too
    }
  }
}

void InstancedMesh3D::unregister_instance() {
  if (renderer.is_valid()) {
    renderer->remove_instance(instance_id);
  }
  renderer.unref();
  instance_id = -1;
}

void InstancedMesh3D::reregister() {
  if (is_inside_tree()) {
    unregister_instance();
    register_instance();
  }
}

void InstancedMesh3D::mark_moved() {
  last_moved_frame = Engine::get_singleton()->get_process_frames();
  if (!sleep_watched && InstanceManager::get_singleton()->get_sleep_after_frames() > 0) {
    sleep_watched = true;
    InstanceManager::get_singleton()->watch_for_sleep(get_instance_id());
  }
}

bool InstancedMesh3D::update_sleep(uint64_t now) {
  int after = InstanceManager::get_singleton()->get_sleep_after_frames();
  if (renderer.is_null() || is_static || sleeping || after <= 0) {
    sleep_watched = false;
    return true;
  }
  if (now - last_moved_frame < (uint64_t)after) {
    return false;
  }
  // At rest there is nothing to interpolate, so switching batches can't pop.
  sleeping = true;
  sleep_watched = false;
  reregister();
  return true;
}

void InstancedMesh3D::set_mesh(const Ref<Mesh> &p_mesh) {
  mesh = p_mesh;
  reregister();
  update_gizmos();
}
Ref<Mesh> InstancedMesh3D::get_mesh() const { return mesh; }

void InstancedMesh3D::set_material(const Ref<Material> &p_material) {
  material = p_material;
  reregister();
}
Ref<Material> InstancedMesh3D::get_material() const { return material; }

void InstancedMesh3D::set_is_static(bool p_static) {
  is_static = p_static;
  sleeping = false;
  reregister();
}
bool InstancedMesh3D::get_is_static() const { return is_static; }

void InstancedMesh3D::set_chunk_size(real_t p_size) {
  chunk_size = p_size;
  reregister();
}
real_t InstancedMesh3D::get_chunk_size() const { return chunk_size; }

void InstancedMesh3D::set_cast_shadow(int p_setting) {
  cast_shadow = p_setting;
  reregister();
}
int InstancedMesh3D::get_cast_shadow() const { return cast_shadow; }

void InstancedMesh3D::set_layers(uint32_t p_layers) {
  layers = p_layers;
  reregister();
}
uint32_t InstancedMesh3D::get_layers() const { return layers; }

void InstancedMesh3D::set_visibility_range_end(real_t p_distance) {
  visibility_range_end = p_distance;
  reregister();
}
real_t InstancedMesh3D::get_visibility_range_end() const {
  return visibility_range_end;
}

// Editing a level's mesh or distance in place only emits `changed`.
void InstancedMesh3D::set_lods(const TypedArray<InstancedMeshLOD> &p_lods) {
  Callable on_changed = callable_mp(this, &InstancedMesh3D::reregister);
  for (int i = 0; i < lods.size(); i++) {
    Ref<InstancedMeshLOD> lod = lods[i];
    if (lod.is_valid() && lod->is_connected("changed", on_changed)) {
      lod->disconnect("changed", on_changed);
    }
  }
  lods = p_lods;
  for (int i = 0; i < lods.size(); i++) {
    Ref<InstancedMeshLOD> lod = lods[i];
    if (lod.is_valid() && !lod->is_connected("changed", on_changed)) {
      lod->connect("changed", on_changed);
    }
  }
  reregister();
}
TypedArray<InstancedMeshLOD> InstancedMesh3D::get_lods() const { return lods; }

} // namespace godot
