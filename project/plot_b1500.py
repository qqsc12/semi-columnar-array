import os
import sys
import numpy as np
import matplotlib.pyplot as plt

sys.path.append("build")
import _semi_columnar_engine as se

def process_and_plot_device(csv_file, output_dir="plots"):
    if not os.path.exists(csv_file):
        print(f"❌ 錯誤：找不到量測檔案 '{csv_file}'")
        return False

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    base_name = os.path.splitext(os.path.basename(csv_file))[0]

    print(f"🚀 正在透過 C++ 讀取並動態解析: {csv_file}")
    engine = se.B1500Parser.load_csv(csv_file)
    
    total_size = engine.size()
    half = total_size // 2  # 1302 / 2 = 651
    
    vg_all = engine.v_g()
    vd_all = engine.v_d()
    id_all = engine.abs_i_d()
    ig_all = engine.abs_i_g()
    is_all = engine.abs_i_s()
    
    # 拆分去程與回程
    vg_fwd, vg_bwd = vg_all[:half], vg_all[half:]
    id_fwd, id_bwd = id_all[:half], id_all[half:]
    ig_fwd, ig_bwd = ig_all[:half], ig_all[half:]
    is_fwd, is_bwd = is_all[:half], is_all[half:]
    
    vd_value = vd_all[0]

    print(f"  📊 成功讀取 {total_size} 筆量測點！(去程: {half} 點, 回程: {half} 點)")

    # ---- 開始繪製元件轉移特性圖 ----
    fig, ax = plt.subplots(figsize=(8, 6))
    
    # ✨ 【去程 Forward】：使用實線 (Solid Line)
    ax.semilogy(vg_fwd, id_fwd, color='red', linestyle='-',  linewidth=2.2, label='$I_d$ (Forward)')
    ax.semilogy(vg_fwd, ig_fwd, color='blue', linestyle='-', linewidth=1.2, label='$I_g$ (Forward)')
    ax.semilogy(vg_fwd, is_fwd, color='green', linestyle='-', linewidth=1.2, label='$I_s$ (Forward)')
    
    # ✨ 【回程 Backward】：使用帶圓點虛線 (Dashed) 做出清晰區隔
    ax.semilogy(vg_bwd, id_bwd, color='darkred', linestyle='--', linewidth=1.8, label='$I_d$ (Backward)')
    ax.semilogy(vg_bwd, ig_bwd, color='darkblue', linestyle='--', linewidth=1.0, label='$I_g$ (Backward)')
    ax.semilogy(vg_bwd, is_bwd, color='darkgreen', linestyle='--', linewidth=1.0, label='$I_s$ (Backward)')
    
    # 🎯 【文字標註】：將 Vd 直接疊在左上角空白處（調整了 y 座標避免離邊界太近）
    ax.text(0.05, 0.92, f'$V_d$ = {vd_value:.1f} V', 
            transform=ax.transAxes, 
            fontsize=14, 
            fontweight='bold', 
            color='black',
            bbox=dict(boxstyle="round,pad=0.3", facecolor="wheat", alpha=0.4),
            va='top')

    # 美化圖表細節
    ax.set_xlabel('Gate Voltage $V_g$ (V)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Log Current |I| (A)', fontsize=12, fontweight='bold')
    ax.set_title('Device Transfer Hysteresis Characteristics', fontsize=12, fontweight='bold', pad=15)
    
    ax.grid(True, which="both", linestyle="--", alpha=0.5)
    
    # 移除手動 set_ylim，讓 Y 軸回歸原本最緊湊完美的自動範圍！
    ax.legend(loc='lower right', fontsize=10, frameon=True, ncol=2)
    
    fig.tight_layout()
    
    output_png = os.path.join(output_dir, f"{base_name}_perfect_check.png")
    plt.savefig(output_png, dpi=300)
    plt.close()
    print(f"  🎉 繪圖成功！完美比例版圖表已儲存至 '{output_png}'\n")
    return True

def main():
    target_file = "BGAF 3_1 106 W64-L5_4_7_2025 4_39_36 PM_Vd=1V(-2.5,3.5).csv"
    process_and_plot_device(target_file)

if __name__ == "__main__":
    main()
    