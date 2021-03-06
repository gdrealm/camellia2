// @HEADER
//
// © 2016 UChicago Argonne.  For licensing details, see LICENSE-Camellia in the licenses directory.
//
// This code is derived from source governed by the license LICENSE-DPGTrilinos in the licenses directory.
//
// @HEADER

#ifndef BASIS_FACTORY
#define BASIS_FACTORY

#include "TypeDefs.h"

// Intrepid includes
#include "Intrepid_Basis.hpp"
#include "Intrepid_FieldContainer.hpp"

// Shards includes
#include "Shards_CellTopology.hpp"

// Teuchos includes
#include "Teuchos_RCP.hpp"

#include "Basis.h"

#include "MultiBasis.h"
#include "PatchBasis.h"
#include "VectorizedBasis.h"

#include "CamelliaIntrepidExtendedTypes.h"

#include "CellTopology.h"

namespace Camellia
{
typedef Teuchos::RCP< Camellia::Basis<> > BasisPtr;

class BasisFactory
{
  typedef Camellia::EFunctionSpace FSE;
private:
  map< pair< pair<int,int>, Camellia::EFunctionSpace >, BasisPtr >
  _existingBases; // keys are ((polyOrder,cellTopoKey),fs))
  map< pair< pair<int,int>, Camellia::EFunctionSpace >, BasisPtr >
  _conformingBases; // keys are ((polyOrder,cellTopoKey),fs))
  map< pair< pair< Camellia::Basis<>*, int>, Camellia::EFunctionSpace>, BasisPtr >
  _spaceTimeBases; // keys are (shards Topo basis, temporal degree, temporal function space)
  map< pair< pair< Camellia::Basis<>*, int>, Camellia::EFunctionSpace>, BasisPtr >
  _conformingSpaceTimeBases; // keys are (shards Topo basis, temporal degree, temporal function space)

  map<unsigned, BasisPtr> _nodalBasisForShardsTopology;
  map<CellTopologyKey, BasisPtr> _nodalBasisForTopology;
  
  // the following maps let us remember what arguments were used to create a basis:
  // (this is useful to, say, create a basis again, but now with polyOrder+1)
  map< Camellia::Basis<>*, int > _polyOrders; // allows lookup of poly order used to create basis
  //  static map< Camellia::Basis<>*, int > _ranks; // allows lookup of basis rank
  map< Camellia::Basis<>*, Camellia::EFunctionSpace > _functionSpaces; // allows lookup of function spaces
  map< Camellia::Basis<>*, int > _cellTopoKeys; // allows lookup of cellTopoKeys
  set< Camellia::Basis<>*> _multiBases;
  map< vector< Camellia::Basis<>* >, Camellia::MultiBasisPtr > _multiBasesMap;
  map< pair< Camellia::Basis<>*, vector<double> >, PatchBasisPtr > _patchBases;
  set< Camellia::Basis<>* > _patchBasisSet;

  int _defaultTemporalPolyOrder;
  
  bool _useEnrichedTraces; // i.e. p+1, not p (default is true: this is what we need to prove optimal convergence)
  bool _useLobattoForQuadHGRAD;
  bool _useLobattoForQuadHDIV;
  bool _useLobattoForLineHGRAD;
  bool _useLegendreForLineHVOL;
  bool _useLegendreForQuadHVOL;
public:
  BasisFactory();

  // This version of getBasis is meant eventually to support tensorial polynomial orders; right now, it does so for space-time
  BasisPtr getBasis(std::vector<int> &H1Order, CellTopoPtr cellTopo, FSE functionSpaceForSpatialTopology,
                    FSE functionSpaceForTemporalTopology = Camellia::FUNCTION_SPACE_HVOL);

  // This version of getBasis handles 0 or 1 temporal dimensions; calls the other version:
  BasisPtr getBasis(int H1Order, CellTopoPtr cellTopo, FSE functionSpaceForSpatialTopology, int temporalH1Order = -1,
                    FSE functionSpaceForTemporalTopology = Camellia::FUNCTION_SPACE_HVOL);
  BasisPtr getBasis( int H1Order, unsigned cellTopoKey, FSE fs);

  // This version of getConformingBasis is meant eventually to support tensorial polynomial orders; right now, it does so for space-time
  BasisPtr getConformingBasis(std::vector<int> &H1Order, CellTopoPtr cellTopo, FSE functionSpaceForSpatialTopology,
                              FSE functionSpaceForTemporalTopology = Camellia::FUNCTION_SPACE_HVOL);

  BasisPtr getConformingBasis( int polyOrder, unsigned cellTopoKey, FSE fs );
  BasisPtr getConformingBasis( int polyOrder, CellTopoPtr cellTopo, FSE fs, int temporalPolyOrder = -1,
                               FSE functionSpaceForTemporalTopology = Camellia::FUNCTION_SPACE_HVOL);

  // ! For L^2 bases that wrap an H^1-conforming basis, returns the H^1-conforming basis.
  BasisPtr getContinuousBasis( BasisPtr basis );
  
  BasisPtr getNodalBasisForCellTopology(CellTopoPtr cellTopo);
  BasisPtr getNodalBasisForCellTopology(unsigned cellTopoKey);

  Camellia::MultiBasisPtr getMultiBasis(vector< BasisPtr > &bases);
  PatchBasisPtr getPatchBasis(BasisPtr parent, Intrepid::FieldContainer<double> &patchNodesInParentRefCell, unsigned cellTopoKey = shards::Line<2>::key);

  BasisPtr addToPolyOrder(BasisPtr basis, int pToAdd);
  BasisPtr setPolyOrder(BasisPtr basis, int polyOrderToSet);

  int basisPolyOrder(BasisPtr basis);
  int getBasisRank(BasisPtr basis);
  Camellia::EFunctionSpace getBasisFunctionSpace(BasisPtr basis);

  bool basisKnown(BasisPtr basis);
  bool isMultiBasis(BasisPtr basis);
  bool isPatchBasis(BasisPtr basis);

  void registerBasis( BasisPtr basis, int basisRank, int polyOrder, int cellTopoKey, FSE fs );

  void setUseEnrichedTraces( bool value );

  // the following convenience methods belong in Basis or perhaps a wrapper thereof
  set<int> sideFieldIndices( BasisPtr basis, bool includeSideSubcells = true); // includeSideSubcells: e.g. include vertices as part of quad sides

  void setUseLegendreForQuadHVol(bool value);
  void setUseLobattoForQuadHGrad(bool value);
  void setUseLobattoForQuadHDiv(bool value);

  static Teuchos::RCP<BasisFactory> basisFactory(); // shared, global BasisFactory
};
}

#endif
