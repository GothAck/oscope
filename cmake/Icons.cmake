function(icons)
  set(icon_targets "")
  set(icon_files "")
  foreach(res IN LISTS ARGN)
    add_custom_target(icon_${res}
        COMMAND mkdir -p icons/hicolor/${res}x${res}
        COMMAND inkscape -z -e icons/hicolor/${res}x${res}/oscope.png -w ${res} -h ${res} ${CMAKE_CURRENT_SOURCE_DIR}/resources/oscope.svg
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/icons/hicolor/${res}x${res}/oscope.png
    )
    list(APPEND icon_targets "icon_${res}")
    list(APPEND icon_files "${CMAKE_CURRENT_BINARY_DIR}/icons/hicolor/${res}x${res}/oscope.png")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/icons/hicolor/${res}x${res}/oscope.png DESTINATION icons/hicolor/${res}x${res})
  endforeach()

  add_custom_target(win_ico
    COMMAND icotool -c -o oscope.ico ${icon_files}
  )

  add_custom_target(icon_build DEPENDS ${icon_targets})

  add_dependencies(win_ico icon_build)

  add_custom_target(icons DEPENDS icon_build win_ico)
  execute_process(COMMAND echo '<RCC><qresource prefix="/"><file alias="icon.png">icons/hicolor/256x256/oscope.png</file></qresource></RCC>' > ${CMAKE_CURRENT_BINARY_DIR}/gen.qrc)
endfunction()
