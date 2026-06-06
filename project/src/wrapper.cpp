#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <memory>
#include "ColumnarArray.hpp"
#include "SemiColumnarEngine.hpp"
#include "B1500Parser.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_semi_columnar_engine, m) {
    m.doc() = "Keysight B1500 Semiconductor SoA Engine & Parser Wrapper";

    py::class_<ColumnarArray<double>>(m, "ColumnarArray")
        .def(py::init<size_t>())
        .def("size", &ColumnarArray<double>::size)
        .def_property_readonly("data", [](ColumnarArray<double> &self) {
            return py::array_t<double>(
                {self.size()}, {sizeof(double)}, self.data(),
                py::capsule(self.data(), [](void *p) {})
            );
        });

    py::class_<SemiColumnarEngine>(m, "SemiColumnarEngine")
        .def(py::init<size_t>())
        .def("size", &SemiColumnarEngine::size)
        // 🌟 註冊帶有去回程方向控制的物理指標萃取核
        .def("extract_on_off_ratio", &SemiColumnarEngine::extract_on_off_ratio, py::arg("is_backward"))
        .def("extract_vth", &SemiColumnarEngine::extract_vth, py::arg("is_backward"))
        .def("extract_ss", &SemiColumnarEngine::extract_ss, py::arg("is_backward"))
        .def("v_g", [](SemiColumnarEngine &self) {
            return py::array_t<double>({self.size()}, {sizeof(double)}, self.v_g().data(), py::cast(self));
        })
        .def("v_d", [](SemiColumnarEngine &self) {
            return py::array_t<double>({self.size()}, {sizeof(double)}, self.v_d().data(), py::cast(self));
        })
        .def("abs_i_g", [](SemiColumnarEngine &self) {
            return py::array_t<double>({self.size()}, {sizeof(double)}, self.abs_i_g().data(), py::cast(self));
        })
        .def("abs_i_d", [](SemiColumnarEngine &self) {
            return py::array_t<double>({self.size()}, {sizeof(double)}, self.abs_i_d().data(), py::cast(self));
        })
        .def("abs_i_s", [](SemiColumnarEngine &self) {
            return py::array_t<double>({self.size()}, {sizeof(double)}, self.abs_i_s().data(), py::cast(self));
        });

    py::class_<B1500Parser>(m, "B1500Parser")
        .def_static("load_csv", &B1500Parser::load_csv);
}
