import pytest
import numpy as np
import _semi_columnar_engine as se

def test_zero_copy_and_math_kernels():
    print("\n=========================================")
    print("🐍 Executing Week 3 Python pytest Suite")
    print("=========================================")

    samples = 4
    engine = se.SemiColumnarEngine(samples)

    # 1. 在 Python 端直接取得 NumPy 視圖 (View)
    vg_view = engine.v_g()
    id_view = engine.i_d()

    # 2. 模擬 Keysight B1500 量測數據寫入 (1.0 到 4.0 mA)
    id_view[0] = 1.0
    id_view[1] = 2.0
    id_view[2] = 3.0
    id_view[3] = 4.0

    # =========================================================================
    # ✨ 零拷貝實證 1：Python 改了數據，C++ 核心算出來的平均值立刻跟著變！
    # =========================================================================
    mean_result = engine.calculate_mean_id()
    variance_result = engine.calculate_variance_id()

    print(se.__doc__)
    print(f"[SUCCESS] Python-side Mean Id: {mean_result} mA")
    print(f"[SUCCESS] Python-side Variance Id: {variance_result}")

    # 驗證數學核心精度
    assert np.isclose(mean_result, 2.5, atol=1e-6)
    assert np.isclose(variance_result, 1.25, atol=1e-6)

    # =========================================================================
    # ✨ 零拷貝實證 2：利用 Python 底層檢查，確認兩邊共享同一個硬體記憶體地址
    # =========================================================================
    # 再拿一次視圖，確保地址指向同一個底層 C++ 指針
    vg_view_again = engine.v_g()
    assert vg_view.__array_interface__['data'][0] == vg_view_again.__array_interface__['data'][0]
    
    print("🎉 [PYTEST SUCCESS] Zero-copy verification passed! Memory shared perfectly.")
    