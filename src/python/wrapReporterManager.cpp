#include "unf/reporterManager.h"

#include <pxr/base/tf/makePyConstructor.h>
#include <pxr/base/tf/pyPtrHelpers.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE

void wrapReporterManager()
{
    class_<ReporterManager, ReporterManagerWeakPtr, boost::noncopyable>(
        "ReporterManager",
        "Intermediate object handling the creation and management of Reporter "
        "instances per stage",
        no_init)

        .def(TfPyRefAndWeakPtr())

        .def(
            "Create",
            &ReporterManager::Create,
            arg("stage"),
            "Create a manager from a Usd Stage.",
            return_value_policy<TfPyRefPtrFactory<> >())
        .staticmethod("Create")

        .def(
            "GetNumReporters",
            &ReporterManager::GetNumReporters,
            "Return number of reporters created")

        .def(
            "GetNumManagers",
            &ReporterManager::GetNumManagers,
            "Return number of factory generators added")
        .staticmethod("GetNumManagers")

        .def(
            "Reset",
            (void (ReporterManager::*)()) & ReporterManager::Reset,
            "Un-register manager")

        .def("ResetAll", &ReporterManager::ResetAll, "Un-register all managers")
        .staticmethod("ResetAll");
}
