varying float v_xPos;
varying float v_yPos;
varying float v_zPos;

vertex_shader {
  attribute vec3 a_Position;

  uniform mat4 u_ProjectionMatrix;
  uniform mat4 u_ModelMatrix;

  void main() {
    v_xPos = a_Position.x + 0.5;
    v_yPos = a_Position.y + 0.5;
    v_zPos = a_Position.z + 0.5;
    gl_Position = u_ProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
  }
}

fragment_shader {
  void main() {
    gl_FragColor = vec4(v_xPos, v_yPos, v_zPos, 1.0);
  }
}
