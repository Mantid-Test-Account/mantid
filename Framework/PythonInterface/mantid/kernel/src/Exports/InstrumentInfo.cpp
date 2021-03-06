#include "MantidKernel/InstrumentInfo.h"
#include "MantidKernel/FacilityInfo.h"
#include "MantidPythonInterface/kernel/StlExportDefinitions.h"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>

using Mantid::Kernel::InstrumentInfo;
using namespace boost::python;

void export_InstrumentInfo() {
  using namespace Mantid::PythonInterface;
  std_vector_exporter<InstrumentInfo>::wrap("std_vector_InstrumentInfo");

  class_<InstrumentInfo>("InstrumentInfo", no_init)
      .def("name", &InstrumentInfo::name, arg("self"),
           ""
           "Returns the full name of the instrument as defined in the "
           "Facilites.xml file")

      .def("shortName", &InstrumentInfo::shortName, arg("self"),
           "Returns the abbreviated name of the instrument as definined in the "
           "Facilites.xml file")

      .def("__str__", &InstrumentInfo::shortName, arg("self"),
           "Returns the abbreviated name of the instrument as definined in the "
           "Facilites.xml file")

      .def("zeroPadding", &InstrumentInfo::zeroPadding,
           (arg("self"), arg("runNumber")),
           "Returns zero padding for this instrument")

      .def("filePrefix", &InstrumentInfo::filePrefix,
           (arg("self"), arg("runNumber")),
           "Returns file prefix for this instrument")

      .def("delimiter", &InstrumentInfo::delimiter, arg("self"),
           "Returns the delimiter "
           "between the instrument "
           "name and the run number.")

      .def("techniques", &InstrumentInfo::techniques, arg("self"),
           return_value_policy<copy_const_reference>(),
           "Return list of techniques this instrument supports")

      .def("facility", &InstrumentInfo::facility, arg("self"),
           return_value_policy<copy_const_reference>(),
           "Returns the facility that contains this instrument.")

      .def("instdae", &InstrumentInfo::liveDataAddress, arg("self"),
           return_value_policy<copy_const_reference>(),
           "Returns the host name and the port of the machine hosting DAE and "
           "providing port to connect to for a live data stream")

      ;
}
