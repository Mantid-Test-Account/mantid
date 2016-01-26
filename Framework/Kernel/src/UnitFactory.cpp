#include "MantidKernel/UnitFactory.h"

namespace Mantid {
namespace Kernel {

UnitFactoryImpl::UnitFactoryImpl() : DynamicFactory<Unit>() {}

UnitFactoryImpl::~UnitFactoryImpl() = default;

} // namespace Kernel
} // namespace Mantid
