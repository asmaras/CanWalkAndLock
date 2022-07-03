FILE(GLOB_RECURSE app_sources *.*)

idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS "."
    PRIV_REQUIRES nvs_flash
    )
