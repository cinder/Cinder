varying vec3 ObjPos;

void main()
{
	float lightMult = distance( vec2( 0.0, 0.0 ), ObjPos.xz );
	lightMult = 1.0 - lightMult / 10.0;
	
	if( ( mod( ObjPos.x, 1.0 ) < 0.1 ) || ( mod( ObjPos.z, 1.0 ) < 0.1 ) )
		gl_FragColor.rgb = vec3( 1.0, 1.0, 1.0 );
	else
		gl_FragColor.rgb = vec3( 0.0, 0.0, 0.0 );
	
	gl_FragColor.rgb *= lightMult;	
	gl_FragColor.a = 1.0;
}