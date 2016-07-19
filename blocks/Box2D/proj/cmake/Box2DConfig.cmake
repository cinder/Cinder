if( NOT TARGET Box2D )
	get_filename_component( BOX2D_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )

	list( APPEND BOX2D_SOURCES
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2BroadPhase.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2CollideCircle.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2CollideEdge.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2CollidePolygon.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2Collision.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2Distance.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2DynamicTree.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/b2TimeOfImpact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/Shapes/b2ChainShape.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/Shapes/b2CircleShape.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/Shapes/b2EdgeShape.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Collision/Shapes/b2PolygonShape.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2BlockAllocator.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2Draw.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2Math.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2Settings.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2StackAllocator.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Common/b2Timer.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2Body.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2ContactManager.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2Fixture.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2Island.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2World.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/b2WorldCallbacks.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2CircleContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2Contact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2GearJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2Joint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2MouseJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2RopeJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2WeldJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Dynamics/Joints/b2WheelJoint.cpp
		${BOX2D_SOURCE_PATH}/Box2D/Rope/b2Rope.cpp
	)

	add_library( Box2D ${BOX2D_SOURCES} )

	target_include_directories( Box2D PUBLIC "${BOX2D_SOURCE_PATH}" )
endif()
