varying vec3 v_viewCoord;

vertex_shader {
  attribute vec3 a_position;

  uniform mat4 u_modelMatrix;
  uniform mat4 u_viewMatrix;
  uniform mat4 u_projectionMatrix;

  void main() {
    vec4 fragCoord = u_modelMatrix * vec4(a_position, 1.0);
    vec4 viewCoord = u_viewMatrix * fragCoord;

    v_viewCoord = viewCoord.xyz;

    gl_Position = u_projectionMatrix * viewCoord;
  }
}

fragment_shader {
  void main() {
    vec3 diffuse = vec3(0.8);

    vec3 X = dFdx(v_viewCoord);
    vec3 Y = dFdy(v_viewCoord);
    vec3 normal=normalize(cross(X,Y));
    float intensity = max(0.2, dot(-normalize(v_viewCoord), normalize(normal)));

    gl_FragColor = vec4(intensity * diffuse, 1.0);
  }
}
