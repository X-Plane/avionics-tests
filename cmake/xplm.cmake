# Use static libc/libgcc etc
function(use_static_libc)
    if(WIN32)
        set(CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS} -static-libgcc"
            PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++"
            PARENT_SCOPE)
        set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS
            "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s"
            PARENT_SCOPE)
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS
            "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}  -static-libgcc -static-libstdc++ -s"
            PARENT_SCOPE)
    endif()
    
endfunction(use_static_libc)

# Import the X-Plane SDK and create targets for it
function(find_xplane_sdk SDK_ROOT SDK_VERSION)
    if(APPLE)
        find_library(XPLM_LIBRARY
            NAMES XPLM
            PATHS ${SDK_ROOT}/Libraries/Mac/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
        )
        find_library(XPWIDGETS_LIBRARY
            XPWidgets
            ${SDK_ROOT}/Libraries/Mac/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
        )
        find_library(CocoaLib Cocoa)
    elseif(WIN32)
        find_library(XPLM_LIBRARY
            NAMES XPLM_64
            PATHS ${SDK_ROOT}/Libraries/Win/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
        )
        find_library(XPWIDGETS_LIBRARY
            NAMES XPWidgets_64
            PATHS ${SDK_ROOT}/Libraries/Win/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
        )
    endif()
    
    if(WIN32)
        add_library(xplm SHARED IMPORTED GLOBAL)
        add_library(xpwidgets SHARED IMPORTED GLOBAL)
        
        set_target_properties(xplm PROPERTIES IMPORTED_IMPLIB "${XPLM_LIBRARY}")
        set_target_properties(xpwidgets PROPERTIES IMPORTED_IMPLIB "${XPWIDGETS_LIBRARY}")
        
        target_compile_definitions(xplm INTERFACE -DAPL=0 -DIBM=1 -DLIN=0)
        target_compile_definitions(xpwidgets INTERFACE -DAPL=0 -DIBM=1 -DLIN=0)
    elseif(APPLE)
        add_library(xplm SHARED IMPORTED GLOBAL)
        add_library(xpwidgets SHARED IMPORTED GLOBAL)
        
        set_target_properties(xplm PROPERTIES IMPORTED_LOCATION "${XPLM_LIBRARY}/XPLM")
        set_target_properties(xpwidgets PROPERTIES IMPORTED_LOCATION "${XPWIDGETS_LIBRARY}/XPWidgets")
        
        target_compile_definitions(xplm INTERFACE -DAPL=1 -DIBM=0 -DLIN=0)
        target_compile_definitions(xpwidgets INTERFACE -DAPL=1 -DIBM=0 -DLIN=0)
    else()
        add_library(xplm INTERFACE)
        add_library(xpwidgets INTERFACE)
        
        target_compile_definitions(xplm INTERFACE -DAPL=0 -DIBM=0 -DLIN=1)
        target_compile_definitions(xpwidgets INTERFACE -DAPL=0 -DIBM=0 -DLIN=1)
    endif()
    
    if(SDK_VERSION EQUAL 300)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)
    elseif(SDK_VERSION EQUAL 301)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)
    elseif(SDK_VERSION EQUAL 302)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1)
    elseif(SDK_VERSION EQUAL 303)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1 -DXPLM303=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1 -DXPLM303=1)
    elseif(SDK_VERSION EQUAL 400)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1  -DXPLM302=1 -DXPLM303=1 -DXPLM400=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1  -DXPLM302=1 -DXPLM303=1 -DXPLM400=1)
    elseif(SDK_VERSION EQUAL 410)
        target_compile_definitions(xplm
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1  -DXPLM302=1 -DXPLM303=1 -DXPLM400=1 -DXPLM410=1)
        target_compile_definitions(xpwidgets
            INTERFACE -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1  -DXPLM302=1 -DXPLM303=1 -DXPLM400=1 -DXPLM410=1)
    else()
        message(FATAL_ERROR "Library version one of: 300, 301, 302, 303, 400")
    endif()
    
    target_include_directories(xplm INTERFACE "${SDK_ROOT}/CHeaders/XPLM")
    target_include_directories(xpwidgets INTERFACE
        "${SDK_ROOT}/CHeaders/XPLM" "${SDK_ROOT}/CHeaders/Widgets")
    
    set(XPLM_INCLUDE_DIR
        "${SDK_ROOT}/CHeaders/XPLM"
        "${SDK_ROOT}/CHeaders/Widgets"
        "${SDK_ROOT}/CHeaders/Wrappers"
        CACHE INTERNAL "XPLM SDK INCLUDE DIRECTORIES"
    )
endfunction(find_xplane_sdk)

function(add_xplane_plugin lib_name ...)
    set(SRC ${ARGV})
    list(REMOVE_AT SRC 0)
    
    add_library(${lib_name} SHARED ${SRC})
    set_target_properties(${lib_name} PROPERTIES PREFIX "")
    set_target_properties(${lib_name} PROPERTIES SUFFIX "")
    set_target_properties(${lib_name} PROPERTIES OUTPUT_NAME "${lib_name}.xpl")
    include_directories(${lib_name} PUBLIC ${XPLM_INCLUDE_DIR})
    
    # Link libraries
    if(APPLE)
        target_compile_definitions(${lib_name} PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
        target_compile_definitions(${lib_name} PUBLIC -DGL_SILENCE_DEPRECATION)
        target_link_libraries(${lib_name} PUBLIC ${CocoaLib})
    	set_target_properties(${lib_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/mac_x64"
            RUNTIME_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/mac_x64")
    elseif(WIN32)
        target_compile_definitions(${lib_name} PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
        target_compile_definitions(${lib_name} PUBLIC -D_WIN32_WINNT=0x0600)
    	set_target_properties(${lib_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/win_x64"
            RUNTIME_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/win_x64"
        )
        
    else()
        target_compile_definitions(${lib_name} PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
    	set_target_properties(${lib_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/lin_x64"
            RUNTIME_OUTPUT_DIRECTORY
                "${PROJECT_SOURCE_DIR}/${lib_name}/lin_x64")
    endif()
    
    # target_link_libraries(${lib_name} PUBLIC xplm xpwidgets)
    
    set_target_properties(${lib_name} PROPERTIES
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        POSITION_INDEPENDENT_CODE ON
    )
    set_target_properties(${lib_name} PROPERTIES LINK_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -fno-stack-protector")
    
endfunction(add_xplane_plugin)

