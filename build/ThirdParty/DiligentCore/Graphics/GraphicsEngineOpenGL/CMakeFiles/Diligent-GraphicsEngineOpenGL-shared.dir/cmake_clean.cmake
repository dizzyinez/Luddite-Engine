file(REMOVE_RECURSE
  "libGraphicsEngineOpenGL.pdb"
  "libGraphicsEngineOpenGL.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/Diligent-GraphicsEngineOpenGL-shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
