#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "ColumnarArray.hpp"
#include "SemiColumnarEngine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_semi_columnar_engine, m) {
    m.doc() = "Keysight B1500 Semiconductor SoA Engine Wrapper";

    py::class_<ColumnarArray<double>>(m, "ColumnarArray")
        .def(py::init<size_t>())
        .def("size", &ColumnarArray<double>::size)
        .def_property_readonly("data", [](ColumnarArray<double> &self) {
            return py::array_t<double>(
                {self.size()},
                {sizeof(double)},
                self.data(),
                py::capsule(self.data(), [](void *p) {})
            );
        });

    py::class_<SemiColumnarEngine>(m, "SemiColumnarEngine")
        .def(py::init<size_t>())
        .def("calculate_mean_id", &SemiColumnarEngine::calculate_mean_id)
        .def("calculate_variance_id", &SemiColumnarEngine::calculate_variance_id)
        // ✨ 終極修正：直接將內部的 ColumnarArray 參考綁定給 Python，並加上基底所有權（keep_alive）
        // 這能確保 Python 拿到的 NumPy 陣列地址與 C++ 內部完全百分之百對齊，絕不產生位元組歪斜！
        .def("v_g", [](SemiColumnarEngine &self) {
            return py::array_t<double>(
                {self.size()}, 
                {sizeof(double)}, 
                self.v_g().data(),
                py::cast(self) // 綁定基底生命週期
            );
        })
        .def("i_d", [](SemiColumnarEngine &self) {
            return py::array_t<double>(
                {self.size()}, 
                {sizeof(double)}, 
                self.i_d().data(),
                py::cast(self)
            );
        });
}
