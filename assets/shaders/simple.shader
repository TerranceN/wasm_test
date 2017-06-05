varying float v_xPos;
varying float v_yPos;
varying float v_zPos;

vertex_shader {
  attribute vec3 a_position;

  uniform mat4 u_modelMatrix;
  uniform mat4 u_projectionMatrix;

  void main() {
    v_xPos = a_position.x + 0.5;
    v_yPos = a_position.y + 0.5;
    v_zPos = a_position.z + 0.5;
    gl_Position = u_projectionMatrix * u_modelMatrix * vec4(a_position, 1.0);
  }
}

fragment_shader {
  void main() {
    gl_FragColor = vec4(v_xPos, v_yPos, v_zPos, 1.0);
  }
}
