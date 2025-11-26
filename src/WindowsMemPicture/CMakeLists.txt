cmake_minimum_required(VERSION 3.20)
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${CURRENT_DIR})

cpp_execute(${PROJECT_NAME})

if(MSVC)
    #设为启动项

    # set_property(DIRECTORY ${CMAKE_BINARY_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME}) 
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${OUT_RUN_PATH})
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "XWindowsFILE")

    # set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/GL")
    # set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LTCG
    # set_target_properties(${PROJECT_NAME} PROPERTIES
    # LINK_FLAGS "/ENTRY:wWinMainCRTStartup"
   
    # 修改入口点
    # target_link_options(${PROJECT_NAME} PRIVATE
    #     "/ENTRY:wWinMainCRTStartup"
    # )

    # 设置 Windows 特性
    # set_target_properties(${PROJECT_NAME} PROPERTIES
    #    WIN32_EXECUTABLE TRUE
    # )
    if(MFC_FOUND)
		#set_target_properties(${name} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
	
		target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:wWinMainCRTStartup)
		
		target_compile_definitions(${PROJECT_NAME} PRIVATE 
		    -DWIN32
		    -D_DEBUG
		    -D_WINDOWS
		    -D_AFXDLL
		)
	endif()
  
endif()