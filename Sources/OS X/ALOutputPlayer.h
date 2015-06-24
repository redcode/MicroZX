/* ACME - ALOutputPlayer.h
  ____    ____    ___ ___     ___
 / __ \  / ___\  / __` __`\  / __`\
/\ \/  \/\ \__/_/\ \/\ \/\ \/\  __/
\ \__/\_\ \_____\ \_\ \_\ \_\ \____\
 \/_/\/_/\/_____/\/_/\/_/\/_/\/____/
Copyright © 2013-2014 Manuel Sainz de Baranda y Goñi.
Released under the terms of the GNU General Public License v3. */

#import <Foundation/Foundation.h>
#import "ALOutput.h"

@interface ALOutputPlayer : NSObject {

	@public
	ALOutput    _ALOutput;
	QRingBuffer _buffer;
	BOOL	    _active;
}

	@property (nonatomic, readonly) QRingBuffer* buffer;

	- (void) setSampleRate: (qsize) sampleRate
		 frameSize:	(qsize) frameSize
		 slotCount:	(quint) slotCount;

	- (void) start;

	- (void) stop;

@end
