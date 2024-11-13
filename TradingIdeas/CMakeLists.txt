# CMakeList.txt: TradingIdeas 的 CMake 專案，在此包含來源及定義
# 專案專屬邏輯。
#

# 將來源新增到此專案的可執行檔。
add_executable (TradingIdeas "TradingIdeas.cpp" "TradingIdeas.h" "K30Strategy.cpp" "K30Strategy.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TradingIdeas PROPERTY CXX_STANDARD 20)
endif()

# TODO: 新增測試，並視需要安裝目標。
