float hash( float n )
{
	n += 1a;
	return fract( sin( n ) * 43758.5453 );
}
