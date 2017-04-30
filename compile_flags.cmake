set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CXX_WARN_FLAGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
    -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast \
    -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
    -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wzero-as-null-pointer-constant -Wuseless-cast")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARN_FLAGS}")