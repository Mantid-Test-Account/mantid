#ifndef MANTID_KERNEL_TYPEDVALIDATOR_H_
#define MANTID_KERNEL_TYPEDVALIDATOR_H_
//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidKernel/IValidator.h"

namespace Mantid
{
  namespace Kernel
  {
    /**
    TypedValidator provides an layer on top of IValidator to ensure
    that the template TYPE is extract from the boost::any instance
    and passed down to the concrete validator instance. Most validators
    will probably want to inherit from this rather than IValidator
    directly.

    A specialised type exists for boost::shared_ptr types

    Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
     */
    template<typename HeldType>
    class DLLExport TypedValidator : public IValidator
    {
    protected:
      /// Override this function to check the validity of the type
      virtual std::string checkValidity(const HeldType &) const = 0;

    private:
      /**
       * Attempts to extract the TYPE from the boost any object and calls
       * the typed checkValidity member
       *  @returns An error message to display to users or an empty string on no error
       */
      std::string check(const boost::any & value) const
      {
        try
        {
          return checkValidity(boost::any_cast<HeldType>(value));
        }
        catch(boost::bad_any_cast&)
        {
          return "Value was not of expected type.";
        }
      }
    };

    /**
     * Specialization for boost::shared_ptr<T> types.
     * boost::any_cast cannot convert between types, the type extracted must match the
     * the stored type. In our case IValidator ensures that all items that inherit from
     * DataItem are stored as a DataItem_sptr within the boost::any instance. Once extracted
     * the DataItem can then be downcast to the Validator::TYPE.
     * The advantage of this is that Validator types then don't have to match
     * their types exactly.
     */
    template<typename ElementType>
    class DLLExport TypedValidator<boost::shared_ptr<ElementType> > : public IValidator
    {
      /// Shared ptr type
      typedef boost::shared_ptr<ElementType> ElementType_sptr;
      
    protected:
      /// Override this function to check the validity of the type
      virtual std::string checkValidity(const ElementType_sptr &) const = 0;
      
    private:
      /**
       * Attempts to extract the TYPE from the boost any object and if
       * the type is a DataItem_sptr then it attempts to downcast the value to
       * the concrete type sepcified by the validator
       * @param value :: The values to verify
       * @returns An error message to display to users or an empty string on no error
       */
      std::string check(const boost::any & value) const
      {
        try
        {
          const ElementType_sptr typedValue = extractValue(value);
          return checkValidity(typedValue);
        }
        catch(std::invalid_argument& exc)
        {
          return exc.what();
        }
      }
      /**
       * Extract the value type as the concrete type
       * @param value :: The value
       * @returns The concrete type
       */
      ElementType_sptr extractValue(const boost::any & value) const
      {
        if( value.type() == typeid(DataItem_sptr) )
        {
          return extractDataItem(value);
        }
        else
        {
          return extractSharedPtr(value);
        }
      }
      /**
       * Extract the DataItem value type by trying to downcast to the concrete type
       * @param value :: The value
       * @returns The concrete type
       */
      ElementType_sptr extractDataItem(const boost::any & value) const
      {
        const DataItem_sptr data = boost::any_cast<DataItem_sptr>(value);
        // First try and push it up to the type of the validator
        ElementType_sptr typedValue = boost::dynamic_pointer_cast<ElementType>(data);
        if(!typedValue)
        {
          throw std::invalid_argument("DataItem \"" + data->name() + "\" is not of the expected type.");
        }
        return typedValue;
      }
      /**
       * Extract the the shared_ptr value type
       * @param value :: The value
       * @returns The concrete type
       */
      ElementType_sptr extractSharedPtr(const boost::any & value) const
      {
        try
        {
          return boost::any_cast<ElementType_sptr>(value);
        }
        catch(boost::bad_any_cast&)
        {
          throw std::invalid_argument("Value is not of the expected type");
        }
      }
    };
  }
}

#endif /* MANTID_KERNEL_TYPEDVALIDATOR_H_ */
