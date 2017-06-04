varying float v_xPos;

vertex_shader {
  attribute vec3 a_Position;

  uniform mat4 u_ProjectionMatrix;
  uniform mat4 u_ModelMatrix;
  uniform mat4 u_ModelMatrix2;

  void main() {
    v_xPos = a_Position.x + 0.8;
    gl_Position = u_ProjectionMatrix * u_ModelMatrix2 * u_ModelMatrix * vec4(a_Position, 1.0);
  }
}

fragment_shader {
  uniform vec2 color;

  void main() {
    gl_FragColor = vec4(v_xPos, color, 1.0);
  }
}
