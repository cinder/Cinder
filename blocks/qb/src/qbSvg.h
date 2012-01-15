//
//	qbSvg.h
//
//  Created by Roger Sodre on 01/07/2011
//	Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "qbPoly.h"

namespace cinder { namespace qb {
	
	/////////////////////////////////
	//
	// SVG IMPORTER
	//
	class qbSvg : public qbPolyGroup {
	public:
		qbSvg() : qbPolyGroup() {};
		
		int		load( const DataSourceRef & _res );
		int		load( const std::string & _f );
		
	protected:
		
		std::string	mCurrLayerName;
		
		int		parseSvg( const XmlTree  & _doc );
		void	parseSvgLayer( const XmlTree  & _l );
		void	parseSvgPolygon( const XmlTree  & _p, bool _closed, qbPoly * apoly );
		void	parseSvgRect( const XmlTree  & _p, qbPoly * apoly );
		void	parseSvgLine( const XmlTree  & _p, qbPoly * apoly );
		void	parseSvgCircle( const XmlTree  & _p, qbPoly * apoly );
		void	parseSvgEllipse( const XmlTree  & _p, qbPoly * apoly );
		void	parseSvgPath( const XmlTree  & _p, qbPoly * apoly );
		std::vector<float> splitPathValues( std::string & _values );
		
	};
	
} } // cinder::qb

