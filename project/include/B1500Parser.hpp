#ifndef B1500_PARSER_HPP
#define B1500_PARSER_HPP

#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <memory>
#include "SemiColumnarEngine.hpp"

class B1500Parser {
private:
    // 去除兩端空格
    static inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    // 轉小寫以包容不規則大小寫
    static inline std::string to_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
        return str;
    }

public:
    static std::unique_ptr<SemiColumnarEngine> load_csv(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        std::string line;
        size_t data_row_count = 0;
        
        // Pass 1: 統計 DataValue 行數
        while (std::getline(file, line)) {
            std::string trimmed = trim(line);
            if (trimmed.rfind("DataValue", 0) == 0) {
                data_row_count++;
            }
        }

        if (data_row_count == 0) {
            throw std::runtime_error("Malformed B1500 CSV: No 'DataValue' rows detected.");
        }

        file.clear();
        file.seekg(0, std::ios::beg);

        int vg_idx = -1, vd_idx = -1, abs_ig_idx = -1, abs_id_idx = -1, abs_is_idx = -1;
        bool header_found = false;

        auto engine = std::make_unique<SemiColumnarEngine>(data_row_count);
        size_t current_row = 0;

        while (std::getline(file, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;

            // 捕捉 DataName 行並動態對齊直行索引
            if (trimmed.rfind("DataName", 0) == 0) {
                std::stringstream ss(trimmed);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(to_lower(trim(token)));
                }

                for (size_t i = 1; i < tokens.size(); ++i) {
                    if (tokens[i] == "vg") vg_idx = i;
                    else if (tokens[i] == "vd") vd_idx = i;
                    else if (tokens[i] == "absig") abs_ig_idx = i;
                    else if (tokens[i] == "absid") abs_id_idx = i;
                    else if (tokens[i] == "absis") abs_is_idx = i;
                }

                if (vg_idx == -1 || vd_idx == -1 || abs_ig_idx == -1 || abs_id_idx == -1 || abs_is_idx == -1) {
                    throw std::runtime_error("Data Robustness Error: Missing mandatory electrical channels in CSV header.");
                }
                header_found = true;
                continue;
            }

            // 捕捉 DataValue 數據並填入渠道
            if (trimmed.rfind("DataValue", 0) == 0) {
                if (!header_found) {
                    throw std::runtime_error("Malformed B1500 CSV: DataValue records appeared before headers.");
                }

                std::stringstream ss(trimmed);
                std::string token;
                std::vector<double> vals;
                
                std::getline(ss, token, ','); // 跳過開頭的 "DataValue"
                
                while (std::getline(ss, token, ',')) {
                    std::string t_tok = trim(token);
                    if (t_tok.empty()) vals.push_back(0.0);
                    else {
                        try {
                            vals.push_back(std::stod(t_tok));
                        } catch (...) {
                            vals.push_back(0.0);
                        }
                    }
                }

                if (current_row < data_row_count) {
                    engine->v_g()[current_row] = vals[vg_idx - 1];
                    engine->v_d()[current_row] = vals[vd_idx - 1];
                    engine->abs_i_g()[current_row] = vals[abs_ig_idx - 1];
                    engine->abs_i_d()[current_row] = vals[abs_id_idx - 1];
                    engine->abs_i_s()[current_row] = vals[abs_is_idx - 1];
                    current_row++;
                }
            }
        }

        return engine;
    }
};

#endif
