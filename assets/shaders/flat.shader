varying vec3 v_fragCoord;
varying vec3 v_viewCoord;

vertex_shader {
  attribute vec3 a_Position;

  uniform mat4 u_ProjectionMatrix;
  uniform mat4 u_ModelMatrix;

  void main() {
    vec4 fragCoord = u_ModelMatrix * vec4(a_Position, 1.0);
    vec4 viewCoord = fragCoord;
    // No view matrix atm
    //vec4 viewCoord = u_viewMatrix * fragCoord;

    v_fragCoord = fragCoord.xyz;
    v_viewCoord = viewCoord.xyz;

    gl_Position = u_ProjectionMatrix * viewCoord;
  }
}

fragment_shader {
  uniform mat4 u_ModelMatrix;

  float determinant(mat2 m) {
    return m[0][0]*m[1][1] - m[1][0]*m[0][1] ;
  }

  mat2 inverse(mat2 m) {
    float d = 1.0 / determinant(m) ;
    return d * mat2( m[1][1], -m[0][1], -m[1][0], m[0][0]) ;
  }

  mat4 inverse(mat4 m) {
    mat2 a = inverse(mat2(m));
    mat2 b = mat2(m[2].xy,m[3].xy);
    mat2 c = mat2(m[0].zw,m[1].zw);
    mat2 d = mat2(m[2].zw,m[3].zw);

    mat2 t = c*a;
    mat2 h = inverse(d - t*b);
    mat2 g = - h*t;
    mat2 f = - a*b*h;
    mat2 e = a - f*t;

    return mat4( vec4(e[0],g[0]), vec4(e[1],g[1]), 
                    vec4(f[0],h[0]), vec4(f[1],f[1]) );
  }

  void main() {
    vec3 diffuse = vec3(0.8);

    vec3 normal = normalize(cross(dFdx(v_fragCoord), dFdy(v_fragCoord)));
    //normal = (inverse(transpose(u_ModelMatrix)) * vec4(normal, 1.0)).xyz;
    float intensity = max(0.2, dot(-normalize(v_viewCoord), normalize(normal)));

    gl_FragColor = vec4(intensity * diffuse, 1.0);
  }
}
