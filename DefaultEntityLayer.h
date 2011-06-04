//
//  EntityLayer.h
//  TrenchBroom
//
//  Created by Kristian Duske on 25.04.11.
//  Copyright 2011 TU Berlin. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "EntityLayer.h"

@class EntityBoundsRenderer;
@class Options;

@interface DefaultEntityLayer : NSObject <EntityLayer> {
    Options* options;
    EntityBoundsRenderer* boundsRenderer;
}

- (id)initWithOptions:(Options *)theOptions;

@end
