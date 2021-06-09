file(REMOVE_RECURSE
  "libGraphicsEngineVk.pdb"
  "libGraphicsEngineVk.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/Diligent-GraphicsEngineVk-shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
