#ifndef MANTIDPYTHONAPI_API_EXPORTS_H_
#define MANTIDPYTHONAPI_API_EXPORTS_H_

/**
 * Define some small wrapper and proxy classes for various Mantid C++ classes
 * so that we can override their behaviour in Python
 */
#include <MantidPythonAPI/FrameworkManagerProxy.h>
#include <MantidAPI/MatrixWorkspace.h>
#include <MantidAPI/WorkspaceOpOverloads.h>
#include <MantidAPI/AnalysisDataService.h>
#include <MantidAPI/IAlgorithm.h>
#include <boost/python.hpp>

namespace Mantid
{
  
namespace PythonAPI
{
  //@cond
  /**
   * A wrapper for the PythonAPI::FrameworkManagerProxy
   */
  struct FrameworkManagerWrapper : FrameworkManagerProxy, boost::python::wrapper<FrameworkManagerProxy>
  {
    void workspaceRemoved(const std::string & name)
    {
      if( boost::python::override dispatcher = this->get_override("workspaceRemoved") )
      {
	dispatcher(name);
      }
      else
      {
	default_workspaceRemoved(name);
      }
    }

    void default_workspaceRemoved(const std::string & name)
    {
      this->FrameworkManagerProxy::workspaceRemoved(name);
    }
  };

  /**
   *Some helpful function pointers typedefs
   */
  typedef Mantid::API::IAlgorithm*(FrameworkManagerProxy::*createAlg_overload1)(const std::string&);
  typedef Mantid::API::IAlgorithm*(FrameworkManagerProxy::*createAlg_overload2)(const std::string&, const int&);
  typedef Mantid::API::IAlgorithm*(FrameworkManagerProxy::*createAlg_overload3)(const std::string&, const std::string&);
  typedef Mantid::API::IAlgorithm*(FrameworkManagerProxy::*createAlg_overload4)(const std::string&, const std::string&, const int&);

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(FrameworkManager_execute_overloads, execute, 2, 3)
  typedef Mantid::API::IAlgorithm*(FrameworkManagerProxy::*exec_ptr)(const std::string&, const std::string&, const int&);


  /**
   *  A proxy struct for implementing workspace algebra operator overloads
   */
  struct MatrixWorkspaceProxy
  {
    typedef API::MatrixWorkspace wraptype;
    typedef boost::shared_ptr<wraptype> wraptype_ptr;
    
    //Binary operation for 2 workspaces
    static wraptype_ptr performBinaryOp(const wraptype_ptr lhs, const wraptype_ptr rhs, char op, bool inplace)
    {
      wraptype_ptr result;
      std::string name(lhs->getName());
      if( inplace )
      {
	switch( op )
	{
	case 'p':
	  lhs += rhs;
	  break;
	case 'm':
	  lhs -= rhs;
	  break;
	case 't':
	  lhs *= rhs;
	  break;
	case 'd':
	  lhs /= rhs;
	}
	result = lhs;
      }
      else
      {
	switch( op )
	{
	case 'p':
	  result = lhs + rhs;
	  break;
	case 'm':
	  result = lhs - rhs;
	  break;
	case 't':
	  result = lhs * rhs;
	  break;
	case 'd':
	  result = lhs / rhs;
	}
	name += std::string("_") + op + std::string("_") + rhs->getName();
      }
      Mantid::API::AnalysisDataService::Instance().addOrReplace(name, result);
      return result;
    }
    //Plus workspace
    static wraptype_ptr plus(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'p', false);
    }
    /// Inplace Plus workspace
    static wraptype_ptr inplace_plus(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'p', true);
    }
    /// Minus workspace
    static wraptype_ptr minus(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'm', false);
    }
    /// Inplace Minus workspace
    static wraptype_ptr inplace_minus(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'm', true);
    }
    /// Multiply workspace
    static wraptype_ptr times(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 't', false);
    }
    /// Inplace Multiply workspace
    static wraptype_ptr inplace_times(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 't', true);
    }
    /// Divide workspace
    static wraptype_ptr divide(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'd', false);
    }
    /// Divide workspace
    static wraptype_ptr inplace_divide(const wraptype_ptr lhs, const wraptype_ptr rhs)
    {
      return performBinaryOp(lhs, rhs, 'd', true);
    }

    /// Binary operation for a workspace and a double
    static wraptype_ptr performBinaryOp(const wraptype_ptr lhs, double rhs, char op, bool inplace)
    {
      wraptype_ptr result;
      std::string name(lhs->getName());
      if( inplace )
      {
	switch( op )
	{
	case 'p':
	  lhs += rhs;
	  break;
	case 'm':
	  lhs -= rhs;
	  break;
	case 't':
	  lhs *= rhs;
	  break;
	case 'd':
	  lhs /= rhs;
	  break;
	}
	result = lhs;
      }
      else
      {
	switch( op )
	{
	case 'p':
	  result = lhs + rhs;
	  break;
	case 'm':
	  result = lhs - rhs;
	  break;
	case 't':
	  result = lhs * rhs;
	  break;
	case 'd':
	  result = lhs / rhs;
	}
	std::ostringstream os;
	os << rhs;
	name += std::string("_") + op + std::string("_") + os.str();
      }
      Mantid::API::AnalysisDataService::Instance().addOrReplace(name, result);
      return result;
    }
    /// Plus
    static wraptype_ptr plus(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'p', false);
    }
    /// Inplace Plus
    static wraptype_ptr inplace_plus(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'p', true);
    }
    /// Minus
    static wraptype_ptr minus(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'm', false);
    }
    /// Inplace Minus
    static wraptype_ptr inplace_minus(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'm', true);
    }
    /// Multiply
    static wraptype_ptr times(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 't', false);
    }
    /// Inplace Multiply
    static wraptype_ptr inplace_times(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 't', true);
    }
    /// Divide
    static wraptype_ptr divide(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'd', false);
    }
    /// Inplace Divide
    static wraptype_ptr inplace_divide(const wraptype_ptr lhs, double rhs)
    {
      return performBinaryOp(lhs, rhs, 'd', true);
    }

  };

  //Overload get and set members for isDistribution
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MatrixWorkspace_isDistribution_overloads_1, isDistribution, 0, 0)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MatrixWorkspace_isDistribution_overloads_2, isDistribution, 1, 1)


  //@endcond
}
}

#endif //MANTIDPYTHONAPI_API_EXPORTS_H_
