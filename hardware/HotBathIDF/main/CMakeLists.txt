FILE(GLOB CSources *.c)

idf_component_register(SRCS ${CSources}
  INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
