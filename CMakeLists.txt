#-------------------------------------------------------------------------
# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# --------------------------- CMAKE Options ------------------------------

#------------------------------ Includes ---------------------------------

# Include platform-specific directories
if (WIN32)

include_directories(${CMAKE_SOURCE_DIR}/Code/Currency_Converter)
include_directories(${CMAKE_SOURCE_DIR}/Code/Currency_Converter_ImGui)
include_directories(${CMAKE_SOURCE_DIR}/Code/ImGui/)
include_directories(${CMAKE_SOURCE_DIR}/Code/ImGui/Include)

elseif (UNIX)

    include_directories(/usr/include)
    include_directories(/usr/local/include)

endif()

#--------------------------- Includes Libraries---------------------------

# Include platform-specific directories
if (WIN32)

include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/include/curl/)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/include)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glfw/lib-vc2010-64/)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include)

elseif (UNIX)

    include_directories(/usr/include)
    include_directories(/usr/local/include)

endif()

#---------------------------- Main Program -------------------------------

# Project name and programming language
project(CurrencyConverter LANGUAGES CXX)

# Set the executable name actualCurrencyd on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_NAME CurrencyConverter)
	# ----- Showing message --------
	message("--------------------------")
	message("Compiling program in Debug")
	message("--------------------------")
    # Set the output directory for executables for Debug version
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Executable/Debug)
else()
    set(EXECUTABLE_NAME ConverterCurrency)
	# ----- Showing message --------
	
	message("----------------------------")
	message("Compiling program in Release")
	message("----------------------------")
    # Set the output directory for executables for Release version
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Executable/Release)
endif()

# Ensure Debug and Release executables go to separate folders
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/Executable/${OUTPUTCONFIG})
endforeach()

# Define header files
set(CurrencyConverterHeaders ${CMAKE_SOURCE_DIR}/Code/Currency_Converter/CurrencyConverter.h)
list(APPEND CurrencyConverterHeaders
    ${CMAKE_SOURCE_DIR}/Code/Currency_Converter_ImGui/ToolStyleEditor.h
    ${CMAKE_SOURCE_DIR}/Code/Currency_Converter_ImGui/CurrencyConverterImGui.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui_impl_glfw.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imconfig.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imstb_rectpack.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imstb_textedit.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imstb_truetype.h
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Include/imgui_impl_opengl3_loader.h
    ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include/glfw3.h
    ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include/glfw3native.h

)

# Define a variable for the code source directory
set(CurrencyConverterSources ${CMAKE_SOURCE_DIR}/Code/Currency_Converter/CurrencyConverter.cpp)
list(APPEND CurrencyConverterSources
    ${CMAKE_SOURCE_DIR}/Code/Currency_Converter_ImGui/ToolStyleEditor.cpp
    ${CMAKE_SOURCE_DIR}/Code/Currency_Converter_ImGui/CurrencyConverterImGui.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Source/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Source/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Source/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/Code/ImGui/Source/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/Code/Source/main.cpp
)
# Add the main.c source file
add_executable(${EXECUTABLE_NAME} ${CurrencyConverterSources} ${CurrencyConverterHeaders})

#------------------------------ Linking ----------------------------------

if (WIN32)

    # Define the path to the curl library
    set(libcurlib ${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/lib/libcurl.lib)

    # Find OpenGL
    find_package(OpenGL REQUIRED)

    # Define OpenGL
    set(OpenGLLibs ${OPENGL_LIBRARIES} opengl32.lib)

    # Define the path for the GLEW libraries
    set(GLFW_LIB_DIR ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/lib-vc2010-64/glfw3.lib)

    # Link the executable with the curl library
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${libcurlib} PRIVATE ${OpenGLLibs} PRIVATE ${GLFW_LIB_DIR})


elseif (UNIX)

    find_package(CURL REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE CURL::libcurl)

endif()

message("------------------------------------")
message("Linking libraries to the program ...")
message("------------------------------------")

#----------------------------- Post-Build Event ---------------------------

if (WIN32)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/bin/libcurl.dll
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/bin/zlib1.dll
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
    COMMENT "Copying required DLLs..."
)

elseif (UNIX)

    # Ensure required shared libraries are copied (not needed if installed system-wide)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "No additional post-build actions required for Linux."
    )

endif()

#-------------------------------- Installer --------------------------------

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "CurrencyConverter")
set(CPACK_PACKAGE_VERSION "0.45")
set(CPACK_PACKAGE_VENDOR "UwUCompany")
set(CPACK_PACKAGE_DESCRIPTION "A simple currency converter application.")
set(CPACK_PACKAGE_CONTACT "Diego Giménez Romanillos")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-V${CPACK_PACKAGE_VERSION}")


# Set package types
if (WIN32)

    set(CPACK_GENERATOR "ZIP") # For simple ZIP archive
    # set(CPACK_GENERATOR "NSIS") # For Windows Installer

elseif (UNIX)

    set(CPACK_GENERATOR "TGZ") # For Linux tar.gz

endif()

# Change CPack output directory
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/InstallerCurrencyConverter")

include(CPack)

#------------------------- Installation Rules ------------------------------

#Check for ${EXECUTABLE_NAME} at the start
if(NOT DEFINED EXECUTABLE_NAME)
    message(FATAL_ERROR "EXECUTABLE_NAME is not defined!")
endif()

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

if (WIN32)
    install(FILES ${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/bin/libcurl.dll 
                 ${CMAKE_SOURCE_DIR}/ThirdParty/x64-windows/bin/zlib1.dll DESTINATION bin)
endif()

#---------------------- Avoid _CPack_Packages Folder --------------------------

add_custom_target(pack
    COMMAND ${CMAKE_COMMAND} --build . --target package
    COMMENT "Generating package and cleaning up temporary files."
)