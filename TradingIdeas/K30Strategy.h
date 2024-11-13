// TradingIdeas.h: 標準系統 include 檔案的 include 檔案，
// 或專案專屬的 Include 檔案。

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>


struct OHLCV {
    std::string datetime;
    double open, high, low, close, volume;
};

class K30Strategy {
    public:
        std::vector<OHLCV> data;

        void loadCSV(const std::string& filename);

        double calculateVWAP(int start, int end);

        double calculateATR(int period, int currentIndex);

        double calculateRSI(int period, int currentIndex);

        void runStrategy(int periodVWAP, int periodATR, int periodRSI);
};