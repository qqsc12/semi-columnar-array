import os
import sys
import time
import glob
import pandas as pd
import numpy as np

sys.path.append("build")
import _semi_columnar_engine as se

def robust_python_parser_sim(filepath):
    """
    常規研究生改進版作法：先用 Python 動態找出 DataName 所在行，
    再用 Pandas 讀取並執行慢速迴圈與資料過濾。
    """
    # Step 1: 動態掃描定位 DataName 欄位，防禦不規則組態行數
    header_line_idx = -1
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        for idx, line in enumerate(f):
            if line.strip().startswith("DataName"):
                header_line_idx = idx
                break
                
    if header_line_idx == -1:
        return # 惡意損毀檔案，直接跳過

    # Step 2: 讓 Pandas 從正確的行數切入吸入
    df = pd.read_csv(filepath, skiprows=header_line_idx)
    df.columns = [c.strip() for c in df.columns]
    
    # 進行資料過濾與通道對齊
    vg = pd.to_numeric(df['Vg'], errors='coerce').dropna().values
    id_val = pd.to_numeric(df['absId'], errors='coerce').dropna().values
    
    # 模擬去程 Vth 萃取 (尋找 Gm 最大值)
    half = len(vg) // 2
    vg_fwd = vg[:half]
    id_fwd = id_val[:half]
    
    gm_max = -1.0
    for i in range(1, len(id_fwd)-1):
        dv = vg_fwd[i+1] - vg_fwd[i-1]
        di = id_fwd[i+1] - id_fwd[i-1]
        if dv != 0:
            gm = abs(di / dv)
            if gm > gm_max:
                gm_max = gm

def batch_python_parser(file_list):
    start_time = time.time()
    for filepath in file_list:
        robust_python_parser_sim(filepath)
    return time.time() - start_time

def batch_optimized_cpp_engine(file_list):
    start_time = time.time()
    for filepath in file_list:
        # C++ 高效 One-Pass 動態重塑流式讀取
        engine = se.B1500Parser.load_csv(filepath)
        # C++ 高效參數計算
        vth_fwd = engine.extract_vth(False)
        vth_bwd = engine.extract_vth(True)
        ss_fwd = engine.extract_ss(False)
    return time.time() - start_time

def main():
    # 自動遍歷根目錄下所有這 15 筆 BGAF 開頭的實戰 CSV
    file_list = glob.glob("BGAF*.csv")
    
    if not file_list:
        print("❌ 錯誤：在目前目錄找不到任何 'BGAF*.csv' 檔案！")
        return
    
    print("==========================================================")
    print("⚡ Starting Real-World Multi-File Performance Benchmark ⚡")
    print("==========================================================")
    print(f"Detected {len(file_list)} unique raw B1500 data files.")
    print("Simulating real wafer acceptance test (WAT) pipeline batch processing...\n")

    # 執行常規 Python 測試 (已加上防崩潰升級)
    print("⏳ Benchmark 1/2: Running Native Python Ingestion (Robust-Scan)...")
    python_time = batch_python_parser(file_list)
    print(f"⏱️ Total Python Time: {python_time:.4f} seconds")

    # 執行我們優化後的 C++ 引擎測試
    print("\n⏳ Benchmark 2/2: Running Our Hardware-Aware C++ SoA Engine...")
    cpp_time = batch_optimized_cpp_engine(file_list)
    print(f"⏱️ Total Optimized C++ SoA Engine Time: {cpp_time:.4f} seconds")

    print("\n==========================================================")
    print("📊 BENCHMARK VERDICT (5週優化成果終極驗收)")
    print("==========================================================")
    speedup = python_time / cpp_time
    print(f"🚀 Speedup Factor: {speedup:.1f}x FASTER!")
    print(f"💡 Latency Reduction: {((python_time - cpp_time)/python_time)*100:.1f}%")
    print("==========================================================")

if __name__ == "__main__":
    main()
    