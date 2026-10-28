#!/usr/bin/env python
import os

env = SConscript("godot-cpp/SConstruct")

# Set by CI; also covers the godot-cpp objects built above.
if os.environ.get("SCONS_CACHE"):
    CacheDir(os.environ["SCONS_CACHE"])

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

output_dir = "demo/addons/instanced_renderer/bin"

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{}/libInstancedRenderer.{}.{}.framework/libInstancedRenderer.{}.{}".format(
            output_dir,
            env["platform"],
            env["target"],
            env["platform"],
            env["target"],
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "{}/libInstancedRenderer.{}.{}.simulator.a".format(
                output_dir, env["platform"], env["target"]
            ),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "{}/libInstancedRenderer.{}.{}.a".format(
                output_dir, env["platform"], env["target"]
            ),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "{}/libInstancedRenderer{}{}".format(
            output_dir, env["suffix"], env["SHLIBSUFFIX"]
        ),
        source=sources,
    )


Default(library)

# Demo-only extension (not part of the addon): `scons demo`.
demo_library = env.SharedLibrary(
    "demo/bin/libDemo{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=Glob("demo_src/*.cpp"),
)
Alias("demo", demo_library)
