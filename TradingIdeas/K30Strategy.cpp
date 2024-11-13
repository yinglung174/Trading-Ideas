// TradingIdeas.cpp: 定義應用程式的進入點。
//

#include "K30Strategy.h"

using namespace std;

 void K30Strategy::loadCSV(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        if (!file.is_open()) {
            std::cerr << "Could not open the file!" << std::endl;
            return;
        }
        // Skip header
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string temp;
            OHLCV ohlcv;

            // Read date-time
            std::getline(ss, ohlcv.datetime, ',');

            std::getline(ss, temp, ',');
            ohlcv.open = std::stod(temp);
            std::getline(ss, temp, ',');
            ohlcv.high = std::stod(temp);
            std::getline(ss, temp, ',');
            ohlcv.low = std::stod(temp);
            std::getline(ss, temp, ',');
            ohlcv.close = std::stod(temp);
            std::getline(ss, temp, ',');
            ohlcv.volume = std::stod(temp);

            data.push_back(ohlcv);
        }
        file.close();
    }

double K30Strategy::calculateVWAP(int start, int end) {
    double cumulativePV = 0, cumulativeVolume = 0;
    for (int i = start; i <= end; ++i) {
        double price = (data[i].high + data[i].low + data[i].close) / 3;
        cumulativePV += price * data[i].volume;
        cumulativeVolume += data[i].volume;
    }
    return (cumulativeVolume == 0) ? 0 : cumulativePV / cumulativeVolume;
}

double K30Strategy::calculateATR(int period, int currentIndex) {
    if (currentIndex < period) return 0;
    double sum = 0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double tr = std::max({ data[i].high - data[i].low,
                                std::abs(data[i].high - data[i - 1].close),
                                std::abs(data[i].low - data[i - 1].close) });
        sum += tr;
    }
    return sum / period;
}

double K30Strategy::calculateRSI(int period, int currentIndex) {
    if (currentIndex < period) return 0;
    double gain = 0, loss = 0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double change = data[i].close - data[i - 1].close;
        if (change > 0)
            gain += change;
        else
            loss -= change;
    }
    double avgGain = gain / period;
    double avgLoss = loss / period;
    double rs = (avgLoss == 0) ? 0 : avgGain / avgLoss;
    return 100 - (100 / (1 + rs));
}

void K30Strategy::runStrategy(int periodVWAP, int periodATR, int periodRSI) {
    bool longPosition = false, shortPosition = false;
    double entryPrice = 0;
    std::string entryDate = ""; // Store entry date for comparison
    double atrMultiplier = 1.5;  // Adjust multiplier based on risk tolerance
    double longStopLoss = 0, shortStopLoss = 0;
    double longTakeProfit = 0, shortTakeProfit = 0;  // For take profit levels

    // Counters for stop loss and take profit hits
    int longStopLossHits = 0, longTakeProfitHits = 0;
    int shortStopLossHits = 0, shortTakeProfitHits = 0;
    int totalLongTrades = 0, totalShortTrades = 0;

    for (int i = periodVWAP; i < data.size(); ++i) {
        double vwap = calculateVWAP(i - periodVWAP + 1, i);
        double atr = calculateATR(periodATR, i);
        double rsi = calculateRSI(periodRSI, i);

        // Long position entry
        if (!longPosition && !shortPosition && rsi < 30 && data[i].close < vwap) {
            //std::cout << "Entering Long at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
            longPosition = true;
            entryPrice = data[i].close;
            entryDate = data[i].datetime;  // Store the entry date
            longStopLoss = entryPrice - atrMultiplier * atr;  // Calculate stop loss for long
            longTakeProfit = entryPrice + atrMultiplier * atr;  // Calculate take profit for long
            totalLongTrades++;
            //std::cout << "Stop Loss for Long: " << longStopLoss << ", Take Profit: " << longTakeProfit << std::endl;
        }
        // Exit long position
        else if (longPosition) {
            if (data[i].datetime > entryDate) {  // Ensure exit date is after entry date
                if (data[i].close < longStopLoss) {  // Stop loss hit
                    //std::cout << "Exiting Long at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
                    //std::cout << "Stop Loss Triggered at: " << longStopLoss << std::endl;
                    longPosition = false;
                    longStopLossHits++;  // Increment stop loss hit counter
                }
                else if (data[i].close > longTakeProfit) {  // Take profit hit
                    //std::cout << "Exiting Long at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
                    //std::cout << "Take Profit Triggered at: " << longTakeProfit << std::endl;
                    longPosition = false;
                    longTakeProfitHits++;  // Increment take profit hit counter
                }
            }
        }

        // Short position entry
        if (!longPosition && !shortPosition && rsi > 70 && data[i].close > vwap) {
            //std::cout << "Entering Short at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
            shortPosition = true;
            entryPrice = data[i].close;
            entryDate = data[i].datetime;  // Store the entry date
            shortStopLoss = entryPrice + atrMultiplier * atr;  // Calculate stop loss for short
            shortTakeProfit = entryPrice - atrMultiplier * atr;  // Calculate take profit for short
            totalShortTrades++;
            //std::cout << "Stop Loss for Short: " << shortStopLoss << ", Take Profit: " << shortTakeProfit << std::endl;
        }
        // Exit short position
        else if (shortPosition) {
            if (data[i].datetime > entryDate) {  // Ensure exit date is after entry date
                if (data[i].close > shortStopLoss) {  // Stop loss hit
                    //std::cout << "Exiting Short at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
                    //std::cout << "Stop Loss Triggered at: " << shortStopLoss << std::endl;
                    shortPosition = false;
                    shortStopLossHits++;  // Increment stop loss hit counter
                }
                else if (data[i].close < shortTakeProfit) {  // Take profit hit
                    //std::cout << "Exiting Short at: " << data[i].close << " on " << data[i].datetime << " with ATR: " << atr << std::endl;
                    //std::cout << "Take Profit Triggered at: " << shortTakeProfit << std::endl;
                    shortPosition = false;
                    shortTakeProfitHits++;  // Increment take profit hit counter
                }
            }
        }
    }

    // Calculate win rates
    double longWinRate = (totalLongTrades > 0) ?
        (double)(longTakeProfitHits) / totalLongTrades * 100 : 0;
    double shortWinRate = (totalShortTrades > 0) ?
        (double)(shortTakeProfitHits) / totalShortTrades * 100 : 0;

    // Overall win rate
    int totalTrades = totalLongTrades + totalShortTrades;
    int totalWins = longTakeProfitHits + shortTakeProfitHits;
    double overallWinRate = (totalTrades > 0) ?
        (double)(totalWins) / totalTrades * 100 : 0;

    // Print the total number of stop loss and take profit hits
    std::cout << "\nSummary of Trades:" << std::endl;
    std::cout << "Long Stop Loss Hits: " << longStopLossHits << std::endl;
    std::cout << "Long Take Profit Hits: " << longTakeProfitHits << std::endl;
    std::cout << "Short Stop Loss Hits: " << shortStopLossHits << std::endl;
    std::cout << "Short Take Profit Hits: " << shortTakeProfitHits << std::endl;
    std::cout << "Long Win Rate: " << longWinRate << "%" << std::endl;
    std::cout << "Short Win Rate: " << shortWinRate << "%" << std::endl;
    std::cout << "Overall Win Rate: " << overallWinRate << "%" << std::endl;
}
