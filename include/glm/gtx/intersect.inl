/// @ref gtx_intersect
/// @file glm/gtx/intersect.inl

namespace glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER bool intersectRayPlane
	(
		genType const & orig, genType const & dir,
		genType const & planeOrig, genType const & planeNormal,
		typename genType::value_type & intersectionDistance
	)
	{
		typename genType::value_type d = glm::dot(dir, planeNormal);
		typename genType::value_type Epsilon = std::numeric_limits<typename genType::value_type>::epsilon();

		if(d < -Epsilon)
		{
			intersectionDistance = glm::dot(planeOrig - orig, planeNormal) / d;
			return true;
		}

		return false;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool intersectRayTriangle
	(
		vec<3, T, P> const& orig, vec<3, T, P> const& dir,
		vec<3, T, P> const& vert0, vec<3, T, P> const& vert1, vec<3, T, P> const& vert2,
		vec<2, T, P>& baryPosition, T& distance
	)
	{
		// find vectors for two edges sharing vert0
		vec<3, T, P> const edge1 = vert1 - vert0;
		vec<3, T, P> const edge2 = vert2 - vert0;

		// begin calculating determinant - also used to calculate U parameter
		vec<3, T, P> const p = glm::cross(dir, edge2);

		// if determinant is near zero, ray lies in plane of triangle
		T const det = glm::dot(edge1, p);

		vec<3, T, P> qvec;

		if(det > std::numeric_limits<T>::epsilon())
		{
			// calculate distance from vert0 to ray origin
			vec<3, T, P> const tvec = orig - vert0;

			// calculate U parameter and test bounds
			baryPosition.x = glm::dot(tvec, p);
			if(baryPosition.x < static_cast<T>(0) || baryPosition.x > det)
				return false;

			// prepare to test V parameter
			qvec = glm::cross(tvec, edge1);

			// calculate V parameter and test bounds
			baryPosition.y = glm::dot(dir, qvec);
			if((baryPosition.y < static_cast<T>(0)) || ((baryPosition.x + baryPosition.y) > det))
				return false;
		}
		else if(det < -std::numeric_limits<T>::epsilon())
		{
			// calculate distance from vert0 to ray origin
			vec<3, T, P> const tvec = orig - vert0;

			// calculate U parameter and test bounds
			baryPosition.x = glm::dot(tvec, p);
			if((baryPosition.x > static_cast<T>(0)) || (baryPosition.x < det))
				return false;

			// prepare to test V parameter
			qvec = glm::cross(tvec, edge1);

			// calculate V parameter and test bounds
			baryPosition.y = glm::dot(dir, qvec);
			if((baryPosition.y > static_cast<T>(0)) || (baryPosition.x + baryPosition.y < det))
				return false;
		}
		else
			return false; // ray is parallel to the plane of the triangle

		T inv_det = static_cast<T>(1) / det;

		// calculate distance, ray intersects triangle
		distance = glm::dot(edge2, qvec) * inv_det;
		baryPosition *= inv_det;

		return true;
	}

/*
		typename genType::value_type Epsilon = std::numeric_limits<typename genType::value_type>::epsilon();
		if(a < Epsilon && a > -Epsilon)
			return false;

		typename genType::value_type f = typename genType::value_type(1.0f) / a;

		genType s = orig - v0;
		baryPosition.x = f * glm::dot(s, p);
		if(baryPosition.x < typename genType::value_type(0.0f))
			return false;
		if(baryPosition.x > typename genType::value_type(1.0f))
			return false;

		genType q = glm::cross(s, e1);
		baryPosition.y = f * glm::dot(dir, q);
		if(baryPosition.y < typename genType::value_type(0.0f))
			return false;
		if(baryPosition.y + baryPosition.x > typename genType::value_type(1.0f))
			return false;

		baryPosition.z = f * glm::dot(e2, q);

		return baryPosition.z >= typename genType::value_type(0.0f);
	}
*/

	template<typename genType>
	GLM_FUNC_QUALIFIER bool intersectLineTriangle
	(
		genType const & orig, genType const & dir,
		genType const & vert0, genType const & vert1, genType const & vert2,
		genType & position
	)
	{
		typename genType::value_type Epsilon = std::numeric_limits<typename genType::value_type>::epsilon();

		genType edge1 = vert1 - vert0;
		genType edge2 = vert2 - vert0;

		genType pvec = cross(dir, edge2);

		float det = dot(edge1, pvec);

		if (det > -Epsilon && det < Epsilon)
			return false;
		float inv_det = typename genType::value_type(1) / det;

		genType tvec = orig - vert0;

		position.y = dot(tvec, pvec) * inv_det;
		if (position.y < typename genType::value_type(0) || position.y > typename genType::value_type(1))
			return false;

		genType qvec = cross(tvec, edge1);

		position.z = dot(dir, qvec) * inv_det;
		if (position.z < typename genType::value_type(0) || position.y + position.z > typename genType::value_type(1))
			return false;

		position.x = dot(edge2, qvec) * inv_det;

		return true;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER bool intersectRaySphere
	(
		genType const & rayStarting, genType const & rayNormalizedDirection,
		genType const & sphereCenter, const typename genType::value_type sphereRadiusSquered,
		typename genType::value_type & intersectionDistance
	)
	{
		typename genType::value_type Epsilon = std::numeric_limits<typename genType::value_type>::epsilon();
		genType diff = sphereCenter - rayStarting;
		typename genType::value_type t0 = dot(diff, rayNormalizedDirection);
		typename genType::value_type dSquared = dot(diff, diff) - t0 * t0;
		if( dSquared > sphereRadiusSquered )
		{
			return false;
		}
		typename genType::value_type t1 = sqrt( sphereRadiusSquered - dSquared );
		intersectionDistance = t0 > t1 + Epsilon ? t0 - t1 : t0 + t1;
		return intersectionDistance > Epsilon;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER bool intersectRaySphere
	(
		genType const & rayStarting, genType const & rayNormalizedDirection,
		genType const & sphereCenter, const typename genType::value_type sphereRadius,
		genType & intersectionPosition, genType & intersectionNormal
	)
	{
		typename genType::value_type distance;
		if( intersectRaySphere( rayStarting, rayNormalizedDirection, sphereCenter, sphereRadius * sphereRadius, distance ) )
		{
			intersectionPosition = rayStarting + rayNormalizedDirection * distance;
			intersectionNormal = (intersectionPosition - sphereCenter) / sphereRadius;
			return true;
		}
		return false;
	}

	template<typename genType>
	GLM_FUNC_QUALIFIER bool intersectLineSphere
	(
		genType const & point0, genType const & point1,
		genType const & sphereCenter, typename genType::value_type sphereRadius,
		genType & intersectionPoint1, genType & intersectionNormal1, 
		genType & intersectionPoint2, genType & intersectionNormal2
	)
	{
		typename genType::value_type Epsilon = std::numeric_limits<typename genType::value_type>::epsilon();
		genType dir = normalize(point1 - point0);
		genType diff = sphereCenter - point0;
		typename genType::value_type t0 = dot(diff, dir);
		typename genType::value_type dSquared = dot(diff, diff) - t0 * t0;
		if( dSquared > sphereRadius * sphereRadius )
		{
			return false;
		}
		typename genType::value_type t1 = sqrt( sphereRadius * sphereRadius - dSquared );
		if( t0 < t1 + Epsilon )
			t1 = -t1;
		intersectionPoint1 = point0 + dir * (t0 - t1);
		intersectionNormal1 = (intersectionPoint1 - sphereCenter) / sphereRadius;
		intersectionPoint2 = point0 + dir * (t0 + t1);
		intersectionNormal2 = (intersectionPoint2 - sphereCenter) / sphereRadius;
		return true;
	}
}//namespace glm
