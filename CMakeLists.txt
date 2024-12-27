cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(SceMemDisk C ASM)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -nostdlib -fno-optimize-sibling-calls")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_executable(SceMemDisk
  src/mddev.c
  src/md_vfs.c
)

target_include_directories(SceMemDisk PUBLIC include/)

target_link_libraries(SceMemDisk
  taihenForKernel_stub
  taihenModuleUtils_stub
  SceCpuForDriver_stub
  SceSysclibForDriver_stub
  SceSysmemForDriver_stub
  SceProcEventForDriver_stub
  SceThreadmgrForDriver_stub
  SceDebugForDriver_stub
  SceSysrootForKernel_stub
  SceProcessmgrForDriver_stub
  SceIofilemgrForDriver_stub
)

vita_create_self(memdisk.skprx SceMemDisk CONFIG exports.yml UNSAFE)

vita_create_stubs(stubs SceMemDisk ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml KERNEL)

add_custom_target(memdisk_all
  ALL
  DEPENDS libSceMemDisk_stub.a
  DEPENDS libSceMemDisk_stub_weak.a
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/stubs/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.a"
)

install(FILES memdisk.h
  DESTINATION include
)
