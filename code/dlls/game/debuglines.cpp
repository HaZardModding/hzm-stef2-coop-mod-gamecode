//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/debuglines.cpp                                $
// $Revision:: 7                                                              $
//     $Date:: 10/13/03 8:53a                                                 $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#include "_pch_cpp.h"
//#include "g_local.h"
#include "debuglines.h"

#define NUM_CIRCLE_SEGMENTS 24

debugline_t    *DebugLines = NULL;
Vector		   currentVertex( 0.0f, 0.0f, 0.0f );
Vector		   vertColor( 1.0f, 1.0f, 1.0f );
float			   vertAlpha = 1.0f;
float			   vertexIndex = 0.0f;
float          linewidth = 10.0f; //[b60011] chrissstrahl - test - fix me back to 1.0
unsigned short lineStippleFactor = 1;
unsigned short linePattern = 0xffff;

void G_InitDebugLines
	(
	void
	)

	{
	*gi.DebugLines = DebugLines;
	*gi.numDebugLines = 0;

	currentVertex = vec_zero;
	vertColor = Vector( 1, 1, 1 );
	vertAlpha = 1;
	vertexIndex = 0;

   linewidth = 1;
   lineStippleFactor = 1;
   linePattern = 0xffff;
	}

void G_AllocDebugLines
	(
	void
	)

	{
   // we do a malloc here so that we don't interfere with the game's memory footprint
   DebugLines = ( debugline_t * )malloc( ( int )g_numdebuglines->integer * sizeof( debugline_t ) );

   G_InitDebugLines();
	}

void G_DeAllocDebugLines
	(
	void
	)

	{
   if ( DebugLines )
      {
      // we do a free here, because we used malloc above
      free( DebugLines );
      DebugLines = NULL;
	   *gi.DebugLines = DebugLines;
	   *gi.numDebugLines = 0;
      }
	}

void G_DebugLine
	(
	const Vector &start,
	const Vector &end,
	float r,
	float g,
	float b,
	float alpha
	)

	{
	debugline_t *line;

   if ( !g_numdebuglines )
      {
      return;
      }

   if ( *gi.numDebugLines >= g_numdebuglines->integer )
		{
      gi.DPrintf( "G_DebugLine: Exceeded MAX_DEBUG_LINES\n" );
		return;
		}

	line = &DebugLines[ *gi.numDebugLines ];
	( *gi.numDebugLines )++;

	VectorCopy( start, line->start );
	VectorCopy( end, line->end );
	VectorSet( line->color, r, g, b );
	line->alpha = alpha;

   line->width = linewidth;
   line->factor = lineStippleFactor;
   line->pattern = linePattern;
	}

void G_DebugLineC
	(
	const vec3_t start,
	const vec3_t end,
	float r,
	float g,
	float b,
	float alpha
	)

	{
	debugline_t *line;

   if ( !g_numdebuglines )
      {
      return;
      }

   if ( *gi.numDebugLines >= g_numdebuglines->integer )
		{
      gi.DPrintf( "G_DebugLine: Exceeded MAX_DEBUG_LINES\n" );
		return;
		}

	line = &DebugLines[ *gi.numDebugLines ];
	( *gi.numDebugLines )++;

	VectorCopy( start, line->start );
	VectorCopy( end, line->end );
	VectorSet( line->color, r, g, b );
	line->alpha = alpha;

   line->width = linewidth;
   line->factor = lineStippleFactor;
   line->pattern = linePattern;
	}

void G_LineStipple
   (
   int factor,
   unsigned short pattern
   )

   {
   lineStippleFactor = factor;
   linePattern = pattern;
   }

void G_LineWidth
   (
   float width
   )

   {
   linewidth = width;
   }

void G_Color3f
	(
	float r,
	float g,
	float b
	)

	{
	vertColor = Vector( r, g, b );
	}

void G_Color3v
	(
	const Vector &color
	)

	{
	vertColor = color;
	}

void G_Color4f
	(
	float r,
	float g,
	float b,
	float alpha
	)

	{
	vertColor = Vector( r, g, b );
	vertAlpha = alpha;
	}

void G_Color3vf
	(
	const Vector &color,
	float alpha
	)

	{
	vertColor = color;
	vertAlpha = alpha;
	}

void G_BeginLine
	(
	void
	)

	{
	currentVertex = vec_zero;
	vertexIndex = 0;
	}

void G_Vertex
	(
	const Vector &v
	)

	{
	vertexIndex++;
	if ( vertexIndex > 1.0f )
		{
		G_DebugLine( currentVertex, v, vertColor[ 0 ], vertColor[ 1 ], vertColor[ 2 ], vertAlpha );
		}
	currentVertex = v;
	}

void G_EndLine
	(
	void
	)

	{
	currentVertex = vec_zero;
	vertexIndex = 0;
	}

void G_DebugBBox
   (
   const Vector &org,
   const Vector &mins,
   const Vector &maxs,
   float r,
   float g,
   float b,
   float alpha
   )
   {
   int i;
   Vector points[8];

	/*
	** compute a full bounding box
	*/
	for ( i = 0; i < 8; i++ )
	   {
		Vector   tmp;

		if ( i & 1 )
			tmp[0] = org[0] +  mins[0];
		else
			tmp[0] = org[0] + maxs[0];

		if ( i & 2 )
			tmp[1] = org[1] + mins[1];
		else
			tmp[1] = org[1] + maxs[1];

		if ( i & 4 )
			tmp[2] = org[2] + mins[2];
		else
			tmp[2] = org[2] + maxs[2];

		points[i] = tmp;
	   }

   G_Color4f( r, g, b, alpha );

   G_BeginLine();
   G_Vertex( points[0] );
   G_Vertex( points[1] );
   G_Vertex( points[3] );
   G_Vertex( points[2] );
   G_Vertex( points[0] );
   G_EndLine();

   G_BeginLine();
   G_Vertex( points[4] );
   G_Vertex( points[5] );
   G_Vertex( points[7] );
   G_Vertex( points[6] );
   G_Vertex( points[4] );
   G_EndLine();

	for ( i = 0; i < 4; i++ )
      {
      G_BeginLine();
      G_Vertex( points[i] );
      G_Vertex( points[4 + i] );
      G_EndLine();
      }
   }

//[b60011] chrissstrahl - trying to crate letters - this is just a test
static int Letters[4][8] =
{
	{ 7, 2, 6, 1, 4, 5, 0, 0 }, // A
	{ 1, 6, 7, 3, 5, 2, 4, 0 }, // B
	{ 1, 6, 7, 3, 0, 0, 0, 0 }, // C 

	{ 6, 7, 2, 0, 0, 0, 0, 0 }, // b
};

//
// LED style digits
//
// ****1***
// *      *		8 == /
// 6     *4
// *    * *
// ****2***
// *  *   *
// 7 *--8 5     9
// **     *    **10
// ****3***  12**
//             11

static int Numbers[ 12 ][ 8 ] =
	{
		{ 1, 3, 4, 5, 6, 7, 0, 0 }, // 0
		{ 4, 5, 0, 0, 0, 0, 0, 0 }, // 1
		{ 1, 4, 2, 7, 3, 0, 0, 0 }, // 2
		{ 1, 4, 2, 5, 3, 0, 0, 0 }, // 3
		{ 6, 4, 2, 5, 0, 0, 0, 0 }, // 4
		{ 1, 6, 2, 5, 3, 0, 0, 0 }, // 5
		{ 1, 6, 2, 5, 7, 3, 0, 0 }, // 6
		{ 1, 8, 0, 0, 0, 0, 0, 0 }, // 7
		{ 1, 2, 3, 4, 5, 6, 7, 0 }, // 8
		{ 1, 6, 4, 2, 5, 3, 0, 0 }, // 9
		{ 9, 10, 11, 12, 0, 0, 0, 0 }, // .
		{ 2, 0, 0, 0, 0, 0, 0, 0 }, // -
	};

static float Lines[ 13 ][ 4 ] =
	{
		{ 0, 0, 0, 0 },		// Unused
		{ -4, 8, 4, 8 },		// 1
		{ -4, 4, 4, 4 },		// 2
		{ -4, 0, 4, 0 },		// 3
		{ 4, 8, 4, 4 },		// 4
		{ 4, 4, 4, 0 },		// 5
		{ -4, 8, -4, 4 },		// 6
		{ -4, 4, -4, 0 },		// 7
		{ 4, 8, -4, 0 },		// 8

      { -1, 2, 1, 2 },		// 9
      { 1, 2, 1, 0 },		// 10
      { -1, 0, 1, 0 },		// 11
      { -1, 0, -1, 2 },		// 12
	};

void G_DrawDebugNumber
   (
   const Vector &org,
	float number,
	float scale,
   float r,
   float g,
   float b,
   int precision
   )

   {
   int i;
	int j;
	int l;
	int num;
	Vector up;
	Vector left;
	Vector pos;
	Vector start;
	Vector ang;
	str text;
	Vector delta;
   char format[ 20 ];

	// only draw entity numbers within a certain radius
   delta = Vector( g_entities[ 0 ].s.origin ) - org;
	if ( ( delta * delta ) > ( 1000.0f * 1000.0f ) )
		{
		return;
		}

   G_Color4f( r, g, b, 1.0 );

	ang = game.clients[ 0 ].ps.viewangles;
	ang.AngleVectors( NULL, &left, &up );

	up *= scale;
	left *= scale;

   if ( precision > 0 )
      {
      sprintf( format, "%%.%df", precision );
      text = va( format, number );
      }
   else
      {
   	text = va( "%d", ( int )number );
      }

	start = org + ( (float)( text.length() - 1 ) * 5.0f * left );

	for( i = 0; i < text.length(); i++ )
		{
      if ( text[ i ] == '.' )
         {
         num = 10;
         }
      else if ( text[ i ] == '-' )
         {
         num = 11;
         }
      else
         {
		   num = text[ i ] - '0';
         }

		for( j = 0; j < 8; j++ )
			{
			l = Numbers[ num ][ j ];
			if ( l == 0 )
				{
				break;
				}

			G_BeginLine();

			pos = start - ( Lines[ l ][ 0 ] * left ) + ( Lines[ l ][ 1 ] * up );
			G_Vertex( pos );

			pos = start - ( Lines[ l ][ 2 ] * left ) + ( Lines[ l ][ 3 ] * up );
			G_Vertex( pos );

			G_EndLine();
			}

		start -= 10.0f * left;
		}
   }

void G_DebugCircle
   (
   const Vector &org,
   float radius,
   float r,
   float g,
   float b,
   float alpha,
   qboolean horizontal
   )
   {
   int i;
   float ang;
   Vector angles;
	Vector forward;
	Vector left;
	Vector pos;
   Vector delta;

	// only draw circles within a certain radius
   delta = Vector( g_entities[ 0 ].s.origin ) - org;
	if ( ( delta * delta ) > ( ( 1000.0f + radius ) * ( 1000.0f + radius ) ) )
		{
		return;
		}

   G_Color4f( r, g, b, alpha );

   if ( horizontal )
      {
      forward = Vector(1, 0, 0);
      left = Vector(0, -1, 0);
      }
   else
      {
	   angles = game.clients[ 0 ].ps.viewangles;
	   angles.AngleVectors( NULL, &left, &forward );
      }

   G_BeginLine();
   for( i = 0; i <= NUM_CIRCLE_SEGMENTS; i++ )
      {
      ang = DEG2RAD( i * 360.0f / NUM_CIRCLE_SEGMENTS );
      pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
      G_Vertex( pos );
      }
   G_EndLine();
   }

void G_DebugOrientedCircle
   (
   const Vector &org,
   float radius,
   float r,
   float g,
   float b,
   float alpha,
   Vector angles
   )
   {
   int i;
   float ang;
	Vector forward;
	Vector left;
	Vector pos;
   Vector delta;

	// only draw circles within a certain radius
   delta = Vector( g_entities[ 0 ].s.origin ) - org;
	if ( ( delta * delta ) > ( ( 1000.0f + radius ) * ( 1000.0f + radius ) ) )
		{
		return;
		}

   G_Color4f( r, g, b, alpha );

   angles.AngleVectors( NULL, &left, &forward );

   G_BeginLine();
   for( i = 0; i <= NUM_CIRCLE_SEGMENTS; i++ )
      {
      ang = DEG2RAD( i * 360.0f / NUM_CIRCLE_SEGMENTS );
      pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
      G_Vertex( pos );
      }
   G_EndLine();

   //
   // Draw the cross sign
   //
   G_BeginLine();
   ang = DEG2RAD( 45.0f * 360.0f / NUM_CIRCLE_SEGMENTS );
   pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
   G_Vertex( pos );
   ang = DEG2RAD( 225.0f * 360.0f / NUM_CIRCLE_SEGMENTS );
   pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
   G_Vertex( pos );

   G_BeginLine();
   ang = DEG2RAD( 315.0f * 360.0f / NUM_CIRCLE_SEGMENTS );
   pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
   G_Vertex( pos );
   ang = DEG2RAD( 135.0f * 360.0f / NUM_CIRCLE_SEGMENTS );
   pos = org + ( sin( ang ) * radius * forward ) - ( cos( ang ) * radius * left );
   G_Vertex( pos );
   }

void G_DebugPyramid
   (
   const Vector &org,
   float radius,
   float r,
   float g,
   float b,
   float alpha
   )
   {
   Vector delta;
   Vector points[ 4 ];

	// only draw pyramids within a certain radius
   delta = Vector( g_entities[ 0 ].s.origin ) - org;
	if ( ( delta * delta ) > ( ( 1000.0f + radius ) * ( 1000.0f + radius ) ) )
		{
		return;
		}

   G_Color4f( r, g, b, alpha );

   points[ 0 ] = org;
   points[ 0 ].z += radius;

   points[ 1 ] = org;
   points[ 1 ].z -= radius;
   points[ 2 ] = points[ 1 ];
   points[ 3 ] = points[ 1 ];

   points[ 1 ].x += (float)cos( DEG2RAD( 0.0f ) ) * radius;
   points[ 1 ].y += (float)sin( DEG2RAD( 0.0f ) ) * radius;
   points[ 2 ].x += (float)cos( DEG2RAD( 120.0f ) ) * radius;
   points[ 2 ].y += (float)sin( DEG2RAD( 120.0f ) ) * radius;
   points[ 3 ].x += (float)cos( DEG2RAD( 240.0f ) ) * radius;
   points[ 3 ].y += (float)sin( DEG2RAD( 240.0f ) ) * radius;

   G_BeginLine();
   G_Vertex( points[ 0 ] );
   G_Vertex( points[ 1 ] );
   G_Vertex( points[ 2 ] );
   G_Vertex( points[ 0 ] );
   G_EndLine();

   G_BeginLine();
   G_Vertex( points[ 0 ] );
   G_Vertex( points[ 2 ] );
   G_Vertex( points[ 3 ] );
   G_Vertex( points[ 0 ] );
   G_EndLine();

   G_BeginLine();
   G_Vertex( points[ 0 ] );
   G_Vertex( points[ 3 ] );
   G_Vertex( points[ 1 ] );
   G_Vertex( points[ 0 ] );
   G_EndLine();

   G_BeginLine();
   G_Vertex( points[ 1 ] );
   G_Vertex( points[ 2 ] );
   G_Vertex( points[ 3 ] );
   G_Vertex( points[ 1 ] );
   G_EndLine();
   }

void G_DrawCoordSystem
   (
   const Vector &pos,
   const Vector &forward,
   const Vector &right,
   const Vector &up,
   int length
   )

   {
   if ( g_showaxis->integer )
      {
      G_DebugLine( pos, pos + ( forward * length ), 1.0f, 0.0f, 0.0f, 1.0f );
      G_DebugLine( pos, pos + ( right * length ), 0.0f, 1.0f, 0.0f, 1.0f );
      G_DebugLine( pos, pos + ( up * length ), 0.0f, 0.0f, 1.0f, 1.0f );
      }
   }

void G_DrawCSystem
	(
	void
	)

	{
	Vector pos;
	Vector ang;
	Vector f;
	Vector l;
	Vector u;
	Vector v;

	pos.x = csys_posx->value;
	pos.y = csys_posy->value;
	pos.z = csys_posz->value;

	ang.x = csys_x->value;
	ang.y = csys_y->value;
	ang.z = csys_z->value;

	ang.AngleVectors( &f, &l, &u );

	G_DebugLine( pos, pos + ( f * 48 ), 1.0f, 0.0f, 0.0f, 1.0f );
	G_DebugLine( pos, pos - ( l * 48 ), 0.0f, 1.0f, 0.0f, 1.0f );
	G_DebugLine( pos, pos + ( u * 48 ), 0.0f, 0.0f, 1.0f, 1.0f );
	}

void G_DebugArrow
   (
   const Vector   &org,
   const Vector   &dir,
   float    length,
   float    r,
   float    g,
   float    b,
   float    alpha
   )
   {
   Vector right;
   Vector up;
   Vector startpoint;
   Vector endpoint;

   PerpendicularVector( right, ( Vector )dir );
	up.CrossProduct( right, dir );

   startpoint = org;

   endpoint = startpoint + ( dir * length );
   length /= 6.0f;
   G_DebugLine( startpoint, endpoint, r, g, b, alpha );
   G_DebugLine( endpoint, endpoint - (right * length) - (dir * length), r, g, b, alpha );
   G_DebugLine( endpoint, endpoint + (right * length) - (dir * length), r, g, b, alpha );
   G_DebugLine( endpoint, endpoint - (up * length) - (dir * length), r, g, b, alpha );
   G_DebugLine( endpoint, endpoint + (up * length) - (dir * length), r, g, b, alpha );
   }

void G_DebugHighlightFacet
   (
   const Vector &org,
   const Vector &mins,
   const Vector &maxs,
   facet_t facet,
   float r,
   float g,
   float b,
   float alpha
   )
   {
   int i;
   Vector points[8];

	/*
	** compute a full bounding box
	*/
	for ( i = 0; i < 8; i++ )
	   {
		Vector   tmp;

		if ( i & 1 )
			tmp[0] = org[0] +  mins[0];
		else
			tmp[0] = org[0] + maxs[0];

		if ( i & 2 )
			tmp[1] = org[1] + mins[1];
		else
			tmp[1] = org[1] + maxs[1];

		if ( i & 4 )
			tmp[2] = org[2] + mins[2];
		else
			tmp[2] = org[2] + maxs[2];

		points[i] = tmp;
	   }

   G_Color4f( r, g, b, alpha );

   switch( facet )
      {
      case facet_north:
         G_BeginLine();
         G_Vertex( points[0] );
         G_Vertex( points[5] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[1] );
         G_Vertex( points[4] );
         G_EndLine();
         break;
      case facet_south:
         G_BeginLine();
         G_Vertex( points[2] );
         G_Vertex( points[7] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[3] );
         G_Vertex( points[6] );
         G_EndLine();
         break;
      case facet_east:
         G_BeginLine();
         G_Vertex( points[0] );
         G_Vertex( points[6] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[4] );
         G_Vertex( points[2] );
         G_EndLine();
         break;
      case facet_west:
         G_BeginLine();
         G_Vertex( points[1] );
         G_Vertex( points[7] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[5] );
         G_Vertex( points[3] );
         G_EndLine();
         break;
      case facet_up:
         G_BeginLine();
         G_Vertex( points[0] );
         G_Vertex( points[3] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[1] );
         G_Vertex( points[2] );
         G_EndLine();
         break;
      case facet_down:
         G_BeginLine();
         G_Vertex( points[4] );
         G_Vertex( points[7] );
         G_EndLine();
         G_BeginLine();
         G_Vertex( points[5] );
         G_Vertex( points[6] );
         G_EndLine();
         break;
      }
   }
