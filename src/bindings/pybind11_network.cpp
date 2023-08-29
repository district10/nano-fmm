#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "nano_fmm/network.hpp"
#include "spdlog/spdlog.h"

namespace nano_fmm
{
namespace py = pybind11;
using namespace pybind11::literals;
using rvp = py::return_value_policy;

void bind_network(py::module &m)
{
    py::class_<ProjectedPoint>(m, "ProjectedPoint", py::module_local()) //
        .def(py::init<const Eigen::Vector3d &,                          //
                      const Eigen::Vector3d &,                          //
                      double,                                           //
                      int64_t,                                          //
                      double>(),
             py::kw_only(),                            //
             "position"_a = Eigen::Vector3d(0, 0, 0),  //
             "direction"_a = Eigen::Vector3d(0, 0, 1), //
             "distance"_a = 0.0,                       //
             "road_id"_a = 0,                          //
             "offset"_a = 0.0)
        //
        .def_property_readonly(
            "position",
            [](const ProjectedPoint &self) { return self.position_; })
        .def_property_readonly(
            "direction",
            [](const ProjectedPoint &self) { return self.direction_; })
        .def_property_readonly(
            "distance",
            [](const ProjectedPoint &self) { return self.distance_; })
        .def_property_readonly(
            "road_id", [](const ProjectedPoint &self) { return self.road_id_; })
        .def_property_readonly(
            "offset", [](const ProjectedPoint &self) { return self.offset_; })
        //
        ;

    py::class_<UbodtRecord>(m, "UbodtRecord", py::module_local()) //
        .def(py::init<int64_t, int64_t, int64_t, int64_t, double>(),
             py::kw_only(),
             "source_road"_a = 0, //
             "target_road"_a = 0, //
             "source_next"_a = 0, //
             "target_prev"_a = 0, //
             "cost"_a = 0.0)
        //
        .def_property_readonly(
            "source_road",
            [](const UbodtRecord &self) { return self.source_road_; })
        .def_property_readonly(
            "target_road",
            [](const UbodtRecord &self) { return self.target_road_; })
        .def_property_readonly(
            "source_next",
            [](const UbodtRecord &self) { return self.source_next_; })
        .def_property_readonly(
            "target_prev",
            [](const UbodtRecord &self) { return self.target_prev_; })
        .def_property_readonly(
            "cost", [](const UbodtRecord &self) { return self.cost_; })
        .def_property_readonly(
            "next", [](const UbodtRecord &self) { return self.next_; },
            rvp::reference_internal)
        //
        .def(py::self == py::self)
        .def(py::self < py::self)
        //
        .def("__repr__", [](const UbodtRecord &self) {
            return fmt::format(
                "UbodtRecord(s->t=[{}->{}], cost:{}, sn:{},tp:{})",
                self.source_road_, self.target_road_, //
                self.cost_,                           //
                self.source_next_, self.target_prev_);
        });
    //
    ;

    py::class_<Network>(m, "Network", py::module_local()) //
                                                          //
        .def(py::init<bool>(), py::kw_only(), "is_wgs84"_a = false)
        //
        .def("add_road", &Network::add_road, "geom"_a, py::kw_only(), "id"_a)
        .def("add_link", &Network::add_link, "source_road"_a, "target_road"_a)
        .def("remove_road", &Network::remove_road, "id"_a)
        .def("remove_link", &Network::remove_link, //
             "source_road"_a, "target_road"_a)
        .def("prev_roads", &Network::prev_roads, "id"_a)
        .def("next_roads", &Network::next_roads, "id"_a)
        .def("roads", &Network::roads)
        //
        .def("road", &Network::road, "road_id"_a, rvp::reference_internal)
        .def("query",
             py::overload_cast<const Eigen::Vector3d &, //
                               double,                  //
                               std::optional<int>,      //
                               std::optional<double>>(&Network::query,
                                                      py::const_),
             "position"_a,         //
             py::kw_only(),        //
             "radius"_a,           //
             "k"_a = std::nullopt, //
             "z_max_offset"_a = std::nullopt)
        .def("query",
             py::overload_cast<const Eigen::Vector4d &>(&Network::query,
                                                        py::const_),
             "bbox"_a)
        //
        .def("build", &Network::build)
        //
        .def_static("load", &Network::load, "path"_a)
        .def("dump", &Network::dump, "path"_a, py::kw_only(),
             "with_config"_a = true)
        //
        .def("build_ubodt",
             py::overload_cast<std::optional<double>>(&Network::build_ubodt,
                                                      py::const_),
             py::kw_only(), "thresh"_a = std::nullopt)
        .def("build_ubodt",
             py::overload_cast<const std::vector<int64_t> &,
                               std::optional<double>>(&Network::build_ubodt,
                                                      py::const_),
             "roads"_a, py::kw_only(), "thresh"_a = std::nullopt)
        .def("clear_ubodt", &Network::clear_ubodt)
        .def("load_ubodt",
             py::overload_cast<const std::vector<UbodtRecord> &>(
                 &Network::load_ubodt),
             "rows"_a)
        .def("load_ubodt",
             py::overload_cast<const std::string &>(&Network::load_ubodt),
             "path"_a)
        .def("dump_ubodt", &Network::dump_ubodt, "path"_a, py::kw_only(),
             "thresh"_a = std::nullopt)
        //
        .def("to_2d", &Network::to_2d)
        //
        ;
}
} // namespace nano_fmm