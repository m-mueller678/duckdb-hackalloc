add_library_unity(
        hackalloc
        OBJECT
        hackalloc.cpp)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:hackalloc>
        PARENT_SCOPE)
