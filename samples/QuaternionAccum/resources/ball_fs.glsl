varying vec3 ObjPos;

void main()
{
	float objY = ( ObjPos.y + 0.5 ) / 1.0;

	if( objY < 0.333 )
		gl_FragColor.rgb = vec3( 1.0, 0.0, 0.0 );
	else if( objY < 0.666 )
		gl_FragColor.rgb = vec3( 1.0, 1.0, 1.0 );
	else
		gl_FragColor.rgb = vec3( 0.0, 0.0, 1.0 );

	gl_FragColor.a = 1.0;
}
