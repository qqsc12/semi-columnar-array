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
    static inline std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

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
        int vg_idx = -1, vd_idx = -1, abs_ig_idx = -1, abs_id_idx = -1, abs_is_idx = -1;
        bool header_found = false;

        // 🌟 建立初始大小為 0 的空引擎
        auto engine = std::make_unique<SemiColumnarEngine>();
        
        // 臨時流式儲存緩衝區
        std::vector<double> vg_vec, vd_vec, ig_vec, id_vec, is_vec;

        // 🌟 完美的 One-Pass 讀取：檔案只讀一遍，效能最大化！
        while (std::getline(file, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;

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
                    throw std::runtime_error("Data Robustness Error: Missing mandatory electrical channels.");
                }
                header_found = true;
                continue;
            }

            if (trimmed.rfind("DataValue", 0) == 0) {
                if (!header_found) {
                    throw std::runtime_error("Malformed B1500 CSV: DataValue records appeared before headers.");
                }

                std::stringstream ss(trimmed);
                std::string token;
                std::vector<double> vals;
                
                std::getline(ss, token, ','); // 跳過 "DataValue" 標記
                
                while (std::getline(ss, token, ',')) {
                    std::string t_tok = trim(token);
                    if (t_tok.empty()) vals.push_back(0.0);
                    else {
                        try { vals.push_back(std::stod(t_tok)); }
                        catch (...) { vals.push_back(0.0); }
                    }
                }

                // 資料流式吸入
                vg_vec.push_back(vals[vg_idx - 1]);
                vd_vec.push_back(vals[vd_idx - 1]);
                ig_vec.push_back(vals[abs_ig_idx - 1]);
                id_vec.push_back(vals[abs_id_idx - 1]);
                is_vec.push_back(vals[abs_is_idx - 1]);
            }
        }

        size_t final_size = vg_vec.size();
        if (final_size == 0) {
            throw std::runtime_error("Malformed B1500 CSV: No data records found.");
        }
        
        // 🌟 讀完後一鍵動態重塑（Resize），瞬間分配嚴格 64-byte 對齊硬體快取空間
        engine->resize(final_size);

        // 搬移到對齊渠道
        for (size_t i = 0; i < final_size; ++i) {
            engine->v_g()[i] = vg_vec[i];
            engine->v_d()[i] = vd_vec[i];
            engine->abs_i_g()[i] = ig_vec[i];
            engine->abs_i_d()[i] = id_vec[i];
            engine->abs_i_s()[i] = is_vec[i];
        }

        return engine;
    }
};

#endif
