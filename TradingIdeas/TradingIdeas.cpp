// TradingIdeas.cpp: 定義應用程式的進入點。
//

#include "TradingIdeas.h"
namespace fs = std::filesystem;
using namespace std;

int main() {
    std::string folderPath = "C:/Users/HP/PycharmProjects/neverlandv2/data/train/";
    int periodVWAP = 14, periodATR = 14, periodRSI = 14;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        // Check if it's a regular file and ends with .csv
        if (fs::is_regular_file(entry) && entry.path().extension() == ".csv") {
            std::string filePath = entry.path().string();
            std::cout << "Loading CSV file: " << filePath << std::endl;

            // Dynamically allocate a new K30Strategy instance for each file
            K30Strategy* bt = new K30Strategy();

            // Load the CSV file and run the strategy for this specific file
            bt->loadCSV(filePath);
            bt->runStrategy(periodVWAP, periodATR, periodRSI);

            // Clean up the dynamically allocated memory after usage
            delete bt;
        }
    }


    return 0;
}
