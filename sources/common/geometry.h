/*	_________  ___
  _____ \_   /\  \/  /	mZX - Common/geometry.h
 |  |  |_/  /__>    <	Copyright © 2013 RedCode Software.
 |   ____________/\__\	All rights reserved.
 |_*/

#ifndef _Mines_geometry_h
#define _Mines_geometry_h

#include <Cocoa/Cocoa.h>
#import <Q/functions/base/Q2DValue.h>
#import <Q/types/mathematics.h>

typedef union {
	Q2D	q;
	NSPoint nspoint;
	CGPoint cgpoint;
	NSSize	nssize;
	CGSize	cgsize;
} __QNS2D;

typedef union {
	QRectangle	q;
	NSRect		nsrect;
	CGRect		cgrect;
} __QNSRectangle;

NS_INLINE NSPoint	QToNSPoint(Q2D	      value)	 {return ((__QNS2D	  *)&value)->nspoint;	}
NS_INLINE CGPoint	QToCGPoint(Q2D	      value)	 {return ((__QNS2D	  *)&value)->cgpoint;	}
NS_INLINE NSSize	QToNSSize (Q2D	      value)	 {return ((__QNS2D	  *)&value)->nssize;	}
NS_INLINE CGSize	QToCGSize (Q2D	      value)	 {return ((__QNS2D	  *)&value)->cgsize;	}
NS_INLINE Q2D		NSPointToQ(NSPoint    value)	 {return ((__QNS2D	  *)&value)->q;		}
NS_INLINE Q2D		CGPointToQ(CGPoint    value)	 {return ((__QNS2D	  *)&value)->q;		}
NS_INLINE Q2D		NSSizeToQ (NSSize     value)	 {return ((__QNS2D	  *)&value)->q;		}
NS_INLINE Q2D		CGSizeToQ (CGSize     value)	 {return ((__QNS2D	  *)&value)->q;		}
NS_INLINE QRectangle	NSRectToQ (NSRect     rectangle) {return ((__QNSRectangle *)&rectangle)->q;	}
NS_INLINE QRectangle	CGRectToQ (CGRect     rectangle) {return ((__QNSRectangle *)&rectangle)->q;	}
NS_INLINE NSRect	QToNSRect (QRectangle rectangle) {return ((__QNSRectangle *)&rectangle)->nsrect;}
NS_INLINE CGRect	QToCGRect (QRectangle rectangle) {return ((__QNSRectangle *)&rectangle)->cgrect;}

#define NSSizeAdd(	a, b)	QToNSSize(q_2d_add	(NSSizeToQ(a), NSSizeToQ(b)))
#define NSSizeAddQ(	a, b)	QToNSSize(q_2d_add	(NSSizeToQ(a),		 b ))
#define NSSizeSubtract(	a, b)	QToNSSize(q_2d_subtract (NSSizeToQ(a), NSSizeToQ(b)))
#define NSSizeSubtractQ(a, b)	QToNSSize(q_2d_subtract (NSSizeToQ(a),		 b ))
#define NSSizeFit(	a, b)	QToNSSize(q_2d_fit	(NSSizeToQ(a), NSSizeToQ(b)))
#define NSSizeFitQ(	a, b)	QToNSSize(q_2d_fit	(NSSizeToQ(a),		 b ))
#define NSSizeContains(	a, b)		  q_2d_contains	(NSSizeToQ(a), NSSizeToQ(b))
#define NSSizeContainsQ(a, b)		  q_2d_contains	(NSSizeToQ(a), 		 b )

#define QAddNSSize(	a, b)	QToNSSize(q_2d_add	(	   a , NSSizeToQ(b)))
#define QSubtractNSSize(a, b)	QToNSSize(q_2d_subtract	(	   a , NSSizeToQ(b)))
#define QFitNSSize(	a, b)	QToNSSize(q_2d_fit	(	   a , NSSizeToQ(b)))
#define QContainsNSSize(a, b)		  q_2d_contains	(	   a , NSSizeToQ(b))


CGFloat	PointAngle				(CGPoint a,
						 CGPoint b);


CGPoint	PointSubtract				(CGPoint a,
						 CGPoint b);

CGFloat	PointDistanceToZero			(CGPoint point);

CGFloat	PointDistance				(CGPoint a,
						 CGPoint b);

CGPoint	PointByVectorAtDistance			(CGPoint vectorA,
						 CGPoint vectorB,
						 CGFloat distance);

CGPoint	PointByVectorOfKnownMagnitideAtDistance	(CGPoint vectorA,
						 CGPoint vectorB,
						 CGFloat magnitude,
						 CGFloat distance);

CGRect	RectBySizeScaledToFitRec		(CGSize	 size,
						 CGRect	 rect);

CGSize	SizeFit					(CGSize	 a,
						 CGSize	 b);

static inline CGSize SizeAdd(CGSize a, CGSize b)
	{return CGSizeMake(a.width + b.width, a.height + b.height);}

static inline CGSize SizeSubtract(CGSize a, CGSize b)
	{return CGSizeMake(a.width - b.width, a.height - b.height);}

static inline bool SizeContains(CGSize a, CGSize b)
	{return (b.width <= a.width && b.height <= a.height);}


#endif
