varying vec3 v_fragCoord;
varying vec3 v_viewCoord;

vertex_shader {
  attribute vec3 a_position;

  uniform mat4 u_modelMatrix;
  uniform mat4 u_viewMatrix;
  uniform mat4 u_projectionMatrix;

  void main() {
    vec4 fragCoord = u_modelMatrix * vec4(a_position, 1.0);
    vec4 viewCoord = u_viewMatrix * fragCoord;

    v_fragCoord = fragCoord.xyz;
    v_viewCoord = viewCoord.xyz;

    gl_Position = u_projectionMatrix * viewCoord;
  }
}

fragment_shader {
  uniform vec3 u_color;

  void main() {
    vec3 diffuse = u_color;

    vec3 X = dFdx(v_fragCoord);
    vec3 Y = dFdy(v_fragCoord);
    vec3 normal=normalize(cross(X,Y));
    float intensity = max(0.2, dot(-normalize(v_viewCoord), normalize(normal)));

    gl_FragColor = vec4(intensity * diffuse, 1.0);
  }
}
