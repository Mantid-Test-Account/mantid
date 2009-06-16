#ifndef MANTID_API_SPECTRADETECTORMAP_H_
#define MANTID_API_SPECTRADETECTORMAP_H_

#include "MantidKernel/System.h"
#include "MantidKernel/Logger.h"
#include "MantidKernel/cow_ptr.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#ifndef HAS_UNORDERED_MAP_H
  #include <map>
#else
  #include <tr1/unordered_map>
#endif

namespace Mantid
{
namespace API
{
/** SpectraDetectorMap provides a multimap between Spectra number (int)
    and detector ID (UDET). For efficiency, an unordered_multimaop is used. The TR1/unordered_map
    header is not included in MVSC++ Express Edition so an alternative with multimap is
    provided.

    @author Laurent C Chapon, ISIS, RAL
    @date 29/04/2008

    Copyright &copy; 2007-9 STFC Rutherford Appleton Laboratory

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
class DLLExport SpectraDetectorMap
{
public:
  // The cow_ptr in which SpectraDetectorMap is held in Workspace needs access to the copy constructor
  friend class Kernel::cow_ptr<SpectraDetectorMap>;
  
#ifndef HAS_UNORDERED_MAP_H
  /// Spectra Detector map typedef
  typedef std::multimap<int,int> smap;
  /// Spectra Detector map iterator typedef
  typedef std::multimap<int,int>::const_iterator smap_it;
#else
  /// Spectra Detector map typedef
  typedef std::tr1::unordered_multimap<int,int> smap;
  /// Spectra Detector map iterator typedef
  typedef std::tr1::unordered_multimap<int,int>::const_iterator smap_it;
#endif
  
  /// Constructor
  SpectraDetectorMap();
  /// Virtual destructor
  virtual ~SpectraDetectorMap();
  /// Populate the Map with _spec and _udet C array
  void populate(const int* _spec, const int* _udet, int nentries);
  /// Link a list of UDETs to the given spectrum
  void addSpectrumEntries(const int spectrum, const std::vector<int>& udetList);
  /// Move a detector from one spectrum to another
  void remap(const int oldSpectrum, const int newSpectrum);
  /// Empties the map
  void clear();
  /// Return number of detectors contributing to this spectrum
  const int ndet(const int spectrum_number) const;
  /// Get a vector of detectors ids contributing to a spectrum
  std::vector<int> getDetectors(const int spectrum_number) const;
  /// Gets a list of spectra corresponding to a list of detector numbers
  std::vector<int> getSpectra(const std::vector<int>& detectorList) const;
  /// Return the size of the map
  int nElements() const {return m_s2dmap.size();}
  
private:
  /// Copy Contructor
  SpectraDetectorMap(const SpectraDetectorMap& copy);
  /// Assignment operator
  SpectraDetectorMap& operator=(const SpectraDetectorMap& rhs);
  /// internal spectra detector map instance
  smap m_s2dmap;

  /// Static reference to the logger class
  static Kernel::Logger& g_log;
};

} // namespace API
} // namespace Mantid

#endif /*MANTID_API_SPECTRADETECTORMAP_H_*/
