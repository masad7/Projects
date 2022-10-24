//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 Ip;

//here create uniforms for all the data we need here
//declarem les uniformes
uniform vec3 ka; 
uniform vec3 kd;
uniform vec3 ks;

uniform vec3 Ia; 
uniform vec3 Id;
uniform vec3 Is;

uniform float alpha;

uniform vec3 eye_position;
uniform vec3 light_position;

void main()
{
	//here we set up the normal as a color to see them as a debug
	vec3 color = v_wNormal;

	//here write the computations for PHONG.
	//for GOURAUD you dont need to do anything here, just pass the color from the vertex shader
	vec3 La = ka.xyz*Ia.xyz; //llum ambient

	vec3 l = light_position.xyz - v_wPos.xyz;
	l = normalize(l);
	vec3 Ld = kd.xyz*(clamp(dot(l, v_wNormal), 0, 1))*Id.xyz; //llum difusa

	vec3 v = eye_position.xyz - v_wPos.xyz;
	v = normalize(v);
	vec3 r = reflect(l, v_wPos);
	r = normalize(r);
	vec3 Ls = ks.xyz*pow(clamp(dot(r, v), 0, 1), alpha)*Is.xyz; //lum especular

	vec3 Ip = La.xyz + Ld.xyz + Ls.xyz;

	//set the ouput color por the pixel
	gl_FragColor = vec4( Ip.xyz, 1.0 ) * 1.0;
}
