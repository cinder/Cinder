uniform sampler2DShadow		depthTexture;

varying vec3				N, V, L;
varying vec4				q;

void main(void)
{
	vec3 normal = normalize( N );
	vec3 R = -normalize( reflect( L, normal ) );
	
	vec4 ambient = gl_FrontLightProduct[0].ambient;
	vec4 diffuse = gl_FrontLightProduct[0].diffuse * max(dot( normal, L), 0.0);
	vec4 specular = gl_FrontLightProduct[0].specular * pow(max(dot(R, V), 0.0), gl_FrontMaterial.shininess);
	
	vec3 coord = 0.5 * (q.xyz / q.w + 1.0);
	float shadow = shadow2D( depthTexture, coord ).r;
	gl_FragColor = (ambient + (0.2 + 0.8 * shadow) * diffuse) + specular * shadow;
}
