if (NOT DEFINED CLANG_STRICT_MODE)
    set(CLANG_STRICT_MODE false)
endif()
if (NOT DEFINED RUNTIME_SANITIZE)
    set(RUNTIME_SANITIZE false)
endif()

# Enforce C/C++ standard level
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Enforce to disable any compiler-specific extensions
set(CMAKE_C_EXTENSIONS NO)
set(CMAKE_CXX_EXTENSIONS NO)

# Flags that enable sanitization
set(COMMON_INSTRUMENTATION_FLAGS "-fsanitize=address -fsanitize=leak -fsanitize=undefined -fstack-check\
    -fstack-protector-all")

# Include sanitization if ${RUNTIME_SANITIZE} is set to true
if (${RUNTIME_SANITIZE})
    set(COMMON_WARN_FLAGS "${COMMON_WARN_FLAGS} ${COMMON_INSTRUMENTATION_FLAGS}")
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Flags that apply to both C and C++
    set(COMMON_WARN_FLAGS "${COMMON_WARN_FLAGS} -pedantic -Wall -Wextra\
    -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wdouble-promotion -Wduplicated-cond\
    -Wduplicated-branches -Wfloat-equal -Wformat=2 -Winit-self -Winline -Winvalid-pch -Wlogical-op\
    -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wno-unused -Wnull-dereference -Wodr\
    -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default\
    -Wswitch-enum -Wwrite-strings -Wundef -Wuninitialized -Wunreachable-code")

    # Flags that apply only to C OR C++
    set(C_WARN_FLAGS "${COMMON_WARN_FLAGS} -Wbad-function-cast -Wmissing-prototypes -Wnested-externs\
    -Wold-style-definition -Wstrict-prototypes")
    set(CXX_WARN_FLAGS "${COMMON_WARN_FLAGS} -Wctor-dtor-privacy -Wnoexcept -Wold-style-cast -Woverloaded-virtual\
    -Wsign-promo -Wstrict-null-sentinel -Wuseless-cast -Wzero-as-null-pointer-constant")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND ${CLANG_STRICT_MODE})
    # Flags that apply to both C and C++
    set(COMMON_WARN_FLAGS "${COMMON_WARN_FLAGS} -Weverything")

    # Flags that apply only to C OR C++
    set(C_WARN_FLAGS "${COMMON_WARN_FLAGS}")
    set(CXX_WARN_FLAGS "${COMMON_WARN_FLAGS}")

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Flags that apply to both C and C++
    set(COMMON_WARN_FLAGS "${COMMON_WARN_FLAGS} -Wpedantic -Wall -Wextra\
    -Warray-bounds-pointer-arithmetic -Wassign-enum -Wcast-align -Wcast-qual -Wconversion -Wdouble-promotion\
    -Wfloat-equal -Wformat=2 -Wgcc-compat -Wgnu -Winfinite-recursion -Winit-self -Winline -Winvalid-pch\
    -Wlogical-not-parentheses -Wlogical-op-parentheses -Wmissing-declarations -Wmissing-format-attribute\
    -Wmissing-include-dirs -Wmissing-prototypes -Wno-unused -Wnull-dereference -Wodr -Wpointer-arith -Wredundant-decls\
    -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default -Wswitch-enum -Wwrite-strings -Wundef\
    -Wuninitialized -Wunreachable-code")

    # Flags that apply only to C OR C++
    set(C_WARN_FLAGS "${COMMON_WARN_FLAGS} -Wbad-function-cast -Wmissing-prototypes -Wnested-externs\
    -Wold-style-definition -Wstrict-prototypes")
    set(CXX_WARN_FLAGS "${COMMON_WARN_FLAGS} -Wold-style-cast -Woverloaded-virtual -Wsign-promo\
    -Wzero-as-null-pointer-constant")

else ()
    message(FATAL "Unknown compiler combination")
endif ()

# Pass them back to the CMake variable
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_WARN_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARN_FLAGS}")