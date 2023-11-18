FILE(GLOB Sources "*.c" "*.cpp")

idf_component_register(SRCS ${Sources}
  INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")