SET(Bin_Path ${PROJECT_SOURCE_DIR}/bin)

# reactive CPP submodule dependency
FIND_LIBRARY(RxCpp_Lib NAMES libRxCpp.so PATHS ${Bin_Path})
