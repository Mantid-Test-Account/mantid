#ifndef MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_
#define MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_
/**
    Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
   National Laboratory & European Spallation Source

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://github.com/mantidproject/mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
 */
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "MantidAPI/IFunction.h"
#include "MantidKernel/ClassMacros.h"

#include <boost/python/object.hpp>

namespace Mantid {
namespace PythonInterface {
/**
 * Provides a layer to hook into the protected functions
 * of IFunction
 */
class IFunctionAdapter : virtual public API::IFunction {
public:
  /// A constructor that looks like a Python __init__ method
  IFunctionAdapter(PyObject *self);

  /// Returns the name of the function
  std::string name() const override;
  /// Specify a category for the function
  const std::string category() const override;
  /// Declare all attributes & parameters
  void init() override;

  /// Declare an attribute with an initial value
  void declareAttribute(const std::string &name,
                        const boost::python::object &defaultValue);
  /// Get a named attribute value
  PyObject *getAttributeValue(const std::string &name);
  /// Returns the attribute's value as a Python object
  PyObject *getAttributeValue(const API::IFunction::Attribute &attr);
  /// Called by the framework when an attribute has been set
  void setAttribute(const std::string &attName,
                    const API::IFunction::Attribute &attr) override;
  /// Store the attribute's value in the default IFunction's cache
  void storeAttributePythonValue(const std::string &name,
                                 const boost::python::object &value);

  // Each overload of declareParameter requires a different name as we
  // can't use a function pointer with a virtual base class

  /**
   * Declare a named parameter with initial value & description
   * @param name :: The name of the parameter
   * @param initValue :: The initial value
   * @param description :: A short description of the parameter
   */
  inline void declareFitParameter(const std::string &name, double initValue,
                                  const std::string &description) {
    this->declareParameter(name, initValue, description);
  }

  /**
   * Declare a named parameter with initial value
   * @param name :: The name of the parameter
   * @param initValue :: The initial value
   */
  inline void declareFitParameterNoDescr(const std::string &name,
                                         double initValue) {
    this->declareFitParameter(name, initValue, "");
  }

  /**
   * Declare a named parameter with initial value = 0.0
   * @param name :: The name of the parameter
   */
  inline void declareFitParameterZeroInit(const std::string &name) {
    this->declareFitParameter(name, 0.0, "");
  }

  ///  Override this method to make fitted parameters different from the
  ///  declared
  double activeParameter(size_t i) const override;
  /// Override this method to make fitted parameters different from the declared
  void setActiveParameter(size_t i, double value) override;

protected:
  /**
   * Returns the PyObject that owns this wrapper, i.e. self
   * @returns A pointer to self
   */
  inline PyObject *getSelf() const { return m_self; }

private:
  /// The PyObject must be supplied to construct the object
  DISABLE_COPY_AND_ASSIGN(IFunctionAdapter)

  /// The name of the function
  std::string m_name;
  /// The Python portion of the object
  PyObject *m_self;
};
}
}

#endif /* MANTID_PYTHONINTERFACE_IFUNCTIONADAPTER_H_ */
