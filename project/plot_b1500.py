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
    half = total_size // 2
    
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

    # 透過 C++ 核心獨立計算去程與回程的物理參數
    vth_fwd = engine.extract_vth(is_backward=False)
    vth_bwd = engine.extract_vth(is_backward=True)
    ss_fwd = engine.extract_ss(is_backward=False)
    ss_bwd = engine.extract_ss(is_backward=True)
    ratio_fwd = engine.extract_on_off_ratio(is_backward=False)
    ratio_bwd = engine.extract_on_off_ratio(is_backward=True)
    dvth = abs(vth_fwd - vth_bwd)

    print(f"  📊 數據解析完成。開始將參數表格繪製進圖片中...")

    # ---- 開始繪製元件轉移特性圖 ----
    fig, ax = plt.subplots(figsize=(8, 7.5)) # 稍微加高高度，給表格騰出完美空間
    
    # 【去程 Forward】：實線
    ax.semilogy(vg_fwd, id_fwd, color='red', linestyle='-',  linewidth=2.2, label='$I_d$ (Forward)')
    ax.semilogy(vg_fwd, ig_fwd, color='blue', linestyle='-', linewidth=1.2, label='$I_g$ (Forward)')
    ax.semilogy(vg_fwd, is_fwd, color='green', linestyle='-', linewidth=1.2, label='$I_s$ (Forward)')
    
    # 【回程 Backward】：虛線
    ax.semilogy(vg_bwd, id_bwd, color='darkred', linestyle='--', linewidth=1.8, label='$I_d$ (Backward)')
    ax.semilogy(vg_bwd, ig_bwd, color='darkblue', linestyle='--', linewidth=1.0, label='$I_g$ (Backward)')
    ax.semilogy(vg_bwd, is_bwd, color='darkgreen', linestyle='--', linewidth=1.0, label='$I_s$ (Backward)')
    
    # 左上角疊加 Vd 標籤
    ax.text(0.04, 0.95, f'$V_d$ = {vd_value:.1f} V', 
            transform=ax.transAxes, fontsize=13, fontweight='bold',
            bbox=dict(boxstyle="round,pad=0.3", facecolor="wheat", alpha=0.5), va='top')

    # 🌟 核心亮點：用程式在圖表內部繪製「半導體電性參數對比表」
    # 準備表格數據
    table_data = [
        ["Forward Sweep", f"{vth_fwd:.2f} V", f"{ss_fwd:.1f}", f"{ratio_fwd:.2e}"],
        ["Backward Sweep", f"{vth_bwd:.2f} V", f"{ss_bwd:.1f}", f"{ratio_bwd:.2e}"],
        ["Hysteresis ΔVth", f"{dvth:.2f} V", "-", "-"]
    ]
    columns = ["Sweep Direction", "Vth (Max Gm)", "SS (mV/dec)", "Ion/Ioff Ratio"]
    
    # 呼叫 matplotlib 的 table 功能，安置在圖表正上方空白位置
    the_table = ax.table(cellText=table_data,
                         colLabels=columns,
                         loc='top',
                         cellLoc='center',
                         colWidths=[0.28, 0.22, 0.22, 0.28])
    
    # 美化表格字型與樣式
    the_table.auto_set_font_size(False)
    the_table.set_fontsize(10)
    the_table.scale(1.0, 1.4) # 微調單元格的高度比例，看起來更寬敞專業
    
    # 設定表格表頭的物理顏色
    for j, col in enumerate(columns):
        cell = the_table[0, j]
        cell.set_text_props(weight='bold', color='white')
        cell.set_facecolor('#2C3E50') # 頂級深藍灰色表頭

    # 美化座標軸細節
    ax.set_xlabel('Gate Voltage $V_g$ (V)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Log Current |I| (A)', fontsize=12, fontweight='bold')
    
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend(loc='lower right', fontsize=10, frameon=True, ncol=2)
    
    # 自動微調佈局，確保上方的表格不會被標題（Title）或圖表邊界切到
    plt.title('Device Transfer Hysteresis & Parametric Summary Table', fontsize=12, fontweight='bold', pad=70)
    
    fig.tight_layout()
    
    output_png = os.path.join(output_dir, f"{base_name}_with_table_report.png")
    plt.savefig(output_png, dpi=300)
    plt.close()
    print(f"🎉 成功！內嵌參數表格的完全體報告圖已儲存至 '{output_png}'\n")
    return True

def main():
    target_file = "BGAF 3_1 106 W64-L5_4_7_2025 4_39_36 PM_Vd=1V(-2.5,3.5).csv"
    process_and_plot_device(target_file)

if __name__ == "__main__":
    main()
    