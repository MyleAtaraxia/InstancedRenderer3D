#pragma once

#include "godot_cpp/classes/editor_node3d_gizmo.hpp"
#include "godot_cpp/classes/editor_node3d_gizmo_plugin.hpp"
#include "godot_cpp/classes/editor_plugin.hpp"

namespace godot {

// InstancedMesh3D isn't a VisualInstance3D, so it needs this to be clickable.
class InstancedMesh3DGizmoPlugin : public EditorNode3DGizmoPlugin {
  GDCLASS(InstancedMesh3DGizmoPlugin, EditorNode3DGizmoPlugin)

protected:
  static void _bind_methods() {}

public:
  InstancedMesh3DGizmoPlugin();
  void load_color();
  String _get_gizmo_name() const override;
  bool _has_gizmo(Node3D *p_node) const override;
  void _redraw(const Ref<EditorNode3DGizmo> &p_gizmo) override;
};

class InstancedMeshEditorPlugin : public EditorPlugin {
  GDCLASS(InstancedMeshEditorPlugin, EditorPlugin)

  Ref<InstancedMesh3DGizmoPlugin> gizmo_plugin;

  void on_settings_changed();

protected:
  static void _bind_methods() {}
  void _notification(int p_what);
};

} // namespace godot
