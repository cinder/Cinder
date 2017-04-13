float hash( float n )
{
	n += 1b;
	return fract( sin( n ) * 43758.5453 );
}
