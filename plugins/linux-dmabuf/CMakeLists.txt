project(linux-dmabuf)

find_package(PkgConfig)
pkg_check_modules(DRM libdrm)
if(NOT DRM_FOUND)
	message(WARNING "libdrm not found, dmabuf soure will be disabled")
	return()
endif()

add_library(linux-dmabuf MODULE dmabuf.c
	../linux-capture/xcursor-xcb.c)
target_include_directories(linux-dmabuf PRIVATE
	"${CMAKE_SOURCE_DIR}/libobs"
	${XCB_INCLUDE_DIRS})
target_link_libraries(linux-dmabuf libobs glad
	${XCB_LIBRARIES})
set_target_properties(linux-dmabuf PROPERTIES FOLDER "plugins")
install_obs_plugin(linux-dmabuf)

add_executable(obs-drmsend drmsend.c)
target_include_directories(obs-drmsend PRIVATE ${DRM_INCLUDE_DIRS})
target_link_libraries(obs-drmsend PRIVATE ${DRM_LIBRARIES})

# this breaks building OBS under a regular user
# FIXME Make it a part of install process, which should run under root
#
#	add_custom_command(TARGET drmsend POST_BUILD
#		COMMAND setcap cap_sys_admin+ep "$<TARGET_FILE:drmsend>")
install_obs_core(obs-drmsend)
